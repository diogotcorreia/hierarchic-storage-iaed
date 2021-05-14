#include "proj2.h"

short is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

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