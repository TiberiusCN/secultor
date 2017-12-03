#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include <stdio.h>
#include <stdlib.h>
#include "pvf.h"

typedef struct isofile_t
{
	int count;
	pvf_t* data;
} isofile_t;

int ResetISO(char* is);
char* ScanISOProp(char* source, pvf_t *isoprop);
int LoadISOFile(const char* filename, isofile_t* out);
int main(int argc, char ** argv);
