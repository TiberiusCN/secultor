#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

typedef struct pvf_t
{
	char type; //symbol
	union
	{
		int ival;
		float fval;
	};
} pvf_t;
