/*---------------------------------------------------------------*
 | H8/3069F I/O address definition Release 1.0 2010-02-12        |
 |                                                               |
 |              Copyright (c) BestTechnology CO.,LTD. 2009-2010  |
 *---------------------------------------------------------------*/

#ifndef _3069_H_
#define _3069_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long int _LONG;
typedef unsigned int      _WORD;
typedef unsigned char     _BYTE;

union _BITFIELD {
  _BYTE   BYTE;
  struct {
    _BYTE B7  :1;
    _BYTE B6  :1;
    _BYTE B5  :1;
    _BYTE B4  :1;
    _BYTE B3  :1;
    _BYTE B2  :1;
    _BYTE B1  :1;
    _BYTE B0  :1;
  }       BIT;
};

union un_p1dr {             /* union P1DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p2pcr {            /* union P2PCR  */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p2dr {             /* union P2DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p3dr {             /* union P3DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p4pcr {            /* union P4PCR  */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p4dr {             /* union P4DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p5pcr {            /* union P5PCR  */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :4;   /*              */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p5dr {             /* union P5DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :4;   /*              */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p6dr {             /* union P6DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p7dr {             /* union P7DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p8dr {             /* union P8DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :3;   /*              */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_p9dr {             /* union P9DR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :2;   /*              */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_padr {             /* union PADR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_pbdr {             /* union PBDR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     B7      :1;   /*    Bit 7     */
    _BYTE     B6      :1;   /*    Bit 6     */
    _BYTE     B5      :1;   /*    Bit 5     */
    _BYTE     B4      :1;   /*    Bit 4     */
    _BYTE     B3      :1;   /*    Bit 3     */
    _BYTE     B2      :1;   /*    Bit 2     */
    _BYTE     B1      :1;   /*    Bit 1     */
    _BYTE     B0      :1;   /*    Bit 0     */
  }           BIT;          /*              */
};                          /*              */
union un_mdcr {             /* union MDCR   */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :5;   /*              */
    _BYTE     MDS     :3;   /*    MDS       */
  }           BIT;          /*              */
};                          /*              */
union un_syscr {            /* union SYSCR  */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE     SSBY    :1;   /*    SSBY      */
    _BYTE     STS     :3;   /*    STS       */
    _BYTE     UE      :1;   /*    UE        */
    _BYTE     NMIEG   :1;   /*    NMIEG     */
    _BYTE     SSOE    :1;   /*    SSOE      */
    _BYTE     RAME    :1;   /*    RAME      */
  }           BIT;          /*              */
};                          /*              */
union un_divcr {            /* union DIVCR  */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :6;   /*              */
    _BYTE     DIV     :2;   /*    DIV       */
  }           BIT;          /*              */
};                          /*              */
union un_mstcr {            /* union MSTCR  */
  _WORD       WORD;         /*  Word Access */
  struct {                  /*  Byte Access */
    _BYTE     H;            /*    High      */
    _BYTE     L;            /*    Low       */
  }           BYTE;         /*              */
  struct {                  /*  Bit  Access */
    _BYTE     PSTOP   :1;   /*    PSTOP     */
    _BYTE             :4;   /*              */
    _BYTE     _SCI2   :1;   /*    MSTPH2    */
    _BYTE     _SCI1   :1;   /*    MSTPH1    */
    _BYTE     _SCI0   :1;   /*    MSTPH0    */
    _BYTE     _DMAC   :1;   /*    MSTPL7    */
    _BYTE             :1;   /*              */
    _BYTE     _DRAM   :1;   /*    MSTPL5    */
    _BYTE     _ITU    :1;   /*    MSTPL4    */
    _BYTE     _TMR01  :1;   /*    MSTPL3    */
    _BYTE     _TMR23  :1;   /*    MSTPL2    */
    _BYTE             :1;   /*              */
    _BYTE     _AD     :1;   /*    MSTPL0    */
  }           BIT;          /*              */
};                          /*              */
struct st_bsc {             /* struct BSC   */
  char        wk1;          /*              */
  union {                   /* BRCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   A23E    :1;   /*    A23E      */
      _BYTE   A22E    :1;   /*    A22E      */
      _BYTE   A21E    :1;   /*    A21E      */
      _BYTE   A20E    :1;   /*    A20E      */
      _BYTE           :3;   /*              */
      _BYTE   BRLE    :1;   /*    BRLE      */
    }         BIT;          /*              */
  }           BRCR;         /*              */
  char        wk2[11];      /*              */
  union {                   /* CSCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CS7E    :1;   /*    CS7E      */
      _BYTE   CS6E    :1;   /*    CS6E      */
      _BYTE   CS5E    :1;   /*    CS5E      */
      _BYTE   CS4E    :1;   /*    CS4E      */
    }         BIT;          /*              */
  }           CSCR;         /*              */
  union {                   /* ABWCR        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   ABW7    :1;   /*    ABW7      */
      _BYTE   ABW6    :1;   /*    ABW6      */
      _BYTE   ABW5    :1;   /*    ABW5      */
      _BYTE   ABW4    :1;   /*    ABW4      */
      _BYTE   ABW3    :1;   /*    ABW3      */
      _BYTE   ABW2    :1;   /*    ABW2      */
      _BYTE   ABW1    :1;   /*    ABW1      */
      _BYTE   ABW0    :1;   /*    ABW0      */
    }         BIT;          /*              */
  }           ABWCR;        /*              */
  union {                   /* ASTCR        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   AST7    :1;   /*    AST7      */
      _BYTE   AST6    :1;   /*    AST6      */
      _BYTE   AST5    :1;   /*    AST5      */
      _BYTE   AST4    :1;   /*    AST4      */
      _BYTE   AST3    :1;   /*    AST3      */
      _BYTE   AST2    :1;   /*    AST2      */
      _BYTE   AST1    :1;   /*    AST1      */
      _BYTE   AST0    :1;   /*    AST0      */
    }         BIT;          /*              */
  }           ASTCR;        /*              */
  union {                   /* WCR          */
    _WORD     WORD;         /*  Word Access */
    struct {                /*  Byte Access */
      _BYTE   H;            /*    WCRH      */
      _BYTE   L;            /*    WCRL      */
    }         BYTE;         /*              */
    struct {                /*  Bit  Access */
      _BYTE   W7      :2;   /*    W7        */
      _BYTE   W6      :2;   /*    W6        */
      _BYTE   W5      :2;   /*    W5        */
      _BYTE   W4      :2;   /*    W4        */
      _BYTE   W3      :2;   /*    W3        */
      _BYTE   W2      :2;   /*    W2        */
      _BYTE   W1      :2;   /*    W1        */
      _BYTE   W0      :2;   /*    W0        */
    }         BIT;          /*              */
  }           WCR;          /*              */
  union {                   /* BCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   ICIS1   :1;   /*    ICIS1     */
      _BYTE   ICIS0   :1;   /*    ICIS0     */
      _BYTE   BROME   :1;   /*    BROME     */
      _BYTE   BRSTS1  :1;   /*    BRSTS1    */
      _BYTE   BRSTS0  :1;   /*    BRSTS0    */
      _BYTE           :1;   /*              */
      _BYTE   RDEA    :1;   /*    RDEA      */
      _BYTE   WAITE   :1;   /*    WAITE     */
    }         BIT;          /*              */
  }           BCR;          /*              */
  char        wk3;          /*              */
  union {                   /* DRCRA        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DRAS    :3;   /*    DRAS      */
      _BYTE           :1;   /*              */
      _BYTE   BE      :1;   /*    BE        */
      _BYTE   RDM     :1;   /*    RDM       */
      _BYTE   SRFMD   :1;   /*    SRFMD     */
      _BYTE   RFSHE   :1;   /*    RFSHE     */
    }         BIT;          /*              */
  }           DRCRA;        /*              */
  union {                   /* DRCRB        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   MXC     :2;   /*    MXC       */
      _BYTE   CSEL    :1;   /*    CSEL      */
      _BYTE   RCYCE   :1;   /*    RCYCE     */
      _BYTE           :1;   /*              */
      _BYTE   _TPC    :1;   /*    TPC       */
      _BYTE   RCW     :1;   /*    RCW       */
      _BYTE   RLW     :1;   /*    RLW       */
    }         BIT;          /*              */
  }           DRCRB;        /*              */
  union {                   /* RTMCSR       */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMF     :1;   /*    CMF       */
      _BYTE   CMIE    :1;   /*    CMIE      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           RTMCSR;       /*              */
  _BYTE       RTCNT;        /* RTCNT        */
  _BYTE       RTCOR;        /* RTCOR        */
};                          /*              */
struct st_intc {            /* struct INTC  */
  union {                   /* ISCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :2;   /*              */
      _BYTE   IRQ5SC  :1;   /*    IRQ5SC    */
      _BYTE   IRQ4SC  :1;   /*    IRQ4SC    */
      _BYTE   IRQ3SC  :1;   /*    IRQ3SC    */
      _BYTE   IRQ2SC  :1;   /*    IRQ2SC    */
      _BYTE   IRQ1SC  :1;   /*    IRQ1SC    */
      _BYTE   IRQ0SC  :1;   /*    IRQ0SC    */
    }         BIT;          /*              */
  }           ISCR;         /*              */
  union {                   /* IER          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :2;   /*              */
      _BYTE   IRQ5E   :1;   /*    IRQ5E     */
      _BYTE   IRQ4E   :1;   /*    IRQ4E     */
      _BYTE   IRQ3E   :1;   /*    IRQ3E     */
      _BYTE   IRQ2E   :1;   /*    IRQ2E     */
      _BYTE   IRQ1E   :1;   /*    IRQ1E     */
      _BYTE   IRQ0E   :1;   /*    IRQ0E     */
    }         BIT;          /*              */
  }           IER;          /*              */
  union {                   /* ISR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :2;   /*              */
      _BYTE   IRQ5F   :1;   /*    IRQ5F     */
      _BYTE   IRQ4F   :1;   /*    IRQ4F     */
      _BYTE   IRQ3F   :1;   /*    IRQ3F     */
      _BYTE   IRQ2F   :1;   /*    IRQ2F     */
      _BYTE   IRQ1F   :1;   /*    IRQ1F     */
      _BYTE   IRQ0F   :1;   /*    IRQ0F     */
    }         BIT;          /*              */
  }           ISR;          /*              */
  char        wk;           /*              */
  union {                   /* IPRA         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   _IRQ0   :1;   /*    IRQ0      */
      _BYTE   _IRQ1   :1;   /*    IRQ1      */
      _BYTE   _IRQ23  :1;   /*    IRQ2,IRQ3 */
      _BYTE   _IRQ45  :1;   /*    IRQ4,IRQ5 */
      _BYTE   _WDT    :1;   /* WDT,DRAM,A/D */
      _BYTE   _ITU0   :1;   /*    ITU0      */
      _BYTE   _ITU1   :1;   /*    ITU1      */
      _BYTE   _ITU2   :1;   /*    ITU2      */
    }         BIT;          /*              */
  }           IPRA;         /*              */
  union {                   /* IPRB         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   _TMR01  :1;   /*    TMR0,1    */
      _BYTE   _TMR23  :1;   /*    TMR2,3    */
      _BYTE   _DMAC   :1;   /*    DMAC      */
      _BYTE           :1;   /*              */
      _BYTE   _SCI0   :1;   /*    SCI0      */
      _BYTE   _SCI1   :1;   /*    SCI1      */
      _BYTE   _SCI2   :1;   /*    SCI2      */
    }         BIT;          /*              */
  }           IPRB;         /*              */
};                          /*              */
#if __CPU__==2              /* Normal Mode  */
struct st_sam {             /* struct DMAC  */
  char        wk[2];        /*              */
  void        *MAR;         /* MAR          */
  _WORD       ETCR;         /* ETCR         */
  _BYTE       IOAR;         /* IOAR         */
  union {                   /* DTCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTE     :1;   /*    DTE       */
      _BYTE   DTSZ    :1;   /*    DTSZ      */
      _BYTE   DTID    :1;   /*    DTID      */
      _BYTE   RPE     :1;   /*    RPE       */
      _BYTE   DTIE    :1;   /*    DTIE      */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCR;         /*              */
};                          /*              */
struct st_fam {             /* struct DMAC  */
  char        wk1[2];       /*              */
  void        *MARA;        /* MARA         */
  _WORD       ETCRA;        /* ETCRA        */
  char        wk2;          /*              */
  union {                   /* DTCRA        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTE     :1;   /*    DTE       */
      _BYTE   DTSZ    :1;   /*    DTSZ      */
      _BYTE   SAID    :1;   /*    SAID      */
      _BYTE   SAIDE   :1;   /*    SAIDE     */
      _BYTE   DTIE    :1;   /*    DTIE      */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCRA;        /*              */
  char        wk3[2];       /*              */
  void        *MARB;        /* MARB         */
  _WORD       ETCRB;        /* ETCRB        */
  char        wk4;          /*              */
  union {                   /* DTCRB        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTME    :1;   /*    DTME      */
      _BYTE           :1;   /*              */
      _BYTE   DAID    :1;   /*    DAID      */
      _BYTE   DAIDE   :1;   /*    DAIDE     */
      _BYTE   TMS     :1;   /*    TMS       */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCRB;        /*              */
};                          /*              */
#else                       /* Advanced Mode*/
struct st_sam {             /* struct DMAC  */
  void        *MAR;         /* MAR          */
  _WORD       ETCR;         /* ETCR         */
  _BYTE       IOAR;         /* IOAR         */
  union {                   /* DTCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTE     :1;   /*    DTE       */
      _BYTE   DTSZ    :1;   /*    DTSZ      */
      _BYTE   DTID    :1;   /*    DTID      */
      _BYTE   RPE     :1;   /*    RPE       */
      _BYTE   DTIE    :1;   /*    DTIE      */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCR;         /*              */
};                          /*              */
struct st_fam {             /* struct DMAC  */
  void        *MARA;        /* MARA         */
  _WORD       ETCRA;        /* ETCRA        */
  char        wk1;          /*              */
  union {                   /* DTCRA        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTE     :1;   /*    DTE       */
      _BYTE   DTSZ    :1;   /*    DTSZ      */
      _BYTE   SAID    :1;   /*    SAID      */
      _BYTE   SAIDE   :1;   /*    SAIDE     */
      _BYTE   DTIE    :1;   /*    DTIE      */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCRA;        /*              */
  void        *MARB;        /* MARB         */
  _WORD       ETCRB;        /* ETCRB        */
  char        wk2;          /*              */
  union {                   /* DTCRB        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DTME    :1;   /*    DTME      */
      _BYTE           :1;   /*              */
      _BYTE   DAID    :1;   /*    DAID      */
      _BYTE   DAIDE   :1;   /*    DAIDE     */
      _BYTE   TMS     :1;   /*    TMS       */
      _BYTE   DTS     :3;   /*    DTS       */
    }         BIT;          /*              */
  }           DTCRB;        /*              */
};                          /*              */
#endif                      /*              */
struct st_flash {           /* struct FLASH */
  char        wk;           /*              */
  union {                   /* RAMCR        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   RAMS    :1;   /*    RAMS      */
      _BYTE   RAM     :3;   /*    RAM       */
    }         BIT;          /*              */
  }           RAMCR;        /*              */
  char        wk1[56];      /*              */
  union {                   /* FCCS         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   FWE     :1;   /*    FWE       */
      _BYTE           :2;   /*              */
      _BYTE   FLER    :1;   /*    FLER      */
      _BYTE           :3;   /*              */
      _BYTE   SCO     :1;   /*    SCO       */
    }         BIT;          /*              */
  }           FCCS;         /*              */
  union {                   /* FPCS         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :7;   /*              */
      _BYTE   PPVS    :1;   /*    PPVS      */
    }         BIT;          /*              */
  }           FPCS;         /*              */
  union {                   /* FECS         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :7;   /*              */
      _BYTE   EPVB    :1;   /*    EPVB      */
    }         BIT;          /*              */
  }           FECS;         /*              */
  char        wk2;          /*              */
  _BYTE       FKEY;         /* FKEY         */
  _BYTE       FMATS;        /* FMATS        */
  char        wk3;          /*              */
  union {                   /* FVACR        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   FVCHGE  :1;   /*    FVCHGE    */
      _BYTE           :3;   /*              */
      _BYTE   FVSEL   :4;   /*    FVSEL     */
    }         BIT;          /*              */
  }           FVACR;        /*              */
  _BYTE       FVADRR;       /* FVADRR       */
  _BYTE       FVADRE;       /* FVADRE       */
  _BYTE       FVADRH;       /* FVADRH       */
  _BYTE       FVADRL;       /* FVADRL       */
};                          /*              */
struct st_itu {             /* struct ITU   */
  union {                   /* TSTR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :5;   /*              */
      _BYTE   STR2    :1;   /*    STR2      */
      _BYTE   STR1    :1;   /*    STR1      */
      _BYTE   STR0    :1;   /*    STR0      */
    }         BIT;          /*              */
  }           TSTR;         /*              */
  union {                   /* TSNC         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :5;   /*              */
      _BYTE   SYNC2   :1;   /*    SYNC2     */
      _BYTE   SYNC1   :1;   /*    SYNC1     */
      _BYTE   SYNC0   :1;   /*    SYNC0     */
    }         BIT;          /*              */
  }           TSNC;         /*              */
  union {                   /* TMDR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   MDF     :1;   /*    MDF       */
      _BYTE   FDIR    :1;   /*    FDIR      */
      _BYTE           :2;   /*              */
      _BYTE   PWM2    :1;   /*    PWM2      */
      _BYTE   PWM1    :1;   /*    PWM1      */
      _BYTE   PWM0    :1;   /*    PWM0      */
    }         BIT;          /*              */
  }           TMDR;         /*              */
  union {                   /* TOLR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :2;   /*              */
      _BYTE   TOB2    :1;   /*    TOB2      */
      _BYTE   TOA2    :1;   /*    TOA2      */
      _BYTE   TOB1    :1;   /*    TOB1      */
      _BYTE   TOA1    :1;   /*    TOA1      */
      _BYTE   TOB0    :1;   /*    TOB0      */
      _BYTE   TOA0    :1;   /*    TOA0      */
    }         BIT;          /*              */
  }           TOLR;         /*              */
  union {                   /* TISRA        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   IMIEA2  :1;   /*    IMIEA2    */
      _BYTE   IMIEA1  :1;   /*    IMIEA2    */
      _BYTE   IMIEA0  :1;   /*    IMIEA2    */
      _BYTE           :1;   /*              */
      _BYTE   IMFA2   :1;   /*    IMFA2     */
      _BYTE   IMFA1   :1;   /*    IMFA1     */
      _BYTE   IMFA0   :1;   /*    IMFA0     */
    }         BIT;          /*              */
  }           TISRA;        /*              */
  union {                   /* TISRB        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   IMIEB2  :1;   /*    IMIEB2    */
      _BYTE   IMIEB1  :1;   /*    IMIEB2    */
      _BYTE   IMIEB0  :1;   /*    IMIEB2    */
      _BYTE           :1;   /*              */
      _BYTE   IMFB2   :1;   /*    IMFB2     */
      _BYTE   IMFB1   :1;   /*    IMFB1     */
      _BYTE   IMFB0   :1;   /*    IMFB0     */
    }         BIT;          /*              */
  }           TISRB;        /*              */
  union {                   /* TISRC        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   OVIE2   :1;   /*    OVIE2     */
      _BYTE   OVIE1   :1;   /*    OVIE2     */
      _BYTE   OVIE0   :1;   /*    OVIE2     */
      _BYTE           :1;   /*              */
      _BYTE   OVF2    :1;   /*    OVF2      */
      _BYTE   OVF1    :1;   /*    OVF1      */
      _BYTE   OVF0    :1;   /*    OVF0      */
    }         BIT;          /*              */
  }           TISRC;        /*              */
};                          /*              */
struct st_itu0 {            /* struct ITU0  */
  union {                   /* TCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKEG    :2;   /*    CKEG      */
      _BYTE   TPSC    :3;   /*    TPSC      */
    }         BIT;          /*              */
  }           TCR;          /*              */
  union {                   /* TIOR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :1;   /*              */
      _BYTE   IOB     :3;   /*    IOB       */
      _BYTE           :1;   /*              */
      _BYTE   IOA     :3;   /*    IOA       */
    }         BIT;          /*              */
  }           TIOR;         /*              */
  _WORD       TCNT;         /* TCNT         */
  _WORD       GRA;          /* GRA          */
  _WORD       GRB;          /* GRB          */
};                          /*              */
union un_wdt {              /* union WDT    */
  struct {                  /* Read  Access */
    union {                 /* TCSR         */
      _BYTE   BYTE;         /*  Byte Access */
      struct {              /*  Bit  Access */
        _BYTE OVF     :1;   /*    OVF       */
        _BYTE WTIT    :1;   /*    WT/IT     */
        _BYTE TME     :1;   /*    TME       */
        _BYTE         :2;   /*              */
        _BYTE CKS     :3;   /*    CKS       */
      }       BIT;          /*              */
    }         TCSR;         /*              */
    _BYTE     TCNT;         /* TCNT         */
    char      wk;           /*              */
    union {                 /* RSTCSR       */
      _BYTE   BYTE;         /*  Byte Access */
      struct {              /*              */
        _BYTE WRST    :1;   /*    WSRT      */
      }       BIT;          /*              */
    }         RSTCSR;       /*              */
  }           READ;         /*              */
  struct {                  /* Write Access */
    _WORD     TCSR;         /* TCSR/TCNT    */
    _WORD     RSTCSR;       /* RSTCSR       */
  } WRITE;                  /*              */
};                          /*              */
struct st_tmr01 {           /* struct TMR01 */
  union {                   /* TCR0         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR0;         /*              */
  union {                   /* TCR1         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR1;         /*              */
  union {                   /* TCSR0        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE   ADTE    :1;   /*    ADTE      */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR0;        /*              */
  union {                   /* TCSR1        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE   ICE     :1;   /*    ICE       */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR1;        /*              */
  _WORD       TCORA;        /* TCORA        */
  _WORD       TCORB;        /* TCORB        */
  _WORD       TCNT;         /* TCNT         */
};                          /*              */
struct st_tmr23 {           /* struct TMR23 */
  union {                   /* TCR2         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR2;         /*              */
  union {                   /* TCR3         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR3;         /*              */
  union {                   /* TCSR2        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE           :1;   /*              */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR2;        /*              */
  union {                   /* TCSR3        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE   ICE     :1;   /*    ICE       */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR3;        /*              */
  _WORD       TCORA;        /* TCORA        */
  _WORD       TCORB;        /* TCORB        */
  _WORD       TCNT;         /* TCNT         */
};                          /*              */
struct st_tmr0 {            /* struct TMR0  */
  union {                   /* TCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR;          /*              */
  char        wk1;          /*              */
  union {                   /* TCSR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE   ADTE    :1;   /*    ADTE      */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR;         /*              */
  char        wk2;          /*              */
  _BYTE       TCORA;        /* TCORA        */
  char        wk3;          /*              */
  _BYTE       TCORB;        /* TCORB        */
  char        wk4;          /*              */
  _BYTE       TCNT;         /* TCNT         */
};                          /*              */
struct st_tmr1 {            /* struct TMR1  */
  union {                   /* TCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR;          /*              */
  char        wk1;          /*              */
  union {                   /* TCSR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE   ICE     :1;   /*    ICE       */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR;         /*              */
  char        wk2;          /*              */
  _BYTE       TCORA;        /* TCORA        */
  char        wk3;          /*              */
  _BYTE       TCORB;        /* TCORB        */
  char        wk4;          /*              */
  _BYTE       TCNT;         /* TCNT         */
};                          /*              */
struct st_tmr2 {            /* struct TMR2  */
  union {                   /* TCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMIEB   :1;   /*    CMIEB     */
      _BYTE   CMIEA   :1;   /*    CMIEA     */
      _BYTE   OVIE    :1;   /*    OVIE      */
      _BYTE   CCLR    :2;   /*    CCLR      */
      _BYTE   CKS     :3;   /*    CKS       */
    }         BIT;          /*              */
  }           TCR;          /*              */
  char        wk1;          /*              */
  union {                   /* TCSR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CMFB    :1;   /*    CMFB      */
      _BYTE   CMFA    :1;   /*    CMFA      */
      _BYTE   OVF     :1;   /*    OVF       */
      _BYTE           :1;   /*              */
      _BYTE   OS      :4;   /*    OS        */
    }         BIT;          /*              */
  }           TCSR;         /*              */
  char        wk2;          /*              */
  _BYTE       TCORA;        /* TCORA        */
  char        wk3;          /*              */
  _BYTE       TCORB;        /* TCORB        */
  char        wk4;          /*              */
  _BYTE       TCNT;         /* TCNT         */
};                          /*              */
union un_dastcr {           /* DASTCR       */
  _BYTE       BYTE;         /*  Byte Access */
  struct {                  /*  Bit  Access */
    _BYTE             :7;   /*              */
    _BYTE     DASTE   :1;   /*    DASTE     */
  }           BIT;          /*              */
};                          /*              */
struct st_da {              /* struct D/A   */
  _BYTE       DADR0;        /* DADR0        */
  _BYTE       DADR1;        /* DADR1        */
  union {                   /* DACR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   DAOE1   :1;   /*    DAOE1     */
      _BYTE   DAOE0   :1;   /*    DAOE0     */
      _BYTE   DAE     :1;   /*    DAE       */
    }         BIT;          /*              */
  }           DACR;         /*              */
};                          /*              */
struct st_tpc {             /* struct TPC   */
  union {                   /* TPMR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   G3NOV   :1;   /*    G3NOV     */
      _BYTE   G2NOV   :1;   /*    G2NOV     */
      _BYTE   G1NOV   :1;   /*    G1NOV     */
      _BYTE   G0NOV   :1;   /*    G0NOV     */
    }         BIT;          /*              */
  }           TPMR;         /*              */
  union {                   /* TPCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   G3CMS   :2;   /*    G3CMS     */
      _BYTE   G2CMS   :2;   /*    G2CMS     */
      _BYTE   G1CMS   :2;   /*    G1CMS     */
      _BYTE   G0CMS   :2;   /*    G0CMS     */
    }         BIT;          /*              */
  }           TPCR;         /*              */
  union {                   /* NDERB        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   B15     :1;   /*    NDER15    */
      _BYTE   B14     :1;   /*    NDER14    */
      _BYTE   B13     :1;   /*    NDER13    */
      _BYTE   B12     :1;   /*    NDER12    */
      _BYTE   B11     :1;   /*    NDER11    */
      _BYTE   B10     :1;   /*    NDER10    */
      _BYTE   B9      :1;   /*    NDER9     */
      _BYTE   B8      :1;   /*    NDER8     */
    }         BIT;          /*              */
  }           NDERB;        /*              */
  union {                   /* NDERA        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   B7      :1;   /*    NDER7     */
      _BYTE   B6      :1;   /*    NDER6     */
      _BYTE   B5      :1;   /*    NDER5     */
      _BYTE   B4      :1;   /*    NDER4     */
      _BYTE   B3      :1;   /*    NDER3     */
      _BYTE   B2      :1;   /*    NDER2     */
      _BYTE   B1      :1;   /*    NDER1     */
      _BYTE   B0      :1;   /*    NDER0     */
    }         BIT;          /*              */
  }           NDERA;        /*              */
  union {                   /* NDRB (H'A4)  */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   B15     :1;   /*    NDR15     */
      _BYTE   B14     :1;   /*    NDR14     */
      _BYTE   B13     :1;   /*    NDR13     */
      _BYTE   B12     :1;   /*    NDR12     */
      _BYTE   B11     :1;   /*    NDR11     */
      _BYTE   B10     :1;   /*    NDR10     */
      _BYTE   B9      :1;   /*    NDR9      */
      _BYTE   B8      :1;   /*    NDR8      */
    }         BIT;          /*              */
  }           NDRB1;        /*              */
  union {                   /* NDRA (H'A5)  */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   B7      :1;   /*    NDR7      */
      _BYTE   B6      :1;   /*    NDR6      */
      _BYTE   B5      :1;   /*    NDR5      */
      _BYTE   B4      :1;   /*    NDR4      */
      _BYTE   B3      :1;   /*    NDR3      */
      _BYTE   B2      :1;   /*    NDR2      */
      _BYTE   B1      :1;   /*    NDR1      */
      _BYTE   B0      :1;   /*    NDR0      */
    }         BIT;          /*              */
  }           NDRA1;        /*              */
  union {                   /* NDRB (H'A6)  */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   B11     :1;   /*    NDR11     */
      _BYTE   B10     :1;   /*    NDR10     */
      _BYTE   B9      :1;   /*    NDR9      */
      _BYTE   B8      :1;   /*    NDR8      */
    }         BIT;          /*              */
  }           NDRB2;        /*              */
  union {                   /* NDRA (H'A7)  */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   B3      :1;   /*    NDR3      */
      _BYTE   B2      :1;   /*    NDR2      */
      _BYTE   B1      :1;   /*    NDR1      */
      _BYTE   B0      :1;   /*    NDR0      */
    }         BIT;          /*              */
  }           NDRA2;        /*              */
};                          /*              */
struct st_sci {             /* struct SCI   */
  union {                   /* SMR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   CA      :1;   /*    C/A       */
      _BYTE   CHR     :1;   /*    CHR       */
      _BYTE   PE      :1;   /*    PE        */
      _BYTE   OE      :1;   /*    O/E       */
      _BYTE   STOP    :1;   /*    STOP      */
      _BYTE   MP      :1;   /*    MP        */
      _BYTE   CKS     :2;   /*    CKS       */
    }         BIT;          /*              */
  }           SMR;          /*              */
  _BYTE       BRR;          /* BRR          */
  union {                   /* SCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   TIE     :1;   /*    TIE       */
      _BYTE   RIE     :1;   /*    RIE       */
      _BYTE   TE      :1;   /*    TE        */
      _BYTE   RE      :1;   /*    RE        */
      _BYTE   MPIE    :1;   /*    MPIE      */
      _BYTE   TEIE    :1;   /*    TEIE      */
      _BYTE   CKE     :2;   /*    CKE       */
    }         BIT;          /*              */
  }           SCR;          /*              */
  _BYTE       TDR;          /* TDR          */
  union {                   /* SSR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   TDRE    :1;   /*    TDRE      */
      _BYTE   RDRF    :1;   /*    RDRF      */
      _BYTE   ORER    :1;   /*    ORER      */
      _BYTE   FER     :1;   /*    FER       */
      _BYTE   PER     :1;   /*    PER       */
      _BYTE   TEND    :1;   /*    TEND      */
      _BYTE   MPB     :1;   /*    MPB       */
      _BYTE   MPBT    :1;   /*    MPBT      */
    }         BIT;          /*              */
  }           SSR;          /*              */
  _BYTE       RDR;          /* RDR          */
  union {                   /* SCMR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   SDIR    :1;   /*    SDIR      */
      _BYTE   SINV    :1;   /*    SINV      */
      _BYTE           :1;   /*              */
      _BYTE   SMIF    :1;   /*    SMIF      */
    }         BIT;          /*              */
  }           SCMR;         /*              */
};                          /*              */
struct st_smci {            /* struct SMCI  */
  union {                   /* SMR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   GM      :1;   /*    GM        */
      _BYTE   CHR     :1;   /*    CHR       */
      _BYTE   PE      :1;   /*    PE        */
      _BYTE   OE      :1;   /*    O/E       */
      _BYTE   STOP    :1;   /*    STOP      */
      _BYTE   MP      :1;   /*    MP        */
      _BYTE   CKS     :2;   /*    CKS       */
    }         BIT;          /*              */
  }           SMR;          /*              */
  _BYTE       BRR;          /* BRR          */
  union {                   /* SCR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   TIE     :1;   /*    TIE       */
      _BYTE   RIE     :1;   /*    RIE       */
      _BYTE   TE      :1;   /*    TE        */
      _BYTE   RE      :1;   /*    RE        */
      _BYTE   MPIE    :1;   /*    MPIE      */
      _BYTE   TEIE    :1;   /*    TEIE      */
      _BYTE   CKE     :2;   /*    CKE       */
    }         BIT;          /*              */
  }           SCR;          /*              */
  _BYTE       TDR;          /* TDR          */
  union {                   /* SSR          */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   TDRE    :1;   /*    TDRE      */
      _BYTE   RDRF    :1;   /*    RDRF      */
      _BYTE   ORER    :1;   /*    ORER      */
      _BYTE   ERS     :1;   /*    ERS       */
      _BYTE   PER     :1;   /*    PER       */
      _BYTE   TEND    :1;   /*    TEND      */
      _BYTE   MPB     :1;   /*    MPB       */
      _BYTE   MPBT    :1;   /*    MPBT      */
    }         BIT;          /*              */
  }           SSR;          /*              */
  _BYTE       RDR;          /* RDR          */
  union {                   /* SCMR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE           :4;   /*              */
      _BYTE   SDIR    :1;   /*    SDIR      */
      _BYTE   SINV    :1;   /*    SINV      */
      _BYTE           :1;   /*              */
      _BYTE   SMIF    :1;   /*    SMIF      */
    }         BIT;          /*              */
  }           SCMR;         /*              */
};                          /*              */
struct st_ad {              /* struct A/D   */
  _WORD       ADDRA;        /* ADDRA        */
  _WORD       ADDRB;        /* ADDRB        */
  _WORD       ADDRC;        /* ADDRC        */
  _WORD       ADDRD;        /* ADDRD        */
  union {                   /* ADCSR        */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   ADF     :1;   /*    ADF       */
      _BYTE   ADIE    :1;   /*    ADIE      */
      _BYTE   ADST    :1;   /*    ADST      */
      _BYTE   SCAN    :1;   /*    SCAN      */
      _BYTE   CKS     :1;   /*    CKS       */
      _BYTE   CH      :3;   /*    CH        */
    }         BIT;          /*              */
  }           ADCSR;        /*              */
  union {                   /* ADCR         */
    _BYTE     BYTE;         /*  Byte Access */
    struct {                /*  Bit  Access */
      _BYTE   TRGE    :1;   /*    TRGE      */
    }         BIT;          /*              */
  }           ADCR;         /*              */
};                          /*              */

