#include "titlebar.h"
#include <QMouseEvent>
#include <QStyle>
#include <QApplication>

// 添加Qt版本检查的辅助函数
static inline QPoint getGlobalPos(const QMouseEvent* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , minimizeButton(new QPushButton(this))
    , maximizeButton(new QPushButton(this))
    , closeButton(new QPushButton(this))
    , layout(new QHBoxLayout(this))
    , isDragging(false)
{
    setFixedHeight(DEFAULT_HEIGHT);
    setMouseTracking(true);

    // 初始化标题标签
    titleLabel->setText(tr("Window Title"));
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    initializeButtons();
    setupLayout();

    // 连接按钮信号
    connect(minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    connect(maximizeButton, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    connect(closeButton, &QPushButton::clicked, this, &TitleBar::closeClicked);
}

void TitleBar::setTitle(const QString &title)
{
    titleLabel->setText(title);
}

QString TitleBar::title() const
{
    return titleLabel->text();
}

void TitleBar::setTitleBarHeight(int height)
{
    setFixedHeight(height);
}

int TitleBar::titleBarHeight() const
{
    return height();
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragPosition = getGlobalPos(event) - window()->frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        window()->move(getGlobalPos(event) - dragPosition);
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void TitleBar::initializeButtons()
{
    // 设置按钮图标
    minimizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    maximizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    // 设置按钮大小
    QSize buttonSize(30, DEFAULT_HEIGHT);
    minimizeButton->setFixedSize(buttonSize);
    maximizeButton->setFixedSize(buttonSize);
    closeButton->setFixedSize(buttonSize);

    // 设置按钮样式
    QString buttonStyle = R"(
        QPushButton {
            border: none;
            background-color: transparent;
        }
        QPushButton:hover {
            background-color: #E5E5E5;
        }
        QPushButton:pressed {
            background-color: #CCCCCC;
        }
    )";
    minimizeButton->setStyleSheet(buttonStyle);
    maximizeButton->setStyleSheet(buttonStyle);
    
    // 关闭按钮特殊样式
    QString closeButtonStyle = R"(
        QPushButton {
            border: none;
            background-color: transparent;
        }
        QPushButton:hover {
            background-color: #E81123;
            color: white;
        }
        QPushButton:pressed {
            background-color: #8B0A14;
            color: white;
        }
    )";
    closeButton->setStyleSheet(closeButtonStyle);
}

void TitleBar::updateButtonStates()
{
    bool isMaximized = window()->isMaximized();
    maximizeButton->setIcon(style()->standardIcon(
        isMaximized ? QStyle::SP_TitleBarNormalButton : QStyle::SP_TitleBarMaxButton
    ));
    maximizeButton->setToolTip(isMaximized ? tr("还原") : tr("最大化"));
}

void TitleBar::setupLayout()
{
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);

    layout->addWidget(titleLabel, 1); // 标题标签占据所有剩余空间
    layout->addWidget(minimizeButton);
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);

    setLayout(layout);
}
