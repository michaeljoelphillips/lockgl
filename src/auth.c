#include <shadow.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "auth.h"

static const char *get_password(const char *user)
{
	struct spwd *sp = getspnam(user);

	if (!sp) {
		return NULL;
	}

	endspent();

	return sp->sp_pwdp;
}

// Restore the uid and gid of the process to that of the current user
static int remove_privileges()
{
	return setgid(getgid()) | setuid(getuid());
};

const user *get_user()
{
	user *current_user = malloc(sizeof(user));

	current_user->username = getenv("USER");
	current_user->password = get_password(current_user->username);

	remove_privileges();

	if (current_user->password == NULL) {
		free(current_user);

		return NULL;
	}

	return current_user;
}

int authenticate(const user *user, const char *password)
{
	const char *hash = crypt(password, user->password);

	return strcmp(hash, user->password) == 0;
}