#define P1DDR   (*(volatile unsigned char   *)0xFEE000) /* P1DDR */
#define P2DDR   (*(volatile unsigned char   *)0xFEE001) /* P2DDR */
#define P3DDR   (*(volatile unsigned char   *)0xFEE002) /* P3DDR */
#define P4DDR   (*(volatile unsigned char   *)0xFEE003) /* P4DDR */
#define P5DDR   (*(volatile unsigned char   *)0xFEE004) /* P5DDR */
#define P6DDR   (*(volatile unsigned char   *)0xFEE005) /* P6DDR */
#define P8DDR   (*(volatile unsigned char   *)0xFEE007) /* P8DDR */
#define P9DDR   (*(volatile unsigned char   *)0xFEE008) /* P9DDR */
#define PADDR   (*(volatile unsigned char   *)0xFEE009) /* PADDR */
#define PBDDR   (*(volatile unsigned char   *)0xFEE00A) /* PBDDR */
#define P2PCR   (*(volatile union  un_p2pcr *)0xFEE03C) /* P2PCR */
#define P4PCR   (*(volatile union  un_p4pcr *)0xFEE03E) /* P4PCR */
#define P5PCR   (*(volatile union  un_p5pcr *)0xFEE03F) /* P5PCR */
#define P1DR    (*(volatile union  un_p1dr  *)0xFFFFD0) /* P1DR */
#define P2DR    (*(volatile union  un_p2dr  *)0xFFFFD1) /* P2DR */
#define P3DR    (*(volatile union  un_p3dr  *)0xFFFFD2) /* P3DR */
#define P4DR    (*(volatile union  un_p4dr  *)0xFFFFD3) /* P4DR */
#define P5DR    (*(volatile union  un_p5dr  *)0xFFFFD4) /* P5DR */
#define P6DR    (*(volatile union  un_p6dr  *)0xFFFFD5) /* P6DR */
#define P7DR    (*(volatile union  un_p7dr  *)0xFFFFD6) /* P7DR */
#define P8DR    (*(volatile union  un_p8dr  *)0xFFFFD7) /* P8DR */
#define P9DR    (*(volatile union  un_p9dr  *)0xFFFFD8) /* P9DR */
#define PADR    (*(volatile union  un_padr  *)0xFFFFD9) /* PADR */
#define PBDR    (*(volatile union  un_pbdr  *)0xFFFFDA) /* PBDR */
#define MDCR    (*(volatile union  un_mdcr  *)0xFEE011) /* MDCR */
#define SYSCR   (*(volatile union  un_syscr *)0xFEE012) /* SYSCR */
#define DIVCR   (*(volatile union  un_divcr *)0xFEE01B) /* DIVCR */
#define MSTCR   (*(volatile union  un_mstcr *)0xFEE01C) /* MSTCR */
#define BSC     (*(volatile struct st_bsc   *)0xFEE012) /* BSC */
#define INTC    (*(volatile struct st_intc  *)0xFEE014) /* INTC */
#define DMAC0A  (*(volatile struct st_sam   *)0xFFFF20) /* DMAC 0A */
#define DMAC0B  (*(volatile struct st_sam   *)0xFFFF28) /* DMAC 0B */
#define DMAC1A  (*(volatile struct st_sam   *)0xFFFF30) /* DMAC 1A */
#define DMAC1B  (*(volatile struct st_sam   *)0xFFFF38) /* DMAC 1B */
#define DMAC0   (*(volatile struct st_fam   *)0xFFFF20) /* DMAC 0 */
#define DMAC1   (*(volatile struct st_fam   *)0xFFFF30) /* DMAC 1 */
#define FLASH   (*(volatile struct st_flash *)0xFEE076) /* FLASH */
#define ITU     (*(volatile struct st_itu   *)0xFFFF60) /* ITU */
#define ITU0    (*(volatile struct st_itu0  *)0xFFFF68) /* ITU0 */
#define ITU1    (*(volatile struct st_itu0  *)0xFFFF70) /* ITU1 */
#define ITU2    (*(volatile struct st_itu0  *)0xFFFF78) /* ITU2 */
#define WDT     (*(volatile union  un_wdt   *)0xFFFF8C) /* WDT */
#define TMR01   (*(volatile struct st_tmr01 *)0xFFFF80) /* TMR01 */
#define TMR23   (*(volatile struct st_tmr23 *)0xFFFF90) /* TMR23 */
#define TMR0    (*(volatile struct st_tmr0  *)0xFFFF80) /* TMR0 */
#define TMR1    (*(volatile struct st_tmr1  *)0xFFFF81) /* TMR1 */
#define TMR2    (*(volatile struct st_tmr2  *)0xFFFF90) /* TMR2 */
#define TMR3    (*(volatile struct st_tmr1  *)0xFFFF91) /* TMR3 */
#define DASTCR  (*(volatile union  un_dastcr*)0xFEE01A) /* DASTCR */
#define DA      (*(volatile struct st_da    *)0xFFFF9C) /* D/A */
#define TPC     (*(volatile struct st_tpc   *)0xFFFFA0) /* TPC */
#define SCI0    (*(volatile struct st_sci   *)0xFFFFB0) /* SCI0 */
#define SCI1    (*(volatile struct st_sci   *)0xFFFFB8) /* SCI1 */
#define SCI2    (*(volatile struct st_sci   *)0xFFFFC0) /* SCI2 */
#define SMCI0   (*(volatile struct st_smci  *)0xFFFFB0) /* SMCI0 */
#define SMCI1   (*(volatile struct st_smci  *)0xFFFFB8) /* SMCI1 */
#define SMCI2   (*(volatile struct st_smci  *)0xFFFFC0) /* SMCI2 */
#define AD      (*(volatile struct st_ad    *)0xFFFFE0) /* A/D */


