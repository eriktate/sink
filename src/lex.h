#ifndef _LEX_H
#define _LEX_H

#define TOKEN_BUFFER_SIZE 32
#define RESULT_BUFFER_SIZE 1024 // probably needs to be MUCH bigger.
#define SYMBOL_GROUP_COUNT 23

#include <stdlib.h>

typedef enum lexing_state {
	NONE,
	SYMBOL,
	IDENT,
	STRING,
	INT,
	FLOAT,
} lexing_state;

typedef enum token_type {
	TOK_ASSIGN,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MULT,
	TOK_DIV,
	TOK_MOD,
	TOK_BANG,
	TOK_LT,
	TOK_GT,
	TOK_AND,
	TOK_OR,
	TOK_XOR,
	TOK_QUESTION,
	TOK_COLON,
	TOK_SEMICOLON,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_DOT,
	TOK_COMMA,

	TOK_EQUAL,
	TOK_INC,
	TOK_DEC,
	TOK_EMPTY_BRACE,
	TOK_NOT_EQUAL,
	TOK_GT_EQUAL,
	TOK_LT_EQUAL,
	TOK_LOGICAL_AND,
	TOK_LOGICAL_OR,
	TOK_SHIFT_LEFT,
	TOK_SHIFT_RIGHT,
	TOK_PLUS_ASSIGN,
	TOK_MINUS_ASSIGN,
	TOK_MULT_ASSIGN,
	TOK_DIV_ASSIGN,
	TOK_MOD_ASSIGN,
	TOK_SHIFT_LEFT_ASSIGN,
	TOK_SHIFT_RIGHT_ASSIGN,
	TOK_AND_ASSIGN,
	TOK_OR_ASSIGN,
	TOK_XOR_ASSIGN,
	TOK_INF_ASSIGN, // This MUST be the last symbol token.

	TOK_FN,
	TOK_SWITCH,
	TOK_CASE,
	TOK_BREAK,
	TOK_CONTINUE,
	TOK_RETURN,
	TOK_IF,
	TOK_ELSE,
	TOK_SIZEOF,
	TOK_FOR,
	TOK_WHILE,
	TOK_U8,
	TOK_U16,
	TOK_U32,
	TOK_U64,
	TOK_I8,
	TOK_I16,
	TOK_I32,
	TOK_I64,
	TOK_F32,
	TOK_F64,
	TOK_STRING,
	TOK_BYTE,
	TOK_BOOL,
	TOK_TRUE,
	TOK_FALSE,

	NUM_TOK,

	// Tokens that can't be instantly recognized.
	TOK_INT_LIT,
	TOK_FLOAT_LIT,
	TOK_STRING_LIT,
	TOK_IDENT,
	TOK_NONE,
} token_type;

union literals {
	unsigned long long int_val;
	double float_val;
	size_t string_val;
	size_t ident;
};

typedef struct token {
	token_type type;
	int line_number;
	int start;
	int end;
	union literals value;
} token;

typedef struct tok_iter {
	int len;
	int cap;
	int idx;
	token *tokens;
} tok_iter;


void lex(tok_iter *, char *);

unsigned char token_equal(token, token);
unsigned char token_equal_string(token, char *);
struct token token_from_string(char *);
char *get_type_name(token_type);
token tok_iterate(tok_iter *);
token tok_peek(tok_iter *);

void print_token( token);
void print_result(tok_iter);

#endif // _LEX_H
