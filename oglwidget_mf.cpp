#include "oglwidget_mf.h"
#include "QVector3D"


OglWidget_mf::OglWidget_mf(QWidget *parent)
    : QOpenGLWidget(parent)
{
    section_number=-1;
    state=0;
    centralX=0;
    centralZ=0;
    regionalCenterX=246344;//puli center
    regionalCenterY=2652399;
//    regionalCenterX=230360;//test center
//    regionalCenterY=2596680;
    xRot=90;
    zRot=0;
    yRot=0;
    lookAt1=0;
    lookAt2=0;
    lookAt3=5;
    lookAt4=0;
    lookAt5=0;
    lookAt6=0;
    zoomScale=1.0;
    attribute_R=new double(0.9); attribute_G=new double(0.9); attribute_B=new double(0.9);
    gravel_R=new double(0.274509804); gravel_G=new double(0.509803922); gravel_B=new double(0.705882353);
    coarse_R=new double(0); coarse_G=new double(1); coarse_B=new double(0);
    fine_R=new double(1); fine_G=new double(1); fine_B=new double(0);
    mud_R=new double(1); mud_G=new double(0.647059); mud_B=new double(0);
    another1_R=new double(1); another1_G=new double(0); another1_B=new double(0);
    another2_R=new double(0.5); another2_G=new double(0); another2_B=new double(0.8);


    //set reference for multisample to antianilising
    QSurfaceFormat format;
    format.setSamples(10);
    this->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
}

OglWidget_mf::~OglWidget_mf()
{

}

void OglWidget_mf::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    //
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
//    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    //

    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);//GLU_NONE, GLU_FLAT
    gluQuadricTexture(quadratic, GL_TRUE);
}

void OglWidget_mf::paintGL()
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

//    gluLookAt (0, 1, 1, 0, 0, 0, 0, 1.0, -1);
    drawCoordinate(500);

    GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightColor0[] = {0.0f, 0.0f, 0.0f, 0.0f};
    GLfloat lightPos0[] = {5000.0f, 8000.0f, -10000.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);



    glScaled(zoomScale, zoomScale, zoomScale);
    glTranslatef(lookAt1, lookAt2, 0);
//    gluLookAt (lookAt1, lookAt2, lookAt3, lookAt4, lookAt5, lookAt6, 0, 1.0, 0);
//    glTranslated(-centralX*800/xMAX, 0, centralZ*800/zMAX);

    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
//    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);



//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE , GL_FALSE);



//    GLfloat lightColor1[] = {0.9f, 1.0f, 1.0f, 1.0f};
//    GLfloat lightPos1[] = {0.0f, 10000.0f, 1000.0f, 0.0f};
//    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
//    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

//    GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
//    GLfloat lightPos0[] = {0.0f, 0.0f, 10000.0f, 0.0f};
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);



//    drawSurface(10000,10000,500);
//    drawSection();
    drawWell();
    drawTerrain();
//    drawcube(1000);
//    drawcubeForPerspective(10);
}

void OglWidget_mf::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

//    glOrtho(-1000, +1000, -1000, +1000, -1500, +1500);
    w=this->width();
    h=this->height();
    glOrtho(-5000*w/h, +5000*w/h, -5000, +5000, -5000*30, +5000*30);

//    gluPerspective(45, width/height, 0.000001, 5000);
}

void OglWidget_mf::slot_addSection()
{
    QString QS_in;
    QString QS_filepath=QFileDialog::getOpenFileName(
                this,
                tr("Select a Section File"),
                "",
                "txt Files (*.txt)");
    if (!QS_filepath.isNull())
    {
        section_number=section_number+1;

        QFile sectionfile(QS_filepath);
        sectionfile.open(QFile::ReadOnly|QFile::Text);
        QTextStream txt_sectionfile_in(&sectionfile);
        QS_in=txt_sectionfile_in.readLine();
        QLsl_section_data.append(new QStringList());//append a new section
        *QLsl_section_data[section_number]=QS_in.split('\t');
        while (!txt_sectionfile_in.atEnd())
        {
            QS_in=txt_sectionfile_in.readLine();
            QLsl_section_data[section_number]->append(QS_in.split('\t'));
        }
        state=1;
        for (int i=0; i<QLsl_section_data.count(); i++)
        {
            QLd_findxMAX.append(new double(QLsl_section_data[section_number][0][1].toDouble()));
            QLd_findxMAX.append(new double(QLsl_section_data[section_number][0][4].toDouble()));
            QLd_findzMAX.append(new double(QLsl_section_data[section_number][0][2].toDouble()));
            QLd_findzMAX.append(new double(QLsl_section_data[section_number][0][5].toDouble()));
            QLd_findxmin.append(new double(QLsl_section_data[section_number][0][1].toDouble()));
            QLd_findxmin.append(new double(QLsl_section_data[section_number][0][4].toDouble()));
            QLd_findzmin.append(new double(QLsl_section_data[section_number][0][2].toDouble()));
            QLd_findzmin.append(new double(QLsl_section_data[section_number][0][5].toDouble()));
        }
        for (int i=1; i<QLd_findxMAX.count(); i++)
        {
            if (*QLd_findxMAX[0] < *QLd_findxMAX[i])
                *QLd_findxMAX[0] = *QLd_findxMAX[i];
        }
        for (int i=1; i<QLd_findzMAX.count(); i++)
        {
            if (*QLd_findzMAX[0] < *QLd_findzMAX[i])
                *QLd_findzMAX[0] = *QLd_findzMAX[i];
        }
        for (int i=1; i<QLd_findxmin.count(); i++)
        {
            if (*QLd_findxmin[0] > *QLd_findxmin[i])
                *QLd_findxmin[0] = *QLd_findxmin[i];
        }
        for (int i=1; i<QLd_findzmin.count(); i++)
        {
            if (*QLd_findzmin[0] > *QLd_findzmin[i])
                *QLd_findzmin[0] = *QLd_findzmin[i];
        }
        xMAX=*QLd_findxMAX[0];
        zMAX=*QLd_findzMAX[0];
        xmin=*QLd_findxmin[0];
        zmin=*QLd_findzmin[0];
        centralX = (xMAX + xmin)/2;
        centralZ = (zMAX + zmin)/2;
//        zoomScale = 540/(xMAX-centralX);
    }

    update();
}

