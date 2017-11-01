#include "createworkpiece.h"

#define angle2Radian 0.0174533

float knot_value[68] = {0.0,0.0,0.0,  0.03125,0.03125,  0.0625,0.0625,  0.09375,0.09375,  0.125,0.125,\
                                                     0.15625,0.15625,  0.1875,0.1875,  0.21875,0.21875,  0.25,0.25,\
                                                     0.28125,0.28125,  0.3125,0.3125,  0.34375,0.34375,  0.375,0.375,\
                                                     0.40625,0.40625,  0.4375,0.4375,  0.46875,0.46875,  0.5,0.5,\
                                                     0.53125,0.53125,  0.5625,0.5625,  0.59375,0.59375,  0.625,0.625,\
                                                     0.65625,0.65625,  0.6875,0.6875,  0.71875,0.71875,  0.75,0.75,\
                                                     0.78125,0.78125,  0.8125,0.8125,  0.84375,0.84375,  0.875,0.875,\
                                                     0.90625,0.90625,  0.9375,0.9375,  0.96875,0.96875,\
                                    1.0,1.0,1.0};  //分度计算  1/((n-6)/2 +1)

float knot1_prepare[44] = {0.00,0.00,0.00,0.05,0.05,0.10,0.10,0.15,0.15,0.20,0.20,\
                                         0.25,0.25,0.30,0.30,0.35,0.35,0.40,0.40,0.45,0.45,0.50,\
                                         0.50,0.55,0.55,0.60,0.60,0.65,0.65,0.70,0.70,0.75,0.75,\
                                         0.80,0.80,0.85,0.85,0.90,0.90,0.95,0.95,1.00,1.00,1.00};  //分度公式1/((n-6)/2 +1)
float knot2_prepare[6] = {0.0,0.0,0.0,1.0,1.0,1.0};

CreateWorkPiece::CreateWorkPiece()
{

}

/********************************************
 *function:创建直线110实体对象
 *input:起点和终点的三维坐标   bool   true:是折弯边的界线   false:不是
 *output:110直线结构体对象的指针
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
line_type *CreateWorkPiece::createLine(Point3F start_point, Point3F end_point, bool isBendLine)
{
    line_type *pLine = new line_type;
    memset(pLine,0,sizeof(line_type));  //清空指针指向的内存
    pLine->typeNum = 110;
    pLine->start = start_point;
    pLine->end = end_point;
    pLine->isBendBound = isBendLine;
    pLine->isVisited = false;
    return pLine;
}

/********************************************
 *function:创建圆弧100实体对象
 *input:圆弧起点  圆弧半径  圆弧角度(度数)  从起点指向圆心的单位向量
 *output:100圆弧结构体对象的指针
 *adding:mormal是从圆弧指向圆心的（由上一条直线的法线算出）单位向量，（圆心减去起点）
 *           lineVector是用上一条直线的终点减去起点得到的方向向量
 *           用来计算圆心在基坐标系下的坐标。  角度输入是角度制
 *           局部坐标系建立在圆弧所在的平面上，坐标系的原点取在圆心处  从起点到终点是按逆时针方向
 *author: wang
 *date: 2017/10/24
 *******************************************/
arc_type *CreateWorkPiece::createArc(Point3F start, float Radius, float angle,Point3F normalVector,Point3F lineVector)
{
    arc_type *pArc = new arc_type;
    memset(pArc,0,sizeof(arc_type));
    float rad = angle * angle2Radian; //将输入的角度换算成弧度
    pArc->typeNum = 100;
    pArc->ZT = 0;   //圆弧所在平面距离局部坐标系的距离，圆弧在局部坐标系上，所以距离为0
    pArc->X1 = 0;   //圆心与局部坐标系XT YT的原点重合，所以圆心在局部坐标系下的坐标（0，0）
    pArc->Y1 = 0;
    pArc->X2 = Radius;  //起点在局部坐标系的XT轴上  所以（radius，0）
    pArc->Y2 = 0;
    pArc->X3 = Radius * cos(rad); //计算圆弧终点在局部坐标系下的坐标
    pArc->Y3 = Radius * sin(rad);
    pArc->angle = rad;//按弧度存储角度，用来转126
    pArc->Radius = Radius;

    //求基坐标系到局部坐标系的变换矩阵
    Point3F centerPoint;  //圆心在基坐标系下的坐标点
    centerPoint.x = start.x + Radius * normalVector.x;
    centerPoint.y = start.y + Radius * normalVector.y;
    centerPoint.z = start.z + Radius * normalVector.z;
    Point3F  unit_x; //单位X向量
    float length_x = sqrt((start.x - centerPoint.x)*(start.x - centerPoint.x) + (start.y - centerPoint.y)*(start.y - centerPoint.y) + (start.z - centerPoint.z)*(start.z - centerPoint.z));
    unit_x.x = (start.x - centerPoint.x)/length_x;
    unit_x.y = (start.y - centerPoint.y)/length_x;
    unit_x.z = (start.z - centerPoint.z)/length_x;
    Point3F  unit_y; //单位y向量
    float length_y = sqrt(lineVector.x*lineVector.x + lineVector.y*lineVector.y + lineVector.z*lineVector.z);
    unit_y.x = lineVector.x/length_y;
    unit_y.y = lineVector.y/length_y;
    unit_y.z = lineVector.z /length_y;
    Point3F unit_z; //单位Z向量   利用x轴和y轴叉乘   叉乘要注意向量的顺序，不要颠倒  右手坐标系  单位向量叉乘还是单位向量（模）
    unit_z.x = unit_x.y*unit_y.z - unit_x.z*unit_y.y;
    unit_z.y = unit_x.z*unit_y.x - unit_x.x*unit_y.z;
    unit_z.z = unit_x.x*unit_y.y - unit_x.y*unit_y.x;

    pArc->pForm.R11 = unit_x.x;
    pArc->pForm.R12 = unit_y.x;
    pArc->pForm.R13 = unit_z.x;
    pArc->pForm.R21 = unit_x.y;
    pArc->pForm.R22 = unit_y.y;
    pArc->pForm.R23 = unit_z.y;
    pArc->pForm.R31 = unit_x.z;
    pArc->pForm.R32 = unit_y.z;
    pArc->pForm.R33 = unit_z.z;
    pArc->pForm.T1 = centerPoint.x;
    pArc->pForm.T2 = centerPoint.y;
    pArc->pForm.T3 = centerPoint.z;

    return pArc;
}

