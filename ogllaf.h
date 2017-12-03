#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

//OpenGL Line/Arc File

#define ogllaf_line 0
#define ogllaf_color 1
#define ogllaf_matrix_proj 2
#define ogllaf_matrix_view 3
#define ogllaf_type 4

typedef struct {float f[3];} ogllaf_color_t;
typedef struct {float f[3];} ogllaf_rotate_t;
