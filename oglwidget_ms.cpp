#include "oglwidget_ms.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QtOpenGL>


OglWidget_ms::OglWidget_ms(QWidget *parent)
    : QOpenGLWidget(parent)
{    

    isNode=false;
    em=0;
    well_nickname=-1;
    quadrilateral_state=0;
    threePOneL_state=0;
    ottf=0;
    ott=0;
    attribute_R=new double(0.9); attribute_G=new double(0.9); attribute_B=new double(0.9);
    gravel_R=new double(0.274509804); gravel_G=new double(0.509803922); gravel_B=new double(0.705882353);
    coarse_R=new double(0); coarse_G=new double(1); coarse_B=new double(0);
    fine_R=new double(1); fine_G=new double(1); fine_B=new double(0);
    mud_R=new double(1); mud_G=new double(0.647059); mud_B=new double(0);
    another1_R=new double(1); another1_G=new double(0); another1_B=new double(0);
    another2_R=new double(0.5); another2_G=new double(0); another2_B=new double(0.8);
    node1_Z=new double(0); node2_Z=new double(0); node3_Z=new double(0); node4_Z=new double(0);

    //set reference for multisample to antianilising
    QSurfaceFormat format;
    format.setSamples(10);
    this->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
}

OglWidget_ms::~OglWidget_ms()
{

}

double *OglWidget_ms::getMostShallow()
{
    return &mostshallow;
}

double *OglWidget_ms::getDeepest()
{
    return &deepest;
}

float OglWidget_ms::getxx()
{
    return xx;
}

float OglWidget_ms::getyy()
{
    return yy;
}

bool OglWidget_ms::getIsNode()
{
    return isNode;
}

void OglWidget_ms::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
    glEnable(GL_MULTISAMPLE);
}

void OglWidget_ms::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (em==1)
    {
        drawsection();
        drawQuadrilateral();
        drawVirtualWell();
        drawNode();
        drawQuadriLine();
    }
}

void OglWidget_ms::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, -1, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, -1, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void OglWidget_ms::mouseClick(int x, int y)
{
    isNode=false;
        update();
    selection(x,y);
    drawNode();//update
}
//void OglWidget_ms::mousePressEvent(QMouseEvent *event)
//{
//    int x = event->x();
//    int y = event->y();
//qDebug()<<x;
//qDebug()<<y;
//    selection(x,y);
//}

void OglWidget_ms::keyPressEvent(QKeyEvent *event)
{
    if (quadrilateral_state == 0 && selectBuf[0] == 2)
    {
        if (hits != 0 && selectBuf[4] == 0 && selectBuf[3] >1)//move virtual well
        {
            if (event->key() == Qt::Key_Right)
                QLLd_node_position[selectBuf[3]][0][0] = QLLd_node_position[selectBuf[3]][0][0] + 0.1;
            if (event->key() == Qt::Key_Left)
                QLLd_node_position[selectBuf[3]][0][0] = QLLd_node_position[selectBuf[3]][0][0] - 0.1;

            if (QLLd_node_position[selectBuf[3]][0][0] > 0.4)
                QLLd_node_position[selectBuf[3]][0][0] = 0.4;
            if (QLLd_node_position[selectBuf[3]][0][0] < -0.4)
                QLLd_node_position[selectBuf[3]][0][0] = -0.4;

            update();
        }
        if (hits != 0 && selectBuf[4] != 0)//move node
        {
            if ( !((selectBuf[3] == 0 && selectBuf[4] <= layers_1+1)
                    || (selectBuf[3] == 1 && selectBuf[4] <= layers_2+1)) )//avoid move nodes of real well
            {
                if (event->key() == Qt::Key_Up)
                    QLLd_node_position[selectBuf[3]][0][selectBuf[4]]
                            = QLLd_node_position[selectBuf[3]][0][selectBuf[4]] + 0.05;
                if (event->key() == Qt::Key_Down)
                    QLLd_node_position[selectBuf[3]][0][selectBuf[4]]
                            = QLLd_node_position[selectBuf[3]][0][selectBuf[4]] - 0.05;

                if (QLLd_node_position[selectBuf[3]][0][selectBuf[4]] > 0.4)
                    QLLd_node_position[selectBuf[3]][0][selectBuf[4]] = 0.4;
                if (QLLd_node_position[selectBuf[3]][0][selectBuf[4]] < -0.4)
                    QLLd_node_position[selectBuf[3]][0][selectBuf[4]] = -0.4;
            }

            update();
        }
        if (event->key() == Qt::Key_Delete)
        {
            slot_deleteObject();

            update();
        }
    }
}

void OglWidget_ms::slot_changeNodeDepth(QString Depth)
{
    if (hits != 0 && selectBuf[4] != 0)
    {
        double depth,y;
        depth = Depth.toDouble();
        if (depth < 0)
            depth = -depth;
        y = 0.4 - 0.8*(mostshallow-depth)/(mostshallow-deepest);
        QLLd_node_position[selectBuf[3]][0][selectBuf[4]] = y;

        update();
    }
}

