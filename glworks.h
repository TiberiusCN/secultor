#pragma once

/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <GL/gl.h>
#ifdef __WINNT__
	#include <windows.h>
#elif defined __unix__
	#include <X11/Xlib.h>
	#include <X11/X.h>
	#include <GL/glx.h>
	#include <X11/keysym.h>
#endif

#ifdef __WINNT__
#define XK_Escape VK_ESCAPE
#define XK_Return VK_RETURN
#define XK_space VK_SPACE
#define XK_a (uint32_t)'A'
#define XK_b (uint32_t)'B'
#define XK_c (uint32_t)'C'
#define XK_d (uint32_t)'D'
#define XK_e (uint32_t)'E'
#define XK_f (uint32_t)'F'
#define XK_g (uint32_t)'G'
#define XK_h (uint32_t)'H'
#define XK_i (uint32_t)'I'
#define XK_j (uint32_t)'J'
#define XK_k (uint32_t)'K'
#define XK_l (uint32_t)'L'
#define XK_m (uint32_t)'M'
#define XK_n (uint32_t)'N'
#define XK_o (uint32_t)'O'
#define XK_p (uint32_t)'P'
#define XK_q (uint32_t)'Q'
#define XK_r (uint32_t)'R'
#define XK_s (uint32_t)'S'
#define XK_t (uint32_t)'T'
#define XK_u (uint32_t)'U'
#define XK_v (uint32_t)'V'
#define XK_w (uint32_t)'W'
#define XK_x (uint32_t)'X'
#define XK_y (uint32_t)'Y'
#define XK_z (uint32_t)'Z'
#endif

int WinInit(int top, int left, int width, int heigth, const char *name);
void WinStart(void(*Render)());
void KeySet(void(*KeyDown)(uint32_t key));
void SetCaption(const char* title);
void ReProject(int rev);
void WinQuit();

extern float gOrthoScale;
