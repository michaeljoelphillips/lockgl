#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auth.h"
#include "display.h"

void clear();
void on_backspace();
void on_escape();
void on_enter();
const char *on_keypress(const key_pressed *event);

const user *current_user;
int password_length = 0;
char password_buffer[255];

callbacks display_callbacks = {
	.on_backspace = on_backspace,
	.on_escape = clear,
	.on_enter = on_enter,
	.on_keypress = on_keypress,
};

const char *on_keypress(const key_pressed *event)
{
	if (password_length + event->size + 1 >= sizeof(password_buffer)) {
		password_length = 0;
	}

	// NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
	strncat(password_buffer, event->data, event->size);
	password_length += event->size;

	return password_buffer;
}

void on_backspace()
{
	password_buffer[--password_length] = '\0';
}

void on_enter()
{
	if (!authenticate(current_user, password_buffer)) {
		return clear();
	}

	return unlock();
}

void clear()
{
	// NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
	memset(password_buffer, 0, sizeof(password_buffer));
	password_length = 0;
}

int main()
{
	current_user = get_user();

	if (current_user == NULL) {
		printf("Failed to access current username or password");

		return 1;
	}

	lock(&display_callbacks);
	free((void *)current_user);
}
