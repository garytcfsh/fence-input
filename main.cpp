#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int x,y;
    QDesktopWidget dt;
    x=dt.width();
    y=dt.availableGeometry().height();


    MainWindow w;
    w.show();
    w.move(0-7,0);//I don`t know why 0,0 aren`t correct 0,0
    w.resize(x*0.3, y-30);

    MainSection *MS_w;
    MS_w=new MainSection();
    MS_w->show();
    MS_w->move(x*0.3-7, 0);
    MS_w->resize(x*0.7, y*0.5-30);


    MainFence *MF_w;
    MF_w=new MainFence();
    MF_w->show();
    MF_w->move(x*0.3-7, y*0.5);
    MF_w->resize(x*0.7, y*0.5-30);




    return a.exec();
}
