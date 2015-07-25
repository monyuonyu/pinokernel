	.file	"bootld.cpp"
	.text
.globl memmove
	.type	memmove, @function
memmove:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L2
.L3:
	movl	-12(%ebp), %eax
	addl	-4(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	-8(%ebp), %edx
	movzbl	(%edx), %edx
	movb	%dl, (%eax)
	addl	$1, -12(%ebp)
.L2:
	movl	-12(%ebp), %eax
	cmpl	16(%ebp), %eax
	setb	%al
	testb	%al, %al
	jne	.L3
	leave
	ret
	.size	memmove, .-memmove
.globl memset
	.type	memset, @function
memset:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L6
.L7:
	movl	-8(%ebp), %eax
	addl	-4(%ebp), %eax
	movl	12(%ebp), %edx
	movb	%dl, (%eax)
	addl	$1, -8(%ebp)
.L6:
	movl	-8(%ebp), %eax
	cmpl	16(%ebp), %eax
	setb	%al
	testb	%al, %al
	jne	.L7
	leave
	ret
	.size	memset, .-memset
#APP
	.code16gcc
#NO_APP
.globl _Z6main16v
	.type	_Z6main16v, @function
_Z6main16v:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	leal	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN9lowcall1610io_debug16Ev
	leal	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN9lowcall168vbe_modeEv
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L10
	leal	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN9lowcall168vga_modeEv
.L10:
	call	_Z10io_A20GATEv
	movl	$25856, %eax
	movb	$-12, (%eax)
	movl	$25857, %eax
	movw	$-533, (%eax)
	movl	$24576, %eax
	movb	$-12, (%eax)
	movl	$24577, %eax
	movw	$-533, (%eax)
	movl	$36872, %eax
	movl	$1610678271, (%eax)
	movl	$13605376, 4(%eax)
	movl	$36880, %eax
	movl	$1694564351, (%eax)
	movl	$13605376, 4(%eax)
	movl	$36864, 8(%esp)
	movl	$255, 4(%esp)
	leal	-9(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18segment_descriptor6LGDT16Etm
	movl	$34048, 8(%esp)
	movl	$255, 4(%esp)
	leal	-9(%ebp), %eax
	movl	%eax, (%esp)
	call	_ZN18segment_descriptor6LIDT16Etm
	call	_Z14io_PROTECTMODEv
	leave
	ret
	.size	_Z6main16v, .-_Z6main16v
#APP
	waitkeybord:           
	in    $0x64,    %al    
	and   $0x02,    %al    
	jnz   waitkeybord      
	ret                    
	
	io_A20GATE:				  
	movb    $0xff,    %al    
	outb    %al,      $0x21  
	nop                      
	outb    %al,      $0xa1  
	cli                      
	call    waitkeybord    
	movb    $0xd1,   %al    
	outb    %al,     $0x64  
	call    waitkeybord    
	movb    $0xdf,   %al    
	outb    %al,     $0x60  
	call    waitkeybord    
	ret                     
	
	io_PROTECTMODE:			
	movw	$0x6000,	%ax		
	movw	%ax,		%ds		
	movw	%ax,		%es		
	movw	%ax,		%fs		
	movw	%ax,		%gs		
	movl    %cr0,      %eax	
	orl     $0x01,     %eax	
	movl    %eax,      %cr0	
	jmp     _piplineflash		
	_piplineflash:				
	ljmp	$0x00008,	$0x00000000		
	
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
