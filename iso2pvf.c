/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "iso2pvf.h"

const char* acceptval = "ABCDEFGHIJKLMNOPQRSTUVWXYZ=";
const char* acceptsig = ";:";

int ResetISO(char* is)
{
	char* os = is;
	while(*is)
	{
		if(*is == '(')
		{
			while(*is && *is != ')') is++;
			if(!*is)
			{
				printf("Err: unfinished comment\n");
				return 1;
			}
			is++;
			continue;
		}
		if(*is == ' ' || *is == '\r' || *is == '\n' || *is == '\t') { is++; continue; }
		*os = *is;
		os++;
		is++;
	}
	*os = 0;
	return 0;
}

char* ScanISOProp(char* source, pvf_t *isoprop)
{
	char *c;
	for(c = acceptval; *c; c++) if(*source == *c) break;
	if(*c)
	{
		isoprop->type = *source;
		source++;
		if((*source >= '0' && *source <= '9') || *source == '+' || *source == '-')
		{
			char* test = source;
			while((*test >= '0' && *test <= '9') || *test == '+' || *test == '-') test++;
			if(*test != '.')
			{
				sscanf(source,"%d",&isoprop->ival);
			} else {
				sscanf(source,"%f",&isoprop->fval);
			}
		}
		while((*source >= '0' && *source <= '9') || *source == '+' || *source == '-' || *source == '.') source++;
		return source;
	}
	for(c = acceptsig; *c; c++) if(*source == *c) break;
	if(*c)
	{
		isoprop->type = *source;
		source++;
		return source;
	}
	return 0;
}

int LoadISOFile(const char* filename, isofile_t* out)
{
	FILE *f = fopen(filename,"rb");
	if(!f) return 1;
	
	fseek(f,0,SEEK_END);
	int len = ftell(f);
	fseek(f,0,SEEK_SET);
	char* data = malloc(len+1);
	fread(data,1,len,f);
	data[len] = 0;
	fclose(f);
	
	int res = ResetISO(data);
	if(res) return 2;
	
	out->count = 0;
	for(char* z = data; *z; z++)
	{
		char *c;
		for(c = acceptval; *c; c++) if(*c == *z) break;
		if(!*c) for(c = acceptsig; *c; c++) if(*c == *z) break;
		if(*c) out->count++;
	}
	
	out->data = malloc( sizeof(pvf_t)*out->count );
	char* q = data;
	int index = 0;
	while(q)
	{
		q = ScanISOProp(q,&out->data[index]);		
		index++;
	}
	
	if(index - 1 != out->count)
	{
		printf("ind %d, count %d\n",index,out->count);
		return 3;
	}
	
	return 0;
}

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		printf("format: iso2pvf <txt iso in> <bin pvf out>\n");
		return 1;
	}
	
	isofile_t iso;
	
	int res;
	
	res = LoadISOFile(argv[1],&iso);
	if(res)
	{
		printf("Read error %d\n",res);
		return 2;
	}
	
	FILE* f = fopen(argv[2],"wb");	
	if(!f)
	{
		printf("Write error %d\n",res);
		return 3;
	}
	for(int i = 0; i < iso.count; i++)
	{
		fwrite(&iso.data[i],sizeof(pvf_t),1,f);
	}
	fclose(f);
	
	return 0;
}