/********************************************
 *function:把100圆弧实体转换成126Nurbs曲线实体
 *input:100实体指针
 *output:126实体指针
 *adding:
 *author: wang
 *date: 2017/10/24
 *******************************************/
nurbsCurve_type *CreateWorkPiece::Arc2NurbsCurve(arc_type *pArc)
{
    nurbsCurve_type *pNurbsCurve = new nurbsCurve_type;
    memset(pNurbsCurve,0,sizeof(nurbsCurve_type));
    pNurbsCurve->typeNum = 126;
    pNurbsCurve->K_num = 64; //B样条曲线次数    取值自选，越大控制点越多
    pNurbsCurve->M_num = 2; //基函数次数
    pNurbsCurve->prop_plane = 1; //平面
    pNurbsCurve->prop_closed = 0;//不封闭曲线
    pNurbsCurve->prop_poly = 1;//多项式
    pNurbsCurve->prop_periodic = 0;//非周期
    int N = 1+pNurbsCurve->K_num - pNurbsCurve->M_num; //63
    int A = N +2* pNurbsCurve->M_num + 1; //68
    float *knot = new float[A];
    for(int i= 0;i<A;i++)
        knot[i] = knot_value[i];
    pNurbsCurve->pKnot = knot; //结点矢量数组
    //权重暂时不赋值，因为在opengl中画图时采用的是不考虑权重的方式，如果要赋值权重都为1

    //下面计算控制点K+1个  pArc中的圆心 起点 终点是用局部坐标和基坐标变换矩阵来表示，最后需要转成基坐标下的表示
    float deltaAngle = pArc->angle / pNurbsCurve->K_num;
    float angle_devide = 0;
    float *control_point = new float[3*(pNurbsCurve->K_num + 1)];
    float x,y,z;
    for(int i = 0;i< pNurbsCurve->K_num + 1;i++)
    {
        angle_devide = i * fabs(deltaAngle);
        x =sin(angle_devide);   //x = pArc->Radius*cos(angle_devide);
        y =(1-cos(angle_devide))/2;    //y = pArc->Radius*sin(angle_devide);
        z = 0 ;
//        matrixMulPoint(&(pArc->pForm),x,y,z);
        control_point[3*i] = x;
        control_point[3*i+1] = y;
        control_point[3*i+2] = z;
    }
    pNurbsCurve->pCtlarray = control_point;
    return pNurbsCurve;
}

/********************************************
 *function:生成圆柱面的128NURBS表示
 *input:母线110  轴线110  圆柱面转过的角度(度数)  母线的两端必须是圆弧的起点
 *output:128曲面指针
 *adding:角度的方向：从轴线的终点看向起点，逆时针方向为正
 *author: wang
 *date: 2017/10/25
 *******************************************/
