#include <a.out.h>
#include <stdlib.h>
#include <types/rtosTypes.h>
#include <os/private/symbolP.h>
#include <os/symLib.h>
#include <stdio.h>

extern SYMTAB_ID sysSymTable;
extern int if_attach();
extern int sysReboot();

int symTblAllSize = 0;
SYMBOL symTblAll[] =
{
    {NULL, "_if_attach", if_attach, 0, N_TEXT | N_EXT},
    {NULL, "_sysReboot", sysReboot, 0, N_TEXT | N_EXT}
};

void symTblAllInit(void)
{
    int i = 0;

    /* Get symbol table size */
    symTblAllSize = NELEMENTS(symTblAll);

    printf("Adding %d symbols for symTblAll.\n", symTblAllSize);

    for (i = 0; i < symTblAllSize; i++)
    {
        symTableAdd(sysSymTable, &symTblAll[i]);
    }
}
