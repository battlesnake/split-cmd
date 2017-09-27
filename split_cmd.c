#include <stdbool.h>
#include <string.h>
#include "split_cmd.h"

const char qt_none = 0;
const char qt_single = '\'';
const char qt_double = '"';

static bool is_delimiter(char c)
{
	return c == ' ' || c == '\t';
}

static bool is_quote(char c)
{
	return c == qt_single || c == qt_double;
}

int split_cmd(const char *cmd, void *state, arg_consumer *consumer)
{
	/* Arg beginning */
	const char *begin = NULL;
	/* Open quote char */
	char quote = 0;
	/* Count */
	int argc = 0;
	for (const char *it = cmd; ; ++it) {
		char c = *it;
		bool d = is_delimiter(c) || c == 0;
		bool q = is_quote(c);
		if (begin) {
			/* End of argument */
			if (quote == qt_none && d || quote != qt_none && c == quote) {
				int ret = consumer ? consumer(state, argc, cmd, begin, it) : 0;
				if (ret < 0) {
					return ret;
				}
				quote = qt_none;
				begin = NULL;
				argc++;
			}
		} else {
			/* Start of argument */
			if (q) {
				quote = c;
				begin = it + 1;
			} else if (!d) {
				quote = qt_none;
				begin = it;
			}
		}
		if (!c) {
			break;
		}
	}
	if (quote) {
		return SC_ERR_UNTERMINATED_QUOTE;
	}
	return argc;
}