void OglWidget_ms::slot_paintsection(QStringList sQSL_welldepth_1, QStringList sQSL_welldepth_2)
{
    QSL_welldepth_1=sQSL_welldepth_1;
    QSL_welldepth_2=sQSL_welldepth_2;
    em=1;
    setNode();

    update();
}

void OglWidget_ms::setNode()
{
    int i=0;
    double ScaleY=1;
    realtop_1=0;
    realbottom_1=0;
    mostshallow_1=0;
    deepest_1=0;
    realtop_2=0;
    realbottom_2=0;
    mostshallow_2=0;
    deepest_2=0;

    layers_1=QSL_welldepth_1.count()/3-1;
    layers_2=QSL_welldepth_2.count()/3-1;


    if (QSL_welldepth_1[3].toDouble()>QSL_welldepth_2[3].toDouble())
        mostshallow=QSL_welldepth_2[3].toDouble();
    else
        mostshallow=QSL_welldepth_1[3].toDouble();

    if (QSL_welldepth_1[QSL_welldepth_1.count()-2].toDouble() > QSL_welldepth_2[QSL_welldepth_2.count()-2].toDouble())
        deepest=QSL_welldepth_1[QSL_welldepth_1.count()-2].toDouble();
    else
        deepest=QSL_welldepth_2[QSL_welldepth_2.count()-2].toDouble();


    QLLd_node_position.append(new QList<double>);
    well_nickname = well_nickname + 1;
    QLLd_node_position[well_nickname]->append(-0.45);
    for (int ii=0;ii<layers_1;ii++)
    {
        realtop_1=0.4-0.8*(QSL_welldepth_1[3+3*i].toDouble())/(deepest-mostshallow);
        realbottom_1=0.4-0.8*(QSL_welldepth_1[4+3*i].toDouble())/(deepest-mostshallow);
        QLLd_node_position[0]->append(realtop_1*ScaleY);
        i++;
    }
    QLLd_node_position[well_nickname]->append(realbottom_1*ScaleY);

    i=0;
    QLLd_node_position.append(new QList<double>);
    well_nickname = well_nickname + 1;
    QLLd_node_position[well_nickname]->append(0.45);//X
    for (int ii=0;ii<layers_2;ii++)
    {
        realtop_2=0.4-0.8*(QSL_welldepth_2[3+3*i].toDouble())/(deepest-mostshallow);
        realbottom_2=0.4-0.8*(QSL_welldepth_2[4+3*i].toDouble())/(deepest-mostshallow);
        QLLd_node_position[well_nickname]->append(realtop_2*ScaleY);//Y
        i++;
    }
    QLLd_node_position[well_nickname]->append(realbottom_2*ScaleY);
}

