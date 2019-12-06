#include "mainfence.h"
#include "ui_mainfence.h"



MainFence::MainFence(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainFence)
{
    ui->setupUi(this);

    connect(this, signal_addSection, ui->QW_fence, &OglWidget_mf::slot_addSection);
    connect(this, signal_addWell, ui->QW_fence, &OglWidget_mf::slot_addWell);
    connect(this, signal_addTerrain, ui->QW_fence, &OglWidget_mf::slot_addTerrain);


    QDir QD_path;
    QS_topPath=QD_path.currentPath();

}

MainFence::~MainFence()
{
    delete ui;
}

void MainFence::on_QPB_addSection_clicked()
{
    emit signal_addSection();
}

void MainFence::on_QPB_addWell_clicked()
{
    emit signal_addWell(QS_topPath);
}

void MainFence::on_QPB_creatFDMLayers_clicked()
{
    QFileDialog QFD_dialog(this);
    QFD_dialog.setDirectory(QS_topPath + "/well_data");
    QFD_dialog.setFileMode(QFileDialog::ExistingFiles);
    QFD_dialog.setNameFilter(tr("txt Files (*.txt)"));
    QFD_dialog.setViewMode(QFileDialog::Detail);
    QStringList QSL_fileNames;
    if (QFD_dialog.exec())
        QSL_fileNames = QFD_dialog.selectedFiles();

    QList<QStringList*> QLsl_welldata;
    QFile QF_file_in,QF_file_out;
    QTextStream txt_file_in,txt_file_out;
    QDir QD_file;
    QString QS_file_in,QS_file_out,QS_num,QS_layer;
    int num;
    if (!QSL_fileNames.isEmpty())
    {
        for (int i=0; i<QSL_fileNames.count(); i++)
        {
            QF_file_in.setFileName(QSL_fileNames[i]);
            QF_file_in.open(QFile::ReadOnly|QFile::Text);
            txt_file_in.setDevice(&QF_file_in);
            QLsl_welldata.append(new QStringList());
            QS_file_in=txt_file_in.readLine();
            QLsl_welldata[i]->append(QS_file_in.split('\t'));
            while (!txt_file_in.atEnd())
            {
                QS_file_in=txt_file_in.readLine();
                QLsl_welldata[i]->append(QS_file_in.split('\t'));
            }
            QF_file_in.close();
        }
        num=QLsl_welldata[0]->count()-2;
        for (int i=0; i<QSL_fileNames.count(); i++)//alern
        {
            if (num != QLsl_welldata[i]->count()-2)
            {
                QMessageBox::information(this,"Error","The layers of wells can`t be different");
                i=QSL_fileNames.count();
                num=0;
            }
        }
        for (int i=0; i<num; i++)
        {
            QD_file.mkpath(QS_topPath + "/FDM_layers");
            QS_num.setNum(i+1);
            QS_file_out="layer"+QS_num;
            QF_file_out.setFileName("FDM_layers/" + QS_file_out + ".txt");
            QF_file_out.open(QFile::WriteOnly|QFile::Text);
            txt_file_out.setDevice(&QF_file_out);
            for (int k=2; k<QLsl_welldata.count(); k++)
            {
                QS_layer=QLsl_welldata[k][0][0]+'\t'+QLsl_welldata[k][0][1]+'\t'+QLsl_welldata[k][0][i+2];
                txt_file_out<<QS_layer<<endl;
            }
            QF_file_out.close();
        }
        QMessageBox::information(this,"Message","Save successfully.");
    }
}


void MainFence::on_QPB_addTerrain_clicked()
{
    emit signal_addTerrain(QSL_puliPath);
}

void MainFence::on_QPB_puliDEM_clicked()
{
    QString QS_in;
    QString QS_filepath=QFileDialog::getOpenFileName(
                this,
                tr("Select a Depth File"),
                "",
                "txt Files (*.txt)");
    if (!QS_filepath.isNull())
    {
        QFile depthfile(QS_filepath);
        depthfile.open(QFile::ReadOnly|QFile::Text);
        QTextStream txt_depthfile_in(&depthfile);
        QS_in=txt_depthfile_in.readLine();
        QSL_puliPath=QS_in.split('\"');
        while (!txt_depthfile_in.atEnd())
        {
            QS_in=txt_depthfile_in.readLine();
            QSL_puliPath.append(QS_in.split('\"'));
        }
        int l=QSL_puliPath.count();
        for (int i=0; i<(l/3); i++)
        {
            QSL_puliPath.removeAt(i);
            QSL_puliPath.removeAt(i+1);
        }
    }
}
