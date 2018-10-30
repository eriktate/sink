#include "expr.h"
#include <stdbool.h>
#include <stdlib.h>

unsigned short int op_score(bin_op o) {
	switch(o) {
	case BIN_OR:
		return 0;
	case BIN_XOR:
		return 1;
	case BIN_AND:
		return 2;
	case BIN_ADD: case BIN_SUB:
		return 3;
	case BIN_MULT: case BIN_DIV: case BIN_MOD:
		return 4;
	default:
		return 0;
	}
}

bool higher_precedence(bin_op higher, bin_op lower) {
	return op_score(higher) > op_score(lower);
}

expr *alloc_expr() {
	return (expr *)malloc(sizeof(expr));
}

expr *alloc_expr_binary(expr_binary bin) {
	expr *e = alloc_expr();
	e->type = EXPR_BINARY;
	e->expr.binary = bin;

	return e;
}

expr *alloc_expr_unary(expr_unary un) {
	expr *e = alloc_expr();
	e->type = EXPR_UNARY;
	e->expr.unary = un;

	return e;
}

expr *alloc_expr_term(expr_term term) {
	expr *e = alloc_expr();
	e->type = EXPR_TERM;
	e->expr.term = term;

	return e;
}

expr *alloc_expr_var_decl(expr_var_decl var) {
	expr *e = alloc_expr();
	e->type = EXPR_VAR_DECL;
	e->expr.var = var;

	return e;
}

expr *alloc_expr_cast(expr_cast cast) {
	expr *e = alloc_expr();
	e->type = EXPR_CAST;
	e->expr.cast = cast;

	return e;
}
