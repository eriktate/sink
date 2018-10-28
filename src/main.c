#include "lex.h"
#include "expr.h"
#include "intern.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DEFAULT_BODY_CAP 100
#define DEFAULT_PARAMS_CAP 10

typedef enum node_type {
	N_PROGRAM,
	N_FUNC_DECL,
	N_VAR_DECL,
} node_type;

typedef struct var_decl {
	data_type type;
	const char *type_ident;
	const char *name;
	expr *assignment; // NULL if no assignment occurs
} var_decl;

typedef struct statement {
	// TODO (erik): Add fields for statements.
} statement;

typedef struct block {
	size_t stmtc;
	statement *statements;
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

block make_block() {
	return (block){};
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

void print_var_decl(var_decl var) {
	printf("Var: %s\n\tType: %d\n", var.name, var.type);
}

// forward parsing declarations
expr_call parse_fn_call(token fn_name, tok_iter *iter);

expr *resolve_binary_expression(expr *e, op new_op) {
	if (e->type != EXPR_BINARY || higher_precedence(e->expr.binary.op, new_op)) {
		return alloc_expr_binary((expr_binary){
			.lhs = e,
			.op = new_op,
			.rhs = NULL,
		});
	}

	expr *new_expr = alloc_expr_binary((expr_binary){
		.lhs = e->expr.binary.rhs,
		.op = new_op,
		.rhs = NULL,
	});

	e->expr.binary.rhs = new_expr;

	return e;
}

// forward declaration
expr *parse_expr(tok_iter *iter) {
	printf("Parsing expression!\n");
	expr *e = alloc_expr();
	e->type = EXPR_NONE;

	for(;;) {
		token tok = tok_peek(iter);
		switch(tok.type) {
		case TOK_INT_LIT:
			if (e->type == EXPR_UNARY) {
				if (e->expr.unary.expr == NULL) {
					e->expr.unary.expr = alloc_expr_term((expr_term) {
						.type = T_I32,
						.value.int_val = tok.value.int_val,
					});
					break;
				}

				printf("Expected an operator, ')', or ';' after unary op\n");
				exit(1);
			}

			if (e->type == EXPR_BINARY) {
				if (e->expr.binary.rhs == NULL) {
					e->expr.binary.rhs = alloc_expr_term((expr_term) {
						.type = T_I32,
						.value.int_val = tok.value.int_val,
					});
					break;
				}

				printf("Expected an operator, ')', or ';' after binary op\n");
				exit(1);
			}

			if (e->type == EXPR_NONE) {
				printf("Found term\n");
				e->type = EXPR_TERM;
				e->expr.term = (expr_term){
					.type = T_I32,
					.value.int_val = tok.value.int_val,
				};

				break;
			}

			printf("Int term can not be evaluated in this context!\n");
			exit(1);

		case TOK_PLUS: case TOK_MINUS: case TOK_MULT: case TOK_DIV: case TOK_MOD:
			resolve_binary_expression(e, (op)tok.type);
			break;

		case TOK_SEMICOLON:
			return e;
		}

		tok_iterate(iter);
	}
}

var_decl parse_var_decl(token var_type, tok_iter *iter) {
	printf("Parsing var decl!\n");
	var_decl *var = &(var_decl){
		.type = (data_type)var_type.type,
		.assignment = NULL,
	};

	// Expect var ident
	token tok = tok_iterate(iter);
	if (tok.type != TOK_IDENT) {
		printf("Invalid token!\n");
		exit(1);
	}

	var->name = tok.value.ident;
	token peek = tok_peek(iter);
	if (peek.type == TOK_ASSIGN) {
		tok_iterate(iter);
		expr *e = parse_expr(iter);
		var->assignment = e;
		peek = tok_peek(iter);
	}

	if (peek.type == TOK_SEMICOLON) {
		// Check assignment expression
		assert(var->assignment != NULL);
		assert(var->assignment->type == EXPR_TERM);
		assert(var->assignment->expr.term.type == T_I32);
		printf("Expr term value: %lld", var->assignment->expr.term.value.int_val);
		assert(var->assignment->expr.term.value.int_val == 12);
		return *var;
	}

	printf("Something strange happened!\n");
	exit(1);
}

expr_call parse_fn_call(token fn_name, tok_iter *iter) {
	expr_call e = (expr_call){.name = fn_name.value.ident};

	token tok = tok_iterate(iter);

	if (tok.type != TOK_LPAREN) {
		printf("Something strange happened!\n");
		exit(1);
	}

	const int max_params_len = 10;
	expr *params = (expr *)malloc(sizeof(expr) * max_params_len); // TODO (erik): Make this growable.
	// parameter loop
	for(int i = 0; i< max_params_len; i++) {
		params[i] = *parse_expr(iter);

		tok = tok_iterate(iter);
		if (tok.type == TOK_COMMA) {
			continue;
		}

		if (tok.type == TOK_RPAREN) {
			e.paramc = i;
			break;
		}
	}

	e.params = params;

	return e;
}

block parse_block(tok_iter *iter) {
	// Types of statements
	// =====================
	// variable declarations
	// variable assignments
	// function calls
	// if statements
	// else statements
	// for loops
	// while loops
	// break statement
	// continue statement
	// return statement
	// switch statements

	printf("Parsing block!\n");
	block b = make_block();
	token tok;
	for(;;) {
		tok = tok_iterate(iter);
		switch(tok.type) {
		case TOK_IDENT:
			if (tok_peek(iter).type != TOK_IDENT) { // two idents back to back must be a variable declarion with a custom type.
				break;
			}
		case TOK_U8: case TOK_U16: case TOK_U32: case TOK_U64:
		case TOK_I8: case TOK_I16: case TOK_I32: case TOK_I64:
		case TOK_F32: case TOK_F64: case TOK_STRING: case TOK_BYTE:
		case TOK_BOOL:
			print_var_decl(parse_var_decl(tok, iter));
			break;
		case TOK_SEMICOLON:
			// push statement to block.
			break;
		case TOK_RBRACE:
			return (block){};
		}
	}
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
		tok = tok_iterate(iter);
		if (!((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
			printf("Not a type declaration...\n");
			exit(1);
			return decl;
		}
		var_decl arg = (var_decl){(data_type)tok.type};
		// TODO (etate): Check for double braces or pointers here.
		tok = tok_iterate(iter);
		expect_token(TOK_IDENT, tok);
		arg.name = tok.value.ident;
		push_arg(&decl, arg);

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
		tok = tok_iterate(iter);
		if (tok.type == TOK_LPAREN) {
			// Return type loop
			for(;;) {
				tok = tok_iterate(iter);
				if (((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
					push_ret(&decl, (data_type)tok.type); // TODO (erik): push actual return type here.
					continue;
				}

				if (tok.type == TOK_COMMA) {
					continue;
				}

				if (tok.type == TOK_RPAREN) {
					break;
				}

				exit(1);
			}
		} else if (((tok.type >= TOK_U8 && tok.type <= TOK_BOOL) || tok.type == TOK_IDENT)) {
			push_ret(&decl, (data_type)tok.type); // TODO (erik): push actual return type here.
		} else {
			printf("Invalid token!\n");
			exit(1);
		}
	}

	// Expect open brace
	tok = tok_iterate(iter);
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
	char *test_program =
		"fn main(i32 argc, str argv) : (i32, str) {\n"
//		"\ti32 test;\n"
		"\ti32 other_test = 12;\n"
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
