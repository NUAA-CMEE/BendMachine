#ifndef BENDGLWIDGET_H
#define BENDGLWIDGET_H

#include <QGLWidget>
#include<QtOpenGL/QtOpenGL>
#include<QtPrintSupport>
#include<gl/glu.h>
#include<gl/gl.h>
#include<time.h>
#include<QDebug>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
#include"globalVar.h"
#include"SysGLData.h"

class bendglWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit bendglWidget(QWidget *parent = 0);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width,int height);


private:
    float xRote,yRote,zRote;
    float vMove,hMove,zMove;
    bool isAxisBeside;
    bool isFirstLoad;

public:
    GLfloat ex,ey,ez,centerx,centery,centerz,upx,upy,upz;
    GLfloat ex1,ey1,ez1,centerx1,centery1,centerz1,upx1,upy1,upz1;
    trimmed_surface_type *test;
    GLUnurbsObj *pNurb;


private:
    void drawCoordinate();
    void drawFloor();
    void drawEntity();
    void test2();

public:
    void resizeWindow(int width,int height);
    void rotateGraph(float xrote,float yrote,float zrote);
    void convertGraph(float para,CONVERT convert);
    void drawTest();

signals:

public slots:

};

#endif // BENDGLWIDGET_H
