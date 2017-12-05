/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "iso2pvf.h"

const char* acceptval = "ABCDEFGHIJKLMNOPQRSTUVWXYZ=";
const char* acceptsig = ";:";

void print_help()
{
	printf("\t-i, --input  <file>  input iso-file\n");
	printf("\t-o, --output <value> output pvf-file\n");
	printf("\t-h, --help           this text\n");
	printf("\niso2pvf v1.0 @ GPL v2.0; Copyright (c) TiCaN <tican@protonmail.com> github.com/TiberiusCN\n");
}

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
	if(res) { free(data); return 2; }
	
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
		printf("Err: readed %d, but supposed %d\n",index,out->count);
		free(data);
		return 3;
	}

	free(data);	
	return 0;
}

int main(int argc, char ** argv)
{
	const char* infile = 0;
	const char* outfile = 0;

	for(int i = 1; i < argc; i++)
	{
		if((!strncmp(argv[i],"-i",3))||(!strncmp(argv[i],"--input",8)))
		{
			i++;
			if(i == argc)
			{
				printf("Err: input file not specified\n");
				return 1;
			}
			infile = argv[i];
			continue;
		}
		if((!strncmp(argv[i],"-o",3))||(!strncmp(argv[i],"--output",9)))
		{
			i++;
			if(i == argc)
			{
				printf("Err: output file not specified\n");
				return 1;
			}
			outfile = argv[i];
			continue;
		}
		if((!strncmp(argv[i],"-h",3))||(!strncmp(argv[i],"--help",8)))
		{
			print_help();
			return 0;
		}
		printf("Err: unknown option %s\n",argv[i]);
		return 1;
	}

	if(!infile || !outfile)
	{
		printf("Err: input or output not specified\n");
		print_help();
		return 1;
	}

	isofile_t iso;
	
	int res;
	
	res = LoadISOFile(infile,&iso);
	if(res)
	{
		printf("Read error %d\n",res);
		return 2;
	}
	
	FILE* f = fopen(outfile,"wb");	
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
	free(iso.data);
	
	return 0;
}