nurbsSurface_type *CreateWorkPiece::createCylinderSurface(line_type generatrix, line_type axis, float angle)
{
    nurbsSurface_type *pNurbsSurface = new nurbsSurface_type;
    memset(pNurbsSurface,0,sizeof(nurbsSurface_type));
    pNurbsSurface->typeNum = 128;
    pNurbsSurface->K1_num = 40; //曲线
    pNurbsSurface->K2_num = 2;  //直线
    pNurbsSurface->M1_num = 2;
    pNurbsSurface->M2_num = 2;
    int N1_num = 1+pNurbsSurface->K1_num-pNurbsSurface->M1_num; //39
    int N2_num = 1+pNurbsSurface->K2_num-pNurbsSurface->M2_num; //1
    int A_num = N1_num+2*pNurbsSurface->M1_num+1; //44  加1是因为数组的关系  公式里是没有加1的
    int B_num = N2_num+2*pNurbsSurface->M2_num+1;//6
    int C_num = (1+pNurbsSurface->K1_num)*(1+pNurbsSurface->K2_num);//123

    //节点序列的赋值
    float *knot1 = new float[A_num];
    float *knot2 = new float[B_num];
    for(int i = 0; i < A_num; i++)
        knot1[i] = knot1_prepare[i];
    for(int i = 0; i < B_num; i++)
        knot2[i] = knot2_prepare[i];
    pNurbsSurface->pKnot1 = knot1;
    pNurbsSurface->pKnot2 = knot2;

    point3f axis_point;//中心旋转轴向量  等于是把旋转轴平移到了基坐标的原点
    axis_point.x = axis.end.x-axis.start.x;
    axis_point.y = axis.end.y-axis.start.y;
    axis_point.z = axis.end.z-axis.start.z;
    float old_x1,old_y1,old_z1,old_x2,old_y2,old_z2,old_midx,old_midy,old_midz;//
    old_x1 = generatrix.start.x-axis.start.x;
    old_y1 = generatrix.start.y-axis.start.y;
    old_z1 = generatrix.start.z-axis.start.z;
    old_x2 = generatrix.end.x-axis.start.x;
    old_y2 = generatrix.end.y-axis.start.y;
    old_z2 = generatrix.end.z-axis.start.z;
    old_midx = (old_x1+old_x2)/2.0;
    old_midy = (old_y1+old_y2)/2.0;
    old_midz = (old_z1+old_z2)/2.0;
    //计算曲面的控制点
    int num = pNurbsSurface->K1_num+1;
    float *ctldata = new float[C_num*3];
    float angle_divide = 0;
    for(int i = 0; i < num; i++) //循环40次
    {
        angle_divide = (angle/pNurbsSurface->K1_num)*i;//旋转角度离散化
        xform_type *matrix = formRevolutinMatrix(axis_point,angle_divide,axis.start);
        ctldata[9*i] = old_x1;
        ctldata[9*i+1] = old_y1;
        ctldata[9*i+2] = old_z1;
        ctldata[9*i+3] = old_midx;
        ctldata[9*i+4] = old_midy;
        ctldata[9*i+5] = old_midz;
        ctldata[9*i+6] = old_x2;
        ctldata[9*i+7] = old_y2;
        ctldata[9*i+8] = old_z2;
        matrixMulPoint(matrix,ctldata[9*i],ctldata[9*i+1],ctldata[9*i+2]);
        matrixMulPoint(matrix,ctldata[9*i+3],ctldata[9*i+4],ctldata[9*i+5]);
        matrixMulPoint(matrix,ctldata[9*i+6],ctldata[9*i+7],ctldata[9*i+8]);
        delete matrix;
    }

    pNurbsSurface->pCtlarray = ctldata;
    return pNurbsSurface;
}

/********************************************
 *function:生成平面的128NURBS表示
 *input:两条平行的110实体
 *output:128曲面指针
 *adding:两个110实体组成的多边形要能将裁剪曲面包含在内  line1和line2要同向，不能反向
 *author: wang
 *date: 2017/10/25
 *******************************************/
