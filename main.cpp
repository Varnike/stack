#include <stdio.h>
#include "stack.h"

int main()
{
	Stack stack = {};
	
	StackCtor(&stack, 8);

	for (int i = 0; i != 17; i++) {
		StackPush(&stack, i);
	}

	stack.size = 9;
		
	for (; stack.size != 0; ) {
		printf("%d\n",StackPop(&stack));
	}

	StackDtor(&stack);
}
