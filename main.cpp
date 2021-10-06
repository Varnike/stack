#include <stdio.h>
#include "stack.h"

int main()
{
	Stack stack = {};

	StackCtor(&stack, 8);
	stack.data = NULL;

	StackPush(&stack, 9);

	StackPop(&stack);

	StackPush(&stack, 9);
	const int cnt = 1000;
	int i = 0;
	for (i = 0; i <= cnt; i++) {
		StackPush(&stack, i);
	}

	printf("\n\n\t\t\tHAHHAHA\n\n");	
	//stack.data[10] = 9499878;
	//stack.data[11] = 0;

	int test_val = 0;
	for (i = cnt ; i >= 0; i--) {
		if ((test_val = StackPop(&stack)) != i)
		       printf("TEST ERROR: %d -- %d\n", i, test_val);	
	}
	
	printf("%d", StackPop(&stack));
	StackDtor(&stack);

	printf("ok\n");
}
