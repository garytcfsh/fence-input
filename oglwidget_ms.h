#ifndef OGLWIDGET_MS_H
#define OGLWIDGET_MS_H

#include <QMainWindow>
#include <QWidget>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_1>
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


class OglWidget_ms : public QOpenGLWidget, public QOpenGLFunctions_1_1
{
//    Q_OBJECT

public:
    OglWidget_ms(QWidget *parent=0);
    ~OglWidget_ms();

    double *getMostShallow();
    double *getDeepest();
    float getxx();
    float getyy();
    bool getIsNode();
    void drawsection();
    void drawNode();
    void selection(int x,int y);
    void processHits (GLint hits, GLuint selectBuf[512]);
    void drawSelectNode();
    void drawSelectWell();
    void drawSelectQuadri();
    void drawVirtualWell();
    void drawQuadrilateral();
    void drawQuadriLine();
    void setNode();
    void saveWellData();
    void mouseClick(int x,int y);


public slots:
    void slot_paintsection(QStringList sQSL_welldepth_1, QStringList sQSL_welldepth_2);
    void slot_addVirtualWell();
    void slot_addNode();
    void slot_changeNodeDepth(QString Depth);
    void slot_deleteObject();
    void slot_setQuadrilateralOn();
    void slot_setQuadrilateralOff();
    void slot_set3P1LOn();
    void slot_set3P1LOff();
    void slot_saveState(QString QS_quadri_attribute);
    void slot_cancelState();
    void slot_clear();
    void slot_save();
    void slot_reset();


protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    double realtop_1,realbottom_1,deepest_1,mostshallow_1;
    double realtop_2,realbottom_2,deepest_2,mostshallow_2;
    QStringList QSL_welldepth_1,QSL_welldepth_2;
    int ottf,ott,well_nickname;
    QStringList QSL_well_position;
    QList<double*> QLd_quadrilateral_data;
    QList<double> QLd_quadriWorld3D,QLd_quadriOpengl3D;
    QList<QList<double>*> QLLd_node_position;
    GLuint selectBuf[512];
    GLint hits;
    double mostshallow,deepest;
    int layers_1,layers_2;
    int em,quadrilateral_state,threePOneL_state;
    double *node1_X,*node1_Y,*node1_Z
            ,*node2_X,*node2_Y,*node2_Z
            ,*node3_X,*node3_Y,*node3_Z
            ,*node4_X,*node4_Y,*node4_Z;
    double *attribute_R,*attribute_G,*attribute_B
            ,*gravel_R,*gravel_G,*gravel_B
            ,*coarse_R,*coarse_G,*coarse_B
            ,*fine_R,*fine_G,*fine_B
            ,*mud_R,*mud_G,*mud_B
            ,*another1_R,*another1_G,*another1_B
            ,*another2_R,*another2_G,*another2_B;
    double threePOneL_XL,threePOneL_XM,threePOneL_XR,
            threePOneL_YL,threePOneL_YM,threePOneL_YR;
    int tpolXL,tpolXM,tpolXR
            ,tpolYL,tpolYM,tpolYR;
    float xx,yy;//for drawNode()
    bool isNode;//for valuebar

};

#endif // OGLWIDGET_MS_H
