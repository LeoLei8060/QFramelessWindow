#include "framelesswindow.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    FramelessWindow w;
    w.setWindowTitle("Frameless Window Demo");
    
    // 创建一个示例中心部件
    QLabel *centralWidget = new QLabel("这是中心部件");
    centralWidget->setAlignment(Qt::AlignCenter);
    centralWidget->setStyleSheet("QLabel { background-color: #f0f0f0; }");
    w.setCentralWidget(centralWidget);
    
    w.show();
    
    return a.exec();
}
