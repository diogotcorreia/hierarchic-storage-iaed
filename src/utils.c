#include "proj2.h"

/**
 * Checks if a character is a space, tab or newline.
 * Returns 1 if so, otherwise returns 0.
 */
short is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

/**
 * Removes whitespace (spaces, tabs or newlines) from the beginning and end
 * of the given string by modifying the string.
 * Returns a pointer to the first non-whitespace character, and places a
 * '\0' after the last non-whitespace character.
 */
char *trim_whitespace(char *str) {
	int i = 0;
	char *last_non_whitespace;
	while (is_whitespace(*str)) ++str;

	for (last_non_whitespace = str; str[i] != '\0'; ++i) {
		if (!is_whitespace(str[i])) {
			last_non_whitespace = str + i;
		}
	}

	*(last_non_whitespace + 1) = '\0';

	return str;
}