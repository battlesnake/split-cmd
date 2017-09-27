#pragma once

/* Parser reached end of string while scanning for a closing quote */
#define SC_ERR_UNTERMINATED_QUOTE -1

/*
 * begin/end refer to positions within the original string, so the argument
 * itself can NOT be assumed to be null-terminated.
 *
 * argi is the number of arguments previously passed to the consumer.
 */
typedef int arg_consumer(void *state, int argi, const char *cmd, const char *begin, const char *end);

/*
 * Splits a delimited command-line string into individual arguments and passes
 * each one in turn to a stateful interator.
 *
 * If first character of an argument is a quote character, argument ends when
 * that character is next seen.
 *
 * Quote characters: ' "
 * Delimiter characters: <space> <tab>
 *
 * Returns number of arguments parsed, or negative value on error.
 * "consumer" may be NULL if you only care about the number of arguments.
 *
 * If consumer returns negative value, then split_cmd aborts and returns that
 * value.
 */
int split_cmd(const char *cmd, void *state, arg_consumer *consumer);
