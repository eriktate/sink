#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

// special symbols allowed
// TODO (optimization): Make this a big switch statement.
const char symbols[] = "+-*/%=!<>&|^~?:;.,{}[]()";
const char symbol_groups[SYMBOL_GROUP_COUNT][4] = {
	{ "++" },
	{ "--" },
	{ "==" },
	{ "!=" },
	{ ">=" },
	{ "<=" },
	{ "&&" },
	{ "||" },
	{ ">>" },
	{ "<<" },
	{ "+=" },
	{ "-=" },
	{ "*=" },
	{ "/=" },
	{ "%=" },
	{ "<<=" },
	{ ">>=" },
	{ "&=" },
	{ "^=" },
	{ "|=" },
	{ "->" },
	{ ":=" }
};

const char whitespace[] = " \t\n";

unsigned char is_symbol(char c) {
	for(int i = 0; i < sizeof(symbols); i++) {
		if (c == symbols[i]) {
			return 1;
		}
	}

	return 0;
}

void print_token(struct token tok) {
	for(int i = 0; i < tok.len; i++) {
		printf("%c", tok.data[i]);
	}
}

unsigned char is_symbol_group(struct token c) {
	for(int i = 0; i < SYMBOL_GROUP_COUNT; i++) {
		for(int j = 0; j < c.len; j++) {
			if (c.data[j] != symbol_groups[i][j]) {
				break;
			}

			if (j == c.len - 1) {
				return 1;
			}
		}
	}

	return 0;
}

unsigned char is_whitespace(char c) {
	for(int i = 0; i < sizeof(whitespace); i++) {
		if (c == whitespace[i]) {
			return 1;
		}
	}

	return 0;
}

struct lex_result init_result() {
	return (struct lex_result) {
		.len = 0,
		.cap = RESULT_BUFFER_SIZE,
		.tokens = (struct token *)malloc(sizeof(struct token) * RESULT_BUFFER_SIZE)
	};
}

struct token new_token(int cap) {
	return (struct token){
		.len = 0,
		.cap = cap,
		.data = (char *)malloc(cap)
	};
}


void print_result(struct lex_result res) {
	for(int i = 0; i < res.len; i++) {
		print_token(res.tokens[i]);
		printf(", ");
	}
}

struct token freeze_token(struct token tok) {
	struct token frozen = new_token(tok.len);
	memcpy(frozen.data, tok.data, tok.len);
	frozen.len = tok.len;

	return frozen;
}

void clear_token(struct token *tok) {
	tok->len = 0;
}

void push_char(struct token *tok, char c) {
	tok->data[tok->len] = c;
	tok->len++;
}

char pop_char(struct token *tok) {
	tok->len--;
	return tok->data[tok->len];
}

void push_token(struct lex_result *res, struct token tok) {
	res->tokens[res->len] = freeze_token(tok);
	res->len++;
}

unsigned char terminate_op(struct token tok) {
	if (tok.len == 2) {
		return 1;
	}

	return 0;
}

unsigned char is_escaped(struct token tok) {
	if (tok.len > 0) {
		if (tok.data[tok.len - 1] == '\\') {
			return 1;
		}
	}

	return 0;
}

void lex(struct lex_result *res, char *input) {
	enum token_state state = EMPTY;

	struct token current_token = new_token(TOKEN_BUFFER_SIZE);

	int idx = 0;
	while (input[idx] != '\0') {
		char c = input[idx++];
		if (is_escaped(current_token)) {
			push_char(&current_token, c);
			continue;
		}

		if (is_symbol(c)) {
			switch (state) {
			case EMPTY:
				push_char(&current_token, c);
				state = SYMBOL;
				break;
			case STRING:
				push_char(&current_token, c);
				break;
			case SYMBOL:
				push_char(&current_token, c);
				if (!is_symbol_group(current_token)) {
					c = pop_char(&current_token);
					push_token(res, current_token);
					clear_token(&current_token);
					push_char(&current_token, c);
					continue;
				}
				break;
			case IDENT:
				push_token(res, current_token);
				clear_token(&current_token);
				push_char(&current_token, c);
				state = SYMBOL;
				break;
			}

			continue;
		}

		if (is_whitespace(c)) {
			switch (state) {
				case EMPTY:
					break;
				case IDENT:
				case SYMBOL:
					push_token(res, current_token);
					clear_token(&current_token);
					state = EMPTY;
					break;
				case STRING:
					push_char(&current_token, c);
					break;
			}

			continue;
		}

		if (c == '"') {
			switch (state) {
			case STRING:
				push_char(&current_token, c);
				push_token(res, current_token);
				clear_token(&current_token);
				state = EMPTY;
				break;
			default:
				push_token(res, current_token);
				clear_token(&current_token);
				push_char(&current_token, c);
				state = STRING;
			}

			continue;
		}

		switch (state) {
			case EMPTY:
			case IDENT:
				push_char(&current_token, c);
				state = IDENT;
				break;
			case STRING:
				push_char(&current_token, c);
				break;
			case SYMBOL:
				push_token(res, current_token);
				clear_token(&current_token);
				push_char(&current_token, c);
				state = IDENT;
				break;
		}
	}
}
