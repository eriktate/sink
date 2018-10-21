#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

#define DEFAULT_BODY_CAP 100
#define DEFAULT_PARAMS_CAP 10

typedef enum node_type {
	N_PROGRAM,
	N_FUNC_DECL,
	N_VAR_DECL,
} node_type;

typedef enum data_type {
	T_I32,
	T_STR,
} data_type;

typedef struct var_decl {
	data_type type;
	const char *name;
} var_decl;

typedef struct block {
	// TODO (erik): Add fields for blocks
} block;

typedef struct fn_decl {
	const char *name;
	int argc;
	var_decl *args;
	int retc;
	data_type *ret; // potential array of return types. Order matters.

	block body;
} fn_decl;

// a program can consist of imports, function declarations, and (global) variable declarations.
typedef struct program {
	char *name;
	int fnc;
	fn_decl *fns;
	int varc;
	var_decl *vars;
} program;

int expect_token(token_type expected, token tok) {
	if (tok.type != expected) {
		printf("parsing failure! Expected %s, but got %s", get_type_name(expected), get_type_name(tok.type));
		exit(1);
	}

	return 0;
}

void push_arg(fn_decl *fn, var_decl arg) {
	fn->args[fn->argc++] = arg;
}

void push_ret(fn_decl *fn, data_type ret) {
	fn->ret[fn->retc++] = ret;
}

fn_decl make_fn() {
	return (fn_decl){
		.args = (var_decl*)malloc(sizeof(var_decl) * 20),
		.ret = (data_type*)malloc(sizeof(data_type) * 10),
	};
}

void print_fn_decl(fn_decl fn) {
	printf("Function: %s\n\tArgs: ", fn.name);
	for(int i = 0; i < fn.argc; i++) {
		printf("%s: %d ", fn.args[i].name, fn.args[i].type);
	}

	printf("\n\tReturn: ");
	for(int i = 0; i < fn.retc; i++) {
		printf("%d ", fn.ret[i]);
	}

	printf("\n");
}

block parse_block(tok_iter *iter) {
	return (block){};
}

fn_decl parse_fn(tok_iter *iter) {
	printf("Parsing fn...\n");
	fn_decl decl = make_fn();

	// First token should be the name.
	token tok = tok_iterate(iter);
	expect_token(TOK_IDENT, tok);
	decl.name = tok.value.ident;

	// Second token should be an open paren
	tok = tok_iterate(iter);
	expect_token(TOK_LPAREN, tok);

	// Paren loop
	for(;;) {
		printf("Expecting type...\n");
		tok = tok_iterate(iter);
		if (!((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
			printf("Not a type declaration...\n");
			exit(1);
			return decl;
		}

		// TODO (etate): Check for double braces or pointers here.
		printf("Expecting name...\n");
		tok = tok_iterate(iter);
		expect_token(TOK_IDENT, tok);
		var_decl arg = (var_decl){T_STR, tok.value.ident};
		push_arg(&decl, arg);

		printf("Checking next token...\n");
		tok = tok_iterate(iter);
		if (tok.type == TOK_COMMA) {
			printf("Found a comma!\n");
			continue;
		}

		if (tok.type == TOK_RPAREN) {
			break;
		}

		printf("Invalid token!\n");
		exit(1);
	}

	tok = tok_iterate(iter);

	if (tok.type == TOK_COLON) {
		printf("Parsing return value\n");
		// TODO (etate): Handle return values
		tok = tok_iterate(iter);
		if (tok.type == TOK_LPAREN) {
			// Return type loop
			for(;;) {
				tok = tok_iterate(iter);
				if (((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
					push_ret(&decl, T_STR); // TODO (erik): push actual return type here.
					continue;
				}

				if (tok.type == TOK_COMMA) {
					continue;
				}

				if (tok.type == TOK_RPAREN) {
					break;
				}

				printf("Invalid token!\n");
				exit(1);
			}
		} else if (((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
			printf("Adding return...\n");
			push_ret(&decl, T_STR); // TODO (erik): push actual return type here.
		} else {
			printf("Invalid token!\n");
			exit(1);
		}
	}

	if (tok.type == TOK_LBRACE) {
		// TODO (etate): Start a new block
		decl.body = parse_block(iter);
	}

	return decl;
}

void parse(tok_iter *iter) {
	program prog;
	for(token tok = tok_iterate(iter); tok.type != TOK_NONE; tok = tok_iterate(iter)) {
		switch(tok.type) {
		case TOK_FN:
			printf("Found a function...\n");
			print_fn_decl(parse_fn(iter));
			break;
		default:
			break;
		}
	}
}

int main() {
	// char *test_program =
	// 	"int main(int argc, void argv[][]) {"
	// 	"if (test >>= other_test)"
	// 	"printf(\"Hello \\\"there\\n\");"
	// 	"}";

	char *test_program =
		"fn main(i32 argc, str argv) : (int, str) {\n"
		"\tprintln(\"Hello \\\"there\\n\");\n"
		"}\n";

	token tokens[RESULT_BUFFER_SIZE];
	tok_iter res = (tok_iter){
		.len = 0,
		.idx = 0,
		.cap = RESULT_BUFFER_SIZE,
		.tokens = tokens,
	};

	printf("Lexing program:\n%s\n", test_program);
	lex(&res, test_program);
	printf("Lex result:\n");
	print_result(res);
	printf("\n");

	printf("Start parsing...\n");
	parse(&res);
	return 0;
}