nurbsSurface_type *CreateWorkPiece::createPlaneSurface(line_type line1, line_type line2)
{
    nurbsSurface_type *pNurbsSurface = new nurbsSurface_type;
    memset(pNurbsSurface,0,sizeof(nurbsSurface_type));
    pNurbsSurface->typeNum = 128;
    pNurbsSurface->K1_num = 1; //B样条1曲线次数
    pNurbsSurface->K2_num = 1; //B样条2曲线次数
    pNurbsSurface->M1_num = 1; //基函数次数
    pNurbsSurface->M2_num = 1; //基函数次数
    pNurbsSurface->prop1_closed = 0; //不封闭
    pNurbsSurface->prop2_closed = 0;  //不封闭
    pNurbsSurface->prop_poly = 1; //多项式
    pNurbsSurface->prop1_periodic = 0; //非周期
    pNurbsSurface->prop2_periodic = 0; //非周期
    int N1_num = 1+pNurbsSurface->K1_num-pNurbsSurface->M1_num; //1
    int N2_num = 1+pNurbsSurface->K2_num-pNurbsSurface->M2_num; //1
    int A_num = N1_num+2*pNurbsSurface->M1_num+1; //4  加1是因为数组的关系  公式里是没有加1的
    int B_num = N2_num+2*pNurbsSurface->M2_num+1;//4
    int C_num = (1+pNurbsSurface->K1_num)*(1+pNurbsSurface->K2_num);//4

    //给节点序列赋值
    float * knot1 = new float [A_num]; //A_num
    float * knot2 = new float [B_num]; //B_num
    knot1[0] = 0.0;
    knot1[1] = 0.0;
    knot1[2] = 1.0;
    knot1[3] = 1.0;
    knot2[0] = 0.0;
    knot2[1] = 0.0;
    knot2[2] = 1.0;
    knot2[3] = 1.0;
    pNurbsSurface->pKnot1 = knot1;
    pNurbsSurface->pKnot2 = knot2;

    //权重都是1  这边和曲面一样，不对曲面赋值

    //控制点计算
     float *ctldata = new float [C_num*3];  //C_num*3
     float pCtlData[C_num][3];
     float ptmp[C_num][3];

//     ctldata[0] = line1.start.x;
//     ctldata[1] = line1.start.y;
//     ctldata[2] = line1.start.z;
//     ctldata[3] = line2.start.x;
//     ctldata[4] = line2.start.y;
//     ctldata[5] = line2.start.z;
//     ctldata[6] = line1.end.x;
//     ctldata[7] = line1.end.y;
//     ctldata[8] = line1.end.z;
//     ctldata[9] = line2.end.x;
//     ctldata[10] = line2.end.y;
//     ctldata[11] = line2.end.z;

     ctldata[0] = line1.start.x;
     ctldata[1] = line1.start.y;
     ctldata[2] = line1.start.z;
     ctldata[3] = line1.end.x;
     ctldata[4] = line1.end.y;
     ctldata[5] = line1.end.z;
     ctldata[6] = line2.start.x;
     ctldata[7] = line2.start.y;
     ctldata[8] = line2.start.z;
     ctldata[9] = line2.end.x;
     ctldata[10] = line2.end.y;
     ctldata[11] = line2.end.z;


//     pCtlData[0][0] = line1.start.x;
//     pCtlData[0][1] = line1.start.y;
//     pCtlData[0][2] = line1.start.z;
//     pCtlData[1][0] = line1.end.x;
//     pCtlData[1][1] = line1.end.y;
//     pCtlData[1][2] = line1.end.z;
//     pCtlData[2][0] = line2.start.x;
//     pCtlData[2][1] = line2.start.y;
//     pCtlData[2][2] = line2.start.z;
//     pCtlData[3][0] = line2.end.x;
//     pCtlData[3][1] = line2.end.y;
//     pCtlData[3][2] = line2.end.z;
//     ptmp[0][0] = line1.start.x;
//     ptmp[0][1] = line1.start.y;
//     ptmp[0][2] = line1.start.z;
//     ptmp[1][0] = line1.end.x;
//     ptmp[1][1] = line1.end.y;
//     ptmp[1][2] = line1.end.z;
//     ptmp[2][0] = line2.start.x;
//     ptmp[2][1] = line2.start.y;
//     ptmp[2][2] = line2.start.z;
//     ptmp[3][0] = line2.end.x;
//     ptmp[3][1] = line2.end.y;
//     ptmp[3][2] = line2.end.z;

//     for(int i = 0; i < pNurbsSurface->K1_num+1; i++)
//     {
//         for(int j = 0; j < pNurbsSurface->K2_num+1; j++)
//         {
//             pCtlData[i*(pNurbsSurface->K2_num+1)+j][0] = ptmp[j*(pNurbsSurface->K1_num+1)+i][0];
//             pCtlData[i*(pNurbsSurface->K2_num+1)+j][1] = ptmp[j*(pNurbsSurface->K1_num+1)+i][1];
//             pCtlData[i*(pNurbsSurface->K2_num+1)+j][2] = ptmp[j*(pNurbsSurface->K1_num+1)+i][2];
//         }
//     }
//     for(int i = 0; i < C_num; i++)
//     {
//         ctldata[i*3] = pCtlData[i][0];
//         ctldata[i*3+1] = pCtlData[i][1];
//         ctldata[i*3+2] = pCtlData[i][2];
//     }
     pNurbsSurface->pCtlarray = ctldata;

     return pNurbsSurface;
}

/********************************************
 *function:创建144的矩形平面  裁剪曲面
 *input:平面的两条同向直线, isBendLine true:是折弯线  false:不是折弯线
 *output:144实体对象的指针
 *adding:四个标记分别对应  line1 start   line1 end  line2 end line2 start  围成的四边形的四条边
 *author: wang
 *date: 2017/10/26
 *******************************************/