#define DI  asm("orc.b #0xc0,ccr")
#define EI  asm("andc.b #0x3f,ccr")

/*--------------------------------------------------------------*/
/* 割り込み関数プロトタイプ宣言                                 */
/*--------------------------------------------------------------*/
// NMI割り込み
void int_nmi (void)     asm ("_int_adi")    __attribute__((interrupt_handler));
/*
// トラップ命令
void int_trap0 (void) __attribute__((interrupt_handler));
void int_trap1 (void) __attribute__((interrupt_handler));
void int_trap2 (void) __attribute__((interrupt_handler));
void int_trap3 (void) __attribute__((interrupt_handler));
*/
// IRQ0～5外部割り込み
void int_irq0 (void)    asm ("_int_irq0")   __attribute__((interrupt_handler));
void int_irq1 (void)    asm ("_int_irq1")   __attribute__((interrupt_handler));
void int_irq2 (void)    asm ("_int_irq2")   __attribute__((interrupt_handler));
void int_irq3 (void)    asm ("_int_irq3")   __attribute__((interrupt_handler));
void int_irq4 (void)    asm ("_int_irq4")   __attribute__((interrupt_handler));
void int_irq5 (void)    asm ("_int_irq5")   __attribute__((interrupt_handler));
// ウォッチドッグタイマ (インターバルタイマ)
void int_wovi (void)    asm ("_int_wovi")   __attribute__((interrupt_handler));
// リフレッシュコントローラ (コンペアマッチ)
void int_cmi (void)     asm ("_int_cmi")    __attribute__((interrupt_handler));

