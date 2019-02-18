#include "util/historyLog.h"
#include "os/symbol.h"
#include <stdarg.h>
#include <stdio.h>
#include <os/symLib.h>
#include <string.h>

extern SYMTAB_ID sysSymTable;
#define NULL ((void *)0)

unsigned int historyInitialized = 0;
unsigned int historySize = 100;

typedef struct
{
    char data[256];
} historyData_t;

historyData_t historyData[100];
unsigned int historyCount = 0;

void historyInit(void)
{
    memset(historyData, 0, 100 * sizeof(historyData_t));
    historyCount = 0;
}

void addToHistory(char *argData)
{
    if (historyCount > 100)
    {
        printf("History full\n");
        return;
    }

    memcpy(historyData[historyCount].data, argData, 256);
    historyCount++;
}

void historyDump(void)
{
    int i = 0;

    for (i = 0; i < historyCount; i++)
    {
        printf("%s\n", historyData[i].data);
    }
}

void historyLogStr(void *argFunction, const char *argFunctionName, const char *argFormat, ...)
{
    char buffer[256] = {0};
    char data[256] = {0};
    va_list args = NULL;

    if (historyInitialized == 0)
    {
        historyInit();
        historyInitialized = 1;
    }

    va_start(args, argFormat);
    vsnprintf(buffer, 256, argFormat, args);

    //printf("0x%x-%s: %s\n", argFunction, argFunctionName, buffer);
    snprintf(data, 256, "0x%x-%s: %s", argFunction, argFunctionName, buffer);
    addToHistory(data);

    va_end (args);
}

void historyLog(void *argFunction, const char *argFormat, ...)
{
    char buffer[256] = {0};
    char data[256] = {0};
    char name[256] = {0};
    SYMBOL_ID symId;
    va_list args = NULL;
    STATUS status = OK;

    if (historyInitialized == 0)
    {
        historyInit();
        historyInitialized = 1;
    }
    va_start(args, argFormat);
    vsnprintf(buffer, 256, argFormat, args);

    status = symFindSymbol(sysSymTable, NULL, (void *)argFunction, SYM_MASK_NONE, SYM_MASK_NONE, &symId);

    if (status == OK)
    {
        /* Get symbol name */
        status = symNameGet(symId, (char*)&name);
        printf("0x%x, %s\n", symId, name);
        if (status == ERROR)
        {
            printf("Failed to find symbol name\n");
            snprintf(name, 256, "UNKNOWN");
        }
    }
    else
    {
        snprintf(name, 256, "UNKNOWN");
        printf("Failed to find symbol\n");
    }

    //printf("0x%x-%s: %s\n", argFunction, name, buffer);
    snprintf(data, 256, "0x%x-%s: %s", argFunction, name, buffer);
    addToHistory(data);

    va_end (args);
}