trimmed_surface_type *CreateWorkPiece::createPlaneTrimmedSurface(line_type line1, line_type line2, bool isBendLine, bool isBendLine2,bool isBendLine3,bool isBendLine4)
{
    trimmed_surface_type *pNurbsTrimmed = new trimmed_surface_type;
    memset(pNurbsTrimmed,0,sizeof(trimmed_surface_type));
    pNurbsTrimmed->typeNum = 144;
    pNurbsTrimmed->bBendSurface = false; //平面不是折弯面
    pNurbsTrimmed->angle = 0;
    pNurbsTrimmed->boundary = 0; //矩形平面不需要进行外环的裁剪，只有圆环面需要进行裁剪
    pNurbsTrimmed->curve_num = 0;//矩形平面上的封闭曲线的个数  内环    决定了curveCount数组内存了几个
    //平面
    pNurbsTrimmed->pSurface = createPlaneSurface(line1,line2); //创建基曲面，返回指针
    pNurbsTrimmed->surfaceType = 128;
    //裁剪曲线（这个只有外环） 裁剪曲线的存放顺序要么按逆时针要么顺时针 不能没有顺序的放
    pNurbsTrimmed->curveCount[0] = 4; //这个数组内只有一个赋值 因为只有一个封闭环  是外环
    pNurbsTrimmed->curveType[0][0] = 110; //四条直线
    pNurbsTrimmed->pCurves[0][0] = createLine(line1.start,line1.end,isBendLine);
    pNurbsTrimmed->curveType[0][1] = 110;
    pNurbsTrimmed->pCurves[0][1] = createLine(line1.end,line2.end,isBendLine2);
    pNurbsTrimmed->curveType[0][2] = 110;
    pNurbsTrimmed->pCurves[0][2] = createLine(line2.end,line2.start,isBendLine3);
    pNurbsTrimmed->curveType[0][3] = 110;
    pNurbsTrimmed->pCurves[0][3] = createLine(line2.start,line1.start,isBendLine4);
    //外轮廓
    pNurbsTrimmed->outlineCount[0] = 4;
    pNurbsTrimmed->outlineType[0][0] = 110;
    pNurbsTrimmed->pOutlines[0][0] = createLine(line1.start,line1.end,isBendLine);
    pNurbsTrimmed->outlineType[0][1] = 110;
    pNurbsTrimmed->pOutlines[0][1] = createLine(line1.end,line2.end,isBendLine2);
    pNurbsTrimmed->outlineType[0][2] = 110;
    pNurbsTrimmed->pOutlines[0][2] = createLine(line2.end,line2.start,isBendLine3);
    pNurbsTrimmed->outlineType[0][3] = 110;
    pNurbsTrimmed->pOutlines[0][3] = createLine(line2.start,line1.start,isBendLine4);

    pNurbsTrimmed->pNext = NULL;
    return pNurbsTrimmed;
}

/********************************************
 *function:创建圆柱面的144实体
 *input:母线  轴线   转过的角度(度数) 是否是折弯面
 *output:
 *adding:一般来说圆柱面都是折弯面，但从2d到3d的过程中，需要用小圆弧衔接两端直线，但不是折弯面，因此需要标志位进行区分
 *author: wang
 *date: 2017/10/26
 *******************************************/
