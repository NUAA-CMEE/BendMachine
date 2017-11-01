#include "bendglwidget.h"

//设置与光源有关的数组
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = {0.0,0.0,0.0,1.0};
GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat Light_Model_Ambient[] = {1.0,1.0,1.0,0.8};
//GLfloat matSpecular[4] = {1.0,1.0,1.0,1.0};
//GLfloat matShininess[] = {50.0};
GLfloat lightAmbient[4] = {0.1,0.1,0.1,1.0};    //环境光，环境光来自四面八方，所有场景中的对象都处于环境光的照射中
GLfloat lightDiffuse[4] = {0.2,0.2,0.2,1.0};    //漫射光，漫射光由特定的光源产生，并在场景中的对象表面产生反射
GLfloat lightPosition[4] = {0.0,0.0,100.0,1.0};
#define AXES_LEN 1000 //绘制的坐标轴的长度

GLfloat backgaung_rColor[4] = {0.2,0.8,0.2,1.0};  //后挡料颜色
GLfloat backgaung_xColor[4] = {0.8,0.8,0.5,1.0};  //后挡料颜色
GLfloat backgaung_zColor[4] = {0.3,0.2,0.3,1.0};  //后挡料颜色
GLfloat workpieceColor[4] = {1.0,0.6,0.2,1.0};  //工件颜色
GLfloat bac_workpieceColor[4] = {0.0,0.75,1.0,1.0};  //工件颜色
GLfloat bendColor[3] = {0.8,0.0,0.0};//折弯颜色

bendglWidget::bendglWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setGeometry(0,0,parentWidget()->width(),parentWidget()->height());

//    pDrawData = NULL;
    xRote = 90.0;
    yRote = 0.0;
    zRote = 0.0;
    zoom = 1.0;
    hMove = 0.0;
    vMove = 0.0;

    ex = 0.0;
    ey = 0.0;
    ez = 1200;  //circle_radius
    isAxisBeside = false;
    isFirstLoad = true;
    test = NULL;
}

/********************************************
 *function:窗口的OPENGGL界面初始化函数，
 *input:
 *output:
 *adding:虚函数  自动触发
 *author: wang
 *date: 2017/10/23
 *******************************************/
void bendglWidget::initializeGL()
{
    //启用阴影平滑，阴影平滑通过多边形精细的混合色彩，并对外部光进行光滑。
    glShadeModel(GL_SMOOTH);
    //设置清除屏幕时所用的颜色
    glClearColor(0.90,0.90,0.90,0.0);
    //设置深度缓存
    glClearDepth(1.0);
    //启用深度测试

    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GLU_MAP1_TRIM_2);
    glEnable(GLU_MAP1_TRIM_3);
    //所作深度测试的类型
    glDepthFunc(GL_LEQUAL);
    //glDepthRange(0.0,1.0);
    //真正精细的透视修正，这一行告诉OPENGL我们希望进行最好的透视修正
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    //设置光源
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,Light_Model_Ambient);//全局光照

    glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);   //设置环境光的发光亮
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuse);   //设置漫射光的发光亮
    glLightfv(GL_LIGHT0,GL_SPECULAR,lightDiffuse);
    GLfloat position[] = {-10.0,10.0,0.0,0.0};
    glLightfv(GL_LIGHT0,GL_POSITION,position);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);  //打开光照渲染处理
    //启动深度测试
    glEnable(GL_DEPTH_TEST);
    //设置材质
//    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
//    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
}

/********************************************
 *function:更新OPENGL绘图界面
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/23
 *******************************************/
