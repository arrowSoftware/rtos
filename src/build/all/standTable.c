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

/* standTable.c - Standalone symbol table */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <setjmp.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <signal.h>
#include <a.out.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <os/symShow.h>
#include <os/envLib.h>
#include <os/envShow.h>
#include <usr/usrLib.h>
#include <usr/usrFsLib.h>
#include <util/classLib.h>
#include <util/classShow.h>
#include <util/objLib.h>
#include <util/cksumLib.h>
#include <util/listLib.h>
#include <util/dllLib.h>
#include <util/sllLib.h>
#include <util/qLib.h>
#include <util/hashLib.h>
#include <util/ringLib.h>
#include <rtos/errnoLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskInfo.h>
#include <rtos/taskShow.h>
#include <rtos/taskVarLib.h>
#include <rtos/memPartLib.h>
#include <rtos/memLib.h>
#include <rtos/memShow.h>
#include <rtos/eventLib.h>
#include <rtos/eventShow.h>
#include <rtos/semLib.h>
#include <rtos/semShow.h>
#include <rtos/semEvLib.h>
#include <rtos/msgQLib.h>
#include <rtos/msgQEvLib.h>
#include <rtos/msgQShow.h>
#include <rtos/ffsLib.h>
#include <rtos/kernLib.h>
#include <rtos/kernQLib.h>
#include <rtos/kernTickLib.h>
#include <os/vmLib.h>
#include <util/hashLib.h>
#include <io/cbioLib.h>
#include <io/cbioShow.h>
#include <rtos/taskHookLib.h>
#include <rtos/taskHookShow.h>
#include <rtos/wdLib.h>
#include <rtos/wdShow.h>
#include <tools/moduleLib.h>
#include <tools/moduleShow.h>
#include <net/mbuf.h>
#include <net/netLib.h>
#include <net/netShow.h>
#include <ugl/ugl.h>
#include <netinet/inetLib.h>

/* Imports */
IMPORT PART_ID memHeapPartId;

