#include "mainsection.h"
#include "ui_mainsection.h"
#include <QDebug>
#include <QtMath>

MainSection::MainSection(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainSection)
{
    ui->setupUi(this);
    connect(this, signal_paintsection, ui->QW_section, &OglWidget_ms::slot_paintsection);
    connect(this, signal_addVirtualWell, ui->QW_section, &OglWidget_ms::slot_addVirtualWell);
    connect(this, signal_addNode, ui->QW_section, &OglWidget_ms::slot_addNode);
    connect(this, signal_deleteObject, ui->QW_section, &OglWidget_ms::slot_deleteObject);
    connect(this, signal_setQuadrilateralOn, ui->QW_section, &OglWidget_ms::slot_setQuadrilateralOn);
    connect(this, signal_setQuadrilateralOff, ui->QW_section, &OglWidget_ms::slot_setQuadrilateralOff);
    connect(this, signal_set3P1LOn, ui->QW_section, &OglWidget_ms::slot_set3P1LOn);
    connect(this, signal_set3P1LOff, ui->QW_section, &OglWidget_ms::slot_set3P1LOff);
    connect(this, signal_saveState, ui->QW_section, &OglWidget_ms::slot_saveState);
    connect(this, signal_cancelState, ui->QW_section, &OglWidget_ms::slot_cancelState);
    connect(this, signal_clear, ui->QW_section, &OglWidget_ms::slot_clear);
    connect(this, signal_save, ui->QW_section, &OglWidget_ms::slot_save);
    connect(this, signal_reset, ui->QW_section, &OglWidget_ms::slot_reset);
    connect(ui->le_node_depth, &QLineEdit::textEdited, ui->QW_section, &OglWidget_ms::slot_changeNodeDepth);


    ui->QW_section->setFocusPolicy(Qt::ClickFocus);//set which widget be effected by keyboard

    ui->QPB_setQuadrilateral->setCheckable(true);
    ui->QPB_threePointOneLine->setCheckable(true);

    ui->QCB_attribute->insertItem(0,tr("attribute"));
    ui->QCB_attribute->insertItem(1,tr("礫石"));
    ui->QCB_attribute->insertItem(2,tr("粗沙"));
    ui->QCB_attribute->insertItem(3,tr("細沙"));
    ui->QCB_attribute->insertItem(4,tr("粉沙、泥"));
    ui->QCB_attribute->insertItem(5,tr("其他1"));
    ui->QCB_attribute->insertItem(6,tr("其他2"));

    em1=0;
    em2=0;
}

MainSection::~MainSection()
{
    delete ui;
}

void MainSection::mousePressEvent(QMouseEvent *event)
{
    //I don`t know how to emit a signal from QW_section to MainSection
    //,so i use this poor methed(What i want is when i click a node in QW_section
    //,the function at MainSeciton should be run).
    int x = event->x();
    int y = event->y();
    int sx = ui->QW_section->x();
    int sy = ui->QW_section->y();
    int sw = ui->QW_section->width();
    int sh = ui->QW_section->height();

    if (x>=sx && x<=(sx+sw) && y>=sy && y<=(sy+sh))
    {
        x=x-sx;
        y=y-sy;
        ui->QW_section->mouseClick(x,y);
        setValueBar();
    }
}

void MainSection::on_QPB_selectewell_1_clicked()
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
        QSL_welldepth_1=QS_in.split('\t');
        while (!txt_depthfile_in.atEnd())
        {
            QS_in=txt_depthfile_in.readLine();
            QSL_welldepth_1.append(QS_in.split('\t'));
        }
        ui->QPB_selectewell_1->setEnabled(false);
        em1=1;
    }
    if (em1==1 && em2==1)
    {
        emit signal_paintsection(QSL_welldepth_1,QSL_welldepth_2);
        setScaleRuler();
    }
}

void MainSection::on_QPB_selectewell_2_clicked()
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
        QSL_welldepth_2=QS_in.split('\t');
        while (!txt_depthfile_in.atEnd())
        {
            QS_in=txt_depthfile_in.readLine();
            QSL_welldepth_2.append(QS_in.split('\t'));

        }
        ui->QPB_selectewell_2->setEnabled(false);
        em2=1;
    }
    if (em1==1 && em2==1)
    {
        emit signal_paintsection(QSL_welldepth_1,QSL_welldepth_2);
        setScaleRuler();
    }
}

void MainSection::on_QPB_addVirtualWell_clicked()
{
    emit signal_addVirtualWell();
}

void MainSection::on_QPB_addNode_clicked()
{
    emit signal_addNode();
}

void MainSection::on_QPB_deleteObject_clicked()
{
    emit signal_deleteObject();
}

