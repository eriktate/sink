#ifndef _EXPR_H
#define _EXPR_H

#include "lex.h"
#include <stdbool.h>

typedef enum data_type {
	T_U8 = TOK_U8,
	T_U16 = TOK_U16,
	T_U32 = TOK_U32,
	T_U64 = TOK_U64,
	T_I8 = TOK_I8,
	T_I16 = TOK_I16,
	T_I32 = TOK_I32,
	T_I64 = TOK_I64,
	T_F32 = TOK_F32,
	T_F64 = TOK_F64,
	T_STR = TOK_STRING,
	T_BOOL = TOK_BOOL,
	T_BYTE = TOK_BYTE,
	T_CUSTOM = TOK_IDENT, // Needs to be determined later.
} data_type;

typedef enum op {
	OP_ADD = TOK_PLUS,
	OP_SUB = TOK_MINUS,
	OP_MULT = TOK_MULT,
	OP_DIV = TOK_DIV,
	OP_MOD = TOK_MOD,
} op;

struct expr; // have to forward declare expr for circular structures

typedef enum expr_type {
	EXPR_NONE,
	EXPR_TERM,
	EXPR_BINARY,
	EXPR_UNARY,
	EXPR_CALL,
} expr_type;

typedef struct expr_term {
	data_type type;
	union literals value;
} expr_term;

typedef struct expr_binary {
	data_type ret_type;
	struct expr *lhs;
	struct expr *rhs;
	op op;
} expr_binary;

typedef struct expr_unary {
	op op;
	struct expr *expr;
} expr_unary;

typedef struct expr_call {
	size_t name;
	int paramc;
	struct expr *params;
} expr_call;

typedef struct expr_group {
	struct expr *expr;
} expr_group;


typedef struct expr {
	expr_type type;

	union ex {
		expr_term term;
		expr_binary binary;
		expr_unary unary;
		expr_call call;
		expr_group group;
	} expr;
} expr;

unsigned short int op_score(op);
bool higher_precedence(op, op);

expr *alloc_expr();
expr *alloc_expr_binary(expr_binary);
expr *alloc_expr_unary(expr_unary);
expr *alloc_expr_term(expr_term);

#endif // _EXPR_H
