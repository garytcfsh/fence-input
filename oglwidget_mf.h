#ifndef OGLWIDGET_MF_H
#define OGLWIDGET_MF_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_1>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


class OglWidget_mf : public QOpenGLWidget, public QOpenGLFunctions_1_1
{
public:
    OglWidget_mf(QWidget *parent=0);
    ~OglWidget_mf();

    void drawSection();
    void drawcube(GLfloat point);
    void drawcubeForPerspective(GLfloat point);
    void drawCoordinate(float length);
    void drawWell();
    void drawSurface(int xLength, int zLength, int size);
    void drawTerrain();
    void computeNormals(int j);
    double caculateWellToTerrain(int w);
    int binarySearch(QList<double> QLd_data, int search, int n);


public slots:
    void slot_addSection();
    void slot_addWell(QString QS_topPath);
    void slot_addTerrain(QStringList QSL_puliPath);


protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void wheelEvent(QWheelEvent *event);
    void setXTranslation(int distance);
    void setYTranslation(int distance);


private:
    QList<QStringList*> QLsl_section_data,QLsl_welldata;
    QList<double*> QLd_findxMAX,QLd_findxmin,QLd_findzMAX,QLd_findzmin;
    double xMAX,xmin,zMAX,zmin;
    int section_number;
    int state;
    double centralX,centralZ;
    int xRot,yRot,zRot;
    int lookAt1,lookAt2,lookAt3,lookAt4,lookAt5,lookAt6;
    QPoint lastPos;
    float zoomScale;
    double *attribute_R,*attribute_G,*attribute_B
            ,*gravel_R,*gravel_G,*gravel_B
            ,*coarse_R,*coarse_G,*coarse_B
            ,*fine_R,*fine_G,*fine_B
            ,*mud_R,*mud_G,*mud_B
            ,*another1_R,*another1_G,*another1_B
            ,*another2_R,*another2_G,*another2_B;
    double regionalCenterX,regionalCenterY;
    float w,h;
    GLUquadric * quadratic;
    QList<int> QLi_Xnum,QLi_Ynum;//the point number of large sampling to avoid too many point let GPU GG
    int Xnum,Ynum;//
    QList<QList<QList<double>*>> QLLLd_demX,QLLLd_demY,QLLLd_demZ;
    QList<QList<double>*> QLLd_demX,QLLd_demY,QLLd_demZ;
    QVector3D*** QV_normals;
    int terrainDataCount;
    int jFile,xIndex,yIndex;
};

#endif // OGLWIDGET_MF_H
