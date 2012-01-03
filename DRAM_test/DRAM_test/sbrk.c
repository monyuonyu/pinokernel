/***********************************************************************/
/*                                                                     */
/*  FILE        :sbrk.c                                                */
/*  DATE        :Fri, Nov 11, 2011                                     */
/*  DESCRIPTION :Program of sbrk                                       */
/*  CPU TYPE    :H8/3069R                                              */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.16).    */
/*                                                                     */
/***********************************************************************/
                  


#include <stdio.h>
#include "typedefine.h"
#include "sbrk.h"
_SBYTE *sbrk(size_t);

//const size_t _sbrk_size=              /* Specifies the minimum unit of */
                                        /* the defined heap area         */

#pragma pack 2
static union {
    _SWORD  dummy;                      /* 2バイト境界にするためのダミー */
    _SBYTE  heap[HEAPSIZE];             /* sbrkで管理する領域の宣言      */
} heap_area;
#pragma unpack

static _SBYTE *brk=(_SBYTE *)&heap_area;  /* sbrkで割り付けた領域の最終アドレス */

/*****************************************************************************/
/*  sbrk:データの書き出し                                                    */
/*       リターン値：割り付けた領域の先頭アドレス（成功）                    */
/*                   -1                          （失敗）                    */
/*****************************************************************************/
extern _SBYTE *sbrk(size_t size)              /* 割り付ける領域のサイズ */
{
    _SBYTE  *p;
    if(brk+size > heap_area.heap+HEAPSIZE){   /* 空き領域のチェック */
        p = (_SBYTE *)-1;
    }
    else {
        p = brk;                              /* 領域の割り付け     */
        brk += size;                          /* 最終アドレスの更新 */
    }

    return p;
}
