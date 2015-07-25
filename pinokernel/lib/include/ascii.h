/*
 * ascii.h
 *
 *  Created on: 2011/05/03
 *      Author: mizu
 */

#ifndef ASCII_H_
#define ASCII_H_

// newlib stddeff.h
#ifndef _STDDEF_H_
#define SIZE_T
typedef unsigned int size_t;
#endif


int decto10(char* _str);

// ASCIIコードをバイナリに変換
void asciitobin(char* _str, size_t size);


#endif /* ASCII_H_ */
