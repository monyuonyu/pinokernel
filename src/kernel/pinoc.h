/*
 * pinoc.h
 *
 *  Created on: 2011/05/07
 *      Author: mizu
 */

#ifndef PINOC_H_
#define PINOC_H_


// システム・コール
void pinoc_run(pinoc_func_t func, char *name, int stack_size, int argc, char* argv[]);
void pinoc_exit();

// ライブラリ関数
void pinoc_start(pinoc_func_t func, char *name, int stack_size, int argc, char* argv[]);
void pinoc_syserr();
void pinoc_syscall(pinoc_syscall_type_t type, pinoc_syscall_param_t* param);

// ユーザー・スレッド
int test08_1_main(int argc, char* argv[]);

#endif /* PINOC_H_ */