void OglWidget_ms::drawsection()
{
    int i=0;
    double ScaleX=0.1;
    double ScaleY=1;
    double ScaleZ=0.1;
    realtop_1=0;
    realbottom_1=0;
    mostshallow_1=0;
    deepest_1=0;
    realtop_2=0;
    realbottom_2=0;
    mostshallow_2=0;
    deepest_2=0;

    layers_1=QSL_welldepth_1.count()/3-1;
    layers_2=QSL_welldepth_2.count()/3-1;


    if (QSL_welldepth_1[3].toDouble()>QSL_welldepth_2[3].toDouble())
        mostshallow=QSL_welldepth_2[3].toDouble();
    else
        mostshallow=QSL_welldepth_1[3].toDouble();

    if (QSL_welldepth_1[QSL_welldepth_1.count()-2].toDouble() > QSL_welldepth_2[QSL_welldepth_2.count()-2].toDouble())
        deepest=QSL_welldepth_1[QSL_welldepth_1.count()-2].toDouble();
    else
        deepest=QSL_welldepth_2[QSL_welldepth_2.count()-2].toDouble();

    for (int ii=0;ii<layers_1;ii++)
    {
        if (QSL_welldepth_1[5+3*ii]=="attribute")
            glColor3d(*attribute_R, *attribute_G, *attribute_B);
        if (QSL_welldepth_1[5+3*ii]=="礫石")
            glColor3d(*gravel_R, *gravel_G, *gravel_B);
        if (QSL_welldepth_1[5+3*ii]=="粗沙")
            glColor3d(*coarse_R, *coarse_G, *coarse_B);
        if (QSL_welldepth_1[5+3*ii]=="細沙")
            glColor3d(*fine_R, *fine_G, *fine_B);
        if (QSL_welldepth_1[5+3*ii]=="粉沙、泥")
            glColor3d(*mud_R, *mud_G, *mud_B);
        if (QSL_welldepth_1[5+3*ii]=="其他1")
            glColor3d(*another1_R, *another1_G, *another1_B);
        if (QSL_welldepth_1[5+3*ii]=="其他2")
            glColor3d(*another2_R, *another2_G, *another2_B);

        realtop_1=0.4-0.8*(QSL_welldepth_1[3+3*i].toDouble())/(deepest-mostshallow);
        realbottom_1=0.4-0.8*(QSL_welldepth_1[4+3*i].toDouble())/(deepest-mostshallow);

        glBegin(GL_POLYGON);//if you want to change x(-0.5 -0.45), remeber to change save function also.
        glVertex3d(-0.5, realtop_1*ScaleY, 0*ScaleZ);
        glVertex3d(-0.5, realbottom_1*ScaleY, 0*ScaleZ);
        glVertex3d(-0.45, realbottom_1*ScaleY, 0*ScaleZ);
        glVertex3d(-0.45, realtop_1*ScaleY, 0*ScaleZ);
        glEnd();
        glFlush( );
        i++;
    }

    i=0;
    qDebug()<<layers_2;
    for (int ii=0;ii<layers_2;ii++)
    {
        qDebug()<<ii;
        if (QSL_welldepth_2[5+3*ii]=="attribute")
            glColor3d(*attribute_R, *attribute_G, *attribute_B);
        if (QSL_welldepth_2[5+3*ii]=="礫石")
            glColor3d(*gravel_R, *gravel_G, *gravel_B);
        if (QSL_welldepth_2[5+3*ii]=="粗沙")
            glColor3d(*coarse_R, *coarse_G, *coarse_B);
        if (QSL_welldepth_2[5+3*ii]=="細沙")
            glColor3d(*fine_R, *fine_G, *fine_B);
        if (QSL_welldepth_2[5+3*ii]=="粉沙、泥")
            glColor3d(*mud_R, *mud_G, *mud_B);
        if (QSL_welldepth_2[5+3*ii]=="其他1")
            glColor3d(*another1_R, *another1_G, *another1_B);
        if (QSL_welldepth_2[5+3*ii]=="其他2")
            glColor3d(*another2_R, *another2_G, *another2_B);

        realtop_2=0.4-0.8*(QSL_welldepth_2[3+3*i].toDouble())/(deepest-mostshallow);
        realbottom_2=0.4-0.8*(QSL_welldepth_2[4+3*i].toDouble())/(deepest-mostshallow);

        glBegin(GL_POLYGON);
        glVertex3d(0.45, realtop_2*ScaleY, 0*ScaleZ);
        glVertex3d(0.45, realbottom_2*ScaleY, 0*ScaleZ);
        glVertex3d(0.5, realbottom_2*ScaleY, 0*ScaleZ);
        glVertex3d(0.5, realtop_2*ScaleY, 0*ScaleZ);
        glEnd();
        glFlush( );
        i++;
    }
}

