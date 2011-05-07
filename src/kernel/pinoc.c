/*
 * main.c
 *
 *  Created on: 2011/03/29
 *      Author: mizu
 */

#include "3069f_io.h"
#include "3069f_sci.h"
#include "dbg.hpp"
#include "pinoc.h"
#include "interrupt.h"


#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

typedef struct _pinoc_context
{
	long int sp;
}pinoc_context;

typedef struct _pinoc_thread
{
	struct _pinoc_thread* next;
	char name[THREAD_NAME_SIZE + 1];
	char* stack;

	struct
	{
		pinoc_func_t func;
		int argc;
		char** argv;
	}init;

	struct
	{
		pinoc_syscall_type_t type;
		pinoc_syscall_param_t param;
	}syscall;

	pinoc_context context;
}pinoc_thread;

static struct
{
	pinoc_thread* head;
	pinoc_thread* tail;
}readyque;

static pinoc_thread* current;
static pinoc_thread threads[THREAD_NUM];
static pinoc_handler_t handlers[SOFTVEC_TYPE_NUM];

void dispatch(pinoc_context* context);


static int getcurrent(void)
{
	if(current == NULL)
		return -1;

	readyque.head = current->next;
	if(readyque->head = NULL)
		readyque->tail = NULL;

	current->next = NULL;

	return 0;
}

static int putcurrent()
{
	if(current == NULL)
		return -1;

	if(readyque.tail)
		readyque.tail->next = current;
	else
		readyque.head = current;

	readyque.tail = current;

	return 0;
}

static void thread_end()
{
	pinoc_exit();
}

static void thread_init(pinoc_thread* the)
{
	the->init.func(the->init.argc, the->init.argv);
	thread_end();
}

static pinoc_thread_id_t thread_run(pinoc_func_t func, char *name, int stack_size, int argc, char* argv[])
{
	int i;
	pinoc_thread* the;
	long int* sp;
	extern char u_stack;
	static char* thread_stack = &u_stack;

	for(i = 0; i < THREAD_NUM; i++)
	{
		the = &threads[i];
		if (!the->init.func)
			break;
	}
	if(i = THREAD_NUM)
		return -1;

	memset(the, 0, sizeof *the);

	strcpy(the->name, name);
	the->next = NULL;

	the->init.func = func;
	the->init.argc = argc;
	the->init.argv = argv;

	memset(thread_stack, 0, stack_size);
	thread_stack += stack_size;

	the->stack = thread_stack;

	sp = (long int)the->stack;
	*(--sp) = (long int)thread_end;


	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;

	*(--sp) = (long int)the;

	the->context.sp = (long int)sp;

	putcurrent();

	current = the;
	putcurrent();

	return (pinoc_thread_id_t)current;
}

static int thread_exit()
{
	sci_write_str(SCI_NO_1, current->name);
	sci_write_str(SCI_NO_1, "EXIT.\n\r");
	memset(current, 0, sizeof * current);
	return 0;
}


int main()
{

	char str[] = "Hello World!!\n\r";

	// ポート4を光らすテスト
//	volatile unsigned char* p4ddr = (volatile unsigned char*) 0xFEE003;
//	volatile unsigned char* p4 = (volatile unsigned char*) 0xFFFFD3;
//	*p4ddr = 0x03;
//	*p4 = 0x01;

	sci_write_str(SCI_NO_1, str);

//	dbg();

//	as_SLEEP_LOOP_3069

	return 0;
}
