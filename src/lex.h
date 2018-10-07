#ifndef _LEX_H
#define _LEX_H

#define TOKEN_BUFFER_SIZE 32
#define RESULT_BUFFER_SIZE 1024 // probably needs to be MUCH bigger.
#define SYMBOL_GROUP_COUNT 22

enum token_state {
	EMPTY,
	SYMBOL,
	IDENT,
	STRING,
	NUMBER,
};

struct token {
	int len;
	int cap;
	char *data;
};

struct lex_result {
	int len;
	int cap;
	struct token *tokens;
};


void lex(struct lex_result *, char *);
struct lex_result init_result();

void print_token(struct token);
void print_result(struct lex_result);

#endif // _LEX_H
