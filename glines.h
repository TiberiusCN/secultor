#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#define acu_div 0.1
#define D_PI 3.14159265
#define D_DELTA 0.0001

#include <GL/gl.h>
#include <math.h>
#include <stdio.h>

typedef struct isomove_t
{
	int code;
	float* params;
	struct isomove_t* next;
	struct isomove_t* prev;
} isomove_t;

typedef struct xpoint_t
{
	float x,y;
} xpoint_t;

typedef struct xline_t
{
	int count;
	xpoint_t* points;
	struct xline_t* next;
} xline_t;

typedef struct sLine_t
{
	float x1;
	float y1;
	float x2;
	float y2;
	float xc;
	float yc;
	int cw; //-1 CCW, 0 Line, +1 CW
} sLine_t, sArc_t;

int MakeXArc(xline_t *line, float ax1, float ay1, float ri, float rj, float ax2, float ay2, int cw, float maxrdif);
int MakeXLine(xline_t *line, float ax1, float ay1, float ax2, float ay2);
void DrawXLine(xline_t *line);

//correction value < 0 => left; abs
void EqLine(sLine_t* source, sLine_t* dest, float cval); 
void EqArc(sArc_t* source, sArc_t* dest, float cval);

void ArcRelToAbs(sArc_t* arc); //c = i,j -> x,y
void ArcAbsToRel(sArc_t* arc); //c = x,y -> i,j

//abs
//rad optional
float ArcAngle1(sArc_t* arc, float* rad);
float ArcAngle2(sArc_t* arc, float* rad);

//abs
int ArcArcInS  (sArc_t* A1,  sArc_t* A2,  float *fx1, float *fy1, float *fx2, float *fy2);
int ArcLineInS (sArc_t* A1,  sLine_t* L1, float *fx1, float *fy1, float *fx2, float *fy2);
int LineLineInS(sLine_t* L1, sLine_t* L2, float *fx, float *fy);
void LineNormal(sLine_t* source, sLine_t* dest);
void LineNormalize(sLine_t* line);

//abs
//ret = 0 -- not a part of line
//точка принадлежит прямой или окружности, проверяется принадлежность отрезку и дуге
int PointInLine(sLine_t* line, float x, float y);
int PointInArc(sArc_t* arc, float x, float y);

//способы подгона
int AdjustLLViaPoint(sLine_t* l1, sLine_t* l2);
int AdjustLAViaPoint(sLine_t* l1, sArc_t* l2 );
int AdjustALViaPoint(sArc_t* l1,  sLine_t* l2);
int AdjustAAViaPoint(sArc_t* l1,  sArc_t* l2 );
int AdjustViaPoint(sLine_t* l1, sLine_t* l2);
