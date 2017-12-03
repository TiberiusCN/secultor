/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include <stdlib.h>
#include <stdio.h>
#include "ogllaf.h"
#include "glworks.h"
#include "lineut.h"

#define acu_div 0.1
#define DIV_MAX 40
#define DIV_MIN 8

#define SPEC_MERGE 1

#define TYPE_DRAW_LINE 1
#define TYPE_DRAW_LINESTRIP 2

#define MODE_CAMERA 0
#define MODE_CURSOR 1
#define MODE_SETLINE 2
#define MODE_SETPART 3
#define MODE_MAX 3

typedef struct xpoint_t
{
	float x,y,z;
} xpoint_t;

typedef struct xline_t
{
	int count;
	xpoint_t* points;
} xline_t;

typedef struct xcolor_t
{
	int index;
	ogllaf_color_t rgb;
} xcolor_t;

typedef struct glmatrix_t
{
	GLfloat f[4*4];
} glmatrix_t;

int MakeXArc (xline_t *out, base_t *arc);
int MakeXLine(xline_t *out, base_t *line);
int MakeX(xline_t *out, base_t *in);
void MakeLine(xline_t *l1, xline_t *l2, xline_t *out);

//--------------------------------------------------------

int mode = MODE_CAMERA; 
float cursor[3] = {0.0,0.0,0.0};
float move[3] = {0.0,0.0,0.0};
float rotate[3] = {-90.0,0.0,0.0};
float tzero[3] = {0.0,0.0,0.0};
float camspeed[2] = {1.0,1.0}; //camera+cursor
xline_t** polylines;
int* clines;
int cpolys;
int* types;
glmatrix_t* matrices;

int curline = 0;
int curpart = 0;
int* fpolys;

xcolor_t** polycolors; //count = cpolys
int* ccolors; //count of color for each polyline

//-------------------------------------------------------

void UpdateMatrix()
{
	glPopMatrix();
	glPushMatrix();
	
	glRotatef(rotate[0],1.0,0.0,0.0);
	glRotatef(rotate[1],0.0,1.0,0.0);
	glRotatef(rotate[2],0.0,0.0,1.0);
	
	glTranslatef(move[0],move[1],move[2]);
}

char cap[256];

void CaptionUpdate()
{
	float dx = cursor[0] - tzero[0];
	float dy = cursor[1] - tzero[1];
	float dz = cursor[2] - tzero[2];
	sprintf(cap,"%f, %f, %f [%f] MOD%d",dx,dy,dz,sqrt(dx*dx+dy*dy+dz*dz),mode);
	SetCaption(cap);
}