void OglWidget_mf::slot_addWell(QString QS_topPath)
{
    QFileDialog QFD_dialog(this);
    QFD_dialog.setDirectory(QS_topPath + "/well_depth");
    QFD_dialog.setFileMode(QFileDialog::ExistingFiles);
    QFD_dialog.setNameFilter(tr("txt Files (*.txt)"));
    QFD_dialog.setViewMode(QFileDialog::Detail);
    QStringList QSL_fileNames;
    if (QFD_dialog.exec())
        QSL_fileNames = QFD_dialog.selectedFiles();

    QFile QF_file_in;
    QTextStream txt_file_in;
    QString QS_file_in;
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
    }
    state=2;
    update();
}

void OglWidget_mf::slot_addTerrain(QStringList QSL_puliPath)
{
//10*10
//    QFileDialog QFD_dialog(this);
//    QFD_dialog.setFileMode(QFileDialog::ExistingFiles);
//    QFD_dialog.setNameFilter(tr("All Files (*) (*.*)"));
//    QFD_dialog.setViewMode(QFileDialog::Detail);
//    QStringList QSL_fileNames;
//    if (QFD_dialog.exec())
//        QSL_fileNames = QFD_dialog.selectedFiles();

//    QFile QF_file_in;
//    QTextStream txt_file_in;
//    QString QS_file_in;
//    QList<QStringList*> QLsl_terrainData;
//    if (!QSL_fileNames.isEmpty())
//    {
//        for (int i=0; i<QSL_fileNames.count(); i++)
//        {
//            QF_file_in.setFileName(QSL_fileNames[i]);
//            QF_file_in.open(QFile::ReadOnly|QFile::Text);
//            txt_file_in.setDevice(&QF_file_in);
//            QLsl_terrainData.append(new QStringList());
//            QS_file_in=txt_file_in.readLine();
//            QLsl_terrainData[i]->append(QS_file_in.split(" "));
//            while (!txt_file_in.atEnd())
//            {
//                QS_file_in=txt_file_in.readLine();
//                QLsl_terrainData[i]->append(QS_file_in.split(" "));
//            }
//            QF_file_in.close();
//        }
//    }
//    terrainDataCount=QLsl_terrainData.count();
//    QV_normals = new QVector3D**[terrainDataCount];
//    for (int j=0; j<terrainDataCount; j++)
//    {
//        QLi_Xnum.append(0);
//        QLi_Ynum.append(0);
//        for (int i=1; i<QLsl_terrainData[j]->count(); i=i+3)
//        {
//            if (QLsl_terrainData[j][0][0]==QLsl_terrainData[j][0][i-1])
//                QLi_Ynum.replace(j, QLi_Ynum[j]+1);
//            if (QLsl_terrainData[j][0][1]==QLsl_terrainData[j][0][i])
//                QLi_Xnum.replace(j, QLi_Xnum[j]+1);
//        }
//        QLi_Xnum.replace(j, 2*QLi_Xnum[j]-1);//x number after interpolation
//        QLi_Ynum.replace(j, 2*QLi_Ynum[j]-1);
//        for (int i=0; i<QLi_Ynum[j]; i++)//create 0 array
//        {
//            QLLLd_demX.append(QList<QList<double>*>());
//            QLLLd_demX[j].append(new QList<double>);
//            QLLLd_demY.append(QList<QList<double>*>());
//            QLLLd_demY[j].append(new QList<double>);
//            QLLLd_demZ.append(QList<QList<double>*>());
//            QLLLd_demZ[j].append(new QList<double>);
//            for (int k=0; k<QLi_Xnum[j]; k++)
//            {
//                QLLLd_demX[j][i]->append(0);
//                QLLLd_demY[j][i]->append(0);
//                QLLLd_demZ[j][i]->append(0);
//            }
//        }
//        for (int i=0; i<QLi_Ynum[j]; i=i+2)//input value fome DEM
//        {
//            for (int k=0; k<QLi_Xnum[j]; k=k+2)
//            {
//                QLLLd_demX[j][i]->replace(k, QLsl_terrainData[j][0][3*((k/2)+(i/2)*(QLi_Xnum[j]+1)/2)].toDouble());
//                QLLLd_demY[j][i]->replace(k, QLsl_terrainData[j][0][3*((k/2)+(i/2)*(QLi_Xnum[j]+1)/2)+1].toDouble());
//                QLLLd_demZ[j][i]->replace(k, QLsl_terrainData[j][0][3*((k/2)+(i/2)*(QLi_Xnum[j]+1)/2)+2].toDouble());
//            }
//        }
//        for (int i=0; i<QLi_Ynum[j]; i++)//interpolation
//        {
//            if ((i+1)%2 == 1)
//            {
//                for (int k=1; k<QLi_Xnum[j]; k=k+2)
//                {
//                    QLLLd_demX[j][i]->replace(k, (QLLLd_demX[j][i][0][k-1]+QLLLd_demX[j][i][0][k+1])/2);
//                    QLLLd_demY[j][i]->replace(k, (QLLLd_demY[j][i][0][k-1]+QLLLd_demY[j][i][0][k+1])/2);
//                    QLLLd_demZ[j][i]->replace(k, (QLLLd_demZ[j][i][0][k-1]+QLLLd_demZ[j][i][0][k+1])/2);
//                }
//            }
//            else
//            {
//                for (int k=0; k<QLi_Xnum[j]; k=k+2)
//                {
//                    QLLLd_demX[j][i]->replace(k, (QLLLd_demX[j][i-1][0][k]+QLLLd_demX[j][i+1][0][k])/2);
//                    QLLLd_demY[j][i]->replace(k, (QLLLd_demY[j][i-1][0][k]+QLLLd_demY[j][i+1][0][k])/2);
//                    QLLLd_demZ[j][i]->replace(k, (QLLLd_demZ[j][i-1][0][k]+QLLLd_demZ[j][i+1][0][k])/2);
//                }
//            }
//        }
//        for (int i=1; i<QLi_Ynum[j]; i=i+2)//interpolation
//        {
//            for (int k=1; k<QLi_Xnum[j]; k=k+2)
//            {
//                QLLLd_demX[j][i]->replace(k,
//                        (QLLLd_demX[j][i-1][0][k-1]+QLLLd_demX[j][i-1][0][k+1]+QLLLd_demX[j][i+1][0][k-1]+QLLLd_demX[j][i+1][0][k+1])/4);
//                QLLLd_demY[j][i]->replace(k,
//                        (QLLLd_demY[j][i-1][0][k-1]+QLLLd_demY[j][i-1][0][k+1]+QLLLd_demY[j][i+1][0][k-1]+QLLLd_demY[j][i+1][0][k+1])/4);
//                QLLLd_demZ[j][i]->replace(k,
//                        (QLLLd_demZ[j][i-1][0][k-1]+QLLLd_demZ[j][i-1][0][k+1]+QLLLd_demZ[j][i+1][0][k-1]+QLLLd_demZ[j][i+1][0][k+1])/4);
//            }
//        }
//        computeNormals(j);
//    }


//20*20
//    QFileDialog QFD_dialog(this);
//    QFD_dialog.setFileMode(QFileDialog::ExistingFiles);
//    QFD_dialog.setNameFilter(tr("All Files (*) (*.*)"));
//    QFD_dialog.setViewMode(QFileDialog::Detail);
//    QStringList QSL_fileNames;
//    if (QFD_dialog.exec())
//    {
//        if (!QSL_puliPath.isEmpty())
//        {
//            QSL_fileNames=QSL_puliPath;
//        }
//        else
//        {
//            QSL_fileNames = QFD_dialog.selectedFiles();
//        }
//    }

//    QFile QF_file_in;
//    QTextStream txt_file_in;
//    QString QS_file_in;
//    QList<QStringList*> QLsl_terrainData;
//    int count=QSL_fileNames.count();
//    qDebug()<<QSL_fileNames;
//    if (!QSL_fileNames.isEmpty())
//    {
//        for (int i=0; i<count; i++)
//        {
//            QF_file_in.setFileName(QSL_fileNames[i]);
//            QF_file_in.open(QFile::ReadOnly|QFile::Text);
//            txt_file_in.setDevice(&QF_file_in);
//            QLsl_terrainData.append(new QStringList());
//            QS_file_in=txt_file_in.readLine();
//            QLsl_terrainData[i]->append(QS_file_in.split(" "));
//            while (!txt_file_in.atEnd())
//            {
//                QS_file_in=txt_file_in.readLine();
//                QLsl_terrainData[i]->append(QS_file_in.split(" "));
//            }
//            QF_file_in.close();
//        }
//    }
//    terrainDataCount=QLsl_terrainData.count();
//    QV_normals = new QVector3D**[terrainDataCount];
//    for (int j=0; j<terrainDataCount; j++)
//    {
//        QLi_Xnum.append(0);
//        QLi_Ynum.append(0);
//        for (int i=1; i<QLsl_terrainData[j]->count(); i=i+3)
//        {
//            if (QLsl_terrainData[j][0][0]==QLsl_terrainData[j][0][i-1])
//                QLi_Ynum.replace(j, QLi_Ynum[j]+1);
//            if (QLsl_terrainData[j][0][1]==QLsl_terrainData[j][0][i])
//                QLi_Xnum.replace(j, QLi_Xnum[j]+1);
//        }
//if (QLsl_terrainData[j][0][0].toDouble() <= 251344 && QLsl_terrainData[j][0][0].toDouble() >= 241344 &&
//        QLsl_terrainData[j][0][1].toDouble() <= 2647399 && QLsl_terrainData[j][0][1].toDouble() <= 2657399)
//    qDebug()<<QSL_fileNames[j];
//        for (int i=0; i<QLi_Ynum[j]; i++)//create 0 array
//        {
//            QLLLd_demX.append(QList<QList<double>*>());
//            QLLLd_demX[j].append(new QList<double>);
//            QLLLd_demY.append(QList<QList<double>*>());
//            QLLLd_demY[j].append(new QList<double>);
//            QLLLd_demZ.append(QList<QList<double>*>());
//            QLLLd_demZ[j].append(new QList<double>);
//            for (int k=0; k<QLi_Xnum[j]; k++)
//            {
//                QLLLd_demX[j][i]->append(0);
//                QLLLd_demY[j][i]->append(0);
//                QLLLd_demZ[j][i]->append(0);
//            }
//        }
//        for (int i=0; i<QLi_Ynum[j]; i=i+1)//input value fome DEM
//        {
//            for (int k=0; k<QLi_Xnum[j]; k=k+1)
//            {
//                QLLLd_demX[j][i]->replace(k, QLsl_terrainData[j][0][3*(k + i*(QLi_Xnum[j]))].toDouble());
//                QLLLd_demY[j][i]->replace(k, QLsl_terrainData[j][0][3*(k + i*(QLi_Xnum[j]))+1].toDouble());
//                QLLLd_demZ[j][i]->replace(k, QLsl_terrainData[j][0][3*(k + i*(QLi_Xnum[j]))+2].toDouble());
//            }
//        }
//        computeNormals(j);
//    }


//40*40
    QFileDialog QFD_dialog(this);
    QFD_dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList filters;
      filters << "All files (*.*)"
              << "Text files (*.txt)"
              << "Grid files (*.grd)";
    QFD_dialog.setNameFilters(filters);
    QFD_dialog.setViewMode(QFileDialog::Detail);
    QStringList QSL_fileNames;
    if (QFD_dialog.exec())
    {
        if (!QSL_puliPath.isEmpty())
        {
            QSL_fileNames=QSL_puliPath;
        }
        else
        {
            QSL_fileNames = QFD_dialog.selectedFiles();
        }
    }

    QFile QF_file_in;
    QTextStream txt_file_in;
    QString QS_file_in;
    QList<QStringList*> QLsl_terrainData;
    int count=QSL_fileNames.count();
    if (!QSL_fileNames.isEmpty())
    {
        for (int i=0; i<count; i++)
        {
            QF_file_in.setFileName(QSL_fileNames[i]);
            QF_file_in.open(QFile::ReadOnly|QFile::Text);
            txt_file_in.setDevice(&QF_file_in);
            QLsl_terrainData.append(new QStringList());
            QS_file_in=txt_file_in.readLine();
            QLsl_terrainData[i]->append(QS_file_in.split(" "));
            while (!txt_file_in.atEnd())
            {
                QS_file_in=txt_file_in.readLine();
                QLsl_terrainData[i]->append(QS_file_in.split(" "));
            }
            QF_file_in.close();
        }
    }
    terrainDataCount=QLsl_terrainData.count();
    QV_normals = new QVector3D**[terrainDataCount];
    int space=2;
    QList<int> QLi_xnum,QLi_ynum;//the point number of initial file
    for (int j=0; j<terrainDataCount; j++)
    {
        QLi_xnum.append(0);
        QLi_ynum.append(0);
        QLi_Xnum.append(0);
        QLi_Ynum.append(0);
        QLLLd_demX.append(QList<QList<double>*>());
        QLLLd_demY.append(QList<QList<double>*>());
        QLLLd_demZ.append(QList<QList<double>*>());
        for (int i=1; i<QLsl_terrainData[j]->count(); i=i+3)
        {
            if (QLsl_terrainData[j][0][0]==QLsl_terrainData[j][0][i-1])
                QLi_ynum.replace(j, QLi_ynum[j]+1);
            if (QLsl_terrainData[j][0][1]==QLsl_terrainData[j][0][i])
                QLi_xnum.replace(j, QLi_xnum[j]+1);
        }
        if (QLi_xnum[j]%space == 0)//caculate point number of X direction
            QLi_Xnum[j]=QLi_xnum[j]/space;
        else
            QLi_Xnum[j]=(QLi_xnum[j]+(space-QLi_xnum[j]%space))/space;

        if (QLi_ynum[j]%space == 0)//caculate point number of Y direction
            QLi_Ynum[j]=QLi_ynum[j]/space;
        else
            QLi_Ynum[j]=(QLi_ynum[j]+(space-QLi_ynum[j]%space))/space;

        for (int i=0; i<QLi_Ynum[j]; i++)//create 0 array
        {
            QLLLd_demX[j].append(new QList<double>);
            QLLLd_demY[j].append(new QList<double>);
            QLLLd_demZ[j].append(new QList<double>);
            for (int k=0; k<QLi_Xnum[j]; k++)
            {
                QLLLd_demX[j][i]->append(0);
                QLLLd_demY[j][i]->append(0);
                QLLLd_demZ[j][i]->append(0);
            }
        }
        for (int i=0; i<QLi_Ynum[j]; i++)//input value fome DEM
        {
            for (int k=0; k<QLi_Xnum[j]; k++)
            {
                QLLLd_demX[j][i]->replace(k, QLsl_terrainData[j][0][ 3*(k*space + i*QLi_xnum[j]*space)].toDouble());
                QLLLd_demY[j][i]->replace(k, QLsl_terrainData[j][0][ 3*(k*space + i*QLi_xnum[j]*space)+1].toDouble());
                QLLLd_demZ[j][i]->replace(k, QLsl_terrainData[j][0][ 3*(k*space + i*QLi_xnum[j]*space)+2].toDouble());
            }
        }

        computeNormals(j);
    }

//find puli
//    for (int a=0; a<terrainDataCount; a++)
//    {
//        qDebug()<<a;
//        if (QLsl_terrainData[a][0][0].toDouble() <= 340000 && QLsl_terrainData[a][0][0].toDouble() >= 330000 &&
//                                    QLsl_terrainData[a][0][1].toDouble() >= 2770000 && QLsl_terrainData[a][0][1].toDouble() <= 2781000)
//                                qDebug()<<QSL_fileNames[a];

//                    if (QLsl_terrainData[a][0][0].toDouble() <= 251344 && QLsl_terrainData[a][0][0].toDouble() >= 241344 &&
//                            QLsl_terrainData[a][0][1].toDouble() >= 2647399 && QLsl_terrainData[a][0][1].toDouble() <= 2657399)
//                        qDebug()<<QSL_fileNames[a];

//    }
}

