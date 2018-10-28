#include "lex.h"
#include "intern.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char *reserved_map[] = {
	[TOK_ASSIGN] = "=",
	[TOK_PLUS] = "+",
	[TOK_MINUS] = "-",
	[TOK_MULT] = "*",
	[TOK_DIV] = "/",
	[TOK_MOD] = "%",
	[TOK_BANG] = "!",
	[TOK_LT] = "<",
	[TOK_GT] = ">",
	[TOK_AND] = "&",
	[TOK_OR] = "|",
	[TOK_XOR] = "^",
	[TOK_QUESTION] = "?",
	[TOK_COLON] = ":",
	[TOK_SEMICOLON] = ";",
	[TOK_LBRACKET] = "[",
	[TOK_RBRACKET] = "]",
	[TOK_LBRACE] = "{",
	[TOK_RBRACE] = "}",
	[TOK_LPAREN] = "(",
	[TOK_RPAREN] = ")",
	[TOK_DOT] = ".",
	[TOK_COMMA] = ",",

	[TOK_EQUAL] = "==",
	[TOK_INC] = "++",
	[TOK_DEC] = "--",
	[TOK_EMPTY_BRACE] = "[]",
	[TOK_NOT_EQUAL] = "!=",
	[TOK_GT_EQUAL] = ">=",
	[TOK_LT_EQUAL] = "<=",
	[TOK_LOGICAL_AND] = "&&",
	[TOK_LOGICAL_OR] = "||",
	[TOK_SHIFT_LEFT] = "<<",
	[TOK_SHIFT_ASSIGN] = ">>",
	[TOK_PLUS_ASSIGN] = "+=",
	[TOK_MINUS_ASSIGN] = "-=",
	[TOK_MULT_ASSIGN] = "*=",
	[TOK_DIV_ASSIGN] = "/=",
	[TOK_MOD_ASSIGN] = "%=",
	[TOK_SHIFT_LEFT_ASSIGN] = "<<=",
	[TOK_SHIFT_RIGHT_ASSIGN] = ">>=",
	[TOK_AND_ASSIGN] = "&=",
	[TOK_OR_ASSIGN] = "|=",
	[TOK_XOR_ASSIGN] = "^=",
	[TOK_INF_ASSIGN] = ":=",
	[TOK_PTR_REF] = "->",

	[TOK_FN] = "fn",
	[TOK_SWITCH] = "switch",
	[TOK_CASE] = "case",
	[TOK_BREAK] = "break",
	[TOK_CONTINUE] = "continue",
	[TOK_RETURN] = "return",
	[TOK_IF] = "if",
	[TOK_ELSE] = "else",
	[TOK_SIZEOF] = "sizeof",
	[TOK_U8] = "u8",
	[TOK_U16] = "u16",
	[TOK_U32] = "u32",
	[TOK_U64] = "u64",
	[TOK_I8] = "i8",
	[TOK_I16] = "i16",
	[TOK_I32] = "i32",
	[TOK_I64] = "i64",
	[TOK_F32] = "f32",
	[TOK_F64] = "f64",
	[TOK_STRING] = "str",
	[TOK_BYTE] = "byte",
	[TOK_BOOL] = "bool",
	[TOK_TRUE] = "true",
	[TOK_FALSE] = "false",
};

token_type check_reserved(char *str, size_t len) {
	for (int i = 0; i < NUM_TOK; i++) {
		for (int j = 0; j < len; j++) {
			if (str[j] != reserved_map[i][j])
				break;

			if (reserved_map[i][j] == '\0') {
				break;
			}

			if (j == len - 1) {
				return i;
			}
		}
	}

	return TOK_NONE;
}

void print_token(struct token tok) {
	printf("Type: %s, Line: %d, Start: %d, End: %d, ", get_type_name(tok.type), tok.line_number, tok.start, tok.end);
	switch (tok.type) {
	case TOK_IDENT:
		printf("%s", get_interned(tok.value.ident));
		break;
	case TOK_STRING_LIT:
		printf("%s", get_interned(tok.value.string_val));
		break;
	case TOK_INT_LIT:
		printf("%lld", tok.value.int_val);
		break;
	case TOK_FLOAT_LIT:
		printf("%g", tok.value.float_val);
		break;
	default:
		if (tok.type < NUM_TOK) {
			printf("%s", reserved_map[tok.type]);
		}
		break;
	}
}

void print_result(tok_iter res) {
	for(int i = 0; i < res.len; i++) {
		print_token(res.tokens[i]);
		printf("\n");
	}
}

unsigned int line = 1;
unsigned int start = 0;
int end = -1;
lexing_state lexing;

const char *make_string(char *src) {
	const size_t len = end - start;
	char *new_string = (char *)malloc(len + 1);
	memcpy(new_string, src + start, len);
	new_string[len] = '\0';
	return (const char *)new_string;
}

const int make_int(char *src) {
	const char *str = make_string(src);
	return atoi(str);
	free(str);
}

const double make_float(char *src) {
	const char *str = make_string(src);
	return atof(str);
	free(str);
}

