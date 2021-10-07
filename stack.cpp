#include "stack.h"

static int StackResize(Stack *stack, size_t nsize);
static void setCanaryPtr(val_t **data);                    
static void setDataPtr(val_t **data);                      

int _StackCtor(Stack *stack, int size, const int src_line, const char *src_file)
{	
	CHECK_FOR_INIT;

	printf("size = %d\n", size);

	stack->LCANARY = CANARYVAL;
	stack->RCANARY = CANARYVAL;

	stack->size = 0;
	stack->capacity = sizeAlign(sizeof(val_t), size, sizeof(uint64_t))/sizeof(val_t);

	stack->data = (val_t*)calloc(sizeAlign(sizeof(val_t), size, sizeof(uint64_t)) + 2 * sizeof(uint64_t), 1);
	
	if (stack->data == NULL) {
		EXIT_DUMP;
	}

	*(uint64_t*)stack->data = CANARYVAL;
	setDataPtr(&stack->data);
	*(uint64_t*)(stack->data + size) = CANARYVAL;
	
	SET_HASH;

	ASSERT_OK;
	return 0;
}

int StackDtor(Stack *stack) 
{	
	ASSERT_OK;
	
	memset(stack->data, 0, stack->size);

	stack->capacity = 0;
	stack->size     =-1;
#if HASH_CHECK == 1
	stack->hash     = 0;
#endif
	setCanaryPtr(&stack->data);

	free(stack->data);
	stack->data = (val_t*)(POISONED_MEM);

	return NO_ERR;
}

val_t StackPop(Stack *stack)
{	
	POP_ASSERT_OK;
	
	if (StackCheck(stack))
		return ERRNUM;
	
	if (stack->size <= 0) {
		ERRNUM = UNDERFLOW_ERR;
		EXIT_DUMP;
	}
	
	val_t val = stack->data[--stack->size];
	stack->data[stack->size] = POISONED_VAL;

	if (stack->size * 4 == stack->capacity && stack->capacity > MINIMUM_STACK_SIZE) {
		SET_HASH;
		if (StackResize(stack, stack->capacity / 2)) 
			return 0;
	}

	SET_HASH;	
	POP_ASSERT_OK;

	return val;
}

int StackPush(Stack *stack, val_t val)
{
	ASSERT_OK;
	
	if (StackCheck(stack))
		return ERRNUM;

	if (stack->size >= stack->capacity) {
		ERRNUM = StackResize(stack, 2 * stack->capacity);
		if (ERRNUM != 0)
			return ERRNUM;
	}
	stack->data[stack->size++] = val;
	
	SET_HASH;
	ASSERT_OK;
	return 0;
}

static int StackResize(Stack *stack, size_t nsize)
{
	ASSERT_OK;

	*(uint64_t*)(stack->data + stack->capacity) = 0; 

	setCanaryPtr(&stack->data);
	
	stack->data = (val_t*)reallocarray(stack->data, sizeAlign(sizeof(val_t), nsize, 
				sizeof(uint64_t)) + 2 * sizeof(uint64_t), 1);

	setDataPtr(&stack->data);
		
	*(uint64_t*)(stack->data + nsize) = CANARYVAL;

	if (stack == NULL)
		return REALL_ERR;

	stack->capacity = sizeAlign(sizeof(val_t), nsize, sizeof(uint64_t))/sizeof(val_t);;
	
	SET_HASH;
	ASSERT_OK;

	return NO_ERR;
}

int StackSetFileName(Stack *stack, const char *name)
{
#if MULTIPLE_LOGS == 1 
	if (name == NULL || stack == NULL)
		return -1;
	else {
		stack->filename = (char *)name;
	}
#endif
	return 0;
}


//#define CHECK_(what, code)  if (what) SET_ERR (code)


int StackCheck(Stack *stack)
{
	CHECK_(stack == NULL, 							NULLPTR_STACK);
	CHECK_(stack->data == NULL, 						UNINIT_DATA);
	CHECK_(stack->data == (val_t *)(POISONED_MEM),				POISONED_STACK);
	CHECK_(stack->size > stack->capacity, 					STACK_OVERFLOW);
	CHECK_(stack->size < 0, 						UNDERFLOW_ERR);
#if CANARIES_CHECK == 1	
	CHECK_(stack->LCANARY != CANARYVAL && stack->RCANARY != CANARYVAL, 	INVALID_CANARIES);
	CHECK_(stack->LCANARY != CANARYVAL, 					INVALID_LCANARY);
	CHECK_(stack->RCANARY != CANARYVAL, 					INVALID_RCANARY);
	CHECK_(*(uint64_t*)((char *)stack->data-sizeof(uint64_t)) != CANARYVAL, INVALID_DATA_LCANARY);
	CHECK_(*(uint64_t*)(stack->data + stack->capacity) != CANARYVAL,        INVALID_DATA_RCANARY);
#endif

#if HASH_CHECK == 1
	CHECK_(stack->hash != StackHash(stack),					INVALID_HASH);
#endif	
	return NO_ERR;
}