void KeyDown(uint32_t key)
{
	int mostl, mosti, mostp;
	float min = 10000000000000000.0;
	float dx,dy,dz,dist;

	switch(key)
	{
	case XK_h:
		if(mode == MODE_CAMERA)
		{
			move[0] += camspeed[0]*cos(-rotate[2]/180*D_LINEUT_PI);
			move[1] += camspeed[0]*sin(-rotate[2]/180*D_LINEUT_PI);
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[0] -= camspeed[1];
			CaptionUpdate();
			break;
		}
		if(mode == MODE_SETLINE)
		{
			curline--;
			if(curline < 0) curline = cpolys-1;
			break;
		}
		if(mode == MODE_SETPART)
		{
			if(curpart) curpart--;
			break;
		}
		break;
	case XK_l:
		if(mode == MODE_CAMERA)
		{
			move[0] -= camspeed[0]*cos(-rotate[2]/180*D_LINEUT_PI);
			move[1] -= camspeed[0]*sin(-rotate[2]/180*D_LINEUT_PI);
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[0] += camspeed[1];
			CaptionUpdate();
			break;
		}
		if(mode == MODE_SETLINE)
		{
			curline++;
			if(curline == cpolys) curline = 0;
			break;
		}
		if(mode == MODE_SETPART)
		{
			curpart++;
			break;
		}
		break;
	case XK_j:
		if(mode == MODE_CAMERA)
		{
			move[0] += camspeed[0]*sin(rotate[2]/180*D_LINEUT_PI);
			move[1] += camspeed[0]*cos(rotate[2]/180*D_LINEUT_PI);
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[1] -= camspeed[1];
			CaptionUpdate();
			break;
		}
		if(mode == MODE_SETLINE)
		{
			fpolys[curline] &= ~1;
			break;
		}
		if(mode == MODE_SETPART)
		{
			break;
		}
		break;
	case XK_k:
		if(mode == MODE_CAMERA)
		{
			move[0] -= camspeed[0]*sin(rotate[2]/180*D_LINEUT_PI);
			move[1] -= camspeed[0]*cos(rotate[2]/180*D_LINEUT_PI);
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[1] += camspeed[1];
			CaptionUpdate();
			break;
		}
		if(mode == MODE_SETLINE)
		{
			fpolys[curline] |= 1;
			break;
		}
		if(mode == MODE_SETPART)
		{
			break;
		}
		break;
	case XK_u:
		if(mode == MODE_CAMERA)
		{
			move[2] += camspeed[0];
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[2] += camspeed[1];
			CaptionUpdate();
			break;
		}
		break;
	case XK_i:
		if(mode == MODE_CAMERA)
		{
			move[2] -= camspeed[0];
			UpdateMatrix();
			break;
		}
		if(mode == MODE_CURSOR)
		{
			cursor[2] -= camspeed[1];
			CaptionUpdate();
			break;
		}
		break;
	case XK_y:
		if(mode == MODE_CAMERA)
		{
			camspeed[0] *= 10.0;
			break;
		}
		if(mode == MODE_CURSOR)
		{
			camspeed[1] *= 10.0;
			break;
		}
		break;
	case XK_o:
		if(mode == MODE_CAMERA)
		{
			camspeed[0] *= 0.1;
			break;
		}
		if(mode == MODE_CURSOR)
		{
			camspeed[1] *= 0.1;
			break;
		}
		break;
		
	case XK_s:
		rotate[0] += 4.0;
		if(rotate[0] > 180.0) rotate[0] = -180.0;
		UpdateMatrix();
		break;
	case XK_w:
		rotate[0] -= 4.0;
		if(rotate[0] < -180.0) rotate[0] = 180.0;
		UpdateMatrix();
		break;
		
	case XK_q:
		gOrthoScale -= 1.0;
		if(gOrthoScale  <=  0.0) gOrthoScale = 0.0;
		ReProject(0);
		break;
	case XK_e:
		gOrthoScale += 1.0;
		ReProject(0);
		break;
	
	
	case XK_d:
		rotate[2] += 4.0;
		if(rotate[2] > 180.0) rotate[2] = -180.0;
		UpdateMatrix();
		break;
	case XK_a:
		rotate[2] -= 4.0;
		if(rotate[2] < -180.0) rotate[2] = 180.0;
		UpdateMatrix();
		break;

	case XK_b:
		mode++;
		if(mode > MODE_MAX) mode = 0;
		CaptionUpdate();
		break;
		
	case XK_n:
		if(rotate[0] - round(rotate[0]/90.0)*90.0 < (round(rotate[0]/90.0)+1.0)*90.0 - rotate[0]) 
		rotate[0] = round(rotate[0]/90.0)*90.0;
		else rotate[0] = (round(rotate[0]/90.0)+1.0)*90.0;
		
		if(rotate[2] - round(rotate[2]/90.0)*90.0 < (round(rotate[2]/90.0)+1.0)*90.0 - rotate[2]) 
		rotate[2] = round(rotate[2]/90.0)*90.0;
		else rotate[2] = (round(rotate[2]/90.0)+1.0)*90.0;
		
		UpdateMatrix();
		break;
		
	case XK_r:
		ReProject(1);
		break;
	
	case XK_Return:
		if(tzero[0] == cursor[0] && tzero[1] == cursor[1] && tzero[2] == cursor[2])
		{
			tzero[0] = 0.0;
			tzero[1] = 0.0;
			tzero[2] = 0.0;
		} else {
			tzero[0] = cursor[0];
			tzero[1] = cursor[1];
			tzero[2] = cursor[2];
		}
		CaptionUpdate();
		break;
		
	case XK_space:
	{
		if(mode == MODE_CURSOR)
		{
			int mostl,mosti,mostp;
			float min = 100000000000.0;
			for(int l = 0; l < cpolys; l++)
			{
				for(int i = 0; i < clines[l]; i++)
				{
					float dx,dy,dz,dist;
					dx = polylines[l][i].points[0].x-cursor[0];
					dy = polylines[l][i].points[0].y-cursor[1];
					dz = polylines[l][i].points[0].z-cursor[2];
					dist = (dx*dx+dy*dy+dz*dz);
					if(dist < min)
					{
						min = dist;
						mostl = l;
						mosti = i;
						mostp = 0;
					}
					dx = polylines[l][i].points[polylines[l][i].count].x-cursor[0];
					dy = polylines[l][i].points[polylines[l][i].count].y-cursor[1];
					dz = polylines[l][i].points[polylines[l][i].count].z-cursor[2];
					dist = (dx*dx+dy*dy+dz*dz);
					if(dist < min)
					{
						min = dist;
						mostl = l;
						mosti = i;
						mostp = polylines[l][i].count;
					}
				}
			}
			cursor[0] = polylines[mostl][mosti].points[mostp].x;
			cursor[1] = polylines[mostl][mosti].points[mostp].y;
			cursor[2] = polylines[mostl][mosti].points[mostp].z;
			CaptionUpdate();
			break;
		}
		if(mode == MODE_SETPART)
		{
			if(!(fpolys[curline] & 1))
			{
				for(int f = 0; f < cpolys; f++)
				{
					if(fpolys[f] & 1)
					{
						curline = f;
						break;
					}
				}
			}
			if(curpart >= clines[curline]) curpart = clines[curline]-1;
			cursor[0] = polylines[curline][curpart].points[0].x;
			cursor[1] = polylines[curline][curpart].points[0].y;
			cursor[2] = polylines[curline][curpart].points[0].z;
			CaptionUpdate();
			break;
		}
		break;
	}
	case XK_z:
		move[0] = -cursor[0];
		UpdateMatrix();
		break;
	case XK_x:
		move[1] = -cursor[1];
		UpdateMatrix();
		break;
	case XK_c:
		move[2] = -cursor[2];
		UpdateMatrix();
		break;
	case XK_Escape:
		WinQuit();
		break;
	}
}

