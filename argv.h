#pragma once
#include <stddef.h>

/*
 * Iterator for split_cmd, which produces a typical argc/argv pair.
 *
 * A NULL pointer is appended after the last argument in argv.
 *
 * "capacity" is for internal use and should be ignored.
 *
 * Initialise your arg_vector with ARG_VECTOR_INIT before passing it (and
 * make_arg_vector) to split_cmd.
 *
 * Use free_arg_vector to free the resources which are allocated by the iteratur
 * in order to hold the argument string list.
 *
 * The strings in the argv array are dynamically allocated (as is argv itself)
 * and are NULL-terminated (as is argv itself).
 */

/* arg_vector was not initialised properly */
#define SC_ERR_MAV_INVALID_ITERATOR -100

/*
 * Grow argv by 8 items if it is full and we need to append another item to it.
 */
#define SC_MAV_ALLOCBY 8

struct arg_vector
{
	int argc;
	char **argv;
	/* For internal use */
	size_t capacity;
	void *superbuf;
	size_t superbuf_idx;
};

static const struct arg_vector ARG_VECTOR_INIT = {
	.argc = 0,
	.argv = NULL,
	.capacity = 0,
	.superbuf = NULL,
	.superbuf_idx = 0,
};

/* Parse an argument vector in one-step, with only one dynamic allocation */
int parse_arg_vector(const char *cmd, struct arg_vector *av);

/* Iterator for use with split_cmd */
int make_arg_vector(void *state, int argi, const char *cmd, const char *begin, const char *end);

/* Free resources tied up by the arg_vector */
void free_arg_vector(struct arg_vector *p);
