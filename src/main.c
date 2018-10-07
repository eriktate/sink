#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

int main() {
	char *test_program =
		"int main(int argc, void argv[][]) {"
		"if (test >>= other_test)"
		"printf(\"Hello \\\"there\\n\");"
		"}";

	struct lex_result res = init_result();
	lex(&res, test_program);
	print_result(res);
	printf("\n");
	return 0;
}