void print_current(char *src) {
	for(int i = start; i < end; i++) {
		printf("%c", src[i]);
	}

	printf("\n");
}

void push_token(tok_iter *res, char *input) {
	// printf("Begin pushing token\n");
	token tok = (token){
		.line_number = line,
		.start = start,
		.end = end,
	};

	switch (lexing) {
	case IDENT:
		tok.type = check_reserved(input + start, end - start);
		if (tok.type == TOK_NONE) {
			tok.type = TOK_IDENT;
			tok.value.ident = intern_slice(input, start, end);
		} else {
			tok.value.ident = reserved_map[tok.type];
		}
		break;
	case STRING:
		tok.type = TOK_STRING_LIT;
		tok.value.string_val = intern_slice(input, start, end);
		break;
	case INT:
		tok.type = TOK_INT_LIT;
		tok.value.int_val = make_int(input);
		// convert value to int.
		break;
	case FLOAT:
		tok.type = TOK_FLOAT_LIT;
		tok.value.float_val  = make_float(input);
		break;
	case SYMBOL:
		tok.type = check_reserved(&input[start], end - start);
		if (tok.type == TOK_NONE) // error;
		break;
	case NONE:
		// error: Shouldn't be pushing an empty lexing state.
		break;
	}

	res->tokens[res->len++] = tok;
	start = end;
}

void lex(tok_iter *res, char *input) {
	unsigned char end_string = 0;

	for (;;) {
		end++;
		if (end_string) { // kind of a hack
			push_token(res, input);
			end_string = 0;
			lexing = NONE;
		}

		switch (input[end]) {
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B':
		case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
		case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
		case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W':
		case 'X': case 'Y': case 'Z':
			switch (lexing) {
			case NONE:
				lexing = IDENT;
				start = end;
				break;
			case STRING:
			case INT:
			case FLOAT:
			case IDENT:
				break;
			case SYMBOL:
				push_token(res, input);
				lexing = IDENT;
			}
			break;

		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
			switch (lexing) {
			case NONE:
				lexing = INT;
				break;
			case INT:
			case FLOAT:
			case STRING:
			case IDENT:
				break;
			case SYMBOL:
				push_token(res, input);
				break;
			}
			break;

		case '_':
			switch (lexing) {
			case NONE:
				lexing = IDENT;
				break;
			case INT:
			case FLOAT:
			case STRING:
			case IDENT:
				break;
			case SYMBOL:
				push_token(res, input);
				lexing = IDENT;
				break;
			}
			break;

		case '.':
			switch (lexing) {
			case NONE:
			case INT:
				lexing = FLOAT;
				break;
			case SYMBOL:
			case IDENT:
				push_token(res, input);
				break;
			case STRING:
			case FLOAT:
				break;
			}
			break;

		case ' ': case '\t': case '\n': case '\r':
			switch (lexing) {
			case NONE:
			case STRING:
				break;
			case IDENT:
			case INT:
			case FLOAT:
			case SYMBOL:
				push_token(res, input);
				lexing = NONE;
				break;
			}
			break;

		case '"':
			switch (lexing) {
			case NONE:
				lexing = STRING;
				break;
			case STRING:
				if (input[end-1] == '\\') { // allow escaped quotes
					break;
				}
				end_string = 1;
				break;
			case INT:
			case FLOAT:
			case IDENT:
				// error
				break;
			case SYMBOL:
				push_token(res, input);
				lexing = STRING;
				break;
			}
			break;

		case '=': case '+': case '-': case '*': case '/': case '%': case '!': case '<': case '>':
		case '&': case '|': case '^': case '?': case ':': case ';': case '[': case ']': case '{':
		case '}': case '(': case ')': case ',':
			switch (lexing) {
			case NONE:
				lexing = SYMBOL;
				start = end;
				break;
			case INT:
			case FLOAT:
			case IDENT:
				push_token(res, input);
				lexing = SYMBOL;
				break;
			case SYMBOL:
				if (check_reserved(input+start, end - start+1) == TOK_NONE) {
					push_token(res, input);
				}
			case STRING:
				break;
			}
			break;

		case '\0': // EOF
			if (lexing != NONE) {
				push_token(res, input);
			}
			return;

		default: // some other utf-8 character
			switch (lexing) {
			case NONE:
			case INT:
			case FLOAT:
			case IDENT:
			case SYMBOL:
				// something is wrong
				break;
			case STRING:
				// this is fine
				break;
			}
			break;
		}

		if (input[end] == '\n') {
			line++;
		}
	}
}

token tok_iterate(tok_iter *iter) {
	if ((iter->idx) < iter->len) {
		return iter->tokens[(iter->idx)++];
	}

	return (token){
		.type = TOK_NONE,
	};
}

token tok_peek(tok_iter *iter) {
	if ((iter->idx) < iter->len) {
		return iter->tokens[iter->idx];
	}

	return (token){
		.type = TOK_NONE,
	};
}

