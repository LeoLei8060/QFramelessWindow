#include "framelesswindow.h"
#include "windowshadow.h"
#include <QApplication>
#include <QMouseEvent>
#include <QScreen>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#endif

// 添加Qt版本检查的辅助函数
static inline QPoint getGlobalPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_titleBar(new TitleBar(this))
    , m_centralWidget(nullptr)
    , m_layout(new QVBoxLayout(this))
{
    initializeWindow();
}

FramelessWindow::~FramelessWindow()
{
    delete shadow;
}

void FramelessWindow::initializeWindow()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    // 设置最小尺寸
    setMinimumSize(200, 150);

    // 设置布局
    m_layout->setContentsMargins(RESIZE_BORDER, RESIZE_BORDER, RESIZE_BORDER, RESIZE_BORDER);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_titleBar);

    shadow = new WindowShadow(this);

    // 连接标题栏信号
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &FramelessWindow::onMinimizeClicked);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, &FramelessWindow::onMaximizeClicked);
    connect(m_titleBar, &TitleBar::closeClicked, this, &FramelessWindow::onCloseClicked);

    // 确保中心部件也能接收鼠标移动事件
    if (m_centralWidget) {
        m_centralWidget->setMouseTracking(true);
    }

    // 设置默认大小
    resize(800, 600);
}

void FramelessWindow::setCentralWidget(QWidget *widget)
{
    if (m_centralWidget) {
        m_layout->removeWidget(m_centralWidget);
        delete m_centralWidget;
    }

    m_centralWidget = widget;
    if (widget) {
        // 确保新的中心部件也能接收鼠标移动事件
        widget->setMouseTracking(true);
        m_layout->addWidget(widget);
    }
}

QWidget *FramelessWindow::centralWidget() const
{
    return m_centralWidget;
}

void FramelessWindow::setWindowTitle(const QString &title)
{
    QWidget::setWindowTitle(title);
    m_titleBar->setTitle(title);
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = getGlobalPos(event) - frameGeometry().topLeft();
        isDragging = true;

        // 检查是否在调整大小区域
        if (isInResizeArea(event->pos())) {
            updateCursor(event->pos());
        } else {
            resizeDir = ResizeDirection::None;
        }
    }
    QWidget::mousePressEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        resizeDir = ResizeDirection::None;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 始终更新光标，无论是否在拖动状态
    updateCursor(event->pos());

    if (!isDragging) {
        return;
    }

    if (resizeDir != ResizeDirection::None) {
        // 处理窗口缩放
        QRect  geometry = frameGeometry();
        QPoint globalPos = getGlobalPos(event);

        // 保存原始几何信息，用于验证
        QRect newGeometry = geometry;

        switch (resizeDir) {
        case ResizeDirection::Left:
            newGeometry.setLeft(globalPos.x());
            break;
        case ResizeDirection::Right:
            newGeometry.setRight(globalPos.x());
            break;
        case ResizeDirection::Top:
            newGeometry.setTop(globalPos.y());
            break;
        case ResizeDirection::Bottom:
            newGeometry.setBottom(globalPos.y());
            break;
        case ResizeDirection::TopLeft:
            newGeometry.setTopLeft(globalPos);
            break;
        case ResizeDirection::TopRight:
            newGeometry.setTopRight(globalPos);
            break;
        case ResizeDirection::BottomLeft:
            newGeometry.setBottomLeft(globalPos);
            break;
        case ResizeDirection::BottomRight:
            newGeometry.setBottomRight(globalPos);
            break;
        default:
            break;
        }

        // 验证新的几何信息是否有效
        if (newGeometry.width() >= minimumWidth() && newGeometry.height() >= minimumHeight()
            && newGeometry.width() <= maximumWidth() && newGeometry.height() <= maximumHeight()
            && newGeometry.width() > 0 && newGeometry.height() > 0) {
            setGeometry(newGeometry);
        }
    } else if (m_titleBar->geometry().contains(event->pos())) {
        // 处理窗口拖动，仅当鼠标在标题栏区域时
        move(getGlobalPos(event) - dragPosition);
    }
}

void FramelessWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (shadow) {
        shadow->updateShadow();
    }
}

bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message, ResultType *result)
{
#ifdef Q_OS_WIN
    MSG *msg = static_cast<MSG *>(message);
    switch (msg->message) {
    case WM_NCHITTEST: {
        // 获取窗口坐标
        RECT winrect;
        GetWindowRect((HWND) winId(), &winrect);

        // 鼠标坐标
        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);

        *result = 0;
        return false;
    }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

bool FramelessWindow::isInResizeArea(const QPoint &pos) const
{
    if (isMaximized()) {
        return false;
    }

    return pos.x() <= RESIZE_BORDER ||           // 左边
           pos.x() >= width() - RESIZE_BORDER || // 右边
           pos.y() <= RESIZE_BORDER ||           // 上边
           pos.y() >= height() - RESIZE_BORDER;  // 下边
}

void FramelessWindow::updateCursor(const QPoint &pos)
{
    // 如果在标题栏区域，保持默认光标
    if (m_titleBar->geometry().contains(pos)) {
        setCursor(Qt::ArrowCursor);
        resizeDir = ResizeDirection::None;
        return;
    }

    // 检查是否在调整大小区域
    if (!isInResizeArea(pos)) {
        setCursor(Qt::ArrowCursor);
        resizeDir = ResizeDirection::None;
        return;
    }

    bool onLeft = pos.x() <= RESIZE_BORDER;
    bool onRight = pos.x() >= width() - RESIZE_BORDER;
    bool onTop = pos.y() <= RESIZE_BORDER;
    bool onBottom = pos.y() >= height() - RESIZE_BORDER;

    if (onTop && onLeft) {
        setCursor(Qt::SizeFDiagCursor);
        resizeDir = ResizeDirection::TopLeft;
    } else if (onTop && onRight) {
        setCursor(Qt::SizeBDiagCursor);
        resizeDir = ResizeDirection::TopRight;
    } else if (onBottom && onLeft) {
        setCursor(Qt::SizeBDiagCursor);
        resizeDir = ResizeDirection::BottomLeft;
    } else if (onBottom && onRight) {
        setCursor(Qt::SizeFDiagCursor);
        resizeDir = ResizeDirection::BottomRight;
    } else if (onLeft) {
        setCursor(Qt::SizeHorCursor);
        resizeDir = ResizeDirection::Left;
    } else if (onRight) {
        setCursor(Qt::SizeHorCursor);
        resizeDir = ResizeDirection::Right;
    } else if (onTop) {
        setCursor(Qt::SizeVerCursor);
        resizeDir = ResizeDirection::Top;
    } else if (onBottom) {
        setCursor(Qt::SizeVerCursor);
        resizeDir = ResizeDirection::Bottom;
    }
}

void FramelessWindow::onMinimizeClicked()
{
    showMinimized();
}

void FramelessWindow::onMaximizeClicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
    m_titleBar->updateButtonStates();
}

void FramelessWindow::onCloseClicked()
{
    close();
}
