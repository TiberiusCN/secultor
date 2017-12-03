/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "lineut.h"

void lu_2dEqLine(base_t* source, base_t* dest, float dist)
{
	/*
	cval < 0 => contra HS (+D_LINEUT_PI/2)
	cval >= 0 => cum HS (-D_LINEUT_PI/2)
	
	rotation matrixa XY:
	cos  sin 0
	-sin cos 0
	0    0   1
	
	CCW matrixa:	CW matrixa:
	0 -1 0			0  1 0
	1  0 0			-1 0 0
	0  0 1			0  0 1
	
	CCW:		CW:
	x = -y;		x = y;
	y = x;		y = -x;
	*/
	
	/* relativi = p2-p1; */
	float rx = source->x2 - source->x1;
	float ry = source->y2 - source->y1;
	dest->x2 = rx;
	dest->y2 = ry;
	dest->cw = source->cw;
	
	//dx, dy = rx, ry / l
	float rad = sqrt(rx*rx+ry*ry);
	rx = (rx/rad)*dist;
	ry = (ry/rad)*dist;
	
	dest->x1 = 0.0-ry+source->x1;
	dest->y1 = 0.0+rx+source->y1;
	dest->x2 = dest->x2-ry+source->x1;
	dest->y2 = dest->y2+rx+source->y1;
	dest->z1 = dest->z2 = source->z1;
}
	
void lu_2dEqArc(base_t* source, base_t* dest, float dist)
{
	/*
	sinister + cum horae sagitta	= R+
	sinister + contra horae sagitta	= R-
	dexter + cum horae sagitta		= R-
	dexter + contra horae sagitta 	= R+
	*/
	
	float rx = source->x1-source->xc;
	float ry = source->y1-source->yc;
	float rad = sqrt(rx*rx+ry*ry);
	dest->xc = source->xc;
	dest->yc = source->yc;
	dest->cw = source->cw;
	if( source->cw < 0 ) dist = 0.0-dist;
	
	//relativi p1, p2
	dest->x1 = source->x1-source->xc;
	dest->x2 = source->x2-source->xc;
	dest->y1 = source->y1-source->yc;
	dest->y2 = source->y2-source->yc;
	
	rad = (rad+dist)/rad;
	
	dest->x1 *= rad;
	dest->x2 *= rad;
	dest->y1 *= rad;
	dest->y2 *= rad;
	
	dest->x1 += dest->xc;
	dest->x2 += dest->xc;
	dest->y1 += dest->yc;
	dest->y2 += dest->yc;
	dest->z1 = dest->z2 = source->z1;
}

void lu_2dArcRelToAbs(base_t* arc)
{
	arc->xc += arc->x1;
	arc->yc += arc->y1;
}

void lu_2dArcAbsToRel(base_t* arc)
{
	arc->xc -= arc->x1;
	arc->yc -= arc->y1;
}

float lu_2dArcAngle1(base_t* arc, float* rad)
{
	float rx = arc->x1-arc->xc;
	float ry = arc->y1-arc->yc;
	float r = sqrt(rx*rx+ry*ry);
	if(rad) *rad = r;
	
	float xcos = rx/r;
	float ysin = ry/r;

	float angle = acos(xcos);
	if(ysin < 0) angle = 2*D_LINEUT_PI - angle; //090-270
	return angle;
}

float lu_2dArcAngle2(base_t* arc, float* rad)
{
	float rx = arc->x2-arc->xc;
	float ry = arc->y2-arc->yc;
	float r = sqrt(rx*rx+ry*ry);
	if(rad) *rad = r;
	
	float xcos = rx/r;
	float ysin = ry/r;

	float angle = acos(xcos);
	if(ysin < 0) angle = 2*D_LINEUT_PI - angle; //090-270
	return angle;
}


