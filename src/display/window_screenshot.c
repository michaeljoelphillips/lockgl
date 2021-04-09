#include <GL/glew.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../shaders.c"

const char *k_fragment_shader;
const char *k_vertex_shader;

static GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER,
			      None };

struct timespec runtime;
struct timespec start_time;
static GLXContext glx_context;
static XWindowAttributes window_attributes;
XImage *ximage;

int frame = 0;
int initialized = 0;
unsigned int vertex_array;
unsigned int shader_program;

Window open_window(Display *display, Window root, int screen)
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

	glewExperimental = GL_TRUE;
	glewInit();

	clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
	ximage = XGetImage(display, RootWindow(display, 0), 0, 0,
			   window_attributes.width, window_attributes.height,
			   AllPlanes, ZPixmap);

	k_vertex_shader =
		load_shader("/usr/share/lockgl/passthrough.vertex.glsl");
	k_fragment_shader = load_shader("/usr/share/lockgl/blur.fragment.glsl");

	return window;
}

void cleanup(Display *display)
{
	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, glx_context);

	free((void *)k_vertex_shader);
	free((void *)k_fragment_shader);
}

void initialize()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	float vertices[] = {
		1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // TR
		1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, // BR
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // BL
		-1.0f, 1.0f,  0.0f, 0.0f, 0.0f, // TL
	};

	unsigned int indices[] = {
		0, 1, 3, 1, 2, 3,
	};

	glGenVertexArrays(1, &vertex_array);

	unsigned int vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		     GL_STATIC_DRAW);

	unsigned int element_buffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		     GL_STATIC_DRAW);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, window_attributes.width,
		     window_attributes.height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
		     (void *)(&(ximage->data[0])));
	glGenerateMipmap(GL_TEXTURE_2D);

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &k_vertex_shader, NULL);
	glCompileShader(vertex_shader);

	int compilation_status;
	char log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compilation_status);
	glGetShaderInfoLog(vertex_shader, 512, NULL, log);

	if (!compilation_status) {
		printf("Failed to compile vertex shader: %s", log);
	}

	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &k_fragment_shader, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compilation_status);
	glGetShaderInfoLog(fragment_shader, 512, NULL, log);

	if (!compilation_status) {
		printf("Failed to compile fragment shader: %s", log);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetShaderiv(shader_program, GL_COMPILE_STATUS, &compilation_status);
	glGetShaderInfoLog(shader_program, 512, NULL, log);

	if (!compilation_status) {
		printf("Failed to create shader program: %s", log);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
			      (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
			      (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	initialized = 1;
}

void draw(Display *display, const Window *windows, int screens)
{
	if (frame < 120) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		frame++;
	}

	if (!initialized) {
		initialize();
	}

	glUseProgram(shader_program);
	glBindVertexArray(vertex_array);
	glUniform1i(glGetUniformLocation(shader_program, "frame"), frame);
	glUniform2f(glGetUniformLocation(shader_program, "screen_size"),
		    window_attributes.width, window_attributes.height);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	for (int s = 0; s < screens; s++) {
		glXSwapBuffers(display, windows[s]);
	}
}
