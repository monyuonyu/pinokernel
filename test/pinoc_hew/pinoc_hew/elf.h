#ifndef _ELF_H_
#define _ELF_H_


// 修正BSDライセンス使用

//Elf32_Addr	32bit符号無し整数
//Elf32_Half	16bit符号無し整数
//Elf32_Off		32bit符号無し整数
//Elf32_Sword	32bit符号付き整数
//Elf32_Word	32bit符号無し整数
//
//Elf64_Addr	64bit符号無し整数
//Elf64_Half	16bit符号無し整数
//Elf64_Off		64bit符号無し整数
//Elf64_Sword	32bit符号付き整数
//Elf64_Word	32bit符号無し整数
//Elf64_Xword	64bit符号無し整数
//Elf64_Sxword	64bit符号付き整数


/********************************************************************************
	ELFヘッダ
********************************************************************************/
// 52byte
struct Elf32_Ehdr {
   unsigned char	chare_ident[16];/* 16byteのマジックコード */
   unsigned short	e_type;			/* 再配置可能や実行可能等のELFの形式を表す。 */
   unsigned short   e_machine;		/* ファイルで要求されるアーキテクチャ */
   unsigned long    e_version;		/* ELFフォーマットのバージョンで現在は1 */
   unsigned long    e_entry;		/* プログラムのエントリポイント。指定無しなら0。 */
   unsigned long    e_phoff;		/* プログラムヘッダテーブルのファイル先頭からのオフセット */
   unsigned long    e_shoff;		/* 実行時には使われない */
   unsigned long    e_flags;		/* プロセッサ固有のフラグ */
   unsigned short   e_ehsize;		/* ELFヘッダのサイズ */
   unsigned short   e_phentsize;	/* プログラムヘッダテーブルの1要素あたりのサイズ */
   unsigned short   e_phnum;		/* プログラムヘッダテーブルの要素数 */
   unsigned short   e_shentsize;	/* 実行時には使われない */
   unsigned short   e_shnum;		/* 実行時には使われない */
   unsigned short   e_shstrndx;		/* 実行時には使われない */
};

// 64byte
//struct Elf64_Ehdr {
//   unsigned char e_ident[16];
//   unsigned short    e_type;
//   unsigned short    e_machine;
//   unsigned long    e_version;
//   unsigned long long    e_entry;
//   unsigned long long     e_phoff;
//   unsigned long long     e_shoff;
//   unsigned long    e_flags;
//   unsigned short    e_ehsize;
//   unsigned short    e_phentsize;
//   unsigned short    e_phnum;
//   unsigned short    e_shentsize;
//   unsigned short    e_shnum;
//   unsigned short    e_shstrndx;
//};

/********************************************************************************
	プログラムヘッダテーブル
********************************************************************************/
enum {
   PT_NULL,    /* 使われないエントリで、他のメンバの値の意味は未定義 */
   PT_LOAD,    /* 実行時にロードされるセグメント */
   PT_DYNAMIC, /* 動的構造体配列を保持するセグメント */
   PT_INTERP,  /* ファイルの解釈に使われるインタプリタのパスを保持するセグメント */
   PT_NOTE,    /* ファイルの解釈には使われない情報を保持するセグメント */
   PT_SHLIB,   /* 予約 */
   PT_PHDR,    /* プログラムヘッダテーブル（プログラムのメモリイメージの一部である場合のみ存在） */
   PT_TLS,     /* スレッド局所記憶領域のテンプレート */

   PT_LOOS = 0x60000000,
   /* OS固有のセマンティクスの為に予約された領域 */
   PT_HIOS = 0x6fffffff,

   PT_LOPROC = 0x70000000,
   /* プロセッサ固有のセマンティクスの為に予約された領域 */
   PT_HIPROC = 0x7fffffff
};

// 32byte
struct Elf32_Phdr {
	unsigned long p_type;   /* エントリのタイプ */
	unsigned long  p_offset; /* 対応するセグメントのファイル先頭からのオフセット */
	unsigned long p_vaddr;  /* メモリ上でのセグメントの第一バイトの仮想アドレス */
	unsigned long p_paddr;  /* 物理番地指定が適切なシステムの為に予約。 */
	unsigned long p_filesz; /* 対応するセグメントのファイルでのサイズ。0も可。 */
	unsigned long p_memsz;  /* 対応するセグメントのメモリ上に展開された時のサイズ。0も可。 */
	unsigned long p_flags;  /* 対応するセグメントに適切なフラグ。 */
	unsigned long p_align;  /* p_offsetとp_vaddrをこの値で割った余りは等しい */
	struct Elf32_Phdr* next;
};

// 56byte
//struct Elf64_Phdr {
//	unsigned long  p_type;
//	unsigned long  p_flags;
//	unsigned long long   p_offset;
//	unsigned long long  p_vaddr;
//	unsigned long long  p_paddr;
//	unsigned long long p_filesz;
//	unsigned long long p_memsz;
//	unsigned long long p_align;
//	struct Elf64_Phdr* next;
//};



void elf_status();
int elf_check(struct Elf32_Ehdr* buff);
int elf_init(char* buff);
void* elf_devlop(char* buff);

#endif
