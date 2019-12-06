#include "oglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>



oglwidget::oglwidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    ogl_k=0;
    ogl_wellnumber=0;
    zoomScale=1.0;
//    glGetFloatv(GL_POINT_SIZE_RANGE);


}

oglwidget::~oglwidget()
{

}

void oglwidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    if (numDegrees.y() < 0)  zoomScale = zoomScale/1.1;
    if (numDegrees.y() > 0)  zoomScale = zoomScale*1.1;
    update();
}

QSize oglwidget::minimumSizeHint() const
{
    return QSize(640, 480);
}

QSize oglwidget::sizeHint() const
{
    return QSize(640, 480);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void oglwidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
//        emit xRotationChanged(angle);
        update();
    }
}

void oglwidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
//        emit yRotationChanged(angle);
        update();
    }
}

void oglwidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
 //       emit zRotationChanged(angle);
        update();
    }
}

void oglwidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();

}

void oglwidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}

void oglwidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);//the color between two point which color are different
    glEnable(GL_MULTISAMPLE);
}

void oglwidget::paintGL()
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
//    gluLookAt (0, 0, 20, 0, 0, 0, 0.0, 1.0, 0.0);
//    glScaled(zoomScale, zoomScale, zoomScale);//change size of object, not vision pull away
//    glTranslatef(0.0, 0.0, -10);//moving model

    drawwell();
    glFlush();
}

void oglwidget::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);//adjust width and height for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, -1, 35.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, -1, 35.0);
#endif

}

void oglwidget::drawwell()
{
    int i=0,iii=0;
    double ScaleX=0.5;
    double ScaleY=1;
    double ScaleZ=0.1;
    realtop=0;
    realbottom=0;
    deepest=0;
    mostshallow=0;

    if (ogl_k != 0)
    {
        while (ogl_QSL_welldata[(ogl_k+1)*3-2-3*iii] == "")
        {
            iii++;
        }
    }
    for (int ii=0;ii<ogl_k-iii;ii++)
    {
        if (ogl_QSL_welldata[5+3*ii]=="attribute")
            glColor3d(0.9, 0.9, 0.9);
        if (ogl_QSL_welldata[5+3*ii]=="礫石")
            glColor3d(0.274509804, 0.509803922, 0.705882353);
        if (ogl_QSL_welldata[5+3*ii]=="粗沙")
            glColor3d(0, 1, 0);
        if (ogl_QSL_welldata[5+3*ii]=="細沙")
            glColor3d(1, 1, 0);
        if (ogl_QSL_welldata[5+3*ii]=="粉沙、泥")
            glColor3d(1, 0.647059, 0);
        if (ogl_QSL_welldata[5+3*ii]=="其他1")
            glColor3d(1, 0, 0);
        if (ogl_QSL_welldata[5+3*ii]=="其他2")
            glColor3d(0.5, 0, 0.8);

        mostshallow=ogl_QSL_welldata[3].toDouble();
        deepest=ogl_QSL_welldata[(ogl_k+1)*3-2-3*iii].toDouble();
        realtop=0.5-1*(ogl_QSL_welldata[3+3*i].toDouble())/(deepest-mostshallow);
        realbottom=0.5-1*(ogl_QSL_welldata[4+3*i].toDouble())/(deepest-mostshallow);

        glBegin(GL_POLYGON);
        glVertex3d( -1*ScaleX, realtop*ScaleY, 0*ScaleZ);
        glVertex3d( -1*ScaleX, realbottom*ScaleY, 0*ScaleZ);
        glVertex3d( 1*ScaleX, realbottom*ScaleY, 0*ScaleZ);
        glVertex3d( 1*ScaleX, realtop*ScaleY, 0*ScaleZ);
        glEnd();
        glFlush( );
        i++;
    }
}

void oglwidget::slot_paintwell(QStringList QSL_welldata,int k,int wellnumber)
{

    ogl_QSL_welldata = QSL_welldata;
    ogl_k = k;
    ogl_wellnumber = wellnumber;

    update();
}



void oglwidget::drawcube(GLfloat point)
{
    glPolygonMode (GL_BACK, GL_LINE);
//    glMatrixMode (GL_MODELVIEW);
//    glLoadIdentity ();


    glColor3f (1.0, 1.0, 0.0);//前 黃
    glBegin (GL_POLYGON);
    glVertex3f (point, point, point);
    glVertex3f (-point, point, point);
    glVertex3f (-point, -point, point);
    glVertex3f (point, -point, point);
    glEnd ();
    glColor3f(1.0, 0.0, 0.0);//後 紅
    glBegin (GL_POLYGON);
    glVertex3f (point, point, -point);
    glVertex3f (point, -point, -point);
    glVertex3f (-point, -point, -point);
    glVertex3f (-point, point, -point);
    glEnd ();
    glColor3f(0.0, 1.0, 0.0);//左 綠
    glBegin (GL_POLYGON);
    glVertex3f (-point, point, point);
    glVertex3f (-point, point, -point);
    glVertex3f (-point, -point, -point);
    glVertex3f (-point, -point, point);
    glEnd ();
    glColor3f(0.0, 0.0, 1.0);//右 藍
    glBegin (GL_POLYGON);
    glVertex3f (point, point, point);
    glVertex3f (point, -point, point);
    glVertex3f (point, -point, -point);
    glVertex3f (point, point, -point);
    glEnd ();
    glColor3f(1.0, 0.0, 1.0); //上 紫紅
    glBegin (GL_POLYGON);
    glVertex3f (point, point, point);
    glVertex3f (point, point, -point);
    glVertex3f (-point, point, -point);
    glVertex3f (-point, point, point);
    glEnd ();
    glColor3f(1.0, 1.0, 1.0);//下 白
    glBegin (GL_POLYGON);
    glVertex3f (point, -point, point);
    glVertex3f (-point, -point, point);
    glVertex3f (-point, -point, -point);
    glVertex3f (point, -point, -point);
    glEnd ();
}
