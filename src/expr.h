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

typedef enum un_op {
	UN_REF = TOK_AND,
	UN_DEREF = TOK_MULT,
	UN_NOT = TOK_BANG,
	UN_INC = TOK_INC,
	UN_DEC = TOK_DEC,
	UN_NEGATE = TOK_MINUS,
} un_op;

typedef enum bin_op {
	BIN_ADD = TOK_PLUS,
	BIN_SUB = TOK_MINUS,
	BIN_MULT = TOK_MULT,
	BIN_DIV = TOK_DIV,
	BIN_MOD = TOK_MOD,
	BIN_AND= TOK_AND,
	BIN_OR = TOK_OR,
	BIN_XOR = TOK_XOR,
	BIN_SHIFTR = TOK_SHIFT_RIGHT,
	BIN_SHIFTL = TOK_SHIFT_LEFT,
} bin_op;

typedef enum log_op {
	LOG_EQUAL = TOK_EQUAL,
	LOG_NOT_EQUAL = TOK_NOT_EQUAL,
	LOG_GT = TOK_GT,
	LOG_GT_EQ = TOK_GT_EQUAL,
	LOG_LT = TOK_LT,
	LOG_LT_EQ = TOK_LT_EQUAL,
	LOG_AND = TOK_LOGICAL_AND,
	LOG_OR = TOK_LOGICAL_OR,
} log_op;

typedef enum as_op {
	AS_ASSIGN = TOK_ASSIGN,
	AS_INF_ASSIGN = TOK_INF_ASSIGN,
	AS_ADD_ASSIGN = TOK_PLUS_ASSIGN,
	AS_SUB_ASSIGN = TOK_MINUS_ASSIGN,
	AS_MULT_ASSIGN = TOK_MULT_ASSIGN,
	AS_DIV_ASSIGN = TOK_DIV_ASSIGN,
	AS_MOD_ASSIGN = TOK_MOD_ASSIGN,
	AS_AND_ASSIGN = TOK_AND_ASSIGN,
	AS_OR_ASSIGN = TOK_OR_ASSIGN,
	AS_XOR_ASSIGN = TOK_XOR_ASSIGN,
	AS_SHIFTR_ASSIGN = TOK_SHIFT_RIGHT_ASSIGN,
	AS_SHIFTL_ASSIGN = TOK_SHIFT_LEFT_ASSIGN,
} as_op;

struct expr; // have to forward declare expr for circular structures

typedef enum expr_type {
	EXPR_NONE,
	EXPR_TERM,
	EXPR_UNARY,
	EXPR_BINARY,
	EXPR_LOGICAL,
	EXPR_ASSIGN,
	EXPR_CALL,
	EXPR_VAR_DECL,
	EXPR_CAST,
} expr_type;

typedef struct expr_term {
	data_type type;
	size_t type_ident;
	union literals value;
} expr_term;

typedef struct expr_unary {
	struct expr *expr;
	un_op op;
} expr_unary;

typedef struct expr_binary {
	data_type ret_type;
	struct expr *lhs;
	struct expr *rhs;
	bin_op op;
} expr_binary;

typedef struct expr_log {
	struct expr *lhs;
	struct expr *rhs;
	log_op op;
} expr_log;

typedef struct expr_as {
	struct expr *lhs;
	struct expr *rhs;
	as_op op;
} expr_as;

typedef struct expr_call {
	size_t name;
	int paramc;
	struct expr *params;
} expr_call;

typedef struct expr_group {
	struct expr *expr;
} expr_group;

typedef struct expr_var_decl {
	data_type type;
	size_t type_ident;
	size_t name;
} expr_var_decl;

typedef struct expr_cast {
	data_type type;
	size_t type_ident;
	struct expr *expr;
} expr_cast;


typedef struct expr {
	expr_type type;

	union ex {
		expr_term term;
		expr_unary unary;
		expr_binary binary;
		expr_log logical;
		expr_as assignment;
		expr_call call;
		expr_group group;
		expr_var_decl var;
		expr_cast cast;
	} expr;
} expr;

unsigned short int op_score(bin_op);
bool higher_precedence(bin_op, bin_op);

expr *alloc_expr();
expr *alloc_expr_binary(expr_binary);
expr *alloc_expr_unary(expr_unary);
expr *alloc_expr_term(expr_term);
expr *alloc_expr_var_decl(expr_var_decl);
expr *alloc_expr_cast(expr_cast);
expr *alloc_expr_log(expr_log);
expr *alloc_expr_as(expr_as);

#endif // _EXPR_H
