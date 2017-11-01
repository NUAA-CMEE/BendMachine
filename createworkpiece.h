#ifndef CREATEWORKPIECE_H
#define CREATEWORKPIECE_H

#include"math.h"
#include<QDebug>
#include"SysGLData.h"

class CreateWorkPiece
{
public:
    CreateWorkPiece();

public:
    line_type *createLine(Point3F start_point, Point3F end_point,bool isBendLine);
    arc_type  *createArc(Point3F start, float Radius, float angle, Point3F normalVector, Point3F lineVector);
    nurbsCurve_type *Arc2NurbsCurve(arc_type *pArc);

    nurbsSurface_type *createCylinderSurface(line_type generatrix,line_type axis,float start_angle);
    nurbsSurface_type *createPlaneSurface(line_type line1,line_type line2);
    trimmed_surface_type *createPlaneTrimmedSurface(line_type line1,line_type line2,bool isBendLine,bool isBendLine2,bool isBendLine3,bool isBendLine4);
    trimmed_surface_type *createCylinderTrimmedSurface(line_type generatrix, line_type axis, float angle,bool isBend1,bool isBend2);
    trimmed_surface_type *createRingTrimmedSurface(line_type genertrix,line_type axis,float angle,float thicness);

    //矩阵计算及变换
    void matrixMulPoint(xform_type *pForm, float &x_pos, float &y_pos, float &z_pos);
    xform_type *formRevolutinMatrix(point3f axis, float angle,point3f off);
};

#endif // CREATEWORKPIECE_H
