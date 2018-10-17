#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lex.h"

#define DEFAULT_BODY_CAP 100
#define DEFAULT_PARAMS_CAP 10

int main() {
	// char *test_program =
	// 	"int main(int argc, void argv[][]) {"
	// 	"if (test >>= other_test)"
	// 	"printf(\"Hello \\\"there\\n\");"
	// 	"}";

	char *test_program =
		"fn main(i32 argc, str[] args) : int {\n"
		"\tprintln(\"Hello \\\"there\\n\");\n"
		"}\n";

	token tokens[RESULT_BUFFER_SIZE];
	lex_result res = (lex_result){
		.len = 0,
		.cap = RESULT_BUFFER_SIZE,
		.tokens = tokens,
	};

	printf("Beginning lexing\n");
	printf("%s\n", test_program);
	lex(&res, test_program);
	printf("Finished lexing\n");
	print_result(res);
	printf("\n");
	return 0;
}
