#ifndef MAINSECTION_H
#define MAINSECTION_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


namespace Ui {
class MainSection;
}

class MainSection : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainSection(QWidget *parent = 0);
    ~MainSection();

    void setScaleRuler();
    void setValueBar();


signals:
    void signal_paintsection(QStringList,QStringList);
    void signal_addVirtualWell();
    void signal_addNode();
    void signal_deleteObject();
    void signal_setQuadrilateralOn();
    void signal_setQuadrilateralOff();
    void signal_set3P1LOn();
    void signal_set3P1LOff();
    void signal_saveState(QString QS_quadri_attribute);
    void signal_cancelState();
    void signal_clear();
    void signal_save();
    void signal_reset();


protected:
    void mousePressEvent(QMouseEvent *event);


private slots:
    void on_QPB_selectewell_1_clicked();

    void on_QPB_selectewell_2_clicked();

    void on_QPB_addVirtualWell_clicked();

    void on_QPB_addNode_clicked();

    void on_QPB_deleteObject_clicked();

    void on_QPB_setQuadrilateral_clicked();

    void on_QPB_saveState_clicked();

    void on_QPB_cancelState_clicked();

    void on_QPB_threePointOneLine_clicked();

    void on_QPB_clear_clicked();

    void on_QPB_save_clicked();

    void on_QPB_reset_clicked();


private:
    Ui::MainSection *ui;
    QStringList QSL_welldepth_1,QSL_welldepth_2;
    QString QS_welldepth_1,QS_welldepth_2;
    int em1,em2;
    double *scale1,*scale2,*scale3,*scale4,scale5,scale6,scale7;

};

#endif // MAINSECTION_H