void bendglWidget::paintGL()
{
    //清除屏幕和深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     //重置当前的模型观察矩阵
     glLoadIdentity();
     gluLookAt(ex, ey, ez,0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

     //平移  旋转  缩放
    glTranslatef(0.0+hMove,0.0+vMove,0.0+zMove);
    glRotatef(xRote,1.0,0.0,0.0);
    glRotatef(yRote,0.0,1.0,0.0);
    glRotatef(zRote,0.0,0.0,1.0);
    glScalef(zoom,zoom,zoom);
    //画地板和坐标系
    drawCoordinate();
    drawFloor();

    //绘制测试的裁剪曲面
    glLineWidth(1);
    glColor3fv(bendColor);
    glColor4fv(workpieceColor);
//    test2();

    //绘制裁剪曲面
    drawEntity();

    glFlush(); //强制刷新缓冲
}

/********************************************
 *function:当绘图OPENGL的窗口大小改变时触发此函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/23
 *******************************************/
void bendglWidget::resizeGL(int width, int height)
{
    //防止height为0
    if(0 == height)
        height = 1;
    //重置当前的视口
    glViewport(0,0,width,height);
    //选择投影矩阵
    glMatrixMode(GL_PROJECTION);
    //重置投影矩阵
    glLoadIdentity();
    //建立透视投影矩阵
    //gluPerspective(45.0,(GLfloat)width/(GLfloat)height,1.0,100.0);
    gluPerspective(45.0,(GLfloat)width/(GLfloat)height,1.0,100000.0);
    //选择模型观察矩阵
    glMatrixMode(GL_MODELVIEW);
    //重置模型观察矩阵
    glLoadIdentity();
}

/********************************************
 *function:图形变换
 *input:
 *output:
 *adding:沿三轴平移  绕三轴旋转
 *author: wang
 *date: 2017/10/24
 *******************************************/
void bendglWidget::convertGraph(float para, CONVERT convert)
{
    switch (convert) {
    case CONVERT_HORIZONTAL://水平移动
    {
        hMove += para;
        updateGL();
        break;
    }
    case CONVERT_VERTICAL:  //垂直移动
    {
        vMove += para;
        updateGL();
        break;
    }
    case CONVERT_ZOOM:      //缩放
    {
        zoom += para;
        if(zoom < 0.1)
            zoom = 0.1;
//        if(zoom > 1.2)
//            zoom = 1.2;
        updateGL();
        break;
    }
    case CONVERT_X:         //绕X轴旋转
    {
        xRote += para;
        updateGL();
        break;
    }
    case CONVERT_Y:          //绕Y轴旋转
    {
        yRote += para;
        updateGL();
        break;
    }
    case CONVERT_Z:         //绕Z轴旋转GLfloat machineColor[4] = {0.1,0.1,0.1,1.0};    //机床本体颜色
    {
        zRote += para;
        updateGL();
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:画出OPENGL的坐标系
 *input:
 *output:
 *adding:应该是和SolidWorks一致的
 *author: wang
 *date: 2017/10/23
 *******************************************/
void bendglWidget::drawCoordinate()
{
    GLUquadricObj *objCylinder = gluNewQuadric();

    glPushMatrix();
    glColor3f(1.0f,0.0f, 0.0f);  //红色
    glRotatef(90,0.0,1.0,0.0);
    gluCylinder(objCylinder,5, 5, AXES_LEN, 10, 5);         //X轴
    glTranslatef(0,0,AXES_LEN);
    gluCylinder(objCylinder,5, 0.0, 5, 10, 5);                 //X
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f,1.0f, 0.0f);    //绿色
    glRotatef(-90,1.0,0.0,0.0);
    gluCylinder(objCylinder,5, 5, AXES_LEN, 10, 5);         //Y轴
    glTranslatef(0,0,AXES_LEN);
    gluCylinder(objCylinder,5, 0.0, 5, 10, 5);                 //Y轴
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f,0.0f,1.0f); //蓝色
    glTranslatef(0,0,0);
    gluCylinder(objCylinder,5,5,AXES_LEN,10,5);  //Z轴
    glTranslatef(0,0,AXES_LEN);
    gluCylinder(objCylinder,5,0.0,5,10,5);  //Z轴
    glPopMatrix();

//    glBegin(GL_LINES);
//    glColor3f( 1.0f, 0.0f, 0.0f);
//    glLineWidth(10);
//    glVertex3f( 0.0f, 0.0f,  0.0f);
//    glVertex3f( 0.0f, 0.0f,400.0f);

//    glVertex3f( 0.0f, 0.0f, 400.0f);
//    glVertex3f( 0.0f, 0.6f, 386.0f);
//    glVertex3f( 0.0f, 0.0f, 400.0f);
//    glVertex3f( 0.0f,-0.6f, 386.0f);
//    glEnd();

//    glBegin(GL_LINES);
//    glLineWidth(10);
//    glColor3f(0.0f,1.0f,0.0f);
//    glVertex3f( 0.0f,  0.0f, 0.0f);
//    glVertex3f( 0.0f, 1000.0f, 0.0f);

//    glVertex3f( 0.0f, 1000.0f, 0.0f);
//    glVertex3f( 15.0f, 965.0f, 0.0f);
//    glVertex3f( 0.0f, 1000.0f, 0.0f);
//    glVertex3f(-15.0f, 965.0f, 0.0f);
//    glEnd();

//    glBegin(GL_LINES);
//    glLineWidth(10);
//    glColor3f(0.0f,0.0f,1.0f);
//    glVertex3f(   0.0f, 0.0f, 0.0f);
//    glVertex3f( 400.0f, 0.0f, 0.0f);

//    glVertex3f( 400.0f, 0.0f, 0.0f);
//    glVertex3f( 386.0f, 6.0f, 0.0f);
//    glVertex3f( 400.0f, 0.0f, 0.0f);
//    glVertex3f( 386.0f,-6.0f, 0.0f);
//    glEnd();
}

/********************************************
 *function:用画网格的方式，画出地板
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/23
 *******************************************/
void bendglWidget::drawFloor()
{
    glLineWidth(0.1);
    glBegin(GL_LINES);
    glEnable(GL_LINE_SMOOTH);
    glColor4f(0.4,0,0,0);
    int y1 = -1000;
    for(int z=-25;z<25;z++)
    {
        glVertex3f(-7500,y1,z*300);
        glVertex3f(7500,y1,z*300);
    }
    for(int x=-25;x<25;x++)
    {
        glVertex3f(x*300,y1,-7500);
        glVertex3f(x*300,y1,7500);
    }
    glEnd();
}

/********************************************
 *function:画出要测试的裁剪曲面
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/27
 *******************************************/
void bendglWidget::drawEntity()
{
    if(test == NULL)
        return;
    trimmed_surface_type *pTmpCur;
    pTmpCur = test;
    nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
    pNurb = gluNewNurbsRenderer();  //创建NURBS对象
    gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
    int knot_count1 = pSurface->K1_num+pSurface->M1_num+2;
    int knot_count2 = pSurface->K2_num+pSurface->M2_num+2;

    //绘制裁剪曲面

    int typeNum = pTmpCur->surfaceType;
    if(typeNum == 128)
    {
        float *xulie1 = pSurface->pKnot1;
        float *xulie2 = pSurface->pKnot2;
        float *ctr = pSurface->pCtlarray;
//        qDebug()<<"knot1："<<xulie1[0]<<" "<<xulie1[1]<<" "<<xulie1[2]<<" "<<xulie1[3]<<" ";
//        qDebug()<<"knot2："<<xulie2[0]<<" "<<xulie2[1]<<" "<<xulie2[2]<<" "<<xulie2[3]<<" ";
        qDebug()<<"pctlarray："<<ctr[0]<<" "<<ctr[1]<<" "<<ctr[2]<<" ";
        qDebug()<<"pctlarray："<<ctr[3]<<" "<<ctr[4]<<" "<<ctr[5]<<" ";
        qDebug()<<"pctlarray："<<ctr[6]<<" "<<ctr[7]<<" "<<ctr[8]<<" ";
        qDebug()<<"pctlarray："<<ctr[9]<<" "<<ctr[10]<<" "<<ctr[11]<<" ";
        gluBeginSurface(pNurb);
        gluNurbsSurface(pNurb,knot_count1,pSurface->pKnot1,knot_count2,\
            pSurface->pKnot2,(pSurface->K1_num+1)*3,3,pSurface->pCtlarray,\
            pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3); //GL_MAP2_VERTEX_3 控制顶点权都为1，每个点记为（x,y,z）
        if(pTmpCur->boundary)
        {
            for(int i = 0; i < pTmpCur->curve_num+1; i++)
            {
                 gluBeginTrim(pNurb);
                for(int j = 0; j < pTmpCur->curveCount[i]; j++)
                {
                    if(pTmpCur->curveType[i][j] == 100)     //圆弧实体
                    {
                        //暂时保留，因为基本上不会出现此种情况
                    }
                    else if(pTmpCur->curveType[i][j] == 110)//直线实体
                    {
                        line_type *pLine = (line_type *)pTmpCur->pCurves[i][j];
                        float linepoints[2][3];
                        linepoints[0][0] = pLine->start.x;
                        linepoints[0][1] = pLine->start.y;
                        linepoints[0][2] = pLine->start.z;
                        linepoints[1][0] = pLine->end.x;
                        linepoints[1][1] = pLine->end.y;
                        linepoints[1][2] = pLine->end.z;
                        qDebug()<<j<<""<<linepoints[0][0]<<" "<<linepoints[0][1]<<" "<<linepoints[0][2];
                        qDebug()<<j<<""<<linepoints[1][0]<<" "<<linepoints[1][1]<<" "<<linepoints[1][2];
                        gluPwlCurve(pNurb,2,&linepoints[0][0],3,GLU_MAP1_TRIM_2); //GLU_MAP1_TRIM_2表示不带权重
                    }
                    else if(pTmpCur->curveType[i][j] == 126)//B样条曲线实体
                    {
                        nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pTmpCur->pCurves[i][j];
                        int knot_count = pNurbsCurve->K_num+pNurbsCurve->M_num+2;

                        float *ctl = pNurbsCurve->pCtlarray;
                        for(int i = 0;i< pNurbsCurve->K_num + 1;i++)
                        {
                             qDebug()<<"X: "<<ctl[3*i]<<" Y:"<<ctl[3*i+1]<<" Z:"<<ctl[3*i+2];
                        }
                        gluNurbsCurve(pNurb,knot_count,pNurbsCurve->pKnot,3,pNurbsCurve->pCtlarray,\
                                      pNurbsCurve->M_num+1,GLU_MAP1_TRIM_2);
                    }
                }
               gluEndTrim(pNurb);
            }
        }
        gluEndSurface(pNurb);
    }
    else if(typeNum == 120)   //是折弯面  和画128的区别：
    {
        gluBeginSurface(pNurb);
        //glColor4fv(SelectColor);
        gluNurbsSurface(pNurb,knot_count2,pSurface->pKnot2,knot_count1,\
            pSurface->pKnot1,3,(pSurface->K2_num+1)*3,pSurface->pCtlarray,\
            pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
        if(pTmpCur->boundary)
        {
            for(int i = 0; i < pTmpCur->curve_num+1; i++)
            {
                gluBeginTrim(pNurb);
                for(int j = pTmpCur->curveCount[i]-1; j >= 0; j--)
                {
                    if(pTmpCur->curveType[i][j] == 100)     //圆弧实体
                    {
                        //暂时保留，因为基本上不会出现此种情况
                    }
                    else if(pTmpCur->curveType[i][j] == 110)//直线实体
                    {
                    }
                    else if(pTmpCur->curveType[i][j] == 126)//B样条曲线实体
                    {
                        nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pTmpCur->pCurves[i][j];
                        int num = pNurbsCurve->K_num;
                        float linepoints[2][3];
                        for(int m = num; m >= 0; m--){
                            linepoints[num-m][0] = pNurbsCurve->pCtlarray[3*m];
                            linepoints[num-m][1] = pNurbsCurve->pCtlarray[3*m+1];
                            linepoints[num-m][2] = pNurbsCurve->pCtlarray[3*m+2];
                        }
                        gluPwlCurve(pNurb,2,&linepoints[0][0],3,GLU_MAP1_TRIM_2);
                    }
                }
                gluEndTrim(pNurb);
            }
        }
        gluEndSurface(pNurb);
    }


    if(pNurb)           //如果对象已经建立，则删除
    {
        gluDeleteNurbsRenderer(pNurb);
        pNurb = NULL;
    }

    //画外轮廓线
    for(int i = 0; i < pTmpCur->curve_num+1; i++)
    {
        for(int j = 0; j < pTmpCur->outlineCount[i]; j++)
        {
            int type = pTmpCur->outlineType[i][j];
            if(type == 110) //画直线
            {
                glLineWidth(30);
                glColor3f(0.0,0.0,0.0); //黑色
                line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
                glBegin(GL_LINES);
                glEnable(GL_LINE_SMOOTH);
                glVertex3f(pline->start.x,pline->start.y,pline->start.z);
                glVertex3f(pline->end.x,pline->end.y,pline->end.z);
                glEnd();
             }
            if(type == 100) //画圆弧
            {

             }
         }
    }
}

void bendglWidget::test2()
{
    if(test != NULL)
    {
        trimmed_surface_type *pTmpCur;
        pTmpCur = test;
        nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
        pNurb = gluNewNurbsRenderer();  //创建NURBS对象
        gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
        int knot_count1 = pSurface->K1_num+pSurface->M1_num+2;
        int knot_count2 = pSurface->K2_num+pSurface->M2_num+2;
        gluBeginSurface(pNurb);
        gluNurbsSurface(pNurb,knot_count1,pSurface->pKnot1,knot_count2,\
            pSurface->pKnot2,(pSurface->K1_num+1)*3,3,pSurface->pCtlarray,\
            pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
         gluEndSurface(pNurb);
    }
}

/********************************************
 *function:调用paintGL
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/27
 *******************************************/
void bendglWidget::drawTest()
{
    updateGL();
}

/********************************************
 *function:改变窗口大小
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void bendglWidget::resizeWindow(int width, int height)
{
    setGeometry(0,0,width,height);
    updateGL();
}

/********************************************
 *function:复合旋转  图形
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void bendglWidget::rotateGraph(float xrote, float yrote, float zrote)
{
    xRote += xrote;
    yRote += yrote;
    zRote += zrote;
    updateGL();
}


