#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<int> width;
    width<<700<<200;
    ui->splitter->setSizes(width);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,1);

    QList<int> height;
    height<<700<<200;
    ui->splitter_2->setSizes(height);
    ui->splitter_2->setStretchFactor(0,1);
    ui->splitter_2->setStretchFactor(1,1);

    openGLwidget = ui->widget;
    openGLwidget->setGeometry(0,0,740,690);
    pBendglWidget = new bendglWidget(openGLwidget);
    pBendglWidget->show();

    pCreate = new CreateWorkPiece();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/********************************************
 *function:界面大小改变时，变化opengl显示窗口的大小
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::resizeEvent(QResizeEvent *event)
{
    event = event;
    updateopenGLWidget();
}

/********************************************
 *function:键盘事件槽函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::keyPressEvent(QKeyEvent *ekey)
{
    int ikey = ekey->key();
    switch(ikey)
    {
    case Qt::Key_Left:
    {

        break;
    }
    case Qt::Key_Right:
    {

        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:鼠标移动时的槽函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int pos_x = event->x();
    int pos_y = event->y();
    if(event->buttons() & Qt::LeftButton) //左键按下
    {
        if(fabs(pos_x-mouse_x) > fabs(pos_y-mouse_y))
            pBendglWidget->convertGraph((pos_x-mouse_x)/2,CONVERT_HORIZONTAL);
        else
            pBendglWidget->convertGraph((mouse_y-pos_y)/2,CONVERT_VERTICAL);
        updateopenGLWidget();
    }
    else if(event->buttons() & Qt::MiddleButton)  //中键按下
    {
        float rote = 4.0;
        float x_off = pos_x-mouse_x;
        float y_off = pos_y-mouse_y;
        if(x_off > 100)
            pBendglWidget->convertGraph(rote,CONVERT_Y);
        else if(x_off < -100)
            pBendglWidget->convertGraph(-rote,CONVERT_Y);
        if(y_off > 100)
            pBendglWidget->convertGraph(rote,CONVERT_X);
        else if(y_off < -100)
            pBendglWidget->convertGraph(-rote,CONVERT_X);
        if(x_off > 100 && y_off > 100)
            pBendglWidget->rotateGraph(1.0,1.0,0.0);
        else if(x_off < -100 && y_off < -100)
            pBendglWidget->rotateGraph(-1.0,-1.0,0.0);
        else if(x_off > 100 && y_off < 100)
            pBendglWidget->rotateGraph(0.0,1.0,0.0);
        updateopenGLWidget();
    }
    else if(event->buttons() & Qt::RightButton)   //右键按下
    {

    }
}

/********************************************
 *function:鼠标按压事件触发
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();
    int button = event->button();
    switch (button)
    {
    case Qt::LeftButton:
    {
        break;
    }
    case Qt::RightButton:
    {
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();
    int button = event->button();
    switch (button) {
    case Qt::RightButton:
    {
        break;
    }
    case Qt::LeftButton:
    {
        break;
    }
    case Qt::MiddleButton:
    {
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:滚轮事件
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::wheelEvent(QWheelEvent *event)
{
    int numDegress = event->delta()/8;  //滚动的角度，*8就是鼠标滚动的距离
    int numSteps = numDegress/15;       //滚动的步数，*15就是鼠标滚动的角度
    float zoom = numSteps*0.1;
    pBendglWidget->convertGraph(zoom,CONVERT_ZOOM);
//    event->accept();
}

/********************************************
 *function:调整窗口大小
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::updateopenGLWidget()
{
    int width = openGLwidget->width();
    int height = openGLwidget->height();
    pBendglWidget->resizeWindow(width,height);
}

/********************************************
 *function:水平分裂器移动时更新opengl窗口的大小
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    int width = openGLwidget->width();
    int height = openGLwidget->height();
    pBendglWidget->resizeWindow(width,height);
}

/********************************************
 *function:垂直分裂器移动时更新opengl窗口的大小
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
void MainWindow::on_splitter_2_splitterMoved(int pos, int index)
{
    int width = openGLwidget->width();
    int height = openGLwidget->height();
    pBendglWidget->resizeWindow(width,height);
}

/********************************************
 *function:画矩形平面的槽函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/27
 *******************************************/