void OglWidget_mf::drawCoordinate(float length)
{
    glPushMatrix();

    glTranslated(-4250*w/h, -4250, 0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);

    glColor3d(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(length, 0, 0);
    glEnd();
    glColor3d(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(0, length, 0);
    glEnd();
    glColor3d(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, -length);
    glEnd();

    glPopMatrix();
}

void OglWidget_mf::drawSection()
{
    double R,G,B,X,Y,Z;
    float scaleX,scaleY,scaleZ;

    scaleX=1;
    scaleY=10;
    scaleZ=1;
    for (int k=0; k<section_number+1; k++)
    {
        for (int i=6; i<QLsl_section_data[k]->count(); i=i+15)
        {
            R=QLsl_section_data[k][0][i].toDouble();
            G=QLsl_section_data[k][0][i+1].toDouble();
            B=QLsl_section_data[k][0][i+2].toDouble();
            glColor3d(R, G, B);
            glBegin(GL_POLYGON);
            for (int ii=i+3; ii<(i+15); ii=ii+3)
            {
                X=QLsl_section_data[k][0][ii].toDouble();
                Y=QLsl_section_data[k][0][ii+1].toDouble();
                Z=QLsl_section_data[k][0][ii+2].toDouble();
                glVertex3d((X-regionalCenterX)*scaleX, -Y*scaleY, (-Z+regionalCenterY)*scaleZ);
//                glVertex3d((X)*scaleX, -Y*scaleY, (-Z)*scaleZ);
            }
            glEnd();
        }
    }
}

void OglWidget_mf::drawWell()
{
    double X,Y,T,elevation;
    float scaleX,scaleY,scaleZ;
    int tri;
    float Pi,radius;

    tri=40;
    Pi=3.1415926;
    radius=50;
    scaleX=1;
    scaleY=1;
    scaleZ=10;
    for (int w=0; w<QLsl_welldata.count(); w++)//wells
    {
        X=QLsl_welldata[w][0][1].toDouble();
        X=X-regionalCenterX;
        Y=QLsl_welldata[w][0][2].toDouble();
        Y=Y-regionalCenterY;
        elevation=caculateWellToTerrain(w);
        for (int l=0; l<QLsl_welldata[w]->count()/3-1; l++)//layers
        {
            glPushMatrix();
            if (QLsl_welldata[w][0][5+3*l]=="attribute")
                glColor3d(*attribute_R, *attribute_G, *attribute_B);
            if (QLsl_welldata[w][0][5+3*l]=="礫石")
                glColor3d(*gravel_R, *gravel_G, *gravel_B);
            if (QLsl_welldata[w][0][5+3*l]=="粗沙")
                glColor3d(*coarse_R, *coarse_G, *coarse_B);
            if (QLsl_welldata[w][0][5+3*l]=="細沙")
                glColor3d(*fine_R, *fine_G, *fine_B);
            if (QLsl_welldata[w][0][5+3*l]=="粉沙、泥")
                glColor3d(*mud_R, *mud_G, *mud_B);
            if (QLsl_welldata[w][0][5+3*l]=="其他1")
                glColor3d(*another1_R, *another1_G, *another1_B);
            if (QLsl_welldata[w][0][5+3*l]=="其他2")
                glColor3d(*another2_R, *another2_G, *another2_B);
            if (l == 0)//draw hat
            {
                glPushMatrix();
                glTranslatef(X, -QLsl_welldata[w][0][3+3*l].toDouble()*scaleZ + elevation, -Y);//move circle
                glRotatef(90,1,0,0);
                glBegin(GL_TRIANGLE_FAN);
                glVertex2f(0,0);
                for ( int i=0; i<=tri; i++)
                {
                    glVertex2f(radius*cos(i*2*Pi/tri), radius*sin(i*2*Pi/tri));
                }
                glEnd();
                glFlush();
                glPopMatrix();
            }

            T=QLsl_welldata[w][0][4+3*l].toDouble() - QLsl_welldata[w][0][3+3*l].toDouble();//thick
            glTranslated(X, -QLsl_welldata[w][0][3+3*l].toDouble()*scaleZ + elevation, -Y);
            glRotatef(90,1,0,0);
            gluCylinder(quadratic, 50.0f, 50.0f, T*scaleZ, 32, 32);
            glPopMatrix();

        }
//        glPushMatrix();
//        glColor3d(*mud_R, *mud_G, *mud_B);
//        glRotatef(270,1,0,0);
//        glBegin(GL_POLYGON);
//        glVertex3d(0,-2500,000);
//        glVertex3d(-2500,-2500,000);
//        glVertex3d(-2500,2500,000);
//        glVertex3d(0,2500,000);
//        glEnd();
//        glPopMatrix();

//        glColor3d(*gravel_R, *gravel_G, *gravel_B);
//        glBegin(GL_POLYGON);
//        glVertex3d(0,-2500,000);
//        glVertex3d(-2500,-2500,000);
//        glVertex3d(-2500,2500,000);
//        glVertex3d(0,2500,000);
//        glEnd();
    }

//    gluCylinder(quadratic, 200.0f, 200.0f, 300.0f, 32, 32);
}

void OglWidget_mf::drawcube(GLfloat point)
{
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
//    glPolygonMode (GL_BACK, GL_LINE);
//    glMatrixMode (GL_MODELVIEW);
//    glLoadIdentity ();


    glColor3f (1.0, 1.0, 0.0);//前 黃
    glBegin (GL_POLYGON);
    glNormal3d(0, 0, 1);
    glVertex3f (point, point, point);
    glVertex3f (-point, point, point);
    glVertex3f (-point, -point, point);
    glVertex3f (point, -point, point);
    glEnd ();
//    glColor3f(1.0, 0.0, 0.0);//後 紅
//    glBegin (GL_POLYGON);
//    glNormal3d(0, 0, -1);
//    glVertex3f (point, point, -point);
//    glVertex3f (point, -point, -point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (-point, point, -point);
//    glEnd ();
//    glColor3f(0.0, 1.0, 0.0);//左 綠
//    glBegin (GL_POLYGON);
//    glNormal3d(-1, 0, 0);
//    glVertex3f (-point, point, point);
//    glVertex3f (-point, point, -point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (-point, -point, point);
//    glEnd ();
//    glColor3f(0.0, 0.0, 1.0);//右 藍
//    glBegin (GL_POLYGON);
//    glNormal3d(1, 0, 0);
//    glVertex3f (point, point, point);
//    glVertex3f (point, -point, point);
//    glVertex3f (point, -point, -point);
//    glVertex3f (point, point, -point);
//    glEnd ();
//    glColor3f(1.0, 0.0, 1.0); //上 紫紅
//    glBegin (GL_POLYGON);
//    glNormal3d(0, 1, 0);
//    glVertex3f (point, point, point);
//    glVertex3f (point, point, -point);
//    glVertex3f (-point, point, -point);
//    glVertex3f (-point, point, point);
//    glEnd ();
//    glColor3f(0.8, 0.8, 0.8);//下 灰
//    glBegin (GL_POLYGON);
//    glNormal3d(0, -1, 0);
//    glVertex3f (point, -point, point);
//    glVertex3f (-point, -point, point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (point, -point, -point);
//    glEnd ();
}

void OglWidget_mf::drawcubeForPerspective(GLfloat point)
{
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
//    glPolygonMode (GL_BACK, GL_LINE);
//    glMatrixMode (GL_MODELVIEW);
//    glLoadIdentity ();


    glColor3f (1.0, 1.0, 0.0);//前 黃
    glBegin (GL_POLYGON);
    glNormal3d(0, 0, 1);
    glVertex3f (point, point, point);
    glVertex3f (-point, point, point);
    glVertex3f (-point, -point, point);
    glVertex3f (point, -point, point);
    glEnd ();
//    glColor3f(1.0, 0.0, 0.0);//後 紅
//    glBegin (GL_POLYGON);
//    glNormal3d(0, 0, -1);
//    glVertex3f (point, point, -point);
//    glVertex3f (point, -point, -point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (-point, point, -point);
//    glEnd ();
//    glColor3f(0.0, 1.0, 0.0);//左 綠
//    glBegin (GL_POLYGON);
//    glNormal3d(-1, 0, 0);
//    glVertex3f (-point, point, point);
//    glVertex3f (-point, point, -point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (-point, -point, point);
//    glEnd ();
//    glColor3f(0.0, 0.0, 1.0);//右 藍
//    glBegin (GL_POLYGON);
//    glNormal3d(1, 0, 0);
//    glVertex3f (point, point, point);
//    glVertex3f (point, -point, point);
//    glVertex3f (point, -point, -point);
//    glVertex3f (point, point, -point);
//    glEnd ();
//    glColor3f(1.0, 0.0, 1.0); //上 紫紅
//    glBegin (GL_POLYGON);
//    glNormal3d(0, 1, 0);
//    glVertex3f (point, point, point);
//    glVertex3f (point, point, -point);
//    glVertex3f (-point, point, -point);
//    glVertex3f (-point, point, point);
//    glEnd ();
//    glColor3f(0.8, 0.8, 0.8);//下 灰
//    glBegin (GL_POLYGON);
//    glNormal3d(0, -1, 0);
//    glVertex3f (point, -point, point);
//    glVertex3f (-point, -point, point);
//    glVertex3f (-point, -point, -point);
//    glVertex3f (point, -point, -point);
//    glEnd ();
}

void OglWidget_mf::drawSurface(int xLength, int zLength, int size)
{
    glColor3d(0.2, 0.2, 0.2);
    for (int i=0; i<zLength/size-1; i++)
    {
        glBegin(GL_LINES);
        glVertex3d(-xLength/2, 0, (-zLength/2+size*i+size));
        glVertex3d(xLength/2, 0, (-zLength/2+size*i+size));
        glEnd();
    }
    for (int i=0; i<xLength/size-1; i++)
    {
        glBegin(GL_LINES);
        glVertex3d(-xLength/2+size*i+size, 0, -zLength/2);
        glVertex3d(-xLength/2+size*i+size, 0, zLength/2);
        glEnd();
    }
}

void OglWidget_mf::drawTerrain()
{
    float scaleY;
    int elevation0,elevation1,elevation2,elevation3,elevation4,interval;
    scaleY=1;
    interval=300;
    elevation0=300;
    elevation1=elevation0+interval;
    elevation2=elevation1+interval;
    elevation3=elevation2+interval;
    elevation4=elevation3+interval;

    for (int j=0; j<terrainDataCount; j++)//front
    {
        for (int k=0; k<QLi_Ynum[j]-1; k++)
        {
//            glPolygonMode(GL_BACK, GL_LINE);

            glBegin(GL_TRIANGLE_STRIP);
            for (int i=0; i<QLi_Xnum[j]; i++)
            {
                if (QLLLd_demZ[j][k][0][i]>=elevation0 && QLLLd_demZ[j][k][0][i]<elevation1)
                    glColor3d(0, (QLLLd_demZ[j][k][0][i]-elevation0)/(elevation1-elevation0), 1);
                if (QLLLd_demZ[j][k][0][i]>=elevation1 && QLLLd_demZ[j][k][0][i]<elevation2)
                    glColor3d(0, 1, 1-(QLLLd_demZ[j][k][0][i]-elevation1)/(elevation2-elevation1));
                if (QLLLd_demZ[j][k][0][i]>=elevation2 && QLLLd_demZ[j][k][0][i]<elevation3)
                    glColor3d((QLLLd_demZ[j][k][0][i]-elevation2)/(elevation3-elevation2), 1, 0);
                if (QLLLd_demZ[j][k][0][i]>=elevation3 && QLLLd_demZ[j][k][0][i]<elevation4)
                    glColor3d(1, 1-(QLLLd_demZ[j][k][0][i]-elevation3)/(elevation4-elevation3), 0);
                if (QLLLd_demZ[j][k][0][i]>=elevation4)
                    glColor3d(1, 0, 0);

                glNormal3f(QV_normals[j][k+1][i].x(), QV_normals[j][k+1][i].y(), QV_normals[j][k+1][i].z());
                glVertex3d(QLLLd_demX[j][k+1][0][i]-regionalCenterX, QLLLd_demZ[j][k+1][0][i]*scaleY, -(QLLLd_demY[j][k+1][0][i]-regionalCenterY));
                glNormal3f(QV_normals[j][k][i].x(), QV_normals[j][k][i].y(), QV_normals[j][k][i].z());
                glVertex3d(QLLLd_demX[j][k][0][i]-regionalCenterX, QLLLd_demZ[j][k][0][i]*scaleY, -(QLLLd_demY[j][k][0][i]-regionalCenterY));
            }
            glEnd();
            glFlush();
        }

    }

//    for (int j=0; j<terrainDataCount; j++)//back
//    {
//        for (int k=0; k<QLi_Ynum[j]-1; k++)
//        {
////            glPolygonMode(GL_BACK, GL_LINE);

//            glBegin(GL_TRIANGLE_STRIP);
//            for (int i=0; i<QLi_Xnum[j]; i++)
//            {
////                if (QLLLd_demZ[j][k][0][i]>=elevation0 && QLLLd_demZ[j][k][0][i]<elevation1)
////                    glColor3d(0, (QLLLd_demZ[j][k][0][i]-elevation0)/(elevation1-elevation0)/2, 1/2);
////                if (QLLLd_demZ[j][k][0][i]>=elevation1 && QLLLd_demZ[j][k][0][i]<elevation2)
////                    glColor3d(0, 1/2, (1-(QLLLd_demZ[j][k][0][i]-elevation1)/(elevation2-elevation1))/2);
////                if (QLLLd_demZ[j][k][0][i]>=elevation2 && QLLLd_demZ[j][k][0][i]<elevation3)
////                    glColor3d((QLLLd_demZ[j][k][0][i]-elevation2)/(elevation3-elevation2)/2, 1/2, 0);
////                if (QLLLd_demZ[j][k][0][i]>=elevation3 && QLLLd_demZ[j][k][0][i]<elevation4)
////                    glColor3d(1/2, (1-(QLLLd_demZ[j][k][0][i]-elevation3)/(elevation4-elevation3))/2, 0);
////                if (QLLLd_demZ[j][k][0][i]>=elevation4)
////                    glColor3d(1/2, 0, 0);

//                if (QLLLd_demZ[j][k][0][i]>=elevation0 && QLLLd_demZ[j][k][0][i]<elevation1)
//                    glColor3d(0.2, (QLLLd_demZ[j][k][0][i]-elevation0)/(elevation1-elevation0), 1);
//                if (QLLLd_demZ[j][k][0][i]>=elevation1 && QLLLd_demZ[j][k][0][i]<elevation2)
//                    glColor3d(0.2, 1, 1-(QLLLd_demZ[j][k][0][i]-elevation1)/(elevation2-elevation1));
//                if (QLLLd_demZ[j][k][0][i]>=elevation2 && QLLLd_demZ[j][k][0][i]<elevation3)
//                    glColor3d((QLLLd_demZ[j][k][0][i]-elevation2)/(elevation3-elevation2), 1, 0.2);
//                if (QLLLd_demZ[j][k][0][i]>=elevation3 && QLLLd_demZ[j][k][0][i]<elevation4)
//                    glColor3d(1, 1-(QLLLd_demZ[j][k][0][i]-elevation3)/(elevation4-elevation3), 0.2);
//                if (QLLLd_demZ[j][k][0][i]>=elevation4)
//                    glColor3d(1, 0.2, 0.2);

//                glVertex3d(QLLLd_demX[j][k][0][i]-regionalCenterX, QLLLd_demZ[j][k][0][i]*scaleY, -(QLLLd_demY[j][k][0][i]-regionalCenterY));
//                glNormal3f(QV_normals[j][k+1][i].x(), QV_normals[j][k+1][i].y(), QV_normals[j][k+1][i].z());
//                glVertex3d(QLLLd_demX[j][k+1][0][i]-regionalCenterX, QLLLd_demZ[j][k+1][0][i]*scaleY, -(QLLLd_demY[j][k+1][0][i]-regionalCenterY));
//            }
//            glEnd();
//            glFlush();
//        }

//    }
}

double OglWidget_mf::caculateWellToTerrain(int w)
{
    QList<double> QLd_demY;

    jFile=-1;
    for (int j=0; j<terrainDataCount; j++)//find well at which file
    {
        if (QLsl_welldata[w][0][1].toDouble() >= QLLLd_demX[j][0][0][0]
                && QLsl_welldata[w][0][1].toDouble() <= QLLLd_demX[j][0][0][QLi_Xnum[j]-1]
                && QLsl_welldata[w][0][2].toDouble() >= QLLLd_demY[j][0][0][0]
                && QLsl_welldata[w][0][2].toDouble() <= QLLLd_demY[j][QLi_Ynum[j]-1][0][0])
            jFile=j;
    }
    if (jFile == -1)
        return 0;
    for (int yy=0; yy<QLi_Ynum[jFile]; yy++)//creat a list of Y coordinate
    {
        QLd_demY.append(QLLLd_demY[jFile][yy][0][0]);
    }
    xIndex=binarySearch(QLLLd_demX[jFile][0][0], QLsl_welldata[w][0][1].toDouble(), QLi_Xnum[jFile]);
    yIndex=binarySearch(QLd_demY, QLsl_welldata[w][0][2].toDouble(), QLi_Ynum[jFile]);
    return QLLLd_demZ[jFile][yIndex][0][xIndex];
//    qDebug()<<QLsl_welldata[w][0][1].toDouble()<<QLsl_welldata[w][0][2].toDouble();
//    qDebug()<<QLLLd_demX[jFile][yIndex][0][xIndex]<<QLLLd_demY[jFile][yIndex][0][xIndex];

}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void OglWidget_mf::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;

        update();
    }
}

