#if 0
set -e
cd "$(dirname "$0")"
gcc -O3 -Wall -Wextra -Wno-parentheses -Werror -ffunction-sections -fdata-sections -flto -Wl,--gc-sections -o test *.c
exec valgrind -q --leak-check=full --track-origins=yes ./test
exit 1
#endif
#include <stdio.h>
#include "split_cmd.h"
#include "argv.h"

static int consumer(void *state, int argi, const char *cmd, const char *begin, const char *end)
{
	(void) state;
	(void) cmd;
	size_t buflen = 1 + end - begin;
	char buf[buflen];
	snprintf(buf, buflen, "%s", begin);
	printf("  [%d] = %s\n", argi, buf);
	return 0;
}

const char *test_strings[] = {
	"",
	"cmd",
	"cmd arg",
	"cmd arg arg",
	"cmd 'single quote' \"double quote\"",
	"cmd 'single\"quote' \"double'quote\"",
	" \t cmd \t arg \t 'single\"quote' \t \"double'quote\" \t ",
	"0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z",
	0
};

static int test_simple()
{
	for (const char **it = test_strings; *it; ++it) {
		const char *s = *it;
		printf("Testing: <%s>\n", s);

		int ret = split_cmd(s, NULL, consumer);

		printf("Result: %d\n\n", ret);

		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

static int test_mav()
{
	for (const char **it = test_strings; *it; ++it) {
		const char *s = *it;
		struct arg_vector av = ARG_VECTOR_INIT;
		printf("Testing: <%s>\n", s);

		int ret = split_cmd(s, &av, make_arg_vector);

		for (int i = 0; i < av.argc; i++) {
			printf("  [%d] = %s\n", i, av.argv[i]);
		}

		free_arg_vector(&av);

		printf("Result: %d\n\n", ret);

		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

static int test_mav_efficient()
{
	for (const char **it = test_strings; *it; ++it) {
		const char *s = *it;
		struct arg_vector av = ARG_VECTOR_INIT;
		printf("Testing: <%s>\n", s);

		int ret = parse_arg_vector(s, &av);

		for (int i = 0; i < av.argc; i++) {
			printf("  [%d] = %s\n", i, av.argv[i]);
		}

		free_arg_vector(&av);

		printf("Result: %d\n\n", ret);

		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

#define RUN_TESTS(fn) \
	do { \
		int ret = (fn)(); \
		if (ret != 0) { \
			fprintf(stderr, "\x1b[1;31mERROR: Test %s failed with code %d\x1b[0m\n", #fn, ret); \
			return ret; \
		} \
	} while (0)

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	RUN_TESTS(test_simple);
	RUN_TESTS(test_mav);
	RUN_TESTS(test_mav_efficient);
	return 0;
}
