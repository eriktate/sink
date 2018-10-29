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

typedef struct statement {
	// TODO (erik): Add fields for statements.
} statement;

typedef struct block {
	size_t stmtc;
	statement *statements;
} block;

typedef struct fn_decl {
	size_t name;
	size_t argc;
	expr_var_decl *args;
	size_t retc;
	data_type *ret; // potential array of return types. Order matters.

	block body;
} fn_decl;

// a program can consist of imports, function declarations, and (global) variable declarations.
typedef struct program {
	size_t name;
	size_t fnc;
	fn_decl *fns;
	size_t varc;
	expr_var_decl *vars;
} program;

int expect_token(token_type expected, token tok) {
	if (tok.type != expected) {
		printf("parsing failure! Expected %s, but got %s", get_type_name(expected), get_type_name(tok.type));
		exit(1);
	}

	return 0;
}

void push_arg(fn_decl *fn, expr_var_decl arg) {
	fn->args[fn->argc++] = arg;
}

void push_ret(fn_decl *fn, data_type ret) {
	fn->ret[fn->retc++] = ret;
}

fn_decl make_fn() {
	return (fn_decl){
		.args = (expr_var_decl*)malloc(sizeof(expr_var_decl) * 20),
		.ret = (data_type*)malloc(sizeof(data_type) * 10),
	};
}

block make_block() {
	return (block){};
}

void print_fn_decl(fn_decl fn) {
	printf("Function: %s\n\tArgs: ", get_interned(fn.name));
	for(int i = 0; i < fn.argc; i++) {
		printf("%s: %d ", get_interned(fn.args[i].name), fn.args[i].type);
	}

	printf("\n\tReturn: ");
	for(int i = 0; i < fn.retc; i++) {
		printf("%d ", fn.ret[i]);
	}

	printf("\n");
}

void print_expr_var_decl(expr_var_decl var) {
	printf("Var: %s\n\tType: %d\n", get_interned(var.name), var.type);
}

// forward parsing declarations
expr_call parse_fn_call(token fn_name, tok_iter *iter);
expr *parse_expr(tok_iter *iter);
expr *parse_expr_binary(expr *lhs, bin_op op, tok_iter *iter);
expr_cast parse_expr_cast(token type, tok_iter *iter);
expr_var_decl parse_expr_var_decl(token type, tok_iter *iter);
expr_unary parse_expr_unary(un_op op, tok_iter *iter);
expr_log parse_expr_log(expr *lhs, log_op op, tok_iter *iter);
expr_as parse_expr_as(expr *lhs, as_op op, tok_iter *iter);

expr *resolve_binary_expression(expr *e, bin_op new_op) {
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

expr_cast parse_expr_cast(token type, tok_iter *iter) {
	return (expr_cast){
		.type = (data_type)type.type,
		.type_ident = type.type == TOK_IDENT ? type.value.ident : 0,
		.expr = parse_expr(iter),
	};
}

expr_term parse_expr_term(token tok) {
	expr_term e = {};
	switch(tok.type) {
	case TOK_INT_LIT:
		e.type = T_I64;
		e.value.int_val = tok.value.int_val;
		break;
	case TOK_FLOAT_LIT:
		e.type = T_F64;
		e.value.float_val = tok.value.float_val;
		break;
	case TOK_STRING_LIT:
		e.type = T_STR;
		e.value.string_val = tok.value.string_val;
		break;
	case TOK_IDENT:
		e.type = T_CUSTOM;
		e.value.ident = tok.value.ident;
		break;
	default:
		printf("Something strange happened\n");
		exit(1);
	}

	return e;
}

expr *parse_expr_binary(expr *lhs, bin_op operator, tok_iter *iter) {
	if (lhs->type != EXPR_BINARY || higher_precedence(lhs->expr.binary.op, operator)) {
		return alloc_expr_binary((expr_binary){
			.lhs = lhs,
			.op = operator,
			.rhs = parse_expr(iter),
		});
	}

	expr *new_expr = alloc_expr_binary((expr_binary){
		.lhs = lhs->expr.binary.rhs,
		.op = operator,
		.rhs = parse_expr(iter),
	});

	lhs->expr.binary.rhs = new_expr;

	return lhs;
}

expr_unary parse_expr_unary(un_op op, tok_iter *iter) {
	return (expr_unary){
		.expr = parse_expr(iter),
		.op = op,
	};
}

// forward declaration
expr *parse_expr(tok_iter *iter) {
	printf("Parsing expression!\n");
	expr *e = NULL;

	for(;;) {
		token tok = tok_iterate(iter);
		switch(tok.type) {
		case TOK_INT_LIT: case TOK_STRING_LIT: case TOK_FLOAT_LIT:
			e = alloc_expr(parse_expr_term(tok));
			break;

		case TOK_INC: case TOK_DEC: case TOK_AND: case TOK_MULT: case TOK_BANG:
			if (e == NULL) {
				parse_expr_unary((un_op)tok.type, iter);
			}
		case TOK_PLUS: case TOK_MINUS: case TOK_DIV: case TOK_MOD:
			e = parse_expr_binary(e, (bin_op)tok.type, iter);
			break;

		case TOK_IDENT:
			if (tok_peek(iter).type != TOK_IDENT && tok_peek(iter).type != TOK_LPAREN) {
				// Create term.
				e = alloc_expr(parse_expr_term(tok));
				break;
			}

		case TOK_U8: case TOK_U16: case TOK_U32: case TOK_U64:
		case TOK_I8: case TOK_I16: case TOK_I32: case TOK_I64:
		case TOK_F32: case TOK_F64: case TOK_STRING: case TOK_BYTE:
		case TOK_BOOL:
			if (tok_peek(iter).type == TOK_LPAREN) {
				parse_expr_cast(tok, iter);
			}

			parse_expr_var_decl(tok, iter);
			break;

		case TOK_SEMICOLON:
			return e;
		}
	}
}

expr_var_decl parse_expr_var_decl(token type, tok_iter *iter) {
	expr_var_decl var = (expr_var_decl){
		.type = (data_type)type.type,
		.type_ident = type.type == TOK_IDENT ? type.value.ident : 0,
	};

	// Expect var ident
	// TODO (erik): Eventually allow for multiple declarations on a single line.
	token tok = tok_iterate(iter);
	if (tok.type != TOK_IDENT) {
		printf("Invalid token!\n");
		exit(1);
	}

	var.name = tok.value.ident;

	return var;
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

statement parse_statement(tok_iter *iter) {
	statement s = (statement){};

	token peek;
	for(;;) {
		peek = tok_peek(iter);

		switch(peek.type) {
		case TOK_IF:
			// parse if statement.
		case TOK_SWITCH:
			// parse switch statement
		case TOK_BREAK:
			// parse break statement.
		case TOK_CONTINUE:
			// parse continue statement.
		case TOK_RETURN:
			// parse return statement.
		case TOK_FOR:
			// parse for loop.
		case TOK_WHILE:
			// parse while loop.
			break;
		default:
			// parse expression statement.
			parse_expr(iter);
		}
	}

	return s;
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
			print_expr_var_decl(parse_expr_var_decl(tok, iter));
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
		expr_var_decl arg = (expr_var_decl){(data_type)tok.type};
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
