#include "auth.c"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

struct spwd *__wrap_getspnam(char *name)
{
	struct spwd *password;

	password = (struct spwd *)malloc(sizeof(struct spwd));

	password->sp_namp = "test_user";
	password->sp_pwdp = "encrypted_password";

	return password;
}

void __wrap_endspent()
{
	function_called();
}

__gid_t __wrap_getgid()
{
	return 1000;
}

__gid_t __wrap_getuid()
{
	return 1000;
}

int __wrap_setuid(__gid_t uid)
{
	check_expected(uid);

	return mock();
}

int __wrap_setgid(__gid_t gid)
{
	check_expected(gid);

	return mock();
}

void test_get_password(void **state)
{
	const char *user = "test_user";

	expect_function_call(__wrap_endspent);

	const char *result = get_password(user);

	assert_non_null(result);
	assert_string_equal(result, "encrypted_password");
}

void test_drop_privileges(void **state)
{
	expect_value(__wrap_setuid, uid, 1000);
	expect_value(__wrap_setgid, gid, 1000);
	will_return(__wrap_setuid, 0);
	will_return(__wrap_setgid, 0);

	assert_int_equal(0, remove_privileges());

	expect_value(__wrap_setuid, uid, 1000);
	expect_value(__wrap_setgid, gid, 1000);
	will_return(__wrap_setuid, -1);
	will_return(__wrap_setgid, 0);

	assert_int_equal(-1, remove_privileges());

	expect_value(__wrap_setuid, uid, 1000);
	expect_value(__wrap_setgid, gid, 1000);
	will_return(__wrap_setuid, 0);
	will_return(__wrap_setgid, -1);

	assert_int_equal(-1, remove_privileges());
}

int main()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_get_password),
		cmocka_unit_test(test_drop_privileges),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
