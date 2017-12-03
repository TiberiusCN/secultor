#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#pragma once

#define D_LINEUT_PI 3.14159265
#define D_LINEUT_DELTA 0.0001

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct base_t
{
	float cw; // -1 ccw, 0 line, +1 cw
	float x1,y1,z1;
	float x2,y2,z2;
	float xc,yc,zc; //normal from center of arc
} base_t;


//correction value < 0 => left; abs
void lu_2dEqLine(base_t* source, base_t* dest, float dist); 
void lu_2dEqArc(base_t* source, base_t* dest, float dist);

void lu_2dArcRelToAbs(base_t* arc); //c = i,j -> x,y
void lu_2dArcAbsToRel(base_t* arc); //c = x,y -> i,j

//abs
//rad optional
float lu_2dArcAngle1(base_t* arc, float* rad);
float lu_2dArcAngle2(base_t* arc, float* rad);

//abs
int  lu_2dArcArcInS  (base_t* A1,     base_t* A2, float *fx1, float *fy1, float *fx2, float *fy2);
int  lu_2dArcLineInS (base_t* A1,     base_t* L1, float *fx1, float *fy1, float *fx2, float *fy2);
int  lu_2dLineLineInS(base_t* L1,     base_t* L2, float *fx, float *fy);
void lu_2dLineNormal (base_t* source, base_t* dest);
void lu_2dLineNormalize(base_t* line);

//abs
//ret = 0 -- not a part of line
//точка принадлежит прямой или окружности, проверяется принадлежность отрезку и дуге
int lu_2dPointInLine(base_t* line, float x, float y);
int lu_2dPointInArc(base_t* arc, float x, float y);

//способы подгона
int lu_2dAdjustLLViaPoint(base_t* l1, base_t* l2);
int lu_2dAdjustLAViaPoint(base_t* l1, base_t* l2 );
int lu_2dAdjustALViaPoint(base_t* l1,  base_t* l2);
int lu_2dAdjustAAViaPoint(base_t* l1,  base_t* l2 );
int lu_2dAdjustViaPoint(base_t* l1, base_t* l2); 
