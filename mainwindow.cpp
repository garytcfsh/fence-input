#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QtOpenGL>
#include "oglwidget.h"
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);



    connect(ui->le_hml, &QLineEdit::editingFinished, this, &MainWindow::creat_table);//connect le_hml and tbw_layer
    connect(ui->tbw_layer, &QTableWidget::itemChanged, this, &MainWindow::update_depth);
    connect(this, signal_paintwell, ui->widget, &oglwidget::slot_paintwell);

    wellnumber=0;
    hml=-1;
    t=0;
    isSave=false;

    ui->le_X_coordinate->setText("");//set x coordinate
    ui->le_Y_coordinate->setText("");
    ui->le_hml->setText("");//how many layers
    ui->tbw_layer->setColumnCount(3);
    ui->tbw_layer->setHorizontalHeaderItem(0,new QTableWidgetItem("top"));
    ui->tbw_layer->setHorizontalHeaderItem(1,new QTableWidgetItem("bottom"));
    ui->tbw_layer->setHorizontalHeaderItem(2,new QTableWidgetItem("attribute"));
//    ui->tbw_layer->horizontalHeader()->setDefaultSectionSize(100);//set all column width
    ui->tbw_layer->setColumnWidth(0,65);
    ui->tbw_layer->setColumnWidth(1,65);
    QLsl_welldata.append(new QStringList());//new a qstringlist in QList to save data

//    ui->widget->resize(w.height());



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
//    k=ui->le_hml->text().toInt();
//    if (QLcb_attribute[k-1]->focusPolicy())
}

void MainWindow::creat_table()
{
    if (ui->le_hml->text().toInt() > 0)
    {
        int k,qlnumber;
        k=ui->le_hml->text().toInt();
//        if (hml!=k)//I forget why need this IF
        {
            for (int i=0;i<hml;i++)//remove old table
            {
                ui->tbw_layer->removeRow(0);
            }
            hml=k;
            ui->tbw_layer->setColumnWidth(0,65);
            ui->tbw_layer->setColumnWidth(1,65);
            for (int i=0;i<k;i++)
            {
                ui->tbw_layer->insertRow(0);
            }

            for (int j=0;j<2;j++)
            {
                for (int i=0;i<k;i++)
                {
                    ui->tbw_layer->setItem(i,j,new QTableWidgetItem(""));
                }
            }
            ui->tbw_layer->setItem(0,0,new QTableWidgetItem("0.0"));

            QLcb_attribute.clear();//clear the item which append at last time
            for (int b=0;b<k;b++)
            {
                QLcb_attribute.append(new QComboBox());
                ui->tbw_layer->setCellWidget(b,2,QLcb_attribute[b]);//creat combobox in item
                QLcb_attribute[b]->insertItem(0,tr("attribute"));
                QLcb_attribute[b]->insertItem(1,tr("礫石"));
                QLcb_attribute[b]->insertItem(2,tr("粗沙"));
                QLcb_attribute[b]->insertItem(3,tr("細沙"));
                QLcb_attribute[b]->insertItem(4,tr("粉沙、泥"));
                QLcb_attribute[b]->insertItem(5,tr("其他1"));
                QLcb_attribute[b]->insertItem(6,tr("其他2"));
                connect(QLcb_attribute[b], &QComboBox::currentTextChanged, this, &MainWindow::update_attribute);
            }
            QLcb_attribute[0]->setEnabled(false);
            for (int b=1;b<k;b++)
            {
                ui->tbw_layer->item(b,0)->setBackground(Qt::gray);
                ui->tbw_layer->item(b,0)->setFlags(Qt::ItemIsEditable);
                QLcb_attribute[b]->setEnabled(false);
            }
            setTabOrder(QLcb_attribute[k-1], ui->QPB_save_data);
            setTabOrder(ui->QPB_save_data, ui->QPB_clear);

            //append qstringlist to save data
            QLsl_welldata[wellnumber]->clear();//clear the qstringlist which append at last time
            qlnumber=3+k*3;
            for (int i=0;i<qlnumber;i++)
            {
                QLsl_welldata[wellnumber]->append("");
            }
        }
    }

}

void MainWindow::update_depth(QTableWidgetItem *item)
{
    if (item->text() != "" && item->text() != "0.0")//avoid updatestatus when howmanylayer change
    {
        int i,j,k,ii,b;
        ii=0;
        i=ui->tbw_layer->row(item);
        j=ui->tbw_layer->column(item);
        k=ui->le_hml->text().toInt();
        if (item == ui->tbw_layer->currentItem())//avoid updatestatus when the item of auto set value change
        {
            b=item->row();
            QLcb_attribute[b]->setEnabled(true);
            for (j=0;j<ui->tbw_layer->rowCount()-1;j++)//auto set value
            {
                ui->tbw_layer->setItem(j+1,0,new QTableWidgetItem(ui->tbw_layer->item(j,1)->text()));
            }

            k=ui->le_hml->text().toInt();
            for (int b=1;b<k;b++)//set unselectable
            {
                ui->tbw_layer->item(b,0)->setFlags(Qt::ItemIsEditable);
                ui->tbw_layer->item(b,0)->setBackground(Qt::gray);
                ui->tbw_layer->item(b,0)->setForeground(Qt::black);
            }

            //instant save data
            QLsl_welldata[wellnumber]->replace(0,ui->le_Name->text());
            QLsl_welldata[wellnumber]->replace(1,ui->le_X_coordinate->text());
            QLsl_welldata[wellnumber]->replace(2,ui->le_Y_coordinate->text());
            for (int i=0;i<k;i++)
            {
                QLsl_welldata[wellnumber]->replace(3+3*ii,ui->tbw_layer->item(i,0)->text());
                QLsl_welldata[wellnumber]->replace(4+3*ii,ui->tbw_layer->item(i,1)->text());
                QLsl_welldata[wellnumber]->replace(5+3*ii,QLcb_attribute[i]->currentText());
                ii++;
            }
            emit signal_paintwell(*QLsl_welldata[wellnumber],k,wellnumber);
        }
    }
}