// A/D (A/D完了)
void int_adi (void)     asm ("_int_adi")    __attribute__((interrupt_handler));

// ITU0 (コンペアマッチ/インプットキャプチャA0)
void int_imia0 (void)   asm ("_int_imia0")  __attribute__((interrupt_handler));
// ITU0 (コンペアマッチ/インプットキャプチャB0)
void int_imib0 (void)   asm ("_int_imib0")  __attribute__((interrupt_handler));
// ITU0 (オーバフロー0)
void int_ovi0 (void)    asm ("_int_ovi0")   __attribute__((interrupt_handler));

// ITU1 (コンペアマッチ/インプットキャプチャA1)
void int_imia1 (void)   asm ("_int_imia1")  __attribute__((interrupt_handler));
// ITU1 (コンペアマッチ/インプットキャプチャB1)
void int_imib1 (void)   asm ("_int_imib1")  __attribute__((interrupt_handler));
// ITU1 (オーバフロー1)
void int_ovi1 (void)    asm ("_int_ovi1")   __attribute__((interrupt_handler));

// ITU2 (コンペアマッチ/インプットキャプチャA2)
void int_imia2 (void)   asm ("_int_imia2")  __attribute__((interrupt_handler));
// ITU2 (コンペアマッチ/インプットキャプチャB2)
void int_imib2 (void)   asm ("_int_imib2")  __attribute__((interrupt_handler));
// ITU2 (オーバフロー2)
void int_ovi2 (void)    asm ("_int_ovi2")   __attribute__((interrupt_handler));

