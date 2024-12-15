#ifndef WINDOWSHADOW_H
#define WINDOWSHADOW_H

#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

class WindowShadow : public QObject
{
    Q_OBJECT

public:
    explicit WindowShadow(QWidget* parent = nullptr);
    ~WindowShadow();

    void updateShadow();

private:
    QWidget* parentWidget;
    
#ifdef Q_OS_WIN
    void applyWindowsShadow();
#else
    void applyQtShadow();
#endif
};

#endif // WINDOWSHADOW_H
