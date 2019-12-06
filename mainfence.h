#ifndef MAINFENCE_H
#define MAINFENCE_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

namespace Ui {
class MainFence;
}

class MainFence : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainFence(QWidget *parent = 0);
    ~MainFence();

signals:
    void signal_addSection();
    void signal_addWell(QString QS_topPath);
    void signal_addTerrain(QStringList QSL_puliPath);


private slots:
    void on_QPB_addSection_clicked();

    void on_QPB_creatFDMLayers_clicked();

    void on_QPB_addWell_clicked();

    void on_QPB_addTerrain_clicked();

    void on_QPB_puliDEM_clicked();

private:
    Ui::MainFence *ui;
    QString QS_topPath;
    QStringList QSL_puliPath;
};

#endif // MAINFENCE_H