trimmed_surface_type *CreateWorkPiece::createCylinderTrimmedSurface(line_type generatrix, line_type axis, float angle, bool isBend1, bool isBend2)
{
    trimmed_surface_type *pNurbsTrimmed = new trimmed_surface_type;
    memset(pNurbsTrimmed,0,sizeof(trimmed_surface_type));
    pNurbsTrimmed->typeNum = 144;
    pNurbsTrimmed->bBendSurface = isBend1;
    if(angle>0)
        pNurbsTrimmed->angleFlag = true;
    else
        pNurbsTrimmed->angleFlag = false;
    pNurbsTrimmed->axis = axis;
    pNurbsTrimmed->generatrix = generatrix;
    pNurbsTrimmed->start_angle = 0;
    pNurbsTrimmed->end_angle = angle;
    pNurbsTrimmed->angle = angle;
    pNurbsTrimmed->boundary = 0; //圆柱面也不需要进行裁剪 因为获得的NURBS曲面的控制点就在外环上，不用再裁剪了
    pNurbsTrimmed->curve_num = 0;//内环封闭曲线个数   决定了curveCount数组内存了几个

    //平面
    pNurbsTrimmed->pSurface = createCylinderSurface(generatrix,axis,angle);
    pNurbsTrimmed->surfaceType = 120;
    //先求圆弧半径
    float x = generatrix.start.x - axis.start.x;
    float y = generatrix.start.y - axis.start.y;
    float z = generatrix.start.z - axis.start.z;
    float radius = sqrt(x*x+y*y+z*z);
    //先把另一条直线边界求出来
    point3f axis_point;//旋转轴
    axis_point.x = axis.end.x - axis.start.x;
    axis_point.y = axis.end.y - axis.end.y;
    axis_point.z = axis.end.z - axis.end.z;
    xform_type *matrix = formRevolutinMatrix(axis_point,angle,axis.start); //绕轴心旋转
    point3f start,end;
    start.x = generatrix.start.x - axis.start.x;
    start.y = generatrix.start.y - axis.start.y;
    start.z = generatrix.start.z - axis.start.z;
    end.x = generatrix.end.x - axis.start.x;
    end.y = generatrix.end.y - axis.start.y;
    end.z = generatrix.end.z - axis.start.z;
    matrixMulPoint(matrix,start.x,start.y,start.z);
    matrixMulPoint(matrix,end.x,end.y,end.z);

    //裁剪曲线（这个只有外环） 和外轮廓线
    pNurbsTrimmed->curveCount[0] = 4; //这个数组内只有一个赋值 因为只有一个封闭环  是外环
    pNurbsTrimmed->outlineCount[0] = 4;
    pNurbsTrimmed->curveType[0][0] = 110;
    pNurbsTrimmed->pCurves[0][0] =  createLine(generatrix.start,generatrix.end,isBend1);
    pNurbsTrimmed->outlineType[0][0] = 110;
    pNurbsTrimmed->pOutlines[0][0] = createLine(generatrix.start,generatrix.end,isBend1);

    pNurbsTrimmed->curveType[0][1] = 126; //先用100表示圆弧再转成126
    pNurbsTrimmed->outlineType[0][1] = 100; //在画外部轮廓时是直接用画圆弧的方式，所以不需要转成126
    Point3F normalVector,lineVector; //起点指向圆心的向量  与圆弧相切，向量以start为起点
    normalVector.x = (axis.end.x - generatrix.end.x)/radius;
    normalVector.y = (axis.end.y - generatrix.end.y)/radius;
    normalVector.z = (axis.end.z - generatrix.end.z)/radius;
    point3f rotate;
    rotate.x = generatrix.end.x - generatrix.start.x;
    rotate.y = generatrix.end.y - generatrix.start.y;
    rotate.z = generatrix.end.z - generatrix.start.z;
    lineVector = normalVector;
    //旋转90 成为切线
    point3f origin;
    origin.x =0 ;
    origin.y = 0;
    origin.z = 0;
    matrix = formRevolutinMatrix(rotate,-90,origin); //generatrix.end
    matrixMulPoint(matrix,lineVector.x,lineVector.y,lineVector.z);

    arc_type *pArc = createArc(generatrix.end,radius,angle,normalVector,lineVector);
    nurbsCurve_type *pCircle = Arc2NurbsCurve(pArc);
    pNurbsTrimmed->pCurves[0][1] = pCircle;
    pNurbsTrimmed->pOutlines[0][1] = pArc;

    pNurbsTrimmed->curveType[0][2] = 110;
    pNurbsTrimmed->outlineType[0][2] = 110;
    pNurbsTrimmed->pCurves[0][2] = createLine(end,start,isBend2);
    pNurbsTrimmed->pOutlines[0][2] = createLine(end,start,isBend2);

    pNurbsTrimmed->curveType[0][3] = 126;
    pNurbsTrimmed->outlineType[0][3] = 100;
    normalVector.x = (axis.start.x - end.x)/radius;
    normalVector.y = (axis.start.y - end.y)/radius;
    normalVector.z = (axis.start.z - end.z)/radius;
    rotate.x = end.x - start.x;
    rotate.y = end.y - start.y;
    rotate.z = end.z - start.z;
    lineVector = normalVector;
    //旋转90 成为切线
    matrix = formRevolutinMatrix(rotate,-90,origin);//generatrix.end
    matrixMulPoint(matrix,lineVector.x,lineVector.y,lineVector.z);

    arc_type *pArc2 = createArc(start,radius,-angle,normalVector,lineVector);
    nurbsCurve_type *pCircle2 = Arc2NurbsCurve(pArc2);
     pNurbsTrimmed->pCurves[0][3] = pCircle2;
     pNurbsTrimmed->pOutlines[0][3] = pArc2;

     pNurbsTrimmed->pNext = NULL;
     return pNurbsTrimmed;
}

/********************************************
 *function:创建圆环面的144实体
 *input:起点所在的母线  圆心轴线   角度(角度)  板料厚度  lineVector是单位切向量
 *output:144实体对象的指针
 *adding:圆环面的各个顶点是line_type的end点，不能是start
 *author: wang
 *date: 2017/10/27
 *******************************************/