void OglWidget_mf::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;

        update();
    }
}

void OglWidget_mf::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;

        update();
    }
}

void OglWidget_mf::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();


}

void OglWidget_mf::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXTranslation(-32 * dx);
        setYTranslation(32 * dy);
    }
    lastPos = event->pos();
}

void OglWidget_mf::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    if (numDegrees.y() < 0)  zoomScale = zoomScale/1.1;
    if (numDegrees.y() > 0)  zoomScale = zoomScale*1.1;

    update();
}

void OglWidget_mf::setXTranslation(int distance)
{
    lookAt1=lookAt1-distance;
    lookAt4=lookAt4+distance;

    update();
}

void OglWidget_mf::setYTranslation(int distance)
{
    lookAt2=lookAt2-distance;
    lookAt5=lookAt5+distance;

    update();
}

void OglWidget_mf::computeNormals(int j)
{
    QV_normals[j] = new QVector3D*[QLi_Ynum[j]];
    for(int i = 0; i < QLi_Ynum[j]; i++)
    {
        QV_normals[j][i] = new QVector3D[QLi_Xnum[j]];
    }

    //Compute the rough version of the normals
    QVector3D** normals2 = new QVector3D*[QLi_Ynum[j]];
    for(int i = 0; i < QLi_Ynum[j]; i++) {
        normals2[i] = new QVector3D[QLi_Xnum[j]];
    }
    for(int z = 0; z < QLi_Ynum[j]; z++) {
        for(int x = 0; x < QLi_Xnum[j]; x++) {
            QVector3D sum(0.0f, 0.0f, 0.0f);

            QVector3D out;
            if (z > 0) {
                out = QVector3D(0.0f, QLLLd_demZ[j][z - 1][0][x] - QLLLd_demZ[j][z][0][x], -1.0f);
            }
            QVector3D in;
            if (z < QLi_Ynum[j] - 1) {
                in = QVector3D(0.0f, QLLLd_demZ[j][z + 1][0][x] - QLLLd_demZ[j][z][0][x], 1.0f);
            }
            QVector3D left;
            if (x > 0) {
                left = QVector3D(-1.0f, QLLLd_demZ[j][z][0][x - 1] - QLLLd_demZ[j][z][0][x], 0.0f);
            }
            QVector3D right;
            if (x < QLi_Xnum[j] - 1) {
                right = QVector3D(1.0f, QLLLd_demZ[j][z][0][x + 1] - QLLLd_demZ[j][z][0][x], 0.0f);
            }

            if (x > 0 && z > 0) {
                sum = sum + out.normal(out, left);
//                sum += out.cross(left).normalize();
            }
            if (x > 0 && z < QLi_Ynum[j] - 1) {
                sum = sum + left.normal(left, in);
//                sum += left.cross(in).normalize();
            }
            if (x < QLi_Xnum[j] - 1 && z < QLi_Ynum[j] - 1) {
                sum = sum + in.normal(in, right);
//                sum += in.cross(right).normalize();
            }
            if (x < QLi_Xnum[j] - 1 && z > 0) {
                sum = sum + right.normal(right, out);
//                sum += right.cross(out).normalize();
            }
            normals2[z][x] = sum;
        }
    }

    //Smooth out the normals
    float FALLOUT_RATIO = 0.5;
    for(int z = 0; z < QLi_Ynum[j]-1; z++) {
        for(int x = 0; x < QLi_Xnum[j]; x++) {

            QVector3D sum = normals2[z][x];

            if (x > 0) {
                sum += normals2[z][x - 1] * FALLOUT_RATIO;
            }
            if (x < QLi_Xnum[j] - 1) {
                sum += normals2[z][x + 1] * FALLOUT_RATIO;
            }
            if (z > 0) {
                sum += normals2[z - 1][x] * FALLOUT_RATIO;
            }
            if (z < QLi_Ynum[j] - 1) {
                sum += normals2[z + 1][x] * FALLOUT_RATIO;
            }
            if (sum.length() == 0) {
                sum = QVector3D(0.0f, 1.0f, 0.0f);
            }
            QV_normals[j][z][x] = sum;
        }
    }
    delete normals2;
}

int OglWidget_mf::binarySearch(QList<double> QLd_data, int search, int n)
{
    //binarySearch
    int low,mid,high;
    low=0;
    high=n-1;

    while (low <= high)
    {
        mid=(low + high) / 2;
        if (search >= QLd_data[mid]-20 && search < QLd_data[mid]+20)//grid size
            return mid;
        else if (QLd_data[mid] > search)
            high=mid-1;
        else if (QLd_data[mid] < search)
            low=mid+1;
    }
    return -1;
}