void OglWidget_ms::drawVirtualWell()
{
    glColor3d(0.7, 0.7, 0.7);
    for (int i=0; i<QLLd_node_position.count(); i++)
    {
        glBegin(GL_LINES);
        glVertex3f(QLLd_node_position[i][0][0], 0.5, 0);
        glVertex3f(QLLd_node_position[i][0][0], -0.5, 0);
        glEnd();
        glFlush();
    }

    if (hits != 0 && selectBuf[4] == 0 && selectBuf[0] == 2)//draw selected well by red
    {
        glColor3d(0.9, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(QLLd_node_position[selectBuf[3]][0][0], 0.5, 0);
        glVertex3f(QLLd_node_position[selectBuf[3]][0][0], -0.5, 0);
        glEnd();
        glFlush();
    }
}

void OglWidget_ms::drawNode()
{
    int tri;
    float Pi,radius;
    tri=20;
    Pi=3.1415926;
    radius=0.01;
    int w=width();
    int h=height();

    for (int W=0; W<QLLd_node_position.count(); W++)//draw common nodes
    {
        xx=QLLd_node_position[W][0][0];
        for(int N=1; N<QLLd_node_position[W]->count(); N++)
        {
            yy=QLLd_node_position[W][0][N];
            glPushMatrix();
            glTranslatef(xx,yy,0);//move circle
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
    }

    if (hits != 0 && selectBuf[4] != 0 && quadrilateral_state == 0 && selectBuf[0] == 2)//draw selected node by red
    {
        isNode=true;
        xx=QLLd_node_position[selectBuf[3]][0][0];
        yy=QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
        glPushMatrix();
        glTranslatef(xx, yy, 0);
        glColor3f(1, 0.0, 0.0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for ( int i=0; i<=tri; i++)
        {
            glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
        }
        glEnd();
        glFlush();
        glPopMatrix();
    }

    if (quadrilateral_state == 1 && selectBuf[0] == 2)//draw red nods for quadrilateral
    {
        if (ottf == 1)
        {
            xx=*node1_X;
            yy=*node1_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
        else if (ottf == 2)
        {
            xx=*node1_X;
            yy=*node1_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node2_X;
            yy=*node2_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
        else if (ottf == 3)
        {
            xx=*node1_X;
            yy=*node1_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node2_X;
            yy=*node2_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node3_X;
            yy=*node3_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
        else if (ottf == 4)
        {
            xx=*node1_X;
            yy=*node1_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node2_X;
            yy=*node2_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node3_X;
            yy=*node3_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=*node4_X;
            yy=*node4_Y;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
    }

    if (threePOneL_state == 1 && selectBuf[0] == 2)//draw red nodes for three points one line
    {
        if (ott == 1)
        {
            xx=threePOneL_XL;
            yy=threePOneL_YL;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
        else if (ott == 2)
        {
            xx=threePOneL_XL;
            yy=threePOneL_YL;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=threePOneL_XM;
            yy=threePOneL_YM;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
        else if (ott == 3)
        {
            xx=threePOneL_XL;
            yy=threePOneL_YL;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=threePOneL_XM;
            yy=threePOneL_YM;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();

            xx=threePOneL_XR;
            yy=threePOneL_YR;
            glPushMatrix();
            glTranslatef(xx, yy, 0);
            glColor3f(1, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
        }
    }
}

void OglWidget_ms::drawQuadrilateral()
{
    for (int i=0; i<(QLd_quadrilateral_data.count()); i=i+15)
    {
        glColor3d(*QLd_quadrilateral_data[i], *QLd_quadrilateral_data[i+1], *QLd_quadrilateral_data[i+2]);
        glBegin(GL_POLYGON);
        for (int ii=i+3; ii<(i+15); ii=ii+3)
        {
            glVertex3d(*QLd_quadrilateral_data[ii], *QLd_quadrilateral_data[ii+1], *QLd_quadrilateral_data[ii+2]);
        }
        glEnd();
        glFlush();
    }
}

void OglWidget_ms::drawQuadriLine()
{
    if (hits != 0 && selectBuf[0] == 3)
    {
        glPushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3d(0.8, 0, 0);
        glBegin(GL_POLYGON);
        for (int ii=selectBuf[5]*15+3; ii<(selectBuf[5]*15+15); ii=ii+3)
        {
            glVertex3d(*QLd_quadrilateral_data[ii], *QLd_quadrilateral_data[ii+1], *QLd_quadrilateral_data[ii+2]);
        }
        glEnd();
        glFlush();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPopMatrix();
    }
}

void OglWidget_ms::drawSelectWell()
{
    for (int i=0; i<QLLd_node_position.count(); i++)
    {
        glLoadName(i);
        glPushName(0);
        glBegin(GL_LINES);
        glVertex3f(QLLd_node_position[i][0][0], 0.5, 0);
        glVertex3f(QLLd_node_position[i][0][0], -0.5, 0);
        glEnd();
        glFlush();
        glPopName();
    }
}

void OglWidget_ms::drawSelectNode()
{
    int tri;
    float Pi,radius,x,y;
    tri=20;
    Pi=3.1415926;
    radius=0.01;
    int w=width();
    int h=height();

    for (int W=0; W<QLLd_node_position.count(); W++)
    {
        glLoadName(W);
        x=QLLd_node_position[W][0][0];
        for(int N=1; N<QLLd_node_position[W]->count(); N++)
        {
            glPushName(N);
            y=QLLd_node_position[W][0][N];
            glPushMatrix();
            glTranslatef(x,y,0);//move circle
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0,0);
            for ( int i=0; i<=tri; i++)
            {
                glVertex2f(radius*cos(i*2*Pi/tri)*h/w, radius*sin(i*2*Pi/tri));
            }
            glEnd();
            glFlush();
            glPopMatrix();
            glPopName();
        }
    }
}

void OglWidget_ms::drawSelectQuadri()
{
    int k=0;
    glLoadName(0);
    glPushName(0);
    glPushName(0);
    for (int i=0; i<(QLd_quadrilateral_data.count()); i=i+15)
    {
        glLoadName(k);//0 0 0 for quadrilateral
        glBegin(GL_POLYGON);
        for (int ii=i+3; ii<(i+15); ii=ii+3)
        {
            glVertex3d(*QLd_quadrilateral_data[ii], *QLd_quadrilateral_data[ii+1], *QLd_quadrilateral_data[ii+2]);
        }
        glEnd();
        glFlush();
        k++;
    }
    glPopName();
    glPopName();

}

void OglWidget_ms::selection(int x,int y)
{
    GLint viewport[4];
    for (int i=0; i<512; i++)
        selectBuf[i] = 0;

    glGetIntegerv(GL_VIEWPORT, viewport);
    viewport[2]=size().width();
    viewport[3]=size().height();
    glSelectBuffer(512, selectBuf);
    (void) glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix( x, viewport[3]-y, 5.0, 5.0, viewport);
    glOrtho(-0.5, +0.5, -0.5, +0.5, -1, 15.0);
    drawSelectNode();
    drawSelectWell();
    drawSelectQuadri();
    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);//return the number of hit action
    qDebug()<<"hits"<<hits;
    processHits(hits, selectBuf);
}

void OglWidget_ms::processHits (GLint hits, GLuint selectBuf[512])
{
    if (quadrilateral_state == 1)
    {
        if (hits != 0 && selectBuf[4] != 0)
        {
            ottf = ottf +1;
            if (ottf == 1)
            {
                node1_X = &QLLd_node_position[selectBuf[3]][0][0];
                node1_Y = &QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
            }
            else if (ottf == 2)
            {
                node2_X = &QLLd_node_position[selectBuf[3]][0][0];
                node2_Y = &QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
            }
            else if (ottf == 3)
            {
                node3_X = &QLLd_node_position[selectBuf[3]][0][0];
                node3_Y = &QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
            }
            else if (ottf == 4)
            {
                node4_X = &QLLd_node_position[selectBuf[3]][0][0];
                node4_Y = &QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
            }
            else if (ottf > 4)
            {
                QMessageBox::information(this,"Message","Select more than four nodes, please click \"cancel\"");
            }
        }
    }
    else if (threePOneL_state == 1)
    {
        if (hits != 0 && selectBuf[4] != 0)
        {
            ott = ott +1;
            if (ott == 1)
            {
                threePOneL_XL=QLLd_node_position[selectBuf[3]][0][0];
                threePOneL_YL=QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
                tpolXL=selectBuf[3];
                tpolYL=selectBuf[4];
            }
            else if (ott == 2)
            {
                threePOneL_XM=QLLd_node_position[selectBuf[3]][0][0];
                threePOneL_YM=QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
                tpolXM=selectBuf[3];
                tpolYM=selectBuf[4];
            }
            else if (ott == 3)
            {
                threePOneL_XR=QLLd_node_position[selectBuf[3]][0][0];
                threePOneL_YR=QLLd_node_position[selectBuf[3]][0][selectBuf[4]];
                tpolXR=selectBuf[3];
                tpolYR=selectBuf[4];
            }
            else if (ott > 3)
            {
                QMessageBox::information(this,"Message","Select more than three nodes, please click \"cancel\"");
            }
        }
    }

    update();
    for (int i=0; i<6; i++)
        qDebug()<<selectBuf[i];
}

void OglWidget_ms::slot_setQuadrilateralOn()
{
    quadrilateral_state=1;
    hits=0;//let red point disappear

    update();
}

void OglWidget_ms::slot_setQuadrilateralOff()
{
    quadrilateral_state=0;
    hits=0;//let red point disappear

    update();
}

void OglWidget_ms::slot_set3P1LOn()
{
    threePOneL_state=1;
    hits=0;//let red point disappear

    update();
}

void OglWidget_ms::slot_set3P1LOff()
{
    threePOneL_state=0;
    hits=0;//let red point disappear

    update();
}

void OglWidget_ms::slot_saveState(QString QS_quadri_attribute)
{
    if (quadrilateral_state == 1)//save select points to a list for draw quadrilateral
    {
        if (ottf == 4)
        {
            if (QS_quadri_attribute == "attribute")
            {
                QLd_quadrilateral_data.append(attribute_R);
                QLd_quadrilateral_data.append(attribute_G);
                QLd_quadrilateral_data.append(attribute_B);
            }
            if (QS_quadri_attribute == "礫石")
            {
                QLd_quadrilateral_data.append(gravel_R);
                QLd_quadrilateral_data.append(gravel_G);
                QLd_quadrilateral_data.append(gravel_B);
            }
            if (QS_quadri_attribute == "粗沙")
            {
                QLd_quadrilateral_data.append(coarse_R);
                QLd_quadrilateral_data.append(coarse_G);
                QLd_quadrilateral_data.append(coarse_B);
            }
            if (QS_quadri_attribute == "細沙")
            {
                QLd_quadrilateral_data.append(fine_R);
                QLd_quadrilateral_data.append(fine_G);
                QLd_quadrilateral_data.append(fine_B);
            }
            if (QS_quadri_attribute == "粉沙、泥")
            {
                QLd_quadrilateral_data.append(mud_R);
                QLd_quadrilateral_data.append(mud_G);
                QLd_quadrilateral_data.append(mud_B);
            }
            if (QS_quadri_attribute == "其他1")
            {
                QLd_quadrilateral_data.append(another1_R);
                QLd_quadrilateral_data.append(another1_G);
                QLd_quadrilateral_data.append(another1_B);
            }
            if (QS_quadri_attribute == "其他2")
            {
                QLd_quadrilateral_data.append(another2_R);
                QLd_quadrilateral_data.append(another2_G);
                QLd_quadrilateral_data.append(another2_B);
            }
            QLd_quadrilateral_data.append(node1_X);
            QLd_quadrilateral_data.append(node1_Y);
            QLd_quadrilateral_data.append(node1_Z);
            QLd_quadrilateral_data.append(node2_X);
            QLd_quadrilateral_data.append(node2_Y);
            QLd_quadrilateral_data.append(node2_Z);
            QLd_quadrilateral_data.append(node3_X);
            QLd_quadrilateral_data.append(node3_Y);
            QLd_quadrilateral_data.append(node3_Z);
            QLd_quadrilateral_data.append(node4_X);
            QLd_quadrilateral_data.append(node4_Y);
            QLd_quadrilateral_data.append(node4_Z);

            hits=0;
        }
        else
        {
            QMessageBox::information(this,"Message","please select 4 nodes");
        }
        ottf=0;
    }
    else if (threePOneL_state == 1)//let three point draw at line
    {
        double sortingX,sortingY;
        int SortingX,SortingY;
        if (ott == 3)
        {
            if (threePOneL_XL > threePOneL_XM)
            {
                sortingX=threePOneL_XL;
                threePOneL_XL=threePOneL_XM;
                threePOneL_XM=sortingX;
                sortingY=threePOneL_YL;
                threePOneL_YL=threePOneL_YM;
                threePOneL_YM=sortingY;
                SortingX=tpolXL;
                tpolXL=tpolXM;
                tpolXM=SortingX;
                SortingY=tpolYL;
                tpolYL=tpolYM;
                tpolYM=SortingY;
            }
            if (threePOneL_XL > threePOneL_XR)
            {
                sortingX=threePOneL_XL;
                threePOneL_XL=threePOneL_XR;
                threePOneL_XR=sortingX;
                sortingY=threePOneL_YL;
                threePOneL_YL=threePOneL_YR;
                threePOneL_YR=sortingY;
                SortingX=tpolXL;
                tpolXL=tpolXR;
                tpolXR=SortingX;
                SortingY=tpolYL;
                tpolYL=tpolYR;
                tpolYR=SortingY;
            }
            if (threePOneL_XM > threePOneL_XR)
            {
                sortingX=threePOneL_XM;
                threePOneL_XM=threePOneL_XR;
                threePOneL_XR=sortingX;
                sortingY=threePOneL_YM;
                threePOneL_YM=threePOneL_YR;
                threePOneL_YR=sortingY;
                SortingX=tpolXM;
                tpolXM=tpolXR;
                tpolXR=SortingX;
                SortingY=tpolYM;
                tpolYM=tpolYR;
                tpolYR=SortingY;
            }
            threePOneL_YM=threePOneL_YL
                    +(threePOneL_XM-threePOneL_XL)*(threePOneL_YR-threePOneL_YL)/(threePOneL_XR-threePOneL_XL);
            QLLd_node_position[tpolXM][0][tpolYM]=threePOneL_YM;
            selectBuf[3]=tpolXM;
            selectBuf[4]=tpolYM;
        }
        else
        {
            QMessageBox::information(this,"Message","please select 3 nodes");
        }
        ott=0;
    }

    update();
}

void OglWidget_ms::slot_cancelState()
{
    if (quadrilateral_state == 1)
    {
        ottf=0;
    }
    else if (threePOneL_state == 1)
    {
        ott=0;
    }
    hits=0;

    update();
}

void OglWidget_ms::slot_addVirtualWell()
{
    QLLd_node_position.append(new QList<double>);
    well_nickname = well_nickname + 1;
    QLLd_node_position[well_nickname]->append(0.0);

    update();
}

void OglWidget_ms::slot_addNode()
{
    if (hits != 0 && selectBuf[4] == 0)
    {
        QLLd_node_position[selectBuf[3]]->append(0.45);

        update();
    }
}

void OglWidget_ms::slot_deleteObject()
{
    if (hits != 0 && selectBuf[4] != 0)
    {
        if ( !((selectBuf[3] == 0 && selectBuf[4] <= layers_1+1)
              || (selectBuf[3] == 1 && selectBuf[4] <= layers_2+1)) )
        {
            for (int N=selectBuf[4]; N<QLLd_node_position[selectBuf[3]]->count()-1; N++)
            {
                QLLd_node_position[selectBuf[3]][0][N]
                        = QLLd_node_position[selectBuf[3]][0][N+1];
            }
            QLLd_node_position[selectBuf[3]]->removeLast();
            selectBuf[4] = selectBuf[4] -1;
            hits=0;

            update();
        }
    }

    if (hits != 0 && selectBuf[4] == 0 && selectBuf[3] >1)
    {
        QLLd_node_position.removeAt(selectBuf[3]);
        well_nickname = well_nickname -1;
        selectBuf[3] = selectBuf[3] -1;
        hits=0;

        update();
    }

    if (hits != 0 && selectBuf[0] == 3)
    {
        for (int i=selectBuf[5]*15; i<(selectBuf[5]*15+15); i++)
        {
            QLd_quadrilateral_data.removeAt(selectBuf[5]*15);
        }
        hits=0;

        update();
    }
}

void OglWidget_ms::slot_reset()
{
    QLLd_node_position.clear();
    QLd_quadrilateral_data.clear();
    hits=0;
    well_nickname=-1;
    setNode();

    update();
}

void OglWidget_ms::slot_clear()
{
    QLLd_node_position.clear();
    QLd_quadrilateral_data.clear();
    QSL_welldepth_1.clear();
    QSL_welldepth_2.clear();
    em=0;
    hits=0;
    well_nickname=-1;

    update();
}

void OglWidget_ms::slot_save()
{
    for (int i=0; i<QLd_quadrilateral_data.count(); i=i+15)
    {
        QLd_quadriWorld3D.append(*QLd_quadrilateral_data[i]);
        QLd_quadriWorld3D.append(*QLd_quadrilateral_data[i+1]);
        QLd_quadriWorld3D.append(*QLd_quadrilateral_data[i+2]);
        for (int ii=i+3; ii<(i+15); ii=ii+3)
        {
            QLd_quadriWorld3D.append(QSL_welldepth_1[1].toDouble()+
                    (QSL_welldepth_2[1].toDouble()-QSL_welldepth_1[1].toDouble())
                    *(*QLd_quadrilateral_data[ii]-(-0.45)) / (0.45-(-0.45)));//X
            QLd_quadriWorld3D.append(QSL_welldepth_1[2].toDouble()+
                    (QSL_welldepth_2[2].toDouble()-QSL_welldepth_1[2].toDouble())
                    *(*QLd_quadrilateral_data[ii]-(-0.45)) / (0.45-(-0.45)));//Y
            QLd_quadriWorld3D.append(mostshallow+
                    (deepest-mostshallow)*(*QLd_quadrilateral_data[ii+1]-0.4) / ((-0.4)-0.4));//Depth
        }
    }
    for (int i=0; i<QLd_quadriWorld3D.count(); i=i+15)
    {
        QLd_quadriOpengl3D.append(QLd_quadriWorld3D[i]);
        QLd_quadriOpengl3D.append(QLd_quadriWorld3D[i+1]);
        QLd_quadriOpengl3D.append(QLd_quadriWorld3D[i+2]);
        for (int ii=i+3; ii<(i+15); ii=ii+3)
        {
            QLd_quadriOpengl3D.append(QLd_quadriWorld3D[ii]);
            QLd_quadriOpengl3D.append(QLd_quadriWorld3D[ii+2]);
            QLd_quadriOpengl3D.append(QLd_quadriWorld3D[ii+1]);
        }
    }

    //save data as a txt file    well_section
    QString QS_file_name,QS_ws1,QS_ws2,QS_ws3;
    QDir QD_well_section;
    QS_file_name=QSL_welldepth_1[0] + "_" + QSL_welldepth_2[0];
    QD_well_section.mkdir("well_section");
    QFile QF_well_section("well_section/" + QS_file_name + ".txt");
    QF_well_section.open(QFile::WriteOnly|QFile::Text);
    QTextStream QTS_well_section_out(&QF_well_section);
    QS_ws1=QSL_welldepth_1[0]+'\t'+QSL_welldepth_1[1]+'\t'+QSL_welldepth_1[2];
    QTS_well_section_out<<QS_ws1<<endl;//save X,Y coordinate of well one
    QS_ws1=QSL_welldepth_2[0]+'\t'+QSL_welldepth_2[1]+'\t'+QSL_welldepth_2[2];
    QTS_well_section_out<<QS_ws1<<endl;//save X,Y coordinate of well two
    for (int i=0; i<QLd_quadriOpengl3D.count(); i=i+3)
    {
        QS_ws1.setNum(QLd_quadriOpengl3D[i], 'f', 3);
        QS_ws2=QS_ws1+'\t'+QS_ws2.setNum(QLd_quadriOpengl3D[i+1], 'f', 3);
        QS_ws3=QS_ws2+'\t'+QS_ws3.setNum(QLd_quadriOpengl3D[i+2], 'f', 3);
        QTS_well_section_out<<QS_ws3<<endl;
    }
    QF_well_section.close();

    QLd_quadriWorld3D.clear();
    QLd_quadriOpengl3D.clear();

    saveWellData();//save nodes depth data
    QMessageBox::information(this,"Message","Save successfully.");
}

void OglWidget_ms::saveWellData()
{
    QList<QStringList*> QLsl_wellData;
    QList<double> sort_node_position;

    for (int i=0; i<QLLd_node_position.count(); i++)//count how many well
    {
        //translate opengl2D position to world coordinate
        QLsl_wellData.append(new QStringList());
        QLsl_wellData[i]->append("");
        QLsl_wellData[i][0][0].setNum(QSL_welldepth_1[1].toDouble()
                + (QSL_welldepth_2[1].toDouble()-QSL_welldepth_1[1].toDouble())
                * (QLLd_node_position[i][0][0]-(-0.45)) / (0.45-(-0.45)), 'f', 3);
        QLsl_wellData[i]->append("");
        QLsl_wellData[i][0][1].setNum(QSL_welldepth_1[2].toDouble()
                + (QSL_welldepth_2[2].toDouble()-QSL_welldepth_1[2].toDouble())
                * (QLLd_node_position[i][0][0]-(-0.45)) / (0.45-(-0.45)), 'f', 3);

        sort_node_position = *QLLd_node_position[i];
        sort_node_position.removeFirst();
        qSort(sort_node_position.begin(), sort_node_position.end(), qGreater<double>());
        for (int ii=0; ii<sort_node_position.count(); ii++)
        {
            QLsl_wellData[i]->append("");
            QLsl_wellData[i][0][ii+2].setNum(mostshallow
                + (deepest-mostshallow)*(sort_node_position[ii]-0.4) / ((-0.4)-0.4), 'f', 3);
        }

    }
    
    //save data as a txt file
    for (int i=0; i<QLsl_wellData.count(); i++)//creat the same number of file and well
    {
        QString QS_well_name,QS_wd,QS_wn,QS_wd_in;
        QDir QD_well_depth;
        QFile QF_well_depth,QF_well_depth_in;
        QTextStream txt_well_depth_out,txt_well_depth_in;
        QStringList QSL_welldepth_new,QSL_welldepth_old;

        QD_well_depth.mkdir("well_data");//creat a folder
        if (i <= 1)//the file path of well_1
        {
            if (i == 0)
                QS_well_name=QSL_welldepth_1[0];
            else if (i == 1)
                QS_well_name=QSL_welldepth_2[0];
            //compare the number of layers(nodes) of well which set with different time
            if ( QF_well_depth.exists("well_data/" + QS_well_name + ".txt") )
            {
                QF_well_depth_in.setFileName("well_data/" + QS_well_name + ".txt");

                QF_well_depth_in.open(QFile::ReadOnly|QFile::Text);
                txt_well_depth_in.setDevice(&QF_well_depth_in);
                QS_wd_in=txt_well_depth_in.readLine();
                QSL_welldepth_old=QS_wd_in.split('\t');
                while (!txt_well_depth_in.atEnd())
                {
                    QS_wd_in=txt_well_depth_in.readLine();
                    QSL_welldepth_old.append(QS_wd_in.split('\t'));
                }
                QF_well_depth_in.close();

                //integration
                QSL_welldepth_new=QLsl_wellData[i][0];
                QSL_welldepth_new.removeFirst();
                QSL_welldepth_new.removeFirst();
                QSL_welldepth_old.removeFirst();
                QSL_welldepth_old.removeFirst();
                QSL_welldepth_new=QSL_welldepth_new+QSL_welldepth_old;
                QSL_welldepth_new.removeDuplicates();
                sort_node_position.clear();
                for (int k=0; k<QSL_welldepth_new.count(); k++)
                {
                    sort_node_position.append(QSL_welldepth_new[k].toDouble());
                }
                qSort(sort_node_position.begin(), sort_node_position.end());
                sort_node_position.prepend(QLsl_wellData[i][0][1].toDouble());
                sort_node_position.prepend(QLsl_wellData[i][0][0].toDouble());
                QLsl_wellData[i]->clear();
                for (int k=0; k<sort_node_position.count(); k++)
                {
                    QLsl_wellData[i]->append("");
                    QLsl_wellData[i][0][k].setNum(sort_node_position[k], 'f', 3);
                }
                QSL_welldepth_new.clear();
                QSL_welldepth_old.clear();
            }
        }
        else//the file path of virtul wells
        {
            QS_wn.setNum(i-1);
            QS_well_name=QSL_welldepth_1[0] + QSL_welldepth_2[0] + "_" + QS_wn;
        }

        QF_well_depth.setFileName("well_data/" + QS_well_name + ".txt");

        QF_well_depth.open(QFile::WriteOnly|QFile::Text);
        txt_well_depth_out.setDevice(&QF_well_depth);
        QS_wd=QLsl_wellData[i][0][0];
        QS_wd=QS_wd+'\t'+QLsl_wellData[i][0][1];
        txt_well_depth_out<<QS_wd<<endl;
        for (int k=2; k<QLsl_wellData[i]->count(); k++)
        {
            QS_wd=QLsl_wellData[i][0][k];
            txt_well_depth_out<<QS_wd<<endl;
        }
        QF_well_depth.close();
    }
    
    

    int df;
    df=3;
}

