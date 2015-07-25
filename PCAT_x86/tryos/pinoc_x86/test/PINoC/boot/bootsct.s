	.file	"bootsct.cpp"
#APP
	.code16gcc
	.text						
	movw	$0x0400, 	%ax		
	movw	%ax,		%ss		
	movl	$0x0,		%eax	
	movl	%eax,		%esp	
	movl	%eax,		%ebp	
	
#NO_APP
	.text
.globl _Z6main16v
	.type	_Z6main16v, @function
_Z6main16v:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	call	_Z9io_load16v
#APP
# 32 "src/bootsct.cpp" 1
	popl	%ebx
# 0 "" 2
# 38 "src/bootsct.cpp" 1
	jmp	0x5000
# 0 "" 2
#NO_APP
	leave
	ret
	.size	_Z6main16v, .-_Z6main16v
.globl _Z9io_load16v
	.type	_Z9io_load16v, @function
_Z9io_load16v:
	pushl	%ebp
	movl	%esp, %ebp
#APP
# 89 "src/bootsct.cpp" 1
	movw	$0x500,	%di		
	movw	%di,		%es		
	movw    $0x00,     %bx  
	movb    $0x00,     %ch  
	movb    $0x00,     %dh  
	movb    $0x02,     %cl  
	movw    $0x00,     %si  
	io_load_retry:          
	movb    $0x02,     %ah  
	movb    $0x08,     %al  
	movb    $0x00,     %dl  
	int     $0x13           
	jnc     io_load_success	
	add     $0x01,     %si  
	cmp     $0x05,     %si  
	jae     io_load_error   
	movb    $0x00,     %ah  
	movb    $0x00,     %dl  
	int     $0x13           
	jmp     io_load_retry   
	
# 0 "" 2
# 91 "src/bootsct.cpp" 1
	io_load_error:
# 0 "" 2
# 96 "src/bootsct.cpp" 1
	io_load_success:
# 0 "" 2
#NO_APP
	popl	%ebp
	ret
	.size	_Z9io_load16v, .-_Z9io_load16v
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
