#define _ASMLANGUAGE
#include <rtos.h>
#include <arch/asm.h>

	/* Internals */
	.globl	GTEXT(sysLoadGdt)
	.globl	GTEXT(sysInByte)
	.globl	GTEXT(sysInWord)
	.globl	GTEXT(sysInLong)
	.globl	GTEXT(sysOutByte)
	.globl	GTEXT(sysOutWord)
	.globl	GTEXT(sysOutLong)
	.globl	GTEXT(sysInWordString)
	.globl	GTEXT(sysInLongString)
	.globl	GTEXT(sysOutLongString)
	.globl	GTEXT(sysOutWordString)
	.globl	GTEXT(sysWait)
	.globl	GTEXT(sysReboot)

	.text
	.balign	16

/*****************************************************************************
* sysLoadGdt - Routine to be called by C program to setup GDT Segment table
*
* RETURNS:   N/A
* PROTOTYPE: void sysLoadGdt(GDT *baseAddr)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysLoadGdt)

	movl    SP_ARG1(%esp),%eax
        lgdt    (%eax)
	movw	$0x0010,%ax	/* Offset in GDT to data segment */
	movw	%ax,%dx
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	jmp	$0x08,$flushCs

flushCs:
	ret

/*****************************************************************************
* sysInByte - Read one byte from I/O space
*
* RETURNS:   Byte data from I/O port
* PROTOTYPE: char sysInByte (int address)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysInByte)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inb	%dx,%al
	jmp	sysInByte0
sysInByte0:
	ret

/*****************************************************************************
* sysInWord - Read one word from I/O space
*
* RETURNS:   Word data from I/O port
* PROTOTYPE: short sysInWord (int address)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysInWord)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inw	%dx,%ax
	jmp	sysInWord0
sysInWord0:
	ret

/*****************************************************************************
* sysInLong - Read one long-word from I/O space
*
* RETURNS:   Long-word data from I/O port
* PROTOTYPE: short sysInLong (int address)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysInLong)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inl	%dx,%eax
	jmp	sysInLong0
sysInLong0:
	ret

/*****************************************************************************
* sysOutByte - Output one byte to I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysOutByte (int address, char data)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysOutByte)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outb	%al,%dx
	jmp	sysOutByte0
sysOutByte0:
	ret

/*****************************************************************************
* sysOutWord - Output one word to I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysOutWord (int address, short data)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysOutWord)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outw	%ax,%dx
	jmp	sysOutWord0
sysOutWord0:
	ret

/*****************************************************************************
* sysOutLong - Output one long-word to I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysOutLong (int address, long data)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysOutLong)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outl	%eax,%dx
	jmp	sysOutLong0
sysOutLong0:
	ret

/*****************************************************************************
* sysInWordString - Input word string from I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysInWordString (int port, short *address, int count)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysInWordString)
	pushl	%edi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%edi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	insw	%dx,(%edi)
	movl	%edi,%eax
	popl	%edi
	ret

/*****************************************************************************
* sysInLongString - Input long string from I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysInLongString (int port, long *address, int count)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysInLongString)
	pushl	%edi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%edi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	insl	%dx,(%edi)
	movl	%edi,%eax
	popl	%edi
	ret

/*****************************************************************************
* sysOutWordString - Output word string to I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysOutWordString (int port, short *address, int count)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysOutWordString)
	pushl	%esi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%esi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	outsw	(%esi),%dx
	movl	%esi,%eax
	popl	%esi
	ret

/*****************************************************************************
* sysOutLongString - Output word string to I/O space
*
* RETURNS:   N/A
* PROTOTYPE: void sysOutLongString (int port, long *address, int count)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysOutLongString)
	pushl	%esi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%esi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	outsl	(%esi),%dx
	movl	%esi,%eax
	popl	%esi
	ret

/*****************************************************************************
* sysWait - Wait for input buffer to become empty
*
* RETURNS:   N/A
* PROTOTYPE: void sysWait(void)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysWait)
	xorl	%ecx,%ecx
sysWait0:
	movl	$0x64,%edx		/* Check if its ready to write */
	inb	%dx,%al
	andb	$2,%al
	loopnz	sysWait0
	ret

/*****************************************************************************
* sysReboot - Reset system
*
* RETURNS:   N/A
* PROTOTYPE: void sysReboot(void)
*
*****************************************************************************/

	.balign	16,0x90

FUNC_LABEL(sysReboot)
	movl	$0,%eax
	lgdt	(%eax)			/* Messup GDT */
	ret
