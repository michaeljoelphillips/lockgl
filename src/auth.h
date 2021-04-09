typedef struct {
	const char *username;
	const char *password;
} user;

const user *get_user();

int authenticate(const user *user, const char *password);
