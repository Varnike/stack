#include <stdio.h>
#include "stack.h"

int main()
{
	Stack stack = {};

	StackCtor(&stack, 8);
	//StackDtor(&stack);	
	
	StackPush(&stack, 9);

	StackPop(&stack);

	StackPush(&stack, 9);
	const int cnt = 100;
	int i = 0;
	for (i = 0; i <= cnt; i++) {
		StackPush(&stack, i);
	}

	stack.LCANARY = 0;	
	stack.data[10] = 9499878;
	
	int test_val = 0;
	for (i = cnt ; i >= 0; i--) {
		if ((test_val = StackPop(&stack)) != i)
		       printf("TEST ERROR: %d -- %d\n", i, test_val);	
	}
	
	
	StackDtor(&stack);
}
