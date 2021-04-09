typedef struct {
	const char *data;
	const int size;
} key_pressed;

typedef void (*backspace_callback)();
typedef void (*cancel_callback)();
typedef void (*enter_callback)();
typedef const char *(*keypress_callback)(const key_pressed *event);

typedef struct {
	backspace_callback on_backspace;
	cancel_callback on_escape;
	enter_callback on_enter;
	keypress_callback on_keypress;
} callbacks;

void lock(callbacks *callbacks);
void unlock();