trimmed_surface_type *CreateWorkPiece::createRingTrimmedSurface(line_type genertrix, line_type axis, float angle, float thicness)
{
    trimmed_surface_type *pNurbsTrimmed = new trimmed_surface_type;
    memset(pNurbsTrimmed,0,sizeof(trimmed_surface_type));
    pNurbsTrimmed->typeNum = 144;
    pNurbsTrimmed->bBendSurface = false;
    pNurbsTrimmed->angle = 0;  //不是折弯面
    pNurbsTrimmed->boundary = 1; //圆环面需要对矩形平面进行裁剪
    pNurbsTrimmed->curve_num = 0;//矩形平面上的封闭曲线的个数  内环    决定了curveCount数组内存了几个 curve_num+1

    //确定平面
    pNurbsTrimmed->surfaceType = 128;
    line_type line1,line2;
    //先求从圆弧起点指向圆心的单位法向量
    point3f normalVector;
    normalVector.x = axis.end.x - genertrix.end.x;
    normalVector.y = axis.end.y - genertrix.end.y;
    normalVector.z = axis.end.z - genertrix.end.z;
    float radius = sqrt(normalVector.x*normalVector.x + normalVector.y*normalVector.y+normalVector.z*normalVector.z);
    normalVector.x = normalVector.x/radius;
    normalVector.y = normalVector.y/radius;
    normalVector.z = normalVector.z/radius;
    point3f lineVector;
    lineVector = normalVector;
    xform_type *matrix;
    point3f origin;
    origin.x =0 ;
    origin.y = 0;
    origin.z = 0;
    point3f rotate;
    rotate.x = genertrix.end.x - genertrix.start.x;
    rotate.y = genertrix.end.y - genertrix.start.y;
    rotate.z = genertrix.end.z - genertrix.start.z;
    if(angle>0)
    {
        matrix = formRevolutinMatrix(rotate,-90,origin);  //genertrix.end
    }
    else
    {
        matrix = formRevolutinMatrix(rotate,90,origin);  //genertrix.end
    }
    matrixMulPoint(matrix,lineVector.x,lineVector.y,lineVector.z);
//    line1.start = genertrix.end;
    line1.start = genertrix.end;
    line1.end.x = genertrix.end.x + 2*radius*normalVector.x;
    line1.end.y = genertrix.end.y + 2*radius*normalVector.y;
    line1.end.z = genertrix.end.z + 2*radius*normalVector.z;

    line2.start.x = line1.start.x + radius*lineVector.x;
    line2.start.y = line1.start.y + radius*lineVector.y;
    line2.start.z = line1.start.z + radius*lineVector.z;
    line2.end.x = line1.end.x + radius*lineVector.x;
    line2.end.y = line1.end.y + radius*lineVector.y;
    line2.end.z = line1.end.z + radius*lineVector.z;



    pNurbsTrimmed->pSurface = createPlaneSurface(line1,line2); //创建基曲面，返回指针

    //确定裁剪曲线和外轮廓线
    pNurbsTrimmed->curveCount[0] = 4; //这个数组内只有一个赋值 因为只有一个封闭环  是外环
    pNurbsTrimmed->outlineCount[0] = 4;
    pNurbsTrimmed->curveType[0][0] = 126;
    pNurbsTrimmed->outlineType[0][0] = 100;

    lineVector = normalVector;
    //旋转90 成为切线
    matrix = formRevolutinMatrix(rotate,-90,origin);  //genertrix.end
    matrixMulPoint(matrix,lineVector.x,lineVector.y,lineVector.z);
    arc_type *pArc = createArc(genertrix.end,radius,angle,normalVector,lineVector);
    nurbsCurve_type *pCircle = Arc2NurbsCurve(pArc);
    pNurbsTrimmed->pCurves[0][0] = pCircle;
    pNurbsTrimmed->pOutlines[0][0] = pArc;

    pNurbsTrimmed->curveType[0][1] = 110;
    pNurbsTrimmed->outlineType[0][1] = 110;
    point3f center2star;//第二个点
    center2star.x = pArc->X3;
    center2star.y = pArc->Y3;
    center2star.z = 0;
    matrixMulPoint(&(pArc->pForm),center2star.x,center2star.y,center2star.z);
    point3f normalVector2;
    normalVector2.x = axis.end.x - center2star.x ;
    normalVector2.y = axis.end.y - center2star.y ;
    normalVector2.z = axis.end.z - center2star.z ;
    float length2 = sqrt(normalVector2.x*normalVector2.x+normalVector2.y*normalVector2.y + normalVector2.z *normalVector2.z );
    normalVector2.x = normalVector2.x/length2;
    normalVector2.y = normalVector2.y/length2;
    normalVector2.z = normalVector2.z/length2;
    center2star.x = sin(fabs(angle* angle2Radian));
    center2star.y = (1-cos(fabs(angle* angle2Radian)))/2;
    center2star.z = 0;

    point3f point3; //第三个点
//    point3.x = center2star.x + thicness*normalVector2.x;
//    point3.y = center2star.y + thicness*normalVector2.y;
//    point3.z = center2star.z + thicness*normalVector2.z;
    point3.x = (1 - thicness/radius)*sin(fabs(angle* angle2Radian));
    point3.y = (1-(1-thicness/radius)*cos(fabs(angle* angle2Radian)))/2;
    point3.z = 0;
    pNurbsTrimmed->pCurves[0][1] = createLine(center2star,point3,false);
    pNurbsTrimmed->pOutlines[0][1] = createLine(center2star,point3,false);

    pNurbsTrimmed->curveType[0][2] = 126;
    pNurbsTrimmed->outlineType[0][2] = 100;

    point3f lineVector2;
    lineVector2.x = normalVector2.x ;
    lineVector2.y = normalVector2.y ;
    lineVector2.z = normalVector2.z ;
    point3f rotate2;
    rotate2.x = axis.end.x - axis.start.x;
    rotate2.y = axis.end.y - axis.start.y;
    rotate2.z = axis.end.z - axis.start.z;

    matrix = formRevolutinMatrix(rotate2,-90,origin);
    matrixMulPoint(matrix,lineVector2.x,lineVector2.y,lineVector2.z); //得到圆弧终点坐标

    arc_type *pArc2 = createArc(point3,radius-thicness,-angle,normalVector2,lineVector2);
    nurbsCurve_type *pCircle2 = Arc2NurbsCurve(pArc2);
    delete pCircle2->pCtlarray;

    //下面计算控制点K+1个  pArc中的圆心 起点 终点是用局部坐标和基坐标变换矩阵来表示，最后需要转成基坐标下的表示
    float deltaAngle = pArc2->angle / 64;
    float angle_devide = 0;
    float *control_point = new float[3*(64 + 1)];
    float x,y,z;
    for(int i = 0;i<64 + 1;i++)
    {
        angle_devide = (64-i) * fabs(deltaAngle);
        x = (1 - thicness/radius)*sin(fabs(angle_devide));   //x = pArc->Radius*cos(angle_devide);
        y = (1-(1-thicness/radius)*cos(fabs(angle_devide)))/2;    //y = pArc->Radius*sin(angle_devide);
        z = 0 ;
//        matrixMulPoint(&(pArc->pForm),x,y,z);
        control_point[3*i] = x;
        control_point[3*i+1] = y;
        control_point[3*i+2] = z;
    }
    pCircle2->pCtlarray = control_point;

    pNurbsTrimmed->pCurves[0][2] = pCircle2;
    pNurbsTrimmed->pOutlines[0][2] = pArc2;

    pNurbsTrimmed->curveType[0][3] = 110;
    pNurbsTrimmed->outlineType[0][3] = 110;
    point3f point4; //第四个点
//    point4.x = pArc2->X3;
//    point4.y = pArc2->Y3;
//    point4.z = 0;
//    matrixMulPoint(&(pArc2->pForm),point4.x,point4.y,point4.z);

//    point4.x = genertrix.end.x + thicness*normalVector.x;
//    point4.y = genertrix.end.y + thicness*normalVector.y;
//    point4.z = genertrix.end.z + thicness*normalVector.z;

    point4.x = 0;
    point4.y = thicness/(2*radius);
    point4.z = 0;
    genertrix.end.x = 0;
    genertrix.end.y = 0;
    genertrix.end.z = 0;

    pNurbsTrimmed->pCurves[0][3] = createLine(point4,genertrix.end,false);
    pNurbsTrimmed->pOutlines[0][3] = createLine(point4,genertrix.end,false);

    pNurbsTrimmed->pNext = NULL;
    return pNurbsTrimmed;
}

