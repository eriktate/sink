#include "intern.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t intern_size = 4096;
const size_t arena_size = intern_size * 1024;

struct intern *intern = NULL;
struct arena *strings = NULL;

void initial_alloc() {
	intern = (struct intern *)malloc(sizeof(struct intern));
	strings = (struct arena *)malloc(sizeof(struct arena));

	intern->len = 0;
	intern->cap = sizeof(char *) * intern_size;
	intern->strings = (char **)malloc(intern->cap);

	strings->len = 0;
	strings->cap = sizeof(char) * arena_size;
	strings->data = (char *)malloc(strings->cap);
	strings->next = strings->data;
}

const int is_interned(const char *str) {
	for(int i = 0; i < intern->len; i++) {
		if (strcmp(intern->strings[i], str) == 0) {
			return i;
		}
	}

	return -1;
}

const int is_interned_slice(const char *str, size_t start, size_t end) {
	int len = end - start;

	for(int i = 0; i < intern->len; i++) {
		for(int j = 0; j < len; j++) {
			// printf("Comparing %c with %c\n", intern->strings[i][j], str[start + j]);
			if (str[j] == '\0') {
				break;
			}

			if (intern->strings[i][j] != str[start + j]) {
				break;
			}

			if (j == len - 1) {
				return i;
			}
		}
	}

	return -1;
}

const size_t intern_string(char *str) {
	if (intern == NULL) {
		initial_alloc();
	}

	int interned = is_interned(str);
	if (interned >= 0) {
		return interned;
	}

	size_t len = strlen(str) + 1;
	if (strings->len + len > strings->cap) {
		printf("Blew out the arena!\n");
		exit(1);
	}

	memcpy(strings->next, str, sizeof(char) * len);
	intern->strings[intern->len] = strings->next;
	strings->next += len;

	return (intern->len)++;
}

const size_t intern_slice(char *str, size_t start, size_t end) {
	if (intern == NULL) {
		initial_alloc();
	}

	int interned = is_interned_slice(str, start, end);
	if (interned >= 0) {
		return interned;
	}

	size_t len = end - start;
	if (strings->len + len > strings->cap) {
		printf("Blew out the arena!\n");
		exit(1);
	}

	memcpy(strings->next, &str[start], sizeof(char) * len);
	intern->strings[intern->len] = strings->next;
	strings->next[len] = '\0';
	strings->next += (len + 1);

	return (intern->len)++;
}

const char *get_interned(size_t idx) {
	return intern->strings[idx];
}
