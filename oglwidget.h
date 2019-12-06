#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_1>
#include <QDebug>
#include <QList>
#include <QString>

#include <GL/gl.h>
#include <GL/glu.h>

//#include <GL/glut.h>

class oglwidget : public QOpenGLWidget, public QOpenGLFunctions_1_1
{
public:
    oglwidget(QWidget *parent=0);
    ~oglwidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void drawcube(GLfloat point);

public slots:
    void slot_paintwell(QStringList QSl_welldata, int k, int wellnumber);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event);
    void drawwell();
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);



private:
    int xRot;
    int yRot;
    int zRot;
    int change;
    float zoomScale;
    double realtop,realbottom,deepest,mostshallow;
    QPoint lastPos;
    QColor qtGreen;
    QColor qtPurple;
    QStringList ogl_QSL_welldata;
    int ogl_k;
    int ogl_wellnumber;

};

#endif // OGLWIDGET_H