/********************************************
 *function:变换矩阵右乘三维坐标，得到变换后的三维坐标
 *input:变换矩阵  坐标
 *output:
 *adding:按引用方式传入坐标值
 *author: wang
 *date: 2017/10/25
 *******************************************/
void CreateWorkPiece::matrixMulPoint(xform_type *pForm, float &x_pos, float &y_pos, float &z_pos)
{
    if(pForm == NULL)
        return;
    float x,y,z;
    x = x_pos;
    y = y_pos;
    z = z_pos;
    x_pos = pForm->R11*x+pForm->R12*y+pForm->R13*z+pForm->T1;
    y_pos = pForm->R21*x+pForm->R22*y+pForm->R23*z+pForm->T2;
    z_pos = pForm->R31*x+pForm->R32*y+pForm->R33*z+pForm->T3;
}

/********************************************
 *function:绕过原点的任意旋转轴旋转angle  再平移off  的变换矩阵
 *input:以原点为起点的旋转轴   旋转角度(度数)   平移矢量
 *output:变换矩阵
 *adding:
 *author: wang
 *date: 2017/10/25
 *******************************************/
xform_type *CreateWorkPiece::formRevolutinMatrix(point3f axis, float angle, point3f off)
{
    angle = angle*angle2Radian;
    xform_type *pform = new xform_type;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float normal = sqrt(x*x+y*y+z*z);
    //求出旋转轴的单位向量
    x = x/normal;
    y = y/normal;
    z = z/normal;
    float c = cos(angle);
    float s = sin(angle);
    //通用旋转变换矩阵
    pform->R11 = x*x*(1-c)+c;
    pform->R12 = x*y*(1-c)-z*s;
    pform->R13 = x*z*(1-c)+y*s;
    pform->R21 = y*x*(1-c)+z*s;
    pform->R22 = y*y*(1-c)+c;
    pform->R23 = y*z*(1-c)-x*s;
    pform->R31 = x*z*(1-c)-y*s;
    pform->R32 = y*z*(1-c)+x*s;
    pform->R33 = z*z*(1-c)+c;
    pform->T1 = off.x;
    pform->T2 = off.y;
    pform->T3 = off.z;
    return pform;
}