char *get_type_name(token_type t) {
	switch(t) {
	case TOK_ASSIGN:
		return "TOK_ASSIGN";
	case TOK_PLUS:
		return "TOK_PLUS";
	case TOK_MINUS:
		return "TOK_MINUS";
	case TOK_MULT:
		return "TOK_MULT";
	case TOK_DIV:
		return "TOK_DIV";
	case TOK_MOD:
		return "TOK_MOD";
	case TOK_BANG:
		return "TOK_BANG";
	case TOK_LT:
		return "TOK_LT";
	case TOK_GT:
		return "TOK_GT";
	case TOK_AND:
		return "TOK_AND";
	case TOK_OR:
		return "TOK_OR";
	case TOK_XOR:
		return "TOK_XOR";
	case TOK_QUESTION:
		return "TOK_QUESTION";
	case TOK_COLON:
		return "TOK_COLON";
	case TOK_SEMICOLON:
		return "TOK_SEMICOLON";
	case TOK_LBRACKET:
		return "TOK_LBRACKET";
	case TOK_RBRACKET:
		return "TOK_RBRACKET";
	case TOK_LBRACE:
		return "TOK_LBRACE";
	case TOK_RBRACE:
		return "TOK_RBRACE";
	case TOK_LPAREN:
		return "TOK_LPAREN";
	case TOK_RPAREN:
		return "TOK_RPAREN";
	case TOK_DOT:
		return "TOK_DOT";
	case TOK_COMMA:
		return "TOK_COMMA";
	case TOK_EQUAL:
		return "TOK_EQUAL";
	case TOK_INC:
		return "TOK_INC";
	case TOK_DEC:
		return "TOK_DEC";
	case TOK_EMPTY_BRACE:
		return "TOK_EMPTY_BRACE";
	case TOK_NOT_EQUAL:
		return "TOK_NOT_EQUAL";
	case TOK_GT_EQUAL:
		return "TOK_GT_EQUAL";
	case TOK_LT_EQUAL:
		return "TOK_LT_EQUAL";
	case TOK_LOGICAL_AND:
		return "TOK_LOGICAL_AND";
	case TOK_LOGICAL_OR:
		return "TOK_LOGICAL_OR";
	case TOK_SHIFT_LEFT:
		return "TOK_SHIFT_LEFT";
	case TOK_SHIFT_ASSIGN:
		return "TOK_SHIFT_ASSIGN";
	case TOK_PLUS_ASSIGN:
		return "TOK_PLUS_ASSIGN";
	case TOK_MINUS_ASSIGN:
		return "TOK_MINUS_ASSIGN";
	case TOK_MULT_ASSIGN:
		return "TOK_MULT_ASSIGN";
	case TOK_DIV_ASSIGN:
		return "TOK_DIV_ASSIGN";
	case TOK_MOD_ASSIGN:
		return "TOK_MOD_ASSIGN";
	case TOK_SHIFT_LEFT_ASSIGN:
		return "TOK_SHIFT_LEFT_ASSIGN";
	case TOK_SHIFT_RIGHT_ASSIGN:
		return "TOK_SHIFT_RIGHT_ASSIGN";
	case TOK_AND_ASSIGN:
		return "TOK_AND_ASSIGN";
	case TOK_OR_ASSIGN:
		return "TOK_OR_ASSIGN";
	case TOK_XOR_ASSIGN:
		return "TOK_XOR_ASSIGN";
	case TOK_PTR_REF:
		return "TOK_PTR_REF";
	case TOK_INF_ASSIGN:
		return "TOK_INF_ASSIGN";
	case TOK_FN:
		return "TOK_FN";
	case TOK_SWITCH:
		return "TOK_SWITCH";
	case TOK_CASE:
		return "TOK_CASE";
	case TOK_BREAK:
		return "TOK_BREAK";
	case TOK_CONTINUE:
		return "TOK_CONTINUE";
	case TOK_RETURN:
		return "TOK_RETURN";
	case TOK_IF:
		return "TOK_IF";
	case TOK_ELSE:
		return "TOK_ELSE";
	case TOK_SIZEOF:
		return "TOK_SIZEOF";
	case TOK_U8:
		return "TOK_U8";
	case TOK_U16:
		return "TOK_U16";
	case TOK_U32:
		return "TOK_U32";
	case TOK_U64:
		return "TOK_U64";
	case TOK_I8:
		return "TOK_I8";
	case TOK_I16:
		return "TOK_I16";
	case TOK_I32:
		return "TOK_I32";
	case TOK_I64:
		return "TOK_I64";
	case TOK_F32:
		return "TOK_F32";
	case TOK_F64:
		return "TOK_F64";
	case TOK_STRING:
		return "TOK_STRING";
	case TOK_BYTE:
		return "TOK_BYTE";
	case TOK_BOOL:
		return "TOK_BOOL";
	case TOK_TRUE:
		return "TOK_TRUE";
	case TOK_FALSE:
		return "TOK_FALSE";
	case NUM_TOK:
		return "NUM_TOK";
	case TOK_INT_LIT:
		return "TOK_INT_LIT";
	case TOK_FLOAT_LIT:
		return "TOK_FLOAT_LIT";
	case TOK_STRING_LIT:
		return "TOK_STRING_LIT";
	case TOK_IDENT:
		return "TOK_IDENT";
	case TOK_NONE:
		return "TOK_NONE";
	}
}
