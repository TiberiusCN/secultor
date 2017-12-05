/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican@protonmail.com> github.com/TiberiusCN
 */

#include "vmcut20p.h"
#include <string.h>

machine_t cut;

void print_help()
{
	printf("\t-i, --input  <file>  programm\n");
	printf("\t-b, --base   <value> Z-level of bottom line (def: 0.0)\n");
	printf("\t-h, --height <value> Z-level of upper line (def: 10.0)\n");
	printf("\t    --help           this text\n");
	printf("\nvmcut20p v 0.5 @ GPL v2.0; Copyright (c) TiCaN <tican@protonmail.com> github.com/TiberiusCN\n");
}

int main(int argc, char** argv)
{
	memset(&cut,0,sizeof(cut));
	
	const char* filein = 0;
	float base = 0.0;
	float height = 10.0;
	
	for(int i = 1; i < argc; i++)
	{
		if((!strncmp(argv[i],"-i",3))||(!strncmp(argv[i],"--input",8)))
		{
			if(i == argc)
			{
				printf("Err: input not specified\n");
				return 1;
			}
			i++;
			filein = argv[i];
			continue;
		}
		if((!strncmp(argv[i],"-b",3))||(!strncmp(argv[i],"--base",7)))
		{
			if(i == argc)
			{
				printf("Err: base not specified\n");
				return 1;
			}
			i++;
			sscanf(argv[i],"%f",&base);
			continue;
		}
		if((!strncmp(argv[i],"-h",3))||(!strncmp(argv[i],"--height",9)))
		{
			if(i == argc)
			{
				printf("Err: height not specified\n");
				return 1;
			}
			i++;
			sscanf(argv[i],"%f",&height);
			continue;
		}
		if(!strncmp(argv[i],"--help",7))
		{
			print_help();
			return 0;
		}
		printf("Err: unknown option %s\n",argv[i]);
		return 1;
	}

	if(!filein) { printf("Err: input not specified!\n"); print_help(); return 1; }
	
	const int cogllaf_line        = ogllaf_line       ;
	const int cogllaf_color       = ogllaf_color      ;
	const int cogllaf_matrix_proj = ogllaf_matrix_proj;
	const int cogllaf_matrix_view = ogllaf_matrix_view;
	const int cogllaf_type        = ogllaf_type       ;
	
	cut.zd = base;
	cut.zu = base+height;
	
	FILE* fpvf = fopen(filein,"rb");
	char fname[256];
	int len = strlen(filein);
	memcpy(fname,filein,len);
	memcpy(fname+len,".up",4);
	FILE* fup = fopen(fname,"wb");
	memcpy(fname+len,".down",6);
	FILE* fdown = fopen(fname,"wb");
	
	pvf_t pvf;
	
	int ucom,dcom;
	int ud = 0; //0 - down, 1 - up
	float *stat = 0;
	
	float pux, puy;
	float pdx, pdy;
	pux = puy = pdx = pdy = 0.0;
	
	base_t line;
	ogllaf_color_t color;
	color.f[0] = 0.5;
	color.f[1] = 0.0;
	color.f[2] = 0.0;
	
	fwrite(&cogllaf_color,sizeof(int),1,fdown);
	fwrite(&color,sizeof(color),1,fdown);
	fwrite(&cogllaf_color,sizeof(int),1,fup);
	fwrite(&color,sizeof(color),1,fup);
	
	while(fread(&pvf,sizeof(pvf),1,fpvf))
	{
		switch(pvf.type)
		{
		case 'G':
			if(pvf.ival >= 0 && pvf.ival <= 3)
			{
				if(ud) 	ucom = pvf.ival;
				else 	dcom = pvf.ival;
			} else {
				switch(pvf.ival)
				{
				case 90:
				case 92:
					printf("WARN: g90/g92\n");
					break;
				case 40:
					color.f[1] = 0.0;
					color.f[2] = 0.0;
					fwrite(&cogllaf_color,sizeof(int),1,fup);
					fwrite(&color,sizeof(color),1,fup);
					fwrite(&cogllaf_color,sizeof(int),1,fdown);
					fwrite(&color,sizeof(color),1,fdown);
					break;
				case 41:
					color.f[1] = 1.0;
					color.f[2] = 0.0;
					fwrite(&cogllaf_color,sizeof(int),1,fup);
					fwrite(&color,sizeof(color),1,fup);
					fwrite(&cogllaf_color,sizeof(int),1,fdown);
					fwrite(&color,sizeof(color),1,fdown);
					break;
				case 42:
					color.f[2] = 1.0;
					color.f[1] = 0.0;
					fwrite(&cogllaf_color,sizeof(int),1,fup);
					fwrite(&color,sizeof(color),1,fup);
					fwrite(&cogllaf_color,sizeof(int),1,fdown);
					fwrite(&color,sizeof(color),1,fdown);
					break;
				case 60:
					cut.dbl = ST_DBLOFF;
					break;
				case 61:
					cut.dbl = ST_DBLON;
					break;
				default:
					printf("ERR: unknown G'%d' (%x)\n",pvf.ival,(int)pvf.ival);
					return 1;
				}
			}
			break;
		case 'M':
			switch(pvf.ival)
			{
			case 60:
				color.f[0] = 1.0;
				fwrite(&cogllaf_color,sizeof(int),1,fup);
				fwrite(&color,sizeof(color),1,fup);
				fwrite(&cogllaf_color,sizeof(int),1,fdown);
				fwrite(&color,sizeof(color),1,fdown);
				break;
			case 50:
				color.f[0] = 0.5;
				fwrite(&cogllaf_color,sizeof(int),1,fup);
				fwrite(&color,sizeof(color),1,fup);
				fwrite(&cogllaf_color,sizeof(int),1,fdown);
				fwrite(&color,sizeof(color),1,fdown);
				break;
			case 02:
				printf("WARN: STOP\n");
				break;
			case 00:
				printf("WARN: pause on\n");
				break;
			default:
				printf("ERR: unknown M'%d' (%x)\n",pvf.ival,(int)pvf.ival);
				return 1;
			}
			break;
		case 'H':
			stat = &cut.h[pvf.ival];
			break;
		case '=':
			*stat = pvf.fval;
			break;
		case 'X':
			if(ud) 	cut.ux = pvf.fval;
			else 	cut.dx = pvf.fval;
			break;
		case 'Y':
			if(ud) 	cut.uy = pvf.fval;
			else 	cut.dy = pvf.fval;
			break;
		case 'U':
			if(cut.dbl != ST_DBLUV)
			{
				printf("UV isn't ready\n");
				return 1;
			}
			cut.ux = cut.dx + pvf.fval;
			break;
		case 'V':
			if(cut.dbl != ST_DBLUV)
			{
				printf("UV isn't ready\n");
				return 1;
			}
			cut.uy = cut.dy + pvf.fval;
			break;
		case 'I':
			if(ud) 	cut.ui = pvf.fval;
			else 	cut.di = pvf.fval;
			break;
		case 'J':
			if(ud) 	cut.uj = pvf.fval;
			else 	cut.dj = pvf.fval;
			break;
		case ':':
			if(cut.dbl != ST_DBLON)
			{
				printf("DBL isn't ready\n");
				return 1;
			}
			ud = 1-ud;
			break;
		case 'C':
			printf("WARN: C\n");
			break;
		case ';':
			if(cut.dbl == ST_DBLOFF)
			{
				cut.ux = cut.dx;
				cut.uy = cut.dy;
				cut.ui = cut.di;
				cut.uj = cut.dj;
				ucom = dcom;
			}
			
			if(dcom == -1 || ucom == -1)
			{
				printf("WARN: empty command!\n");
				break;
			}
			
			if(cut.dbl == ST_DBLON)
			{
				if(!ud)
				{
					printf("ERR: double stop!\n");
					return 3;
				}
				ud = 1-ud;
			}
			
			line.cw = 1.0;
			line.x1 = pux;
			line.y1 = puy;
			line.x2 = cut.ux;
			line.y2 = cut.uy;
			line.z1 = cut.zu;
			line.z2 = cut.zu;
			line.xc = cut.ui;
			line.yc = cut.uj;
			switch(ucom)
			{
			case COM_RAPID:
			case COM_LINE:
				line.cw = 0.0;
				break;
			case COM_CCW:
				line.cw = -1.0;
			case COM_CW:
				lu_2dArcRelToAbs(&line);
				break;
			}
			
			fwrite(&cogllaf_line,sizeof(int),1,fup);
			fwrite(&line,sizeof(line),1,fup);
			
			line.cw = 1.0;
			line.x1 = pdx;
			line.y1 = pdy;
			line.x2 = cut.dx;
			line.y2 = cut.dy;
			line.z1 = cut.zd;
			line.z2 = cut.zd;
			line.xc = cut.di;
			line.yc = cut.dj;
			switch(dcom)
			{
			case COM_RAPID:
			case COM_LINE:
				line.cw = 0.0;
				break;
			case COM_CCW:
				line.cw = -1.0;
			case COM_CW:
				lu_2dArcRelToAbs(&line);
				break;
			}
			fwrite(&cogllaf_line,sizeof(int),1,fdown);
			fwrite(&line,sizeof(line),1,fdown);
			
			pux = cut.ux;
			puy = cut.uy;
			pdx = cut.dx;
			pdy = cut.dy;
			
			dcom = ucom = -1;
			
			break;
		default:
			printf("ERR: Unknown command: '%c' (%x)\n",pvf.type, (int)pvf.type);
			return 2;
		}
	}
	
	fclose(fpvf);
	fclose(fup);
	fclose(fdown);
	
	return 0;
}