void MainWindow::update_attribute(const QString &text)
{
    //instant save data
    int k,ii;
    ii=0;
    k=ui->le_hml->text().toInt();
    QLsl_welldata[wellnumber]->replace(0,ui->le_Name->text());
    QLsl_welldata[wellnumber]->replace(1,ui->le_X_coordinate->text());
    QLsl_welldata[wellnumber]->replace(2,ui->le_Y_coordinate->text());
    for (int i=0;i<k;i++)
    {
        QLsl_welldata[wellnumber]->replace(3+3*ii,ui->tbw_layer->item(i,0)->text());
        QLsl_welldata[wellnumber]->replace(4+3*ii,ui->tbw_layer->item(i,1)->text());
        QLsl_welldata[wellnumber]->replace(5+3*ii,QLcb_attribute[i]->currentText());
        ii++;
    }
    emit signal_paintwell(*QLsl_welldata[wellnumber],k,wellnumber);
}

void MainWindow::on_QPB_save_data_clicked()
{
    if (ui->le_Name->text() != "" && ui->le_X_coordinate->text() != ""
            && ui->le_Y_coordinate->text() != "" && ui->le_hml->text() != "")
    {
        int k,ii=0;
        k=ui->le_hml->text().toInt();
        QLsl_welldata[wellnumber]->replace(0,ui->le_Name->text());
        QLsl_welldata[wellnumber]->replace(1,ui->le_X_coordinate->text());
        QLsl_welldata[wellnumber]->replace(2,ui->le_Y_coordinate->text());
        for (int i=0;i<k;i++)
        {
            QLsl_welldata[wellnumber]->replace(3+3*ii,ui->tbw_layer->item(i,0)->text());
            QLsl_welldata[wellnumber]->replace(4+3*ii,ui->tbw_layer->item(i,1)->text());
            QLsl_welldata[wellnumber]->replace(5+3*ii,QLcb_attribute[i]->currentText());
            ii++;
        }

        //save data as a txt file
        QString QS_well_name,QS_wd;
        QDir QD_well_depth;
        QS_well_name=QLsl_welldata[wellnumber][0][0];
        QD_well_depth.mkdir("well_depth");
        QFile QF_well_depth("well_depth/" + QS_well_name + ".txt");

        QF_well_depth.open(QFile::WriteOnly|QFile::Text);
        QTextStream txt_well_depth_out(&QF_well_depth);
        QS_wd=QLsl_welldata[wellnumber][0][0];
        QS_wd=QS_wd+'\t'+QLsl_welldata[wellnumber][0][1];
        QS_wd=QS_wd+'\t'+QLsl_welldata[wellnumber][0][2];
        txt_well_depth_out<<QS_wd<<endl;
        for (int i=0;i<k;i++)
        {
            QS_wd=QLsl_welldata[wellnumber][0][3+3*i];
            QS_wd=QS_wd+'\t'+QLsl_welldata[wellnumber][0][4+3*i];
            QS_wd=QS_wd+'\t'+QLsl_welldata[wellnumber][0][5+3*i];
            txt_well_depth_out<<QS_wd<<endl;
        }
        QF_well_depth.close();

        QLsl_welldata.append(new QStringList());//new next qstringlist for next well
        wellnumber++;//prepare next well

        QMessageBox::information(this,"Message","Save successfully.");
        isSave=true;
        on_QPB_clear_clicked();
        ui->le_Name->setFocus();
    }
    else
    {
        QMessageBox::information(this,"Message","Well data can't be NULL !");
    }

}

void MainWindow::on_QPB_clear_clicked()
{
    if (isSave)
        wellnumber=wellnumber-1;
    if (ui->le_hml->text() > 0)
    {
        ui->le_Name->setText("");
        ui->le_X_coordinate->setText("");
        ui->le_Y_coordinate->setText("");
        for (int i=0; i<ui->le_hml->text().toInt(); i++)
            ui->tbw_layer->removeRow(0);
        ui->le_hml->setText("");

        QLsl_welldata[wellnumber][0][3]="0";
        QLsl_welldata[wellnumber][0][4]="0";
        QLsl_welldata[wellnumber][0][5]="attribute";
        emit signal_paintwell(*QLsl_welldata[wellnumber],1,wellnumber);
    }
    if (isSave)
        wellnumber=wellnumber+1;

    isSave=false;
}

void MainWindow::on_QPB_TWD67to97_clicked()
{
    double X67,Y67;
    QString X97,Y97;
    double A=0.00001549, B=0.000006521;

    //reference from http://wiki.osgeo.org/wiki/Taiwan_datums#.E8.BD.89.E6.8F.9B.E5.8F.83.E6.95.B8
    X67=ui->le_X_coordinate->text().toDouble();
    Y67=ui->le_Y_coordinate->text().toDouble();
    X97.setNum(X67+807.8+A*X67+B*Y67 ,'f', 1);
    Y97.setNum(Y67-248.6+A*Y67+B*X67, 'f', 1);
    ui->le_X_coordinate->clear();
    ui->le_X_coordinate->insert(X97);
    ui->le_Y_coordinate->clear();
    ui->le_Y_coordinate->insert(Y97);
}
