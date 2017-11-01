#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<bendglwidget.h>
#include"createworkpiece.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget *openGLwidget;
    bendglWidget *pBendglWidget;
    CreateWorkPiece *pCreate;

    int mouse_x,mouse_y;

public:

private:
   void  resizeEvent(QResizeEvent *event);
   void  keyPressEvent(QKeyEvent *ekey);
   void  mouseMoveEvent(QMouseEvent *event);
   void  mousePressEvent(QMouseEvent *event);
   void  mouseReleaseEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *event);

   void updateopenGLWidget();

protected:
    void paintGL();
private slots:

    void on_splitter_splitterMoved(int pos, int index);
    void on_splitter_2_splitterMoved(int pos, int index);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
};

#endif // MAINWINDOW_H
