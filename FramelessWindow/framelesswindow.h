#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>
#include <QPoint>
#include <QVBoxLayout>
#include "titlebar.h"

// 添加Qt版本检查
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ResultType = qintptr;
#else
    using ResultType = long;
#endif

class WindowShadow;

class FramelessWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    ~FramelessWindow();

    // 设置中心窗口部件
    void setCentralWidget(QWidget *widget);
    // 获取中心窗口部件
    QWidget *centralWidget() const;
    // 设置窗口标题
    void setWindowTitle(const QString &title);
    // 获取标题栏
    TitleBar *titleBar() const { return m_titleBar; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, ResultType *result) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void initializeWindow();
    void updateCursor(const QPoint &pos);
    bool isInResizeArea(const QPoint &pos) const;
    
    enum class ResizeDirection {
        None,
        Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };

    QPoint dragPosition;
    bool isDragging = false;
    ResizeDirection resizeDir = ResizeDirection::None;
    WindowShadow* shadow = nullptr;
    const int RESIZE_BORDER = 5;

    // 新增成员
    TitleBar *m_titleBar;
    QWidget *m_centralWidget;
    QVBoxLayout *m_layout;

private slots:
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();
};

#endif // FRAMELESSWINDOW_H
