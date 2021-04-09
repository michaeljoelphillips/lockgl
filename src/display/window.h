#include <X11/Xlib.h>

Window open_window(Display *display, Window root, int screen);
void cleanup(Display *display);
void draw(Display *display, const Window *windows, int screens);
