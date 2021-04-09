#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <stdlib.h>

static GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER,
			      None };

static GLXContext glx_context;
static XWindowAttributes window_attributes;

Window OpenWindow(Display *display, Window root, int screen)
{
	XVisualInfo *xvisual_info =
		glXChooseVisual(display, screen, attributes);
	glx_context = glXCreateContext(display, xvisual_info, NULL, GL_TRUE);

	XGetWindowAttributes(display, root, &window_attributes);

	XSetWindowAttributes set_xwindow_attrib = {
		.colormap = XCreateColormap(display, root, xvisual_info->visual,
					    AllocNone),
		.event_mask = ExposureMask | KeyPressMask,
		.override_redirect = 1,
		.background_pixel = 65280,
	};

	Window window = XCreateWindow(
		display, root, 0, 0, window_attributes.width,
		window_attributes.height, 0, xvisual_info->depth, InputOutput,
		xvisual_info->visual,
		CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
		&set_xwindow_attrib);

	glXMakeCurrent(display, window, glx_context);
	glEnable(GL_DEPTH_TEST);

	XMapRaised(display, window);
	XStoreName(display, window, "nlock");
	XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync,
		      CurrentTime);

	return window;
}

void Cleanup(Display *display)
{
	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, glx_context);
}

void Draw(Display *display, const Window *windows, int screens)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glBegin(GL_QUADS);
	glColor3f(1., 0., 0.);
	glVertex3f(-1., -1., 0.);
	glColor3f(0., 1., 0.);
	glVertex3f(1., -1., 0.);
	glColor3f(0., 0., 1.);
	glVertex3f(1., 1., 0.);
	glColor3f(1., 1., 0.);
	glVertex3f(-1., 1., 0.);
	glEnd();

	for (int s = 0; s < screens; s++) {
		glXSwapBuffers(display, windows[s]);
	}
}