void MainWindow::on_pushButton_clicked()
{
    line_type line1,line2;
    line1.start.x = ui->plane_start1x->text().toFloat();
    line1.start.y = ui->plane_start1y->text().toFloat();
    line1.start.z = ui->plane_start1z->text().toFloat();
    line1.end.x = ui->plane_end1x->text().toFloat();
    line1.end.y = ui->plane_end1y->text().toFloat();
    line1.end.z = ui->plane_end1z->text().toFloat();
    line2.start.x = ui->plane_start2x->text().toFloat();
    line2.start.y = ui->plane_start2y->text().toFloat();
    line2.start.z = ui->plane_start2z->text().toFloat();
    line2.end.x = ui->plane_end2x->text().toFloat();
    line2.end.y = ui->plane_end2y->text().toFloat();
    line2.end.z = ui->plane_end2z->text().toFloat();
    line1.isBendBound = false;
    line2.isBendBound = false;
    pBendglWidget->test = pCreate->createPlaneTrimmedSurface(line1,line2,false,false,false,false);
    pBendglWidget->drawTest();
}

/********************************************
 *function:画圆柱面的槽函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/27
 *******************************************/
void MainWindow::on_pushButton_2_clicked()
{
    line_type generatrix,axis;
    generatrix.start.x = ui->generatrix_startx->text().toFloat();
    generatrix.start.y = ui->generatrix_starty->text().toFloat();
    generatrix.start.z = ui->generatrix_startz->text().toFloat();
    generatrix.end.x = ui->generatrix_endx->text().toFloat();
    generatrix.end.y = ui->generatrix_endy->text().toFloat();
    generatrix.end.z = ui->generatrix_endz->text().toFloat();
    axis.start.x = ui->axis_startx->text().toFloat();
    axis.start.y = ui->axis_starty->text().toFloat();
    axis.start.z = ui->axis_startz->text().toFloat();
    axis.end.x = ui->axis_endx->text().toFloat();
    axis.end.y = ui->axis_endy->text().toFloat();
    axis.end.z = ui->axis_endz->text().toFloat();
    float angle;
    angle = ui->cylinderAngle->text().toFloat();
    pBendglWidget->test = pCreate->createCylinderTrimmedSurface(generatrix,axis,angle,false,false);
    pBendglWidget->drawTest();
}

/********************************************
 *function:画扇形面
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2017/10/27
 *******************************************/
void MainWindow::on_pushButton_3_clicked()
{
    line_type generatrix,axis;
    generatrix.start.x = ui->ringGeneratrix_startx->text().toFloat();
    generatrix.start.y = ui->ringGeneratrix_starty->text().toFloat();
    generatrix.start.z = ui->ringGeneratrix_startz->text().toFloat();
    generatrix.end.x = ui->ringGeneratrix_endx->text().toFloat();
    generatrix.end.y = ui->ringGeneratrix_endy->text().toFloat();
    generatrix.end.z = ui->ringGeneratrix_endz->text().toFloat();
    axis.start.x = ui->ringAxis_startx->text().toFloat();
    axis.start.y = ui->ringAxis_starty->text().toFloat();
    axis.start.z= ui->ringAxis_startz->text().toFloat();
    axis.end.x = ui->ringAxis_endx->text().toFloat();
    axis.end.y = ui->ringAxis_endy->text().toFloat();
    axis.end.z= ui->ringAxis_endz->text().toFloat();
    float angle,thickness;
    angle = ui->ringAngle->text().toFloat();
    thickness = ui->thickness->text().toFloat();
    pBendglWidget->test = pCreate->createRingTrimmedSurface(generatrix,axis,angle,thickness);
    pBendglWidget->drawTest();
}