// TMR0 (コンペアマッチA0)
void int_cmia0 (void)   asm ("_int_cmia0")  __attribute__((interrupt_handler));
// TMR0 (コンペアマッチB0)
void int_cmib0 (void)   asm ("_int_cmib0")  __attribute__((interrupt_handler));
// TMR1 (コンペアマッチA1/B1)
void int_cmiab1 (void)  asm ("_int_cmiab1") __attribute__((interrupt_handler));
// TMR0,1 (オーバフロー0/1)
void int_tovi01 (void)  asm ("_int_tovi01") __attribute__((interrupt_handler));

// TMR2 (コンペアマッチA2)
void int_cmia2 (void)   asm ("_int_cmia2")  __attribute__((interrupt_handler));
// TMR2 (コンペアマッチB2)
void int_cmib2 (void)   asm ("_int_cmib2")  __attribute__((interrupt_handler));
// TMR3 (コンペアマッチA3/B3)
void int_cmiab3 (void)  asm ("_int_cmiab3") __attribute__((interrupt_handler));
// TMR2,3 (オーバフロー2/3)
void int_tovi23 (void)  asm ("_int_tovi23") __attribute__((interrupt_handler));

// DMAC0 (DMAC0/DMAC0A転送完了)
void int_dend0a (void)  asm ("_int_dend0a") __attribute__((interrupt_handler));
// DMAC0 (DMAC0B転送完了)
void int_dend0b (void)  asm ("_int_dend0b") __attribute__((interrupt_handler));
// DMAC1 (DMAC1/DMAC1A転送完了)
void int_dend1a (void)  asm ("_int_dend1a") __attribute__((interrupt_handler));
// DMAC1 (DMAC1B転送完了)
void int_dend1b (void)  asm ("_int_dend1b") __attribute__((interrupt_handler));

