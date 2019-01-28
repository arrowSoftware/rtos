/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2008 - 2009 Surplus Users Ham Society
*
*   Real rtos is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 2.1 of the License, or
*   (at your option) any later version.
*
*   Real rtos is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Real rtos.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/* usrConfigMin.c - Minimal usrInit() user code */

#include "../dreamcast/config.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/memPartitionLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskLib.h>
#include <rtos/kernLib.h>
#include <arch/intArchLib.h>

/* Imports */
IMPORT PARTITION_ID memSysPartitionId;
IMPORT TCB_ID taskIdCurrent;

void fun1(void)
{
  puts(".");
}

void fun2(void)
{
  puts(":");
}

void fun3(void)
{
  puts("|");
}

void fun4(void)
{
  static int count = 0;

  switch(count) {
    case 0 : puts("A"); count++; break;
    case 1 : puts("B"); count++; break;
    case 2 : puts("C"); count++; break;
    case 3 : puts("D"); count++; break;
    case 4 : puts("E"); count++; break;
    case 5 : puts("F"); count++; break;
    case 6 : puts("G"); count++; break;
    case 7 : puts("H"); count++; break;
    case 8 : puts("I"); count++; break;
    case 9 : puts("J"); count++; break;
    default: count = 0; break;
  }
}

#include <arch/iv.h>
IMPORT void kernVMATable();
IMPORT u_int32_t intEvtAdrs;
IMPORT void excStub(void);
IMPORT u_int32_t excStubSize;
IMPORT void tlbStub(void);
IMPORT u_int32_t tlbStubSize;
IMPORT void intStub(void);
IMPORT u_int32_t intStubSize;
void printStub(int addr, int size)
{
  char *pFunc = (char *) addr;
  int i;
  unsigned result;

  for (i = 0; i < size; i++) {
    result = 0;
    result = (int) pFunc[i];
    logStringAndAddress("asm: ",
		        (unsigned) result,
		        LOG_ALL,
		        LOG_LEVEL_FULL);
  }
}

void printInfo(void)
{
  u_int32_t virtAddr, virtBase, stubAddr, val;
  char pTmp[255];
  virtAddr = (u_int32_t) intVecBaseSet;
  virtBase = ((u_int32_t) kernVMATable & 0x1fffffff) |
	     (virtAddr & 0xe0000000);

  /* Check INTEVT */
  stubAddr = virtBase + INT_EVT_ADRS_OFFSET;
  val = *(u_int32_t *) stubAddr;
  logStringAndAddress("intEvtAdrs",
		      (unsigned) intEvtAdrs,
		      LOG_ALL,
		      LOG_LEVEL_FULL);
  logStringAndAddress("INT_EVT_ADRS",
		      (unsigned) val,
		      LOG_ALL,
		      LOG_LEVEL_FULL);

  /* Check intStub */
  printStub((int) intStub, intStubSize);
  puts("\n");
  stubAddr = virtBase + INT_STUB_OFFSET;
  memcpy(pTmp, (char *) stubAddr, intStubSize);
  printStub((int) pTmp, intStubSize);
}

TCB_ID pTcb;
int testTask(void)
{
  short k;
  int i,j;
  puts("\nTest task...\n");
  //printInfo();
  //taskIdCurrent = NULL;
  for (i = 0; i < 2000; i++) {
    fun1();
    //fun2();
    //fun3();
    //fun4();
    for (j = 0; j < 0x6fffff; j++);
  }

  return 0;
}

void usrInit(void)
{
  int i, j;
  FUNCPTR func_p;
  sysHwInit0();
  sysHwInit1();
  setLogFlags(LOG_ARCH_LIB);
  setLogLevel(LOG_LEVEL_ERROR|LOG_LEVEL_WARNING|LOG_LEVEL_INFO|LOG_LEVEL_CALLS);

  if (memPartitionLibInit((char *) 0x0c010000, 0x00f10000) != OK) {
    puts("Mem error\n");
    return;
  }
  if (taskLibInit() != OK) {
    puts("Task error\n");
    return;
  }

  kernInit(testTask);

  pTcb = taskCreate("testTask", 0, 0,
		     1024, testTask,
		     0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  if (pTcb == NULL) {
    puts("Allocation failed!\n");
    return;
  }

  intLock();
  rtosResume(pTcb);
  taskIdCurrent = pTcb;
  timer_init();
  timer_ms_enable();
  intEnable();

  func_p = (FUNCPTR) pTcb->regs.pc;
  (*func_p)();

  for (i = 0; i < 25; i++) { for (j = 0; j < 0xfffff; j++); puts("."); }

  puts("Shutdown...");
  irq_shutdown();
  puts("Down!");
}

