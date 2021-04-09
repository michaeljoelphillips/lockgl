#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../display.h"
#include "window.h"

XEvent event;
KeySym keysym;
char buffer[32];
int locked = 1;

Display *display;
int screens;
Window *windows;

pthread_t animation_thread;

void create_windows()
{
	screens = ScreenCount(display);
	windows = (Window *)malloc(sizeof(Window) * screens + 1);

	for (int s = 0; s < screens; s++) {
		windows[s] = open_window(display, RootWindow(display, s), s);
	}
}

int is_special(KeySym keysym)
{
	return IsFunctionKey(keysym) || IsKeypadKey(keysym) ||
	       IsMiscFunctionKey(keysym) || IsPFKey(keysym) ||
	       IsPrivateKeypadKey(keysym);
}

void tick(callbacks *callbacks)
{
	draw(display, windows, screens);

	for (int idx = 0; idx < screens; idx++) {
		XMapRaised(display, windows[idx]);
		XGrabKeyboard(display, windows[idx], True, GrabModeAsync,
			      GrabModeAsync, CurrentTime);
	}

	if (!XCheckTypedEvent(display, KeyPress, &event)) {
		return;
	}

	int length =
		XLookupString(&event.xkey, buffer, sizeof buffer, &keysym, 0);

	switch (keysym) {
	case XK_Escape:
		callbacks->on_escape();
		break;
	case XK_BackSpace:
		callbacks->on_backspace();
		break;
	case XK_Return:
	case XK_KP_Enter:
		callbacks->on_enter();
		break;
	default:
		if (is_special(keysym)) {
			break;
		}

		const key_pressed key_event = {
			.data = buffer,
			.size = length,
		};

		callbacks->on_keypress(&key_event);

		break;
	}
}

void lock(callbacks *callbacks)
{
	display = XOpenDisplay(NULL);
	create_windows();

	while (locked) {
		tick(callbacks);
	}

	cleanup(display);
	XCloseDisplay(display);
}

void unlock()
{
	locked = 0;
}
