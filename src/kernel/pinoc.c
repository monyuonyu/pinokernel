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
#include "intr.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

// ユーザー毎のコンテキストが保存されたスタックポインタ
typedef struct _pinoc_context
{
	long int sp;
}pinoc_context;

/*
 * スレッドの型
 */
typedef struct _pinoc_thread
{
	struct _pinoc_thread* next;			// レディーキュー
	char name[THREAD_NAME_SIZE + 1];	// スレッド名
	char* stack;						//

	struct
	{
		pinoc_func_t func;				// 関数名
		int argc;						// 引数1
		char** argv;					// 引数2
	}init;

	struct
	{
		pinoc_syscall_type_t type;		// システムコールの種類
		pinoc_syscall_param_t param;	// システみコールの引数
	}syscall;

	pinoc_context context;				// このスレッドのコンテキスト情報
}pinoc_thread;

// レディースキュー
static struct
{
	pinoc_thread* head;
	pinoc_thread* tail;
}readyque;

/*
 * グローバル変数
 */
static pinoc_thread* current;						// 使用中のスレッド情報が格納される
static pinoc_thread threads[THREAD_NUM];			// スレッドの数だけ領域確保
static pinoc_handler_t handlers[SOFTVEC_TYPE_NUM];	// ハンドラの数だけ領域確保

/*
 * ディスパッチの内容は外部ファイルにアセンブラで記述してある
 */
void dispatch(pinoc_context* context);

/*
 * レディースキュー操作
 */
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

/*
 * スレッド生成
 */
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

static void thread_intr(softvec_type_t type, unsigned long sp)
{
	current->context.sp = sp;

	// handlerに登録してあれば、実行
	if(handlers[type])
		handlers[type]();

	// スレッドのスケジューリング		ラウンドロビン方式
	// schedule();

	// ディスパッチ
	dispatch(&current->context);
}

//void setintr(softvec_type_t type, )
//{
//
//}

// 二重の割り込みベクターから呼び出される
/*
 * システムコール呼び出し
 */
void syscall_intr()		// 未実装
{

}

/*
 * システムエラー呼び出し
 */
void syserror_intr()	// 未実装
{

}

/*
 * OSのスレッド関連の割り込みベクター ※ 二十構造
 */
void thread_intr(softvec_type_t type, unsigned long sp)
{
	switch(typy)
	{
	case SOFTVEC_TYPE_SYSTEM:
		handlers[type]();
		break;
	case SOFTVEC_TYPE_SOFTERR:
		handlers[type]();
		break;

	}

}

// Threadとして起動	テスト用関数
int test08_1_main(int argc, char* argv[])
{
	static char str1[] = "Hello test_tsk!!\n\r";
	sci_write_str(SCI_NO_1, str1);
	while(1);

}

/*
 * 手動でスレッドの生成
 */
void pinoc_start(pinoc_func_t func, char *name, int stack_size, int argc, char* argv[])
{
	// コンテキストを初期化
	current = NULL;

	// レディースキューを初期化
	readyque.head = NULL;
	readyque.tail = NULL;

	// スレッドと、ハンドラ領域の初期化
	memset(threads, sizeof(threads));
	memset(handlers, sizeof(handlers));

	/*
	 * スレッド用割り込みハンドラの初期化
	 * ※ 二重構造に注意
	 */
	softvec_setintr(SOFTVEC_TYPE_SYSTEM, thread_intr);
	softvec_setintr(SOFTVEC_TYPE_SOFTERR, thread_intr);

	/*
	 * threadVectorへ登録
	 * RomVector -> SoftVector -> Handlers(threadVector)
	 */
	handlers[SOFTVEC_TYPE_SYSTEM] = syscall_intr;
	handlers[SOFTVEC_TYPE_SOFTERR] = syserror_intr;

	/*
	 * スレッドの生成してスレッドIDを返却
	 * スレッドを生成するだけで処理は帰ってくる
	 */
	current = (pinoc_thread*)thread_run(func, name, stack_size, argc, argv);

	/*
	 * ディスパッチ
	 * 現在のレディースキューに格納されているスタック情報(IPも含め)元通りに復元する
	 */
	dispatch(&current->context);

	// ここへは帰ってこない
}

/*
 * スレッド起動
 */
int start_thread()
{
	pinoc_run(test08_1_main);
	return 0;
}


// ここからカーネルのプログラムが始まる。
int main()
{

	static char str2[] = "Hello World!!\n\r";

	// ポート4を光らすテスト
//	volatile unsigned char* p4ddr = (volatile unsigned char*) 0xFEE003;
//	volatile unsigned char* p4 = (volatile unsigned char*) 0xFFFFD3;
//	*p4ddr = 0x03;
//	*p4 = 0x01;

	sci_write_str(SCI_NO_1, str2);

//	dbg();

//	as_SLEEP_LOOP_3069

//	pinoc_start()

	// 初期スレッド開始 	※中でディスパッチが行われるのでこの関数へ帰ってこない
	pinoc_start(start_thread, "test_tsk", 0x100, NULL, NULL);

	return 0;
}


