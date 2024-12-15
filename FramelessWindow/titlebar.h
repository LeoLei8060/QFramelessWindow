#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar() override = default;

    // 设置标题文本
    void setTitle(const QString &title);
    // 获取标题文本
    QString title() const;
    // 设置标题栏高度
    void setTitleBarHeight(int height);
    // 获取标题栏高度
    int titleBarHeight() const;

    virtual void updateButtonStates();

protected:
    // 双击标题栏最大化/还原
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    // 处理鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;
    // 处理鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    // 处理鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;

    // 子类可以重写这些函数来自定义按钮样式
    virtual void initializeButtons();
    virtual void setupLayout();

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    QLabel      *titleLabel;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;
    QHBoxLayout *layout;

    QPoint    dragPosition;
    bool      isDragging;
    const int DEFAULT_HEIGHT = 30;
};

#endif // TITLEBAR_H