void MainSection::on_QPB_setQuadrilateral_clicked()
{
    if (ui->QPB_setQuadrilateral->isChecked())
    {
        if (ui->QPB_threePointOneLine->isChecked())
        {
            QMessageBox::information(this,"Message"
                                     ,"Can`t do \"three point one line\" and \"quadrilateral\" at same time");
            ui->QPB_setQuadrilateral->setChecked(false);
        }
        else
            emit signal_setQuadrilateralOn();
    }
    else
        emit signal_setQuadrilateralOff();
}

void MainSection::on_QPB_threePointOneLine_clicked()
{
    if (ui->QPB_threePointOneLine->isChecked())
    {
        if (ui->QPB_setQuadrilateral->isChecked())
        {
            QMessageBox::information(this,"Message"
                                     ,"Can`t do \"quadrilateral\" and \"three point one line\" at one time");
            ui->QPB_threePointOneLine->setChecked(false);
        }
        else
            emit signal_set3P1LOn();
    }
    else
        emit signal_set3P1LOff();
}

void MainSection::on_QPB_saveState_clicked()
{
    emit signal_saveState(ui->QCB_attribute->currentText());
}

void MainSection::on_QPB_cancelState_clicked()
{
    emit signal_cancelState();
}

void MainSection::on_QPB_reset_clicked()
{
    emit signal_reset();
}

void MainSection::on_QPB_clear_clicked()
{
    QSL_welldepth_1.clear();
    QSL_welldepth_2.clear();
    em1=0;
    em2=0;
    ui->QL_scale1->clear();
    ui->QL_scale2->clear();
    ui->QL_scale3->clear();
    ui->QL_scale4->clear();
    ui->QL_scale5->clear();
    ui->QL_scale6->clear();
    ui->QL_scale7->clear();
    ui->QPB_selectewell_1->setEnabled(true);
    ui->QPB_selectewell_2->setEnabled(true);
    emit signal_clear();
}

void MainSection::on_QPB_save_clicked()
{
    emit signal_save();
}

void MainSection::setScaleRuler()
{
    scale1=ui->QW_section->getMostShallow();
    scale4=ui->QW_section->getDeepest();
    scale2=new double(*scale1 + (*scale4-*scale1)/4);
    scale3=new double(*scale4 - (*scale4-*scale1)/4);
    scale5=0;
    scale7=qPow((QSL_welldepth_1[1].toDouble()-QSL_welldepth_2[1].toDouble()), 2)
            + qPow((QSL_welldepth_1[2].toDouble()-QSL_welldepth_2[2].toDouble()), 2);
    scale7=qPow(scale7, 0.5);
    scale6=scale7/2;
    QString QS_scale;
    ui->QL_scale1->setText(QS_scale.setNum(*scale1, 'f', 1) + "  -");
    ui->QL_scale2->setText(QS_scale.setNum(*scale2, 'f', 1) + "  -");
    ui->QL_scale3->setText(QS_scale.setNum(*scale3, 'f', 1) + "  -");
    ui->QL_scale4->setText(QS_scale.setNum(*scale4, 'f', 1) + "  -");
    ui->QL_scale5->setText(QS_scale.setNum(scale5, 'f', 1));
    ui->QL_scale6->setText(QS_scale.setNum(scale6, 'f', 1));
    ui->QL_scale7->setText(QS_scale.setNum(scale7, 'f', 1));

}

void MainSection::setValueBar()
{
    ui->QL_XP->clear();
    ui->QL_YP->clear();
    ui->QL_DP->clear();
    if (ui->QW_section->getIsNode())
    {
        double x = ui->QW_section->getxx();//auto type transformation
        double y = ui->QW_section->getyy();
        double worldX,worldY,worldD;//worldDepth
        double mostshallow = *(ui->QW_section->getMostShallow());
        double deepest = *(ui->QW_section->getDeepest());

        worldX = QSL_welldepth_1[1].toDouble()
                + (QSL_welldepth_2[1].toDouble()-QSL_welldepth_1[1].toDouble())
                * (x-(-0.45)) / (0.45-(-0.45));//X
        worldY = QSL_welldepth_1[2].toDouble()
                + (QSL_welldepth_2[2].toDouble()-QSL_welldepth_1[2].toDouble())
                * (x-(-0.45)) / (0.45-(-0.45));//Y
        worldD = mostshallow + (deepest-mostshallow) * (y-0.4) / ((-0.4)-0.4);//Depth
        QString world;
        ui->QL_XP->setText(world.setNum(worldX, 'f', 1));
        ui->QL_YP->setText(world.setNum(worldY, 'f', 1));
        ui->QL_DP->setText(world.setNum(worldD, 'f', 1));
    }
}
