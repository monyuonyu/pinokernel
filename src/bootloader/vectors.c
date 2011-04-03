/*
 * vectors.c
 *
 *  Created on: 2010/11/27
 *      Author: pino
 */

extern void start(void);

void (*vectors[])(void) =
{
	start, 0, 0, 0
};
