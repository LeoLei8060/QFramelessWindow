#include "windowshadow.h"
#include <QGraphicsDropShadowEffect>

WindowShadow::WindowShadow(QWidget* parent)
    : QObject(parent)
    , parentWidget(parent)
{
    updateShadow();
}

WindowShadow::~WindowShadow()
{
}

void WindowShadow::updateShadow()
{
#ifdef Q_OS_WIN
    applyWindowsShadow();
#else
    applyQtShadow();
#endif
}

#ifdef Q_OS_WIN
void WindowShadow::applyWindowsShadow()
{
    if (!parentWidget) return;

    HWND hwnd = (HWND)parentWidget->winId();
    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

    MARGINS margins = {1, 1, 1, 1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}
#else
void WindowShadow::applyQtShadow()
{
    if (!parentWidget) return;

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(parentWidget);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 0);
    parentWidget->setGraphicsEffect(shadowEffect);
}
#endif