// SCI0 (受信エラー0)
void int_eri0 (void)    asm ("_int_eri0")   __attribute__((interrupt_handler));
// SCI0 (受信データフル0)
void int_rxi0 (void)    asm ("_int_rxi0")   __attribute__((interrupt_handler));
// SCI0 (送信データエンプティ0)
void int_txi0 (void)    asm ("_int_txi0")   __attribute__((interrupt_handler));
// SCI0 (送信終了0)
void int_tei0 (void)    asm ("_int_tei0")   __attribute__((interrupt_handler));

// SCI1 (受信エラー1)
void int_eri1 (void)    asm ("_int_eri1")   __attribute__((interrupt_handler));
// SCI1 (受信データフル1)
void int_rxi1 (void)    asm ("_int_rxi1")   __attribute__((interrupt_handler));
// SCI1 (送信データエンプティ1)
void int_txi1 (void)    asm ("_int_txi1")   __attribute__((interrupt_handler));
// SCI1 (送信終了1)
void int_tei1 (void)    asm ("_int_tei1")   __attribute__((interrupt_handler));

// SCI2 (受信エラー2)
void int_eri2 (void)    asm ("_int_eri2")   __attribute__((interrupt_handler));
// SCI2 (受信データフル2)
void int_rxi2 (void)    asm ("_int_rxi2")   __attribute__((interrupt_handler));
// SCI2 (送信データエンプティ2)
void int_txi2 (void)    asm ("_int_txi2")   __attribute__((interrupt_handler));
// SCI2 (送信終了2)
void int_tei2 (void)    asm ("_int_tei2")   __attribute__((interrupt_handler));