int lu_2dArcArcInS(base_t* A1, base_t* A2, float *fx1, float *fy1, float *fx2, float *fy2)
{
	base_t rads;
	rads.x1 = A1->xc;
	rads.x2 = A2->xc;
	rads.y1 = A1->yc;
	rads.y2 = A2->yc;
	
	float rx,ry;
	
	rx = A1->x2-A1->xc;
	ry = A1->y2-A1->yc;
	float r1 = sqrt(rx*rx+ry*ry);
	rx = A2->x2-A2->xc;
	ry = A2->y2-A2->yc;
	float r2 = sqrt(rx*rx+ry*ry);
	
	rx = rads.x2-rads.x1;
	ry = rads.y2-rads.y1;
	float l = sqrt(rx*rx+ry*ry);
	
	float sr = r1+r2;
	
	if( l > sr ) return 0;
	
	if( fabs(l - sr) < D_LINEUT_DELTA )
	{
		r1 /= l;
		*fx1 = rads.x1+(rads.x2-rads.x1)*r1;
		*fy1 = rads.y1+(rads.y2-rads.y1)*r1;
		return 1;
	} else {
		/*
			l делится нормалью на a (r1->n) & b (r2->n)
			пусть h -- высота нормали
			т.о. получается 3 уравнения
			R1^2 = a^2 + h^2
			R2^2 = b^2 + h^2
			a + b = l
			далее
			h^2 = R1^2 - a^2
			b = R1 + R2 - a
			(l - a)^2 + R1^2 - a^2 = R2^2
			l^2 - 2*l*a + R1^2 - R2^2 = 0
			2*l*a = SR^2 + R1^2 - R2^2
			a = (l^2 + R1^2 - R2^2) / (2*l)
		*/
		
		float a = (l*l+r1*r1-r2*r2)/(2*l);
		float h = sqrt(r1*r1-a*a);
		a /= l;
		
		rads.x2 = rads.x1+(rads.x2-rads.x1)*a;
		rads.y2 = rads.y1+(rads.y2-rads.y1)*a;
		a = rads.x2; rads.x2 = rads.x1; rads.x1 = a; 
		a = rads.y2; rads.y2 = rads.y1; rads.y1 = a; 
		base_t normal;
		lu_2dLineNormal(&rads,&normal);
		
		*fx1 = normal.x1+(normal.x2-normal.x1)*h;
		*fy1 = normal.y1+(normal.y2-normal.y1)*h;
		
		*fx2 = normal.x1-(normal.x2-normal.x1)*h;
		*fy2 = normal.y1-(normal.y2-normal.y1)*h;
		
		return 2;
	}
}

int lu_2dArcLineInS(base_t* A1, base_t* L1, float *fx1, float *fy1, float *fx2, float *fy2)
{
	base_t normal;
	lu_2dLineNormal(L1,&normal);
	
	//нормаль из центра окр.
	float dx = normal.x1 - A1->xc;
	float dy = normal.y1 - A1->yc;
	normal.x1 -= dx;
	normal.y1 -= dy;
	normal.x2 -= dx;
	normal.y2 -= dy;
	
	//точка пересечения прямой и перпендикуляра из центра, если совпадают -- параметры заданы не верно
	if(lu_2dLineLineInS(L1,&normal,&dx,&dy) != 1) return -1;
	
	normal.x1 = dx;
	normal.y1 = dy;
	
	dx -= A1->xc;
	dy -= A1->yc;
	float a = sqrt(dx*dx+dy*dy);
	
	dx = A1->xc - A1->x1;
	dy = A1->yc - A1->y1;
	float b = sqrt(dx*dx+dy*dy);
	
	/*
		a -- до прямой
		b -- радиус
		a < b -- 2 пересечения
		a == b -- касание
		a > b -- нет пересечений
	*/
	
	
	if(fabs(a-b) < D_LINEUT_DELTA)
	{
		*fx1 = normal.x1;
		*fy1 = normal.y1;
		return 1;
	}
	
	if(a > b) return 0;
	
	//величина симметричного отступа от точки пересечения прямых до симметричных точек пересечения
	float c = sqrt(b*b-a*a);
	
	//прямая, отложенная от точки пересечения, приводимая к длине c
	normal.x2 = normal.x1 + (L1->x2 - L1->x1);
	normal.y2 = normal.y1 + (L1->y2 - L1->y1);
	
	lu_2dLineNormalize(&normal);
	
	normal.x2 = normal.x1+(normal.x2-normal.x1)*c;
	normal.y2 = normal.y1+(normal.y2-normal.y1)*c;
	
	*fx1 = normal.x2;
	*fy1 = normal.y2;
	
	*fx2 = normal.x1 - (normal.x2 - normal.x1);
	*fy2 = normal.y1 - (normal.y2 - normal.y1);
	
	return 2;
}

int lu_2dLineLineInS(base_t* L1, base_t* L2, float *fx, float *fy)
{
	float k1, m1, k2, m2;
	k1 = (L1->y2-L1->y1)/(L1->x2-L1->x1);
	k2 = (L2->y2-L2->y1)/(L2->x2-L2->x1);
	m1 = L1->y1 - (L1->x1 * k1);
	m2 = L2->y1 - (L2->x1 * k2);
	if( ( fabs(k1-k2) < D_LINEUT_DELTA ) && ( fabs(m1-m2) < D_LINEUT_DELTA ) ) return -1;
	
	float x = (m2-m1)/(k1-k2);
	float y = k1*x+m1;
	
	*fx = x;
	*fy = y;
	
	return 1;
}

void lu_2dLineNormal(base_t* source, base_t* dest)
{
	//~EqLine
	
	/* relativi = p2-p1; */
	float rx = source->x2 - source->x1;
	float ry = source->y2 - source->y1;
	dest->x2 = rx;
	dest->y2 = ry;
	
	//dx, dy = rx, ry / l
	float rad = sqrt(rx*rx+ry*ry);
	rx = rx/rad;
	ry = ry/rad;
	
	dest->x1 = source->x1;
	dest->y1 = source->y1;
	dest->x2 = 0.0-ry+source->x1;
	dest->y2 = 0.0+rx+source->y1;
	dest->z1 = dest->z2 = source->z1;
}

