#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf.h"
#include "ogllaf.h"
#include "lineut.h"

#define ST_REFLS  0
#define ST_REFLX  5
#define ST_REFLY  6
#define ST_REFLZ  7
#define ST_REFLE  8

#define COM_RAPID  0
#define COM_LINE   1
#define COM_CW     2
#define COM_CCW    3

#define ST_DBLOFF 0
#define ST_DBLON  1
#define ST_DBLUV  2

#define COM_STOP   102
#define COM_PAUSE  101
#define COM_FPAUSE 100
#define COM_CUT    150
#define COM_WIRE   160

//#define STYLE_NONE 	0
//#define STYLE_BASE 	1
//#define STYLE_RAPID	2


typedef struct machine_t
{
	float zd,zu;
	float ux, uy, ui, uj;
	float dx, dy, di, dj;
	float equid; //-1.0, 0.0, 1.0
	float con; //-... +...
	
	char refl;
	char dbl;
	char rel;
	char wire;

	float h[10];
} machine_t;
