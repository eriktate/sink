#include "expr.h"
#include <stdbool.h>
#include <stdlib.h>

unsigned short int op_score(op o) {
	switch(o) {
	case OP_ADD: case OP_SUB:
		return 1;
	case OP_MULT: case OP_DIV: case OP_MOD:
		return 2;
	default:
		return 0;
	}
}

bool higher_precedence(op higher, op lower) {
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