void Render(uint32_t dt)
{	
	glClear( GL_COLOR_BUFFER_BIT );
	
	for(int l = 0; l < cpolys; l++)
	{
		glPushMatrix();
		glMultMatrixf(matrices[l].f);
		int color = 0;
		
		if( types[l] == TYPE_DRAW_LINESTRIP ) glBegin( GL_LINE_STRIP );
		if( types[l] == TYPE_DRAW_LINE ) glBegin( GL_LINES );

		float r,g,b;

		if(mode == MODE_SETLINE)
		{
			r = g = 0.0;
			b = 1.0;
			if(l == curline) r = 1.0;
			if(fpolys[l] & 1) g = 1.0;
			glColor3f(r,g,b);
		}
		
		for(int i = 0; i < clines[l]; i++)
		{
			switch(mode)
			{
			case MODE_CAMERA:
			case MODE_CURSOR:
				while(color >= 0 && polycolors[l][color].index == i)
				{
					glColor3f(
					polycolors[l][color].rgb.f[0],
					polycolors[l][color].rgb.f[1],
					polycolors[l][color].rgb.f[2]);
					color++;
					if(color == ccolors[l]) color = -1;
				}
				break;

			case MODE_SETLINE:
				while(color >= 0 && polycolors[l][color].index == i)
				{
					color++;
					if(color == ccolors[l]) color = -1;
				}
				break;
			case MODE_SETPART:
				while(color >= 0 && polycolors[l][color].index == i)
				{
					r = polycolors[l][color].rgb.f[0];
					g = polycolors[l][color].rgb.f[1];
					b = polycolors[l][color].rgb.f[2];
					color++;
					if(color == ccolors[l]) color = -1;
				}
				if((fpolys[l] & 1) && curpart == i) glColor3f(r,g,b);
				else glColor3f(0.2*r,0.2*g,0.2*b);
				break;
			}

			for(int p = 0; p < polylines[l][i].count; p++)
			{
				glVertex3f(polylines[l][i].points[p].x,polylines[l][i].points[p].y,polylines[l][i].points[p].z);
			}
		}
		
		glEnd();
		glPopMatrix();
	}
	
	if(mode == MODE_SETPART)
	{
		for(int l = 0; l < cpolys; l++)
		{
			if(!(fpolys[l] & 1) || (curpart >= clines[l])) continue;
			glPushMatrix();
			glMultMatrixf(matrices[l].f);
			int color = 0;

			float r,g,b;
			int i;

			for(i = 0; i < curpart; i++)
			{
				while(color >= 0 && polycolors[l][color].index == i)
				{
					r = polycolors[l][color].rgb.f[0];
					g = polycolors[l][color].rgb.f[1];
					b = polycolors[l][color].rgb.f[2];
					color++;
					if(color == ccolors[l]) color = -1;
				}
			}

			if( types[l] == TYPE_DRAW_LINESTRIP ) glBegin( GL_LINE_STRIP );
			if( types[l] == TYPE_DRAW_LINE ) glBegin( GL_LINES );

			glColor3f(r,g,b);	

			for(int p = 0; p < polylines[l][i].count; p++)
			{
				glVertex3f(polylines[l][i].points[p].x,polylines[l][i].points[p].y,polylines[l][i].points[p].z);
			}
				
			glEnd();
			glPopMatrix();
		}
	}

	
	glBegin(GL_LINES);
	glColor3f(1.0,0.0,0.0);
	glVertex3f(cursor[0]-50.0,cursor[1],cursor[2]);
	glVertex3f(cursor[0]+50.0,cursor[1],cursor[2]);
	glColor3f(0.0,1.0,0.0);
	glVertex3f(cursor[0],cursor[1]-50.0,cursor[2]);
	glVertex3f(cursor[0],cursor[1]+50.0,cursor[2]);
	glColor3f(0.0,1.0,1.0);
	glVertex3f(cursor[0],cursor[1],cursor[2]-50.0);
	glVertex3f(cursor[0],cursor[1],cursor[2]+50.0);
	glEnd();
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("prog?\n");
		return 1;
	}
	
	int res;
	res = WinInit(000,000,0,0,"Tradira");
	if(res)
	{
		printf("WinInit Err: %d\n",res);
		fflush(0);
		return res;
	}
	
	const char** progs;
	
	if(argc == 3 && !strcmp(argv[1],"-s"))
	{
		//vis
	} else {
		cpolys = argc-1;
		for(int i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i],"-m")) cpolys -= 4;
			if(!strcmp(argv[i],"-r")) cpolys -= 4;
		}
		progs = malloc(sizeof(*progs)*cpolys);
		matrices = malloc(sizeof(*matrices)*cpolys);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		float rx,ry,rz,mx,my,mz;
		rx = ry = rz = mx = my = mz = 0.0;
		
		cpolys = 0;
		for(int i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i],"-m"))
			{
				if(argc-i < 4)
				{
					printf("-m x y z\n");
					return 1;
				}
				
				sscanf(argv[i+1],"%f",&mx);
				sscanf(argv[i+2],"%f",&my);
				sscanf(argv[i+3],"%f",&mz);
				
				glLoadIdentity();
				glRotatef(rx,1.0,0.0,0.0);
				glRotatef(ry,0.0,1.0,0.0);
				glRotatef(rz,0.0,0.0,1.0);
	            
				glTranslatef(mx,my,mz);
				
				i += 3; 
				continue; 
			}
			if(!strcmp(argv[i],"-r"))
			{
				if(argc-i < 4)
				{
					printf("-r x y z\n");
					return 1;
				}
				
				sscanf(argv[i+1],"%f",&rx);
				sscanf(argv[i+2],"%f",&ry);
				sscanf(argv[i+3],"%f",&rz);
				
				glLoadIdentity();
				glRotatef(rx,1.0,0.0,0.0);
				glRotatef(ry,0.0,1.0,0.0);
				glRotatef(rz,0.0,0.0,1.0);
	            
				glTranslatef(mx,my,mz);
				
				i += 3; 
				continue; 
			}
			
			glGetFloatv(GL_MODELVIEW_MATRIX,matrices[cpolys].f);
			progs[cpolys] = argv[i];
			cpolys++;
		}
		glPopMatrix();
	}
	
	if(!cpolys)
	{
		printf("Prog?\n");
		return 1;
	}
	
	clines = malloc(sizeof(*clines)*cpolys);
	types = malloc(sizeof(*clines)*cpolys);
	polylines = malloc(sizeof(*polylines)*cpolys);
	polycolors = malloc(sizeof(*polycolors)*cpolys);
	ccolors = malloc(sizeof(*ccolors)*cpolys);
	fpolys = malloc(sizeof(*fpolys)*cpolys);
	
	memset(clines,0,sizeof(*clines)*cpolys);
	memset(ccolors,0,sizeof(*ccolors)*cpolys);
	memset(types,0,sizeof(*types)*cpolys);
	
	int special = 0;
	
	for(int l = 0; l < cpolys; l++)
	{
		types[l] = TYPE_DRAW_LINESTRIP;
		if(!strcmp("-x",progs[l]))
		{
			if( l < 1 || l + 1 > cpolys )
			{
				printf("<f1> -x <f2>\n");
				return 3;
			}
			types[l] = TYPE_DRAW_LINE;
			special = SPEC_MERGE;
			continue;
		}
		
		FILE* f = fopen(progs[l],"rb");
		if(!f)
		{
			printf("file? (%s)\n",progs[l]);
			return 2;
		}
		
		int com, lines, colors;
		base_t line;
		ogllaf_color_t color;
		
		lines = colors = 0;
		
		while(fread(&com,sizeof(int),1,f))
		{
			switch(com)
			{
			case ogllaf_line:
				fseek(f,sizeof(line),SEEK_CUR);
				lines++;
				break;
			case ogllaf_color:
				fseek(f,sizeof(color),SEEK_CUR);
				colors++;
				break;
			default: return 2;
			}
		}
		
		colors++;
		fseek(f,0,SEEK_SET);
		
		polylines[l] = malloc(sizeof(*(polylines[l]))*lines);
		polycolors[l] = malloc(sizeof(*(polycolors[l]))*colors);
		
		ccolors[l]++;
		polycolors[l][0].rgb.f[0] = 1.0;
		polycolors[l][0].rgb.f[1] = 1.0;
		polycolors[l][0].rgb.f[2] = 1.0;
		polycolors[l][0].index = 0;
		
		while(fread(&com,sizeof(int),1,f))
		{
			switch(com)
			{
			case ogllaf_line:
				fread(&line,sizeof(line),1,f);
				
				if(special == SPEC_MERGE)
					polylines[l][clines[l]].count = polylines[l-2][clines[l]].count;
				else 
					polylines[l][clines[l]].count = 0;
				
				MakeX(&(polylines[l])[clines[l]],&line);
				clines[l]++;
				break;
			case ogllaf_color:
				fread(&polycolors[l][ccolors[l]].rgb,sizeof(ogllaf_color_t),1,f);
				polycolors[l][ccolors[l]].index = clines[l];
				ccolors[l]++;
				break;
			}
		}
		
		fclose(f);
		
		if(special == SPEC_MERGE)
		{
			clines[l-1] = clines[l];
			ccolors[l-1] = ccolors[l];
			
			polylines[l-1] = malloc(sizeof(*(polylines[l-1]))*lines);
			for(int i = 0; i < lines; i++)
			{
				MakeLine(&(polylines[l-2][i]),&(polylines[l][i]),&(polylines[l-1][i]));
			}
			
			polycolors[l-1] = malloc(sizeof(*(polycolors[l-1]))*colors);
			for(int i = 0; i < colors; i++)
			{
				memcpy(&polycolors[l-1][i], &polycolors[l][i], sizeof(polycolors[l-1][i]));
			}
			
			special = 0;
		}
	}
	
	KeySet(KeyDown);	
	UpdateMatrix();	
	WinStart(Render);
		
	return 0;
}

