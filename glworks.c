/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "glworks.h"

#ifdef __WINNT__
	HWND gwnd = 0;
	HDC gdc = 0;
	HANDLE grc = 0;
#elif defined __unix__
	Display *dpy;
	Window root, gwnd;
	GLint att[] = { GLX_RGBA,  GLX_DOUBLEBUFFER, None };
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes swa;
	GLXContext glc;
	XWindowAttributes gwa;
	XEvent xev;
#endif

float gaspect = 1.0;
int gw,gh;
float gOrthoScale = 50.0;
void(*GKeyDown)(uint32_t key) = 0;
int quitflag = 0;

void SetCaption(const char* title)
{
#ifdef __WINNT__
	SetWindowText(gwnd,title);
#elif defined __unix__
	XStoreName(dpy,gwnd,title);
#endif
}

void WinQuit()
{
	quitflag = 1;
}

void ReProject(int rev)
{
	static int gproj = 0;
	
	if(rev) gproj = 1-gproj;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(gproj)
	{
		glOrtho(-gOrthoScale*gaspect,gOrthoScale*gaspect,-gOrthoScale,gOrthoScale,1.0,1000.0);
	} else {
		glFrustum(-gaspect,gaspect,-1.0,1.0,1.0,1000.0);
	}
	glMatrixMode(GL_MODELVIEW);
}

void KeySet(void(*KeyDown)(uint32_t key))
{
	GKeyDown = KeyDown;
}

#ifdef __WINNT__
	LRESULT CALLBACK MainWinProc(HWND hw,UINT msg,WPARAM wp,LPARAM lp)
	{
		float hf; float wf;

		if(quitflag) PostQuitMessage(0);
		
		switch (msg) 
		{
		case WM_SIZE:
			gw = LOWORD(lp); wf = gw;
			gh = HIWORD(lp); hf = gh;
			
			gaspect = wf/hf;
			
			glViewport(0,0,gw,gh);
			ReProject(0);
			
			Render();
			SwapBuffers( gdc );
			return 0;
		case WM_KEYDOWN:
			if(GKeyDown)
			{
				uint32_t key = wp;
				GKeyDown(key);
				Render();
				SwapBuffers( gdc );
			}
			return 0;
		case WM_CREATE:
			return 0;
		case WM_PAINT:
			Render();
			SwapBuffers( gdc );
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hw,msg,wp,lp);
	}
#endif

int WinInit(int top, int left, int width, int height, const char* name)
{	
	gw = width;
	gh = height;
	gaspect = (float)gw/(float)gh;

	#ifdef __WINNT__
		HINSTANCE hInst = GetModuleHandle(0);
		
		WNDCLASS wc;
		wc.style = CS_HREDRAW|CS_VREDRAW;
		wc.lpfnWndProc = MainWinProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+3);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = name;
		if (!RegisterClass(&wc)) return 1;
		
		gwnd = CreateWindow(name,name,WS_OVERLAPPEDWINDOW,left,top,width,height,0,0,hInst,0);
		
		if(!gwnd) return 2;
		
		if(gw+gh) ShowWindow(gwnd,1);
		else ShowWindow(gwnd,SW_MAXIMIZE);
		UpdateWindow(gwnd);
		
		gdc = GetDC( gwnd );

		PIXELFORMATDESCRIPTOR pfd;
		int format;
		
		ZeroMemory( &pfd, sizeof( pfd ) );
		pfd.nSize = sizeof( pfd );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;
		format = ChoosePixelFormat( gdc, &pfd );
		SetPixelFormat( gdc, format, &pfd );
		
		grc = wglCreateContext( gdc );
		wglMakeCurrent( gdc, grc );
		
		const char *extensions = (const char*)glGetString( GL_EXTENSIONS );
		
		if( strstr( extensions, ("WGL_EXT_swap_control") ) == 0 )
			return;
		else
		{
			BOOL (APIENTRY *wglSwapIntervalEXT)( int ) = 
			wglGetProcAddress( "wglSwapIntervalEXT" );
		
		#ifdef VSYNC
			BOOL vs = TRUE;
		#else
			BOOL vs = FALSE;
		#endif
		
		if( wglSwapIntervalEXT )
			wglSwapIntervalEXT( vs );
		}
	#elif defined __unix__
		dpy = XOpenDisplay(0);
		if(!dpy)
		{
			printf("XServer?\n");
			return 1;
		}
		root = DefaultRootWindow(dpy);
		vi = glXChooseVisual(dpy,0,att);
		if(!vi)
		{
			printf("Bad visual\n");
			return 2;
		}

		cmap = XCreateColormap(dpy,root,vi->visual,AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask;

		gwnd = XCreateWindow(dpy,root,left,top,600,600,0,vi->depth,InputOutput,vi->visual,CWColormap|CWEventMask,&swa);
		XMapWindow(dpy,gwnd);
		XStoreName(dpy,gwnd,"Ogllaf");

		glc = glXCreateContext(dpy,vi,0,GL_TRUE);
		glXMakeCurrent(dpy,gwnd,glc);

	#endif
	

	return 0;
}

void WinStart(void(*Render)())
{
	if(!Render) return;

	quitflag = 0;

	#ifdef __WINNT__
		MSG msg;
		
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glViewport(0,0,gw,gh);
		ReProject(0);
		Render();
		SwapBuffers( gdc );
	 
		while (GetMessage(&msg,NULL,0,0)) 
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( grc );
		ReleaseDC( gwnd, gdc );
		DestroyWindow(gwnd);
	#elif defined __unix__
		while(1)
		{
			if(quitflag)
			{
				glXMakeCurrent(dpy,None,0);
				glXDestroyContext(dpy,glc);
				XDestroyWindow(dpy,gwnd);
				XCloseDisplay(dpy);
				break;
			}
			XNextEvent(dpy,&xev);
			if(xev.type == Expose)
			{
				XGetWindowAttributes(dpy,gwnd,&gwa);
				gw = gwa.width;
				gh = gwa.height;
				gaspect = (float)gw/(float)gh;
				glViewport(0,0,gw,gh);
				ReProject(0);
				Render();
				glXSwapBuffers(dpy,gwnd);
			}
			else if (xev.type == KeyPress)
			{
				if(GKeyDown) GKeyDown(XKeycodeToKeysym(dpy,((XKeyPressedEvent*)&xev)->keycode,0));
				Render();
				glXSwapBuffers(dpy,gwnd);
			}
		}
	#endif
}
