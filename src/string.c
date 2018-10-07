#include <stdlib.h>
#include <string.h>

typedef struct string {
	size_t len;
	size_t cap;
	char *data;
} string;

string new_string(int cap) {
	return (string){
		.len = 0,
		.cap = cap,
		.data = (char *)malloc(cap)
	};
}

string new_string_from_char(char *cstring) {
	const size_t len = strlen(cstring);
	return (string){
		.len = len,
		.cap = len,
		.data = cstring,
	};
}

unsigned char str_equal(string a, string b) {
	if (a.len != b.len) {
		return 0;
	}

	for (int i = 0; i < a.len; i++) {
		if (a.data[i] != b.data[i]) {
			return 0;
		}
	}

	return 1;
}

unsigned char str_grow(string *str, int amount) {
	const size_t new_len = str->cap + amount;
	char *new_data = (char *) malloc(new_len);
	if (new_data == NULL) {
		return 1;
	}

	memcpy(new_data, str->data, str->len);

	str->data = new_data;
	return 0;
}

int str_append(string *a, string b) {
	if ((a->cap - a->len) >= b.len || !str_grow(a, b.len * 2)) {
		memcpy(&a->data[a->len], b.data, b.len);
		a->len += b.len;

		return 0;
	}

	return 1;
}

unsigned char str_push(string *a, char c) {
	if ((a->cap - a->len) > 1 || !str_grow(a, 10)) {
		a->data[a->len++] = c;
		return 0;
	}

	return 1;
}

