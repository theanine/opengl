#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <GL/glext.h>

#define WINDOW_WIDTH   300
#define WINDOW_HEIGHT  600
#define COLOR_DEPTH    3

void DrawAQuad(void)
{	
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, 20.);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0.);
	
	glBegin(GL_QUADS);
		glColor3f(1, 0, 0); glVertex3f(-.75, -.75, 0.);
		glColor3f(1, 0, 0); glVertex3f( .75, -.75, 0.);
		glColor3f(1, 0, 0); glVertex3f( .75,  .75, 0.);
		glColor3f(1, 1, 0); glVertex3f(-.75,  .75, 0.);
	glEnd();
}

void DrawPixel(int x, int y)
{
	printf("Pixel (%d, %d)\n", x, y);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
		glVertex2i(100, 40);
		glVertex2i(120, 50);
		glVertex2i(140, 80);
	glEnd();
	// glBegin(GL_POINTS);
	//  glColor3f(1, 1, 1); glVertex2i(x, y);
	// glEnd();
	glFlush();
}

void DrawACircle(int x0, int y0, int radius)
{
	int x = radius;
	int y = 0;
	int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0
	
	while (y <= x) {
		DrawPixel( x + x0,  y + y0); // Octant 1
		DrawPixel( y + x0,  x + y0); // Octant 2
		DrawPixel(-x + x0,  y + y0); // Octant 4
		DrawPixel(-y + x0,  x + y0); // Octant 3
		DrawPixel(-x + x0, -y + y0); // Octant 5
		DrawPixel(-y + x0, -x + y0); // Octant 6
		DrawPixel( x + x0, -y + y0); // Octant 7
		DrawPixel( y + x0, -x + y0); // Octant 8
		y++;
		if (decisionOver2<=0) {
			decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
		} else {
			x--;
			decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		}
	}
}

int main(int argc, char *argv[])
{
	Display* x_disp = XOpenDisplay(NULL);
	
	if (!x_disp) {
		printf("ERROR: Failed to connect to X server\n");
		exit(1);
	}
	
	Window root = DefaultRootWindow(x_disp);
	
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* vi = glXChooseVisual(x_disp, 0, att);
	
	if (!vi) {
		printf("ERROR: Unable to find a visual\n");
		exit(2);
	} else {
		printf("Visual 0x%lX selected\n", vi->visualid);
	}
	
	XSetWindowAttributes swa;
	swa.colormap   = XCreateColormap(x_disp, root, vi->visual, AllocNone);
	swa.event_mask = ExposureMask | KeyPressMask;
	
	Window win = XCreateWindow(x_disp, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	
	XMapWindow(x_disp, win);
	XStoreName(x_disp, win, "VERY SIMPLE APPLICATION");
	
	GLXContext glc = glXCreateContext(x_disp, vi, NULL, GL_TRUE);
	glXMakeCurrent(x_disp, win, glc);
	
	glEnable(GL_DEPTH_TEST); 
	
	while (1) {
		XEvent xev;
		XNextEvent(x_disp, &xev);
		
		if (xev.type == Expose) {
			XWindowAttributes gwa;
			XGetWindowAttributes(x_disp, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			DrawAQuad();
			glXSwapBuffers(x_disp, win);
		} else if (xev.type == KeyPress) {
			glXMakeCurrent(x_disp, None, NULL);
			glXDestroyContext(x_disp, glc);
			XDestroyWindow(x_disp, win);
			XCloseDisplay(x_disp);
			exit(0);
		}
	}
}