#ifndef _INTERN_H
#define _INTERN_H

#include <stdbool.h>
#include <stdlib.h>

struct arena {
	size_t len;
	size_t cap;
	char *next;
	char *data;
};

struct intern {
	size_t len;
	size_t cap;
	char **strings;
};

extern struct intern *intern;
extern struct arena *strings;

int is_interned(const char *str);
int is_interned_slice(const char *str, size_t start, size_t end);
size_t intern_string(char *str);
size_t intern_slice(char *str, size_t start, size_t end);
char *get_interned(size_t idx);

#endif // _INTERN_H
