#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QTableWidgetItem>
#include <QLabel>
#include <QComboBox>
#include <QList>
#include <QOpenGLFunctions_1_1>
#include <QtOpenGL>
#include <QGLWidget>
#include <QOpenGLWidget>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <mainsection.h>
#include <mainfence.h>



namespace Ui {
class MainWindow;

}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void signal_paintwell(QStringList,int,int);


protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


private slots:
    void on_QPB_save_data_clicked();
    void creat_table();
    void update_depth(QTableWidgetItem *item);
    void update_attribute(const QString &text);
    void on_QPB_clear_clicked();
    void on_QPB_TWD67to97_clicked();


private:
    Ui::MainWindow *ui;
    QLabel *cellLabel;
    QList<QStringList *> QLsl_welldata;
    QStringList *sl_welldata;
    QList<QComboBox *> QLcb_attribute;
    QComboBox *combo;

    float x,y;
    int wellnumber,hml,t;
    bool isSave;
};



#endif // MAINWINDOW_H
