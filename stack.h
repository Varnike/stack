#ifndef STACK_H
#define STACK_H

#include <cstring>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <stdint.h>
#include <inttypes.h>
#include "error.h"
#include "config.h"
#include "printval.h"

#define VAR_NAME(v) #v

#define StackDump(stack) _StackDump(stack, __func__, __FILE__, __LINE__);

#define ASSERT_OK do {if (StackCheck(stack)){\
			EXIT_DUMP;}}while(0);

#define EXIT_DUMP StackDump(stack);\
			EXIT_ERR; 

#if DEBUG == 1
#define EXIT_ERR assert(!"ok");
#define POP_ASSERT_OK do {if (StackCheck(stack)){\
			EXIT_DUMP;}}while(0);  


#else 
#define EXIT_ERR return ERRNUM;
#define POP_ASSERT_OK do {if (StackCheck(stack)){\
                        StackDump(stack);\
			return 0;}}while(0);     
#endif

#define CHECK_FOR_INIT if(stack->size != 0 || stack->capacity != 0 || stack->data != NULL) {\
					ERRNUM = INVALID_STACK;\
					_StackDump(stack, __func__, src_file,src_line);\
					EXIT_ERR;}

#define StackCtor(stack, size) _StackCtor(stack, size,__LINE__, __FILE__)

#define SET_ERR(errtype) return ERRNUM = errtype;

#define SET_HASH stack->hash = StackHash(stack);

typedef int val_t;

const int POISONED_MEM = 1488;
const int POISONED_VAL = 0xDEDBEAF;
const uint64_t CANARYVAL    = 0xDABABAF;
const int MINIMUM_STACK_SIZE = 8;

struct Stack {
	uint64_t LCANARY;
	
	val_t *data;
	int size;
	size_t capacity;
#if MULTIPLE_LOGS == 1
	char *filename;
#endif
	uint32_t hash;
	uint64_t RCANARY;
};

int StackPush(Stack *stack, val_t val);
val_t StackPop(Stack *stack);
int _StackCtor(Stack *stack, int size = MINIMUM_STACK_SIZE, const int src_line = 0, 
		const char *src_file = NULL);
int StackDtor(Stack *stack);
int StackSetFileName(const char *name);
int StackCheck(Stack *stack);
void StackPrint(Stack *stack);
void _StackDump(Stack *stack, const char *srcfunc, const char *srcfile, const int line);
int sizeAlign(size_t val_s, int numb, int aligment);
uint32_t djb_hash(const char *message, size_t data_length);
uint32_t StackHash(Stack *stack);
#endif