void lu_2dLineNormalize(base_t* line)
{
	float dx = line->x2 - line->x1;
	float dy = line->y2 - line->y1;
	float r = sqrt(dx*dx+dy*dy);
	dx /= r;
	dy /= r;
	line->x2 = line->x1+dx;
	line->y2 = line->y1+dy;
}

int lu_2dlu_2dPointInLine(base_t* line, float x, float y)
{
	x = fabs(x);
	y = fabs(y);
	if
	(
		(x >= fabs(line->x1)) &&
		(x <= fabs(line->x2)) &&
		(y >= fabs(line->y1)) &&
		(y <= fabs(line->y2)) 
	) return 1;
	return 0;
}

int lu_2dPointInArc(base_t* arc, float x, float y)
{
	base_t ta;
	ta.x1 = x;
	ta.y1 = y;
	ta.xc = arc->xc;
	ta.yc = arc->yc;
	
	float min, max;
	
	if(arc->cw > 0)
	{
		min = lu_2dArcAngle1(arc,0);
		max = lu_2dArcAngle2(arc,0);
	} else {
		max = lu_2dArcAngle1(arc,0);
		min = lu_2dArcAngle2(arc,0);
	}
	
	float test = lu_2dArcAngle1(&ta,0);
	
	if(arc->cw*(max-min) > 0.0) max -= 2*D_LINEUT_PI*arc->cw;
	if(min < 0.0 || max < 0.0)
	{
		min += 2*D_LINEUT_PI;
		max += 2*D_LINEUT_PI;
	}
	if(test < 0.0) //impossible, but can take place
	{
		test += 2*D_LINEUT_PI;
	}
	
	if(test >= min && test <= max) return 1;
	return 0;
}

int lu_2dAdjustLLViaPoint(base_t* l1, base_t* l2)
{
	float x, y;
	if(1 != lu_2dLineLineInS(l1,l2,&x,&y)) return -1;
	l1->x2 = x;
	l2->x1 = x;
	l1->y2 = y;
	l2->y1 = y;
	return 0;
}

int lu_2dAdjustLAViaPoint(base_t* l1, base_t* l2 )
{
	float x1,x2,y1,y2,dx,dy;
	int ps = lu_2dArcLineInS(l2,l1,&x1,&y1,&x2,&y2);
	switch(ps)
	{
	case 2:
		dx = l1->x2 - x1;
		dy = l1->y2 - y1;
		float r1 = sqrt(dx*dx+dy*dy);
		dx = l1->x2 - x2;
		dy = l1->y2 - y2;
		float r2 = sqrt(dx*dx+dy*dy);
		if(r2 < r1)
		{
			x1 = x2;
			y1 = y2;
		}
	case 1:
		l1->x2 = x1;
		l2->x1 = x1;
		l1->y2 = y1;
		l2->y1 = y1;
		return 0;
	default: return -1;
	}
}

int lu_2dAdjustALViaPoint(base_t* l1,  base_t* l2)
{
	float x1,x2,y1,y2,dx,dy;
	int ps = lu_2dArcLineInS(l1,l2,&x1,&y1,&x2,&y2);
	switch(ps)
	{
	case 2:
		dx = l1->x2 - x1;
		dy = l1->y2 - y1;
		float r1 = sqrt(dx*dx+dy*dy);
		dx = l1->x2 - x2;
		dy = l1->y2 - y2;
		float r2 = sqrt(dx*dx+dy*dy);
		if(r2 < r1)
		{
			x1 = x2;
			y1 = y2;
		}
	case 1:
		l1->x2 = x1;
		l2->x1 = x1;
		l1->y2 = y1;
		l2->y1 = y1;
		return 0;
	default: return -1;
	}
}

int lu_2dAdjustAAViaPoint(base_t* l1,  base_t* l2 )
{
	float x1,x2,y1,y2,dx,dy;
	int ps = lu_2dArcArcInS(l1,l2,&x1,&y1,&x2,&y2);
	switch(ps)
	{
	case 2:
		dx = l1->x2 - x1;
		dy = l1->y2 - y1;
		float r1 = sqrt(dx*dx+dy*dy);
		dx = l1->x2 - x2;
		dy = l1->y2 - y2;
		float r2 = sqrt(dx*dx+dy*dy);
		if(r2 < r1)
		{
			x1 = x2;
			y1 = y2;
		}
	case 1:
		l1->x2 = x1;
		l2->x1 = x1;
		l1->y2 = y1;
		l2->y1 = y1;
		return 0;
	default: return -1;
	}
}

int lu_2dAdjustViaPoint(base_t* l1, base_t* l2)
{
	if(!l1->cw)
	{
		if(!l2->cw) return lu_2dAdjustLLViaPoint(l1,l2);
		else return lu_2dAdjustLAViaPoint(l1,l2);
	} else {
		if(!l2->cw) return lu_2dAdjustALViaPoint(l1,l2);
		else return lu_2dAdjustAAViaPoint(l1,l2);
	}
}