/*--------------------------------------------------------------*/
/* シリアル通信関数(FIFOバッファ付)                             */
/*--------------------------------------------------------------*/
/*
typedef enum { // ボーレート設定リスト at 20MHz
  br4800  = 129,
  br9600  = 64,
  br19200 = 32,
  br38400 = 15,
  br57600 = 10
} TBaudRate;
*/

typedef enum { // ボーレート設定リスト at 25MHz
  br4800  = 162,
  br9600  = 80,
  br19200 = 40,
  br38400 = 19,
  br57600 = 13,
  br115200 = 6
} TBaudRate;

// SCI初期化
extern void SCI0_INIT (TBaudRate, char *, short, char *, short);
extern void SCI1_INIT (TBaudRate, char *, short, char *, short);
extern void SCI2_INIT (TBaudRate, char *, short, char *, short);
// 1文字送信
extern void SCI0_OUT_DATA (char);
extern void SCI1_OUT_DATA (char);
extern void SCI2_OUT_DATA (char);
// 文字列送信
extern void SCI0_OUT_STRING (char *);
extern void SCI1_OUT_STRING (char *);
extern void SCI2_OUT_STRING (char *);
// バイナリ列送信
extern void SCI0_OUT_STRINGB (char *, short);
extern void SCI1_OUT_STRINGB (char *, short);
extern void SCI2_OUT_STRINGB (char *, short);
// 受信バッファチェック
extern short SCI0_IN_DATA_CHECK (void);
extern short SCI1_IN_DATA_CHECK (void);
extern short SCI2_IN_DATA_CHECK (void);
// 送信バッファチェック
extern short SCI0_OUT_DATA_CHECK (void);
extern short SCI1_OUT_DATA_CHECK (void);
extern short SCI2_OUT_DATA_CHECK (void);
// 受信バッファクリア
extern void SCI0_IN_DATA_CLEAR (void);
extern void SCI1_IN_DATA_CLEAR (void);
extern void SCI2_IN_DATA_CLEAR (void);
// 送信バッファクリア
extern void SCI0_OUT_DATA_CLEAR (void);
extern void SCI1_OUT_DATA_CLEAR (void);
extern void SCI2_OUT_DATA_CLEAR (void);
// 1文字受信
extern char SCI0_IN_DATA (void);
extern char SCI1_IN_DATA (void);
extern char SCI2_IN_DATA (void);
// 文字列送信
extern short SCI0_IN_STRING (char *, short);
extern short SCI1_IN_STRING (char *, short);
extern short SCI2_IN_STRING (char *, short);
// 簡易フォーマット出力
extern int SCI0_PRINTF (const char *, ...);
extern int SCI1_PRINTF (const char *, ...);
extern int SCI2_PRINTF (const char *, ...);

/*--------------------------------------------------------------*/
/* インライン命令                                               */
/*--------------------------------------------------------------*/
extern __inline__ void sleep (void) __attribute__ ((always_inline));
extern __inline__ void nop (void) __attribute__ ((always_inline));
extern __inline__ void set_ccr (unsigned char mask) __attribute__ ((always_inline));
extern __inline__ unsigned char get_ccr (void) __attribute__ ((always_inline));
extern __inline__ void *get_sp (void) __attribute__ ((always_inline));

extern __inline__ void sleep (void)
{
  asm ("sleep");
}

extern __inline__ void nop (void)
{
  asm ("nop");
}

extern __inline__ void set_ccr (unsigned char mask)
{
  asm ("ldc %0l,ccr"::"r"(mask));
}

extern __inline__ unsigned char get_ccr (void)
{
  unsigned char value;
  asm ("stc ccr,%0l":"=g"(value):);
  return(value);
}

#ifdef __cplusplus
}
#endif

#endif /* _3069_H_ */
