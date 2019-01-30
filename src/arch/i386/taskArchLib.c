#include <stdlib.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <arch/regs.h>
#include <arch/taskArchLib.h>

IMPORT void kernTaskEntry(void);

REG_INDEX taskRegName[] = 
{
    {"edi", G_REG_OFFSET(0)},
    {"esi", G_REG_OFFSET(1)},
    {"ebp", G_REG_OFFSET(2)},
    {"esp", G_REG_OFFSET(3)},
    {"ebx", G_REG_OFFSET(4)},
    {"edx", G_REG_OFFSET(5)},
    {"ecx", G_REG_OFFSET(6)},
    {"eax", G_REG_OFFSET(7)},
    {"eflags", SR_OFFSET},
    {"pc", PC_OFFSET},
    {NULL, 0}
};

/*******************************************************************************
* taskRegsInit - Initialize architecture depedant tcb data
*
* RETURNS: N/A
*******************************************************************************/
void taskRegsInit(TCB *pTcb, char *pStackBase)
{
    pTcb->regs.eflags = TASK_INITIAL_FLAGS;
    pTcb->regs.pc = (INSTR *) kernTaskEntry;

    pTcb->regs.edi = 0;
    pTcb->regs.esi = 0;
    pTcb->regs.ebp = 0;
    pTcb->regs.ebx = 0;
    pTcb->regs.edx = 0;
    pTcb->regs.ecx = 0;
    pTcb->regs.eax = 0;

    /* Setup initial stack */
    pTcb->regs.esp = (u_int32_t) (pStackBase - (MAX_TASK_ARGS * sizeof(u_int32_t)) );
}

/*******************************************************************************
* taskRetValueSet - Set task return value
*
* RETURNS: N/A
*******************************************************************************/
void taskRetValueSet(TCB_ID pTcb, int val)
{
    pTcb->regs.eax = val;
}

/*******************************************************************************
* taskArgSet - Setup task arguments
*
* RETURNS: N/A
*******************************************************************************/
void taskArgSet(TCB_ID pTcb, char *pStackBase, ARG args[])
{
    int i;
    ARG *sp;

    sp = (ARG *) pStackBase;

    /* Push args on stack */
    for (i = MAX_TASK_ARGS - 1; i >= 0; --i)
        *--sp = (ARG) args[i];
}

/*******************************************************************************
* taskArgGet - Read task arguments
*
* RETURNS: N/A
*******************************************************************************/
void taskArgGet(TCB_ID pTcb, char *pStackBase, ARG args[])
{
    int i;
    ARG *sp;

    sp = (ARG *) pStackBase;

    /* Push args on stack */
    for (i = MAX_TASK_ARGS - 1; i >= 0; --i)
        args[i] = *--sp;
}