/* Gloabals */
int standTableSize = 0;
SYMBOL standTable[] = {

  /* usrLib */
  {NULL, "_show", show, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdFigure", taskIdFigure, 0, N_TEXT | N_EXT},
  {NULL, "_help", help, 0, N_TEXT | N_EXT},
  {NULL, "_i", i, 0, N_TEXT | N_EXT},
  {NULL, "_ti", ti, 0, N_TEXT | N_EXT},
  {NULL, "_sp", sp, 0, N_TEXT | N_EXT},
  {NULL, "_ts", ts, 0, N_TEXT | N_EXT},
  {NULL, "_tr", tr, 0, N_TEXT | N_EXT},
  {NULL, "_td", td, 0, N_TEXT | N_EXT},
  {NULL, "_d", d, 0, N_TEXT | N_EXT},
  {NULL, "_m", m, 0, N_TEXT | N_EXT},
  {NULL, "_pc", pc, 0, N_TEXT | N_EXT},
  {NULL, "_devs", devs, 0, N_TEXT | N_EXT},
  {NULL, "_ld", ld, 0, N_TEXT | N_EXT},
  {NULL, "_lkup", lkup, 0, N_TEXT | N_EXT},
  {NULL, "_checkStack", checkStack, 0, N_TEXT | N_EXT},
  {NULL, "_periodRun", periodRun, 0, N_TEXT | N_EXT},
  {NULL, "_period", period, 0, N_TEXT | N_EXT},
  {NULL, "_repeatRun", repeatRun, 0, N_TEXT | N_EXT},
  {NULL, "_repeat", repeat, 0, N_TEXT | N_EXT},
  {NULL, "_version", version, 0, N_TEXT | N_EXT},
  {NULL, "_printLogo", printLogo, 0, N_TEXT | N_EXT},

  /* usrFsLib */
  {NULL, "_ioHelp", ioHelp, 0, N_TEXT | N_EXT},
  {NULL, "_cd", cd, 0, N_TEXT | N_EXT},
  {NULL, "_pwd", pwd, 0, N_TEXT | N_EXT},
  {NULL, "_ls", ls, 0, N_TEXT | N_EXT},
  {NULL, "_ll", ll, 0, N_TEXT | N_EXT},
  {NULL, "_lsr", lsr, 0, N_TEXT | N_EXT},
  {NULL, "_llr", llr, 0, N_TEXT | N_EXT},
  {NULL, "_copy", copy, 0, N_TEXT | N_EXT},

  /* stdio */
  {NULL, "_fpClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_stdioFpCreate", stdioFpCreate, 0, N_TEXT | N_EXT},
  {NULL, "_stdioFpInit", stdioFpInit, 0, N_TEXT | N_EXT},
  {NULL, "_stdioFpDestroy", stdioFpDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_ferror", ferror, 0, N_TEXT | N_EXT},
  {NULL, "_feof", feof, 0, N_TEXT | N_EXT},
  {NULL, "_fflush", fflush, 0, N_TEXT | N_EXT},
  {NULL, "_fopen", fopen, 0, N_TEXT | N_EXT},
  {NULL, "_fdopen", fdopen, 0, N_TEXT | N_EXT},
  {NULL, "_fclose", fclose, 0, N_TEXT | N_EXT},
  {NULL, "_setvbuf", setvbuf, 0, N_TEXT | N_EXT},
  {NULL, "_fread", fread, 0, N_TEXT | N_EXT},
  {NULL, "_fwrite", fwrite, 0, N_TEXT | N_EXT},
  {NULL, "_putc", putc, 0, N_TEXT | N_EXT},
  {NULL, "_fputc", fputc, 0, N_TEXT | N_EXT},
  {NULL, "_putchar", putchar, 0, N_TEXT | N_EXT},
  {NULL, "_ungetc", ungetc, 0, N_TEXT | N_EXT},
  {NULL, "_getc", getc, 0, N_TEXT | N_EXT},
  {NULL, "_fgetc", fgetc, 0, N_TEXT | N_EXT},
  {NULL, "_gets", gets, 0, N_TEXT | N_EXT},
  {NULL, "_fgets", fgets, 0, N_TEXT | N_EXT},
  {NULL, "_getchar", getchar, 0, N_TEXT | N_EXT},
  {NULL, "_puts", puts, 0, N_TEXT | N_EXT},
  {NULL, "_printf", printf, 0, N_TEXT | N_EXT},
  {NULL, "_asprintf", asprintf, 0, N_TEXT | N_EXT},
  {NULL, "_sprintf", sprintf, 0, N_TEXT | N_EXT},
  {NULL, "_snprintf", snprintf, 0, N_TEXT | N_EXT},
  {NULL, "_fprintf", fprintf, 0, N_TEXT | N_EXT},
  {NULL, "_fdprintf", fdprintf, 0, N_TEXT | N_EXT},
  {NULL, "_vdprintf", vdprintf, 0, N_TEXT | N_EXT},
  {NULL, "_vfnprintf", vfnprintf, 0, N_TEXT | N_EXT},
  {NULL, "_scanf", scanf, 0, N_TEXT | N_EXT},
  {NULL, "_sscanf", sscanf, 0, N_TEXT | N_EXT},
  {NULL, "_fscanf", fscanf, 0, N_TEXT | N_EXT},
  {NULL, "_vfscanf", vfscanf, 0, N_TEXT | N_EXT},

  /* stdlib */
  {NULL, "_malloc", malloc, 0, N_TEXT | N_EXT},
  {NULL, "_realloc", realloc, 0, N_TEXT | N_EXT},
  {NULL, "_memalign", memalign, 0, N_TEXT | N_EXT},
  {NULL, "_valloc", valloc, 0, N_TEXT | N_EXT},
  {NULL, "_calloc", calloc, 0, N_TEXT | N_EXT},
  {NULL, "_free", free, 0, N_TEXT | N_EXT},
  {NULL, "_cfree", cfree, 0, N_TEXT | N_EXT},
  {NULL, "_exit", exit, 0, N_TEXT | N_EXT},
  {NULL, "_abs", abs, 0, N_TEXT | N_EXT},
  {NULL, "_itoa", itoa, 0, N_TEXT | N_EXT},
  {NULL, "_itox", itox, 0, N_TEXT | N_EXT},
  {NULL, "_strtol", strtol, 0, N_TEXT | N_EXT},
  {NULL, "_rand", rand, 0, N_TEXT | N_EXT},
  {NULL, "_div", div, 0, N_TEXT | N_EXT},
  {NULL, "_ldiv", ldiv, 0, N_TEXT | N_EXT},
  {NULL, "_putenv", putenv, 0, N_TEXT | N_EXT},
  {NULL, "_getenv", getenv, 0, N_TEXT | N_EXT},

  /* string */
  {NULL, "_memchr", memchr, 0, N_TEXT | N_EXT},
  {NULL, "_memcmp", memcmp, 0, N_TEXT | N_EXT},
  {NULL, "_memcpy", memcpy, 0, N_TEXT | N_EXT},
  {NULL, "_memcpyw", memcpyw, 0, N_TEXT | N_EXT},
  {NULL, "_memmove", memmove, 0, N_TEXT | N_EXT},
  {NULL, "_memset", memset, 0, N_TEXT | N_EXT},
  {NULL, "_memsetw", memsetw, 0, N_TEXT | N_EXT},
  {NULL, "_strlen", strlen, 0, N_TEXT | N_EXT},
  {NULL, "_strpbrk", strpbrk, 0, N_TEXT | N_EXT},
  {NULL, "_strcpy", strcpy, 0, N_TEXT | N_EXT},
  {NULL, "_strncpy", strncpy, 0, N_TEXT | N_EXT},
  {NULL, "_strcat", strcat, 0, N_TEXT | N_EXT},
  {NULL, "_strchr", strchr, 0, N_TEXT | N_EXT},
  {NULL, "_strspn", strspn, 0, N_TEXT | N_EXT},
  {NULL, "_strrchr", strrchr, 0, N_TEXT | N_EXT},
  {NULL, "_strstr", strstr, 0, N_TEXT | N_EXT},
  {NULL, "_strcmp", strcmp, 0, N_TEXT | N_EXT},
  {NULL, "_strncmp", strncmp, 0, N_TEXT | N_EXT},
  {NULL, "_strtok_r", strtok_r, 0, N_TEXT | N_EXT},
  {NULL, "_strtok", strtok, 0, N_TEXT | N_EXT},
  {NULL, "_strdup", strdup, 0, N_TEXT | N_EXT},
  {NULL, "_memccpy", memccpy, 0, N_TEXT | N_EXT},
  {NULL, "_strcspn", strcspn, 0, N_TEXT | N_EXT},
  {NULL, "_strncat", strncat, 0, N_TEXT | N_EXT},

  /* ctype */
  {NULL, "_isalnum", isalnum, 0, N_TEXT | N_EXT},
  {NULL, "_isalpha", isalpha, 0, N_TEXT | N_EXT},
  {NULL, "_iscntrl", iscntrl, 0, N_TEXT | N_EXT},
  {NULL, "_isdigit", isdigit, 0, N_TEXT | N_EXT},
  {NULL, "_isgraph", isgraph, 0, N_TEXT | N_EXT},
  {NULL, "_islower", islower, 0, N_TEXT | N_EXT},
  {NULL, "_isprint", isprint, 0, N_TEXT | N_EXT},
  {NULL, "_ispunct", ispunct, 0, N_TEXT | N_EXT},
  {NULL, "_isspace", isspace, 0, N_TEXT | N_EXT},
  {NULL, "_isupper", isupper, 0, N_TEXT | N_EXT},
  {NULL, "_isxdigit", isxdigit, 0, N_TEXT | N_EXT},
  {NULL, "_tolower", tolower, 0, N_TEXT | N_EXT},
  {NULL, "_toupper", toupper, 0, N_TEXT | N_EXT},

  /* fcntl */
  {NULL, "_open", open, 0, N_TEXT | N_EXT},
  {NULL, "_creat", creat, 0, N_TEXT | N_EXT},

  /* unistd */
  {NULL, "_lseek", lseek, 0, N_TEXT | N_EXT},
  {NULL, "_read", read, 0, N_TEXT | N_EXT},
  {NULL, "_write", write, 0, N_TEXT | N_EXT},
  {NULL, "_getcwd", getcwd, 0, N_TEXT | N_EXT},
  {NULL, "_chdir", chdir, 0, N_TEXT | N_EXT},
  {NULL, "_rmdir", rmdir, 0, N_TEXT | N_EXT},
  {NULL, "_isatty", isatty, 0, N_TEXT | N_EXT},
  {NULL, "_close", close, 0, N_TEXT | N_EXT},
  {NULL, "_fpathconf", fpathconf, 0, N_TEXT | N_EXT},
  {NULL, "_pathconf", pathconf, 0, N_TEXT | N_EXT},
  {NULL, "_ftruncate", ftruncate, 0, N_TEXT | N_EXT},
  {NULL, "_symlink", symlink, 0, N_TEXT | N_EXT},
  {NULL, "_readlink", readlink, 0, N_TEXT | N_EXT},

  /* time */
  {NULL, "_time", time, 0, N_TEXT | N_EXT},
  {NULL, "_mktime", mktime, 0, N_TEXT | N_EXT},
  {NULL, "_localtime", localtime, 0, N_TEXT | N_EXT},
  {NULL, "_strftime", strftime, 0, N_TEXT | N_EXT},

  /* setjmp */
  {NULL, "_longjmp", longjmp, 0, N_TEXT | N_EXT},

  /* dirent */
  {NULL, "_closedir", closedir, 0, N_TEXT | N_EXT},
  {NULL, "_opendir", opendir, 0, N_TEXT | N_EXT},
  {NULL, "_readdir", readdir, 0, N_TEXT | N_EXT},
  {NULL, "_rewinddir", rewinddir, 0, N_TEXT | N_EXT},

  /* ioctl */
  {NULL, "_ioctl", ioctl, 0, N_TEXT | N_EXT},

  /* stat */
  {NULL, "_fstat", fstat, 0, N_TEXT | N_EXT},
  {NULL, "_lstat", lstat, 0, N_TEXT | N_EXT},
  {NULL, "_mkdir", mkdir, 0, N_TEXT | N_EXT},
  {NULL, "_stat", stat, 0, N_TEXT | N_EXT},

  /* uio */
  {NULL, "_uiomove", uiomove, 0, N_TEXT | N_EXT},

  /* socket */
  {NULL, "_socket", socket, 0, N_TEXT | N_EXT},
  {NULL, "_bind", bind, 0, N_TEXT | N_EXT},
  {NULL, "_listen", listen, 0, N_TEXT | N_EXT},
  {NULL, "_accept", accept, 0, N_TEXT | N_EXT},
  {NULL, "_connect", connect, 0, N_TEXT | N_EXT},
  {NULL, "_connectWithTimeout", connectWithTimeout, 0, N_TEXT | N_EXT},
  {NULL, "_getsockname", getsockname, 0, N_TEXT | N_EXT},
  {NULL, "_getpeername", getpeername, 0, N_TEXT | N_EXT},
  {NULL, "_sendto", sendto, 0, N_TEXT | N_EXT},
  {NULL, "_send", send, 0, N_TEXT | N_EXT},
  {NULL, "_sendmsg", sendmsg, 0, N_TEXT | N_EXT},
  {NULL, "_recvfrom", recvfrom, 0, N_TEXT | N_EXT},
  {NULL, "_recv", recv, 0, N_TEXT | N_EXT},
  {NULL, "_recvmsg", recvmsg, 0, N_TEXT | N_EXT},
  {NULL, "_shutdown", shutdown, 0, N_TEXT | N_EXT},
  {NULL, "_setsockopt", setsockopt, 0, N_TEXT | N_EXT},
  {NULL, "_getsockopt", getsockopt, 0, N_TEXT | N_EXT},

  /* signal */
  {NULL, "_signal", signal, 0, N_TEXT | N_EXT},
  {NULL, "_sigreturn", sigreturn, 0, N_TEXT | N_EXT},
  {NULL, "_sigprocmask", sigprocmask, 0, N_TEXT | N_EXT},
  {NULL, "_sigaction", sigaction, 0, N_TEXT | N_EXT},
  {NULL, "_sigemptyset", sigemptyset, 0, N_TEXT | N_EXT},
  {NULL, "_sigfillset", sigfillset, 0, N_TEXT | N_EXT},
  {NULL, "_sigaddset", sigaddset, 0, N_TEXT | N_EXT},
  {NULL, "_sigdelset", sigdelset, 0, N_TEXT | N_EXT},
  {NULL, "_sigismember", sigismember, 0, N_TEXT | N_EXT},
  {NULL, "_sigpending", sigpending, 0, N_TEXT | N_EXT},
  {NULL, "_sigsuspend", sigsuspend, 0, N_TEXT | N_EXT},
  {NULL, "_pause", pause, 0, N_TEXT | N_EXT},
  {NULL, "_kill", kill, 0, N_TEXT | N_EXT},
  {NULL, "_raise", raise, 0, N_TEXT | N_EXT},
  {NULL, "_sigqueue", sigqueue, 0, N_TEXT | N_EXT},

  /* classLib */
  {NULL, "_rootClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_classCreate", classCreate, 0, N_TEXT | N_EXT},
  {NULL, "_classInit", classInit, 0, N_TEXT | N_EXT},
  {NULL, "_classShowConnect", classShowConnect, 0, N_TEXT | N_EXT},
  {NULL, "_classDestroy", classDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_classShow", classShow, 0, N_TEXT | N_EXT},

  /* objLib */
  {NULL, "_objAllocPad", objAllocPad, 0, N_TEXT | N_EXT},
  {NULL, "_objAlloc", objAlloc, 0, N_TEXT | N_EXT},
  {NULL, "_objShow", objShow, 0, N_TEXT | N_EXT},
  {NULL, "_objFree", objFree, 0, N_TEXT | N_EXT},
  {NULL, "_objCoreInit", objCoreInit, 0, N_TEXT | N_EXT},
  {NULL, "_objCoreTerminate", objCoreTerminate, 0, N_TEXT | N_EXT},

  /* cksumLib */
  {NULL, "_checksum", checksum, 0, N_TEXT | N_EXT},

  /* listLib */
  {NULL, "_listInit", listInit, 0, N_TEXT | N_EXT},
  {NULL, "_listInsert", listInsert, 0, N_TEXT | N_EXT},
  {NULL, "_listAdd", listAdd, 0, N_TEXT | N_EXT},
  {NULL, "_listGet", listGet, 0, N_TEXT | N_EXT},
  {NULL, "_listRemove", listRemove, 0, N_TEXT | N_EXT},
  {NULL, "_listCount", listCount, 0, N_TEXT | N_EXT},

  /* dllLib */
  {NULL, "_dllInit", dllInit, 0, N_TEXT | N_EXT},
  {NULL, "_dllAdd", dllAdd, 0, N_TEXT | N_EXT},
  {NULL, "_dllRemove", dllRemove, 0, N_TEXT | N_EXT},
  {NULL, "_dllInsert", dllInsert, 0, N_TEXT | N_EXT},
  {NULL, "_dllGet", dllGet, 0, N_TEXT | N_EXT},
  {NULL, "_dllCount", dllCount, 0, N_TEXT | N_EXT},

  /* sllLib */
  {NULL, "_sllInit", sllInit, 0, N_TEXT | N_EXT},
  {NULL, "_sllPutAtHead", sllPutAtHead, 0, N_TEXT | N_EXT},
  {NULL, "_sllPutAtTail", sllPutAtTail, 0, N_TEXT | N_EXT},
  {NULL, "_sllPrevious", sllPrevious, 0, N_TEXT | N_EXT},
  {NULL, "_sllAdd", sllAdd, 0, N_TEXT | N_EXT},
  {NULL, "_sllRemove", sllRemove, 0, N_TEXT | N_EXT},
  {NULL, "_sllInsert", sllInsert, 0, N_TEXT | N_EXT},
  {NULL, "_sllGet", sllGet, 0, N_TEXT | N_EXT},
  {NULL, "_sllCount", sllCount, 0, N_TEXT | N_EXT},
  {NULL, "_sllEach", sllEach, 0, N_TEXT | N_EXT},

  /* qLib */
  {NULL, "_qCreate", qCreate, 0, N_TEXT | N_EXT},
  {NULL, "_qInit", qInit, 0, N_TEXT | N_EXT},
  {NULL, "_qDestroy", qDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_qTerminate", qTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_qFirst", qFirst, 0, N_TEXT | N_EXT},
  {NULL, "_qPut", qPut, 0, N_TEXT | N_EXT},
  {NULL, "_qGet", qGet, 0, N_TEXT | N_EXT},
  {NULL, "_qRemove", qRemove, 0, N_TEXT | N_EXT},
  {NULL, "_qMove", qMove, 0, N_TEXT | N_EXT},
  {NULL, "_qAdvance", qAdvance, 0, N_TEXT | N_EXT},
  {NULL, "_qExpired", qExpired, 0, N_TEXT | N_EXT},
  {NULL, "_qKey", qKey, 0, N_TEXT | N_EXT},
  {NULL, "_qOffset", qOffset, 0, N_TEXT | N_EXT},
  {NULL, "_qInfo", qInfo, 0, N_TEXT | N_EXT},
  {NULL, "_qEach", qEach, 0, N_TEXT | N_EXT},

  /* hashLib */
  {NULL, "_hashTableCreate", hashTableCreate, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableInit", hashTableInit, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableDestroy", hashTableDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableTerminate", hashTableTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableDelete", hashTableDelete, 0, N_TEXT | N_EXT},
  {NULL, "_hashTablePut", hashTablePut, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableFind", hashTableFind, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableEach", hashTableEach, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableRemove", hashTableRemove, 0, N_TEXT | N_EXT},
  {NULL, "_hashKeyCmp", hashKeyCmp, 0, N_TEXT | N_EXT},
  {NULL, "_hashKeyStringCmp", hashKeyStringCmp, 0, N_TEXT | N_EXT},

  /* ringLib */
  {NULL, "_ringCreate", ringCreate, 0, N_TEXT | N_EXT},
  {NULL, "_ringDelete", ringDelete, 0, N_TEXT | N_EXT},
  {NULL, "_ringFlush", ringFlush, 0, N_TEXT | N_EXT},
  {NULL, "_ringBufferGet", ringBufferGet, 0, N_TEXT | N_EXT},
  {NULL, "_ringBufferPut", ringBufferPut, 0, N_TEXT | N_EXT},
  {NULL, "_ringIsEmpty", ringIsEmpty, 0, N_TEXT | N_EXT},
  {NULL, "_ringIsFull", ringIsFull, 0, N_TEXT | N_EXT},
  {NULL, "_ringFreeBytes", ringFreeBytes, 0, N_TEXT | N_EXT},
  {NULL, "_ringNBytes", ringNBytes, 0, N_TEXT | N_EXT},
  {NULL, "_ringPutAhead", ringPutAhead, 0, N_TEXT | N_EXT},
  {NULL, "_ringMoveAhead", ringMoveAhead, 0, N_TEXT | N_EXT},

  /* errnoLib */
  {NULL, "_errnoSet", errnoSet, 0, N_TEXT | N_EXT},
  {NULL, "_errnoGet", errnoGet, 0, N_TEXT | N_EXT},
  {NULL, "_errnoOfTaskGet", errnoOfTaskGet, 0, N_TEXT | N_EXT},

  /* taskLib */
  {NULL, "_taskClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_taskSpawn", taskSpawn, 0, N_TEXT | N_EXT},
  {NULL, "_taskCreat", taskCreat, 0, N_TEXT | N_EXT},
  {NULL, "_taskInit", taskInit, 0, N_TEXT | N_EXT},
  {NULL, "_taskDestroy", taskDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_taskDelete", taskDelete, 0, N_TEXT | N_EXT},
  {NULL, "_taskDeleteForce", taskDeleteForce, 0, N_TEXT | N_EXT},
  {NULL, "_taskActivate", taskActivate, 0, N_TEXT | N_EXT},
  {NULL, "_taskSuspend", taskSuspend, 0, N_TEXT | N_EXT},
  {NULL, "_taskResume", taskResume, 0, N_TEXT | N_EXT},
  {NULL, "_taskDelay", taskDelay, 0, N_TEXT | N_EXT},
  {NULL, "_taskUndelay", taskUndelay, 0, N_TEXT | N_EXT},
  {NULL, "_taskPrioritySet", taskPrioritySet, 0, N_TEXT | N_EXT},
  {NULL, "_taskPriorityGet", taskPriorityGet, 0, N_TEXT | N_EXT},
  {NULL, "_taskRestart", taskRestart, 0, N_TEXT | N_EXT},
  {NULL, "_taskExit", taskExit, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdSelf", taskIdSelf, 0, N_TEXT | N_EXT},
  {NULL, "_taskTcb", taskTcb, 0, N_TEXT | N_EXT},
  {NULL, "_taskStackAllot", taskStackAllot, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdVerify", taskIdVerify, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdDefault", taskIdDefault, 0, N_TEXT | N_EXT},
  {NULL, "_taskName", taskName, 0, N_TEXT | N_EXT},
  {NULL, "_taskRegsGet", taskRegsGet, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdListGet", taskIdListGet, 0, N_TEXT | N_EXT},
  {NULL, "_taskInfoGet", taskInfoGet, 0, N_TEXT | N_EXT},
  {NULL, "_taskShow", taskShow, 0, N_TEXT | N_EXT},
  {NULL, "_taskStatusString", taskStatusString, 0, N_TEXT | N_EXT},
  {NULL, "_taskOptionsString", taskOptionsString, 0, N_TEXT | N_EXT},
  {NULL, "_taskRegsShow", taskRegsShow, 0, N_TEXT | N_EXT},
  {NULL, "_taskIdListSort", taskIdListSort, 0, N_TEXT | N_EXT},

  /* taskVarLib */
  {NULL, "_taskVarAdd", taskVarAdd, 0, N_TEXT | N_EXT},
  {NULL, "_taskVarSet", taskVarSet, 0, N_TEXT | N_EXT},
  {NULL, "_taskVarGet", taskVarGet, 0, N_TEXT | N_EXT},
  {NULL, "_taskVarInfo", taskVarInfo, 0, N_TEXT | N_EXT},
  {NULL, "_taskVarDelete", taskVarDelete, 0, N_TEXT | N_EXT},

  /* memLib */
  {NULL, "_memPartOptionsSet", memPartOptionsSet, 0, N_TEXT | N_EXT},
  {NULL, "_memPartClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_memSysPartId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_memPartCreate", memPartCreate, 0, N_TEXT | N_EXT},
  {NULL, "_memPartInit", memPartInit, 0, N_TEXT | N_EXT},
  {NULL, "_memPartDestroy", memPartDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_memPartAddToPool", memPartAddToPool, 0, N_TEXT | N_EXT},
  {NULL, "_memPartAlignedAlloc", memPartAlignedAlloc, 0, N_TEXT | N_EXT},
  {NULL, "_memPartAlloc", memPartAlloc, 0, N_TEXT | N_EXT},
  {NULL, "_memPartRealloc", memPartRealloc, 0, N_TEXT | N_EXT},
  {NULL, "_memPartFree", memPartFree, 0, N_TEXT | N_EXT},
  {NULL, "_memPartOptionsSet", memPartOptionsSet, 0, N_TEXT | N_EXT},
  {NULL, "_memPartShow", memPartShow, 0, N_TEXT | N_EXT},

  /* eventLib */
  {NULL, "_eventReceive", eventReceive, 0, N_TEXT | N_EXT},
  {NULL, "_eventSend", eventSend, 0, N_TEXT | N_EXT},
  {NULL, "_eventClear", eventClear, 0, N_TEXT | N_EXT},
  {NULL, "_eventResourceInit", eventResourceInit, 0, N_TEXT | N_EXT},
  {NULL, "_eventResourceRegister", eventResourceRegister, 0, N_TEXT | N_EXT},
  {NULL, "_eventResourceSend", eventResourceSend, 0, N_TEXT | N_EXT},
  {NULL, "_eventResourceTerminate", eventResourceTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_eventTaskShow", eventTaskShow, 0, N_TEXT | N_EXT},
  {NULL, "_eventResourceShow", eventResourceShow, 0, N_TEXT | N_EXT},

  /* semLib */
  {NULL, "_semClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_semCreate", semCreate, 0, N_TEXT | N_EXT},
  {NULL, "_semInit", semInit, 0, N_TEXT | N_EXT},
  {NULL, "_semGive", semGive, 0, N_TEXT | N_EXT},
  {NULL, "_semTake", semTake, 0, N_TEXT | N_EXT},
  {NULL, "_semDelete", semDelete, 0, N_TEXT | N_EXT},
  {NULL, "_semDestroy", semDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_semShow", semShow, 0, N_TEXT | N_EXT},
  {NULL, "_semInvalid", semInvalid, 0, N_TEXT | N_EXT},
  {NULL, "_semQFlush", semQFlush, 0, N_TEXT | N_EXT},
  {NULL, "_semQFlushDefer", semQFlushDefer, 0, N_TEXT | N_EXT},
  {NULL, "_semBCreate", semBCreate, 0, N_TEXT | N_EXT},
  {NULL, "_semBInit", semBInit, 0, N_TEXT | N_EXT},
  {NULL, "_semBGive", semBGive, 0, N_TEXT | N_EXT},
  {NULL, "_semBTake", semBTake, 0, N_TEXT | N_EXT},
  {NULL, "_semBGiveDefer", semBGiveDefer, 0, N_TEXT | N_EXT},
  {NULL, "_semMCreate", semMCreate, 0, N_TEXT | N_EXT},
  {NULL, "_semMInit", semMInit, 0, N_TEXT | N_EXT},
  {NULL, "_semMGive", semMGive, 0, N_TEXT | N_EXT},
  {NULL, "_semMTake", semMTake, 0, N_TEXT | N_EXT},
  {NULL, "_semCCreate", semCCreate, 0, N_TEXT | N_EXT},
  {NULL, "_semCInit", semCInit, 0, N_TEXT | N_EXT},
  {NULL, "_semCGive", semCGive, 0, N_TEXT | N_EXT},
  {NULL, "_semCTake", semCTake, 0, N_TEXT | N_EXT},
  {NULL, "_semCGiveDefer", semCGiveDefer, 0, N_TEXT | N_EXT},
  {NULL, "_semRWCreate", semRWCreate, 0, N_TEXT | N_EXT},
  {NULL, "_semRWInit", semRWInit, 0, N_TEXT | N_EXT},
  {NULL, "_semWriterTake", semWriterTake, 0, N_TEXT | N_EXT},
  {NULL, "_semReaderTake", semReaderTake, 0, N_TEXT | N_EXT},
  {NULL, "_semRWUpgrade", semRWUpgrade, 0, N_TEXT | N_EXT},
  {NULL, "_semRWDowngrade", semRWDowngrade, 0, N_TEXT | N_EXT},

  /* semEvLib */
  {NULL, "_semEvRegister", semEvRegister, 0, N_TEXT | N_EXT},
  {NULL, "_semEvUnregister", semEvUnregister, 0, N_TEXT | N_EXT},

  /* msgQLib */
  {NULL, "_msgQClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_msgQCreate", msgQCreate, 0, N_TEXT | N_EXT},
  {NULL, "_msgQInit", msgQInit, 0, N_TEXT | N_EXT},
  {NULL, "_msgQTerminate", msgQTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_msgQDelete", msgQDelete, 0, N_TEXT | N_EXT},
  {NULL, "_msgQDestroy", msgQDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_msgQShow", msgQShow, 0, N_TEXT | N_EXT},
  {NULL, "_msgQSend", msgQSend, 0, N_TEXT | N_EXT},
  {NULL, "_msgQReceive", msgQReceive, 0, N_TEXT | N_EXT},

  /* msgQEvLib */
  {NULL, "_msgQEvRegister", msgQEvRegister, 0, N_TEXT | N_EXT},
  {NULL, "_msgQEvUnregister", msgQEvUnregister, 0, N_TEXT | N_EXT},

  /* ffsLib */
  {NULL, "_ffsLsb", ffsLsb, 0, N_TEXT | N_EXT},
  {NULL, "_ffsMsb", ffsMsb, 0, N_TEXT | N_EXT},

  /* kernLib */
  {NULL, "_kernelVersion", kernelVersion, 0, N_TEXT | N_EXT},
  {NULL, "_kernelTimeSlice", kernelTimeSlice, 0, N_TEXT | N_EXT},

  /* kernLib */
  {NULL, "_kernQAdd0", kernQAdd0, 0, N_TEXT | N_EXT},
  {NULL, "_kernQAdd1", kernQAdd1, 0, N_TEXT | N_EXT},
  {NULL, "_kernQAdd2", kernQAdd2, 0, N_TEXT | N_EXT},

  /* kernTickLib */
  {NULL, "_tickSet", tickSet, 0, N_TEXT | N_EXT},
  {NULL, "_tickGet", tickGet, 0, N_TEXT | N_EXT},
  {NULL, "_tick64Set", tick64Set, 0, N_TEXT | N_EXT},
  {NULL, "_tick64Get", tick64Get, 0, N_TEXT | N_EXT},

  /* vmLib */
  {NULL, "_vmContextClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_vmGlobalMapInit", vmGlobalMapInit, 0, N_TEXT | N_EXT},
  {NULL, "_vmContextCreate", vmContextCreate, 0, N_TEXT | N_EXT},
  {NULL, "_vmContextInit", vmContextInit, 0, N_TEXT | N_EXT},
  {NULL, "_vmContextDestroy", vmContextDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_vmStateSet", vmStateSet, 0, N_TEXT | N_EXT},
  {NULL, "_vmStateGet", vmStateGet, 0, N_TEXT | N_EXT},
  {NULL, "_vmGlobalMap", vmGlobalMap, 0, N_TEXT | N_EXT},
  {NULL, "_vmMap", vmMap, 0, N_TEXT | N_EXT},
  {NULL, "_vmCurrentSet", vmCurrentSet, 0, N_TEXT | N_EXT},
  {NULL, "_vmEnable", vmEnable, 0, N_TEXT | N_EXT},

  /* hashLib */
  {NULL, "_hashClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_hashTableCreate", hashTableCreate, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableInit", hashTableInit, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableDestroy", hashTableDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableTerminate", hashTableTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableDelete", hashTableDelete, 0, N_TEXT | N_EXT},
  {NULL, "_hashTablePut", hashTablePut, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableFind", hashTableFind, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableEach", hashTableEach, 0, N_TEXT | N_EXT},
  {NULL, "_hashTableRemove", hashTableRemove, 0, N_TEXT | N_EXT},
  {NULL, "_hashKeyCmp", hashKeyCmp, 0, N_TEXT | N_EXT},
  {NULL, "_hashKeyStringCmp", hashKeyStringCmp, 0, N_TEXT | N_EXT},

  /* symLib */
  {NULL, "_symTableClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_symTableCreate", symTableCreate, 0, N_TEXT | N_EXT},
  {NULL, "_symTableInit", symTableInit, 0, N_TEXT | N_EXT},
  {NULL, "_symTableDestroy", symTableDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_symTableDelete", symTableDelete, 0, N_TEXT | N_EXT},
  {NULL, "_symTableTerminate", symTableTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_symCreate", symCreate, 0, N_TEXT | N_EXT},
  {NULL, "_symInit", symInit, 0, N_TEXT | N_EXT},
  {NULL, "_symDestroy", symDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_symAdd", symAdd, 0, N_TEXT | N_EXT},
  {NULL, "_symTableAdd", symTableAdd, 0, N_TEXT | N_EXT},
  {NULL, "_symFindByName", symFindByName, 0, N_TEXT | N_EXT},
  {NULL, "_symFindByNameAndType", symFindByNameAndType, 0, N_TEXT | N_EXT},
  {NULL, "_symFindSymbol", symFindSymbol, 0, N_TEXT | N_EXT},
  {NULL, "_symNameGet", symNameGet, 0, N_TEXT | N_EXT},
  {NULL, "_symValueGet", symValueGet, 0, N_TEXT | N_EXT},
  {NULL, "_symTypeGet", symTypeGet, 0, N_TEXT | N_EXT},
  {NULL, "_symEach", symEach, 0, N_TEXT | N_EXT},
  {NULL, "_symRemove", symRemove, 0, N_TEXT | N_EXT},
  {NULL, "_symTableRemove", symTableRemove, 0, N_TEXT | N_EXT},
  {NULL, "_symShow", symShow, 0, N_TEXT | N_EXT},

  /* envLib */
  {NULL, "_envPrivateCreate", envPrivateCreate, 0, N_TEXT | N_EXT},
  {NULL, "_envPrivateDestroy", envPrivateDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_envShow", envShow, 0, N_TEXT | N_EXT},

  /* cbioLib */
  {NULL, "_cbioClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_cbioDevCreate", cbioDevCreate, 0, N_TEXT | N_EXT},
  {NULL, "_cbioDevVerify", cbioDevVerify, 0, N_TEXT | N_EXT},
  {NULL, "_cbioLock", cbioLock, 0, N_TEXT | N_EXT},
  {NULL, "_cbioUnlock", cbioUnlock, 0, N_TEXT | N_EXT},
  {NULL, "_cbioBlkRW", cbioBlkRW, 0, N_TEXT | N_EXT},
  {NULL, "_cbioBytesRW", cbioBytesRW, 0, N_TEXT | N_EXT},
  {NULL, "_cbioBlkCopy", cbioBlkCopy, 0, N_TEXT | N_EXT},
  {NULL, "_cbioIoctl", cbioIoctl, 0, N_TEXT | N_EXT},
  {NULL, "_cbioModeSet", cbioModeSet, 0, N_TEXT | N_EXT},
  {NULL, "_cbioModeGet", cbioModeGet, 0, N_TEXT | N_EXT},
  {NULL, "_cbioRdyChgdGet", cbioRdyChgdGet, 0, N_TEXT | N_EXT},
  {NULL, "_cbioParamsGet", cbioParamsGet, 0, N_TEXT | N_EXT},
  {NULL, "_cbioShow", cbioShow, 0, N_TEXT | N_EXT},

  /* taskHookLib */
  {NULL, "_taskCreateHookAdd", taskCreateHookAdd, 0, N_TEXT | N_EXT},
  {NULL, "_taskCreateHookDelete", taskCreateHookDelete, 0, N_TEXT | N_EXT},
  {NULL, "_taskCreateHookShow", taskCreateHookShow, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwitchHookAdd", taskSwitchHookAdd, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwitchHookDelete", taskSwitchHookDelete, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwitchHookShow", taskSwitchHookShow, 0, N_TEXT | N_EXT},
  {NULL, "_taskDeleteHookAdd", taskDeleteHookAdd, 0, N_TEXT | N_EXT},
  {NULL, "_taskDeleteHookDelete", taskDeleteHookDelete, 0, N_TEXT | N_EXT},
  {NULL, "_taskDeleteHookShow", taskDeleteHookShow, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwapHookAdd", taskSwapHookAdd, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwapHookDelete", taskSwapHookDelete, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwapHookAttach", taskSwapHookAttach, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwapHookDetach", taskSwapHookDetach, 0, N_TEXT | N_EXT},
  {NULL, "_taskSwapHookShow", taskSwapHookShow, 0, N_TEXT | N_EXT},

  /* wdLib */
  {NULL, "_wdCreate", wdCreate, 0, N_TEXT | N_EXT},
  {NULL, "_wdInit", wdInit, 0, N_TEXT | N_EXT},
  {NULL, "_wdDestroy", wdDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_wdDelete", wdDelete, 0, N_TEXT | N_EXT},
  {NULL, "_wdTerminate", wdTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_wdStart", wdStart, 0, N_TEXT | N_EXT},
  {NULL, "_wdCancel", wdCancel, 0, N_TEXT | N_EXT},
  {NULL, "_wdShow", wdShow, 0, N_TEXT | N_EXT},

  /* moduleLib */
  {NULL, "_moduleClassId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_moduleCreate", moduleCreate, 0, N_TEXT | N_EXT},
  {NULL, "_moduleInit", moduleInit, 0, N_TEXT | N_EXT},
  {NULL, "_moduleDestroy", moduleDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_moduleTerminate", moduleTerminate, 0, N_TEXT | N_EXT},
  {NULL, "_moduleDelete", moduleDelete, 0, N_TEXT | N_EXT},
  {NULL, "_moduleIdFigure", moduleIdFigure, 0, N_TEXT | N_EXT},
  {NULL, "_moduleIdListGet", moduleIdListGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleNameGet", moduleNameGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleGroupGet", moduleGroupGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleFlagsGet", moduleFlagsGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleFormatGet", moduleFormatGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleFindByName", moduleFindByName, 0, N_TEXT | N_EXT},
  {NULL, "_moduleFindByNameAndPath", moduleFindByNameAndPath, 0, N_TEXT | N_EXT},
  {NULL, "_moduleFindByGroup", moduleFindByGroup, 0, N_TEXT | N_EXT},
  {NULL, "_moduleEach", moduleEach, 0, N_TEXT | N_EXT},
  {NULL, "_moduleInfoGet", moduleInfoGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleCheck", moduleCheck, 0, N_TEXT | N_EXT},
  {NULL, "_moduleCreateHookAdd", moduleCreateHookAdd, 0, N_TEXT | N_EXT},
  {NULL, "_moduleCreateHookDelete", moduleCreateHookDelete, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegAdd", moduleSegAdd, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegGet", moduleSegGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegFirst", moduleSegFirst, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegNext", moduleSegNext, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegEach", moduleSegEach, 0, N_TEXT | N_EXT},
  {NULL, "_moduleSegInfoGet", moduleSegInfoGet, 0, N_TEXT | N_EXT},
  {NULL, "_moduleShow", moduleShow, 0, N_TEXT | N_EXT},

  /* netLib */
  {NULL, "_netJobAdd", netJobAdd, 0, N_TEXT | N_EXT},
  {NULL, "_netPoolShow", netPoolShow, 0, N_TEXT | N_EXT},
  {NULL, "_ifShow", ifShow, 0, N_TEXT | N_EXT},
  {NULL, "_ifAddrAdd", ifAddrAdd, 0, N_TEXT | N_EXT},
  {NULL, "_routeShow", routeShow, 0, N_TEXT | N_EXT},
  {NULL, "_domainShow", domainShow, 0, N_TEXT | N_EXT},

  /* ugl */
  {NULL, "_uglModeAvailGet", uglModeAvailGet, 0, N_TEXT | N_EXT},
  {NULL, "_uglModeSet", uglModeSet, 0, N_TEXT | N_EXT},
  {NULL, "_uglGcCreate", uglGcCreate, 0, N_TEXT | N_EXT},
  {NULL, "_uglGcCopy", uglGcCopy, 0, N_TEXT | N_EXT},
  {NULL, "_uglGcDestroy", uglGcDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_uglGcSet", uglGcSet, 0, N_TEXT | N_EXT},
  {NULL, "_uglPixelSet", uglPixelSet, 0, N_TEXT | N_EXT},
  {NULL, "_uglClutSet", uglClutSet, 0, N_TEXT | N_EXT},
  {NULL, "_uglClutGet", uglClutGet, 0, N_TEXT | N_EXT},
  {NULL, "_uglColorAllocExt", uglColorAllocExt, 0, N_TEXT | N_EXT},
  {NULL, "_uglColorAlloc", uglColorAlloc, 0, N_TEXT | N_EXT},
  {NULL, "_uglColorFree", uglColorFree, 0, N_TEXT | N_EXT},
  {NULL, "_uglBitmapCreate", uglBitmapCreate, 0, N_TEXT | N_EXT},
  {NULL, "_uglBitmapDestroy", uglBitmapDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_uglOsLockCreate", uglOsLockCreate, 0, N_TEXT | N_EXT},
  {NULL, "_uglOsLockDestroy", uglOsLockDestroy, 0, N_TEXT | N_EXT},
  {NULL, "_uglOsLock", uglOsLock, 0, N_TEXT | N_EXT},
  {NULL, "_uglOsUnLock", uglOsUnLock, 0, N_TEXT | N_EXT},
  {NULL, "_uglUgiDevInit", uglUgiDevInit, 0, N_TEXT | N_EXT},
  {NULL, "_uglUgiDevDeinit", uglUgiDevDeinit, 0, N_TEXT | N_EXT},
  {NULL, "_uglGraphicsDevDestroy", uglGraphicsDevDestroy, 0, N_TEXT | N_EXT},

  /* Heap memory partition */
  {NULL, "_memHeapPartId", NULL, 0, N_DATA | N_EXT}
};

/* Locals */
LOCAL void standTableAddData(char *name, void *data);

/* Functions */

/*******************************************************************************
 * standTableInit - Initialize standalone symbol table
 *
 * RETURNS: N/A
 ******************************************************************************/

void standTableInit(void)
{
  /* Get symbol table size */
  standTableSize = NELEMENTS(standTable);

  /* Add data symbols */
  standTableAddData("_fpClassId", fpClassId);
  standTableAddData("_rootClassId", rootClassId);
  standTableAddData("_taskClassId", taskClassId);
  standTableAddData("_memPartClassId", memPartClassId);
  standTableAddData("_memSysPartId", memSysPartId);
  standTableAddData("_semClassId", semClassId);
  standTableAddData("_msgQClassId", msgQClassId);
  standTableAddData("_vmContextClassId", vmContextClassId);
  standTableAddData("_hashClassId", hashClassId);
  standTableAddData("_symTableClassId", symTableClassId);
  standTableAddData("_cbioClassId", cbioClassId);
  standTableAddData("_moduleClassId", moduleClassId);
  standTableAddData("_memHeapPartId", memHeapPartId);
}

/*******************************************************************************
 * standTableAddData - Add data symbol to symbol table
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void standTableAddData(char *name, void *data)
{
  int i;

  for (i = 0; i < standTableSize; i++) {

    if ( strcmp(name, standTable[i].name) == 0) {

      standTable[i].value = data;
      break;

    }

  }

}