void _StackDump(Stack *stack, const char *srcfunc, const char *srcfile, const int line) {
#if MULTIPLE_LOGS == 0
	FILE *file = fopen("log.txt", "w");
#else 
	FILE *file = NULL;
	if (stack->filename != NULL)
		file = fopen(stack->filename, "w");
	else 
		file = fopen("log.txt", "w");
#endif
	if (file == NULL) 
		perror("Can't open/create log file\n");

	if (ERRNUM) {
		fprintf(file,"ERROR : %s\n", errmsg(ERRNUM));
	}

	if (stack == NULL) {
		fclose(file);
		return;
	}
	
	fprintf(file, "Stack<%s>[%p]:",typeid(val_t).name(), stack);	
	if (ERRNUM)
		fprintf(file, "ERROR!");
	else
		fprintf(file, "ok!");

	fprintf(file, "; \"%s\" called from %s at %s (%d)\n",
		       VAR_NAME(stack), srcfunc, srcfile, line);
#if HASH_CHECK == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)
		fprintf(file, "\thash :  %" PRIu32"\n", StackHash(stack));	
#endif 
#if MULTIPLE_LOGS == 1
	fprintf(file,"\tstructure left canary = %" PRIu64 "\n\tstructure right canary = %" 
			PRIu64 "\n",stack->LCANARY,stack->RCANARY);
#endif	
	fprintf(file, "{\n\tcapasity = %zu;\n\tsize = %d;\n", stack->capacity, stack->size);

#if MULTIPLE_LOGS == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)	
		fprintf(file,"\tleft data canary[%p] = %" PRIu64 "\n", (char *)stack->data-sizeof(uint64_t),*(uint64_t*)((char *)stack->data-sizeof(uint64_t)));
#endif

	fprintf(file, "\tdata[%p]\n\t{\n", stack->data);

	if (stack->data == NULL) {
		fprintf(file, "\t\t[0] = NULL;\n");
	} 
	else if (stack->data == (val_t*)POISONED_MEM) {
		fprintf(file, "\t\t[0] = %d[POISONED];\n", POISONED_MEM);
	} 
	else {
		for (int i = 0; i != stack->capacity; i++) {
			fprintf(file,"\t\t*[%d] [%p] = ", i, stack->data + i);
			if (stack->data[i] == POISONED_VAL && i >= stack->size)
				fprintf(file,"POISONED\n");
			else
				printStackVal(stack->data[i], file);
		}
	}

	fprintf(file,"\t}\n");
#if MULTIPLE_LOGS == 1
	if (stack->data != (val_t *)(POISONED_MEM) &&  stack->data != NULL)
		fprintf(file,"\tright data canary[%p] : %" PRIu64 "\n", 
			stack->data + stack->capacity,*(uint64_t*)(stack->data + stack->capacity));
#endif
	fprintf(file,"}\n\n");
	fclose(file);
}

static void setCanaryPtr(val_t **data) 
{
	if (*data != NULL)
		*data = (val_t*)((char*)(*data) - sizeof(uint64_t));
}

static void setDataPtr(val_t **data)
{
	if (*data != NULL)
		*data = (val_t*)((char*)(*data) + sizeof(uint64_t));
}

int sizeAlign(size_t val_s, int numb, int aligment)
{
	int len = val_s * numb;
	if (len % aligment != 0)
		return (len + val_s * (len % aligment));	
	return len;
}

//An algorithm produced by Professor Daniel J. Bernstein

uint32_t djb_hash(const char* data, size_t length)
{
	unsigned int hash = 5381; // MAGIC NUMBER
	
	for (int i = 0; i < length; ++i) {
		hash = (hash << 5) + hash + data[i];
	}

	return hash;
}
#if HASH_CHECK == 1
uint32_t StackHash(Stack *stack)
{
	Stack stack_ex = {};

	uint32_t dhash = 0, stackhash = 0;

	dhash = djb_hash((const char *)stack->data, stack->size * sizeof(val_t));

	uint32_t current_hash = stack->hash;	
	stack->hash = dhash;
		
	dhash = djb_hash((const char *)stack, sizeof(stack_ex));

	stack->hash = current_hash;

	return dhash;	
}
#endif