void MakeLine(xline_t *l1, xline_t *l2, xline_t *out)
{
	if(l1->count != l2->count) 
	{
		out->count = 0;
		return;
	}
	out->count = 4;
	out->points = malloc(sizeof(*(out->points))*out->count);
	out->points[0].x = l1->points[0].x;
	out->points[0].y = l1->points[0].y;
	out->points[0].z = l1->points[0].z;
	
	out->points[1].x = l2->points[0].x;
	out->points[1].y = l2->points[0].y;
	out->points[1].z = l2->points[0].z;
	
	out->points[2].x = l1->points[l1->count-1].x;
	out->points[2].y = l1->points[l1->count-1].y;
	out->points[2].z = l1->points[l1->count-1].z;
	
	out->points[3].x = l2->points[l2->count-1].x;
	out->points[3].y = l2->points[l2->count-1].y;
	out->points[3].z = l2->points[l2->count-1].z;
}

int MakeXArc(xline_t *out, base_t *arc)
{
	float r1,r2;
	float angle0 = lu_2dArcAngle1(arc,&r1);
	float angle1 = lu_2dArcAngle2(arc,&r2);

	if(arc->cw*(angle1-angle0) > 0.0)
	{
		angle1 -= 2*D_LINEUT_PI*arc->cw;
	}
	
	int lacu_div;
	if(out->count)
	{
		lacu_div = out->count;
	} else {
		lacu_div = round(fabs(round((angle1-angle0)*2*r1)/acu_div)); //c = pi*r*2*a/pi = 2*r*a
		if(lacu_div > DIV_MAX || lacu_div < 0) lacu_div = DIV_MAX;
		if(lacu_div < DIV_MIN) lacu_div = DIV_MIN;
		out->count = lacu_div;
	}
	
	float da = -1.0*arc->cw*fabs(angle1-angle0)/lacu_div;
	
	float tx0 = arc->x1;
	float ty0 = arc->y1;
	
	float tx1 = tx0;
	float ty1 = ty0;
	
	angle0 += da;
	
	out->points = malloc(sizeof(xpoint_t)*(lacu_div));

	for(int i = 0; i < lacu_div-1; i++)
	{		
		out->points[i].x = tx0;
		out->points[i].y = ty0;
		out->points[i].z = arc->z1;
		
		tx0 = tx1;
		ty0 = ty1;
		tx1 = arc->xc+cos(angle0)*r1;
		ty1 = arc->yc+sin(angle0)*r1;
		angle0 += da;
	}
	
	out->points[lacu_div-1].x = arc->x2;
	out->points[lacu_div-1].y = arc->y2;
	out->points[lacu_div-1].z = arc->z1;
	
	return 0;
}

int MakeXLine(xline_t *out, base_t* line)
{	
	out->count = 2;
	out->points = malloc(sizeof(xpoint_t)*2);
	out->points[0].x = line->x1;
	out->points[0].y = line->y1;
	out->points[0].z = line->z1;
	out->points[1].x = line->x2;
	out->points[1].y = line->y2;
	out->points[1].z = line->z2;

	return 0;
}

int MakeX(xline_t *out, base_t *in)
{
	if(in->cw == 0.0 || in->cw == -0.0) return MakeXLine(out,in);
	else return MakeXArc(out,in);
}
