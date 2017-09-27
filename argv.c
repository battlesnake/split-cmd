#include <string.h>
#include <stdlib.h>
#include "argv.h"
#include "split_cmd.h"

int parse_arg_vector(const char *cmd, struct arg_vector *av)
{
	*av = ARG_VECTOR_INIT;
	int argc = split_cmd(cmd, NULL, NULL);
	if (argc <= 0) {
		return argc;
	}
	av->capacity = argc + 1;
	size_t argv_size = sizeof(*av->argv) * av->capacity;
	size_t sb_len = argv_size + strlen(cmd) + 1;
	av->superbuf = malloc(sb_len);
	av->superbuf_idx = argv_size;
	av->argv = av->superbuf;
	int ret = split_cmd(cmd, av, make_arg_vector);
	if (ret < 0) {
		free_arg_vector(av);
		return ret;
	}
	return argc;
}

int make_arg_vector(void *state, int argi, const char *cmd, const char *begin, const char *end)
{
	(void) cmd;
	struct arg_vector *p = state;
	if (p->argc != argi) {
		free_arg_vector(state);
		return SC_ERR_MAV_INVALID_ITERATOR;
	}
	if ((size_t) p->argc + 1 >= p->capacity) {
		p->capacity += SC_MAV_ALLOCBY;
		p->argv = realloc(p->argv, sizeof(*p->argv) * p->capacity);
	}
	size_t arglen = end - begin;
	size_t buflen = arglen + 1;
	char *s = p->superbuf ? (char *) p->superbuf + p->superbuf_idx : malloc(buflen);
	memcpy(s, begin, arglen);
	s[arglen] = 0;
	p->argv[p->argc++] = s;
	p->argv[p->argc] = 0;
	p->superbuf_idx += buflen;
	return 0;
}

void free_arg_vector(struct arg_vector *p)
{
	if (p->superbuf) {
		free(p->superbuf);
	} else {
		for (int i = 0; i < p->argc; i++) {
			free(p->argv[i]);
		}
		free(p->argv);
	}
}
