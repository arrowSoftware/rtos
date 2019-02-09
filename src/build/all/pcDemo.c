#include "configAll.h"
#include <setjmp.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <a.out.h>
#include <elf.h>
#include <assert.h>
#include <rtos.h>
#include <arch/iv.h>
#include <arch/sysArchLib.h>
#include <arch/intArchLib.h>
#include <arch/taskArchLib.h>
#include <rtos/taskLib.h>
#include <rtos/taskShow.h>
#include <rtos/kernLib.h>
#include <rtos/rtosLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/taskHookShow.h>
#include <rtos/eventLib.h>
#include <rtos/msgQLib.h>
#include <rtos/memPartLib.h>
#include <rtos/memLib.h>
#include <rtos/memShow.h>
#include <os/vmLib.h>
#include <io/ioLib.h>
#include <io/ttyLib.h>
#include <io/cbioLib.h>
#include <io/cbioShow.h>
#include <os/logLib.h>
#include <os/excLib.h>
#include <os/sigLib.h>
#include <os/symLib.h>
#include <os/symShow.h>
#include <util/ringLib.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <util/classShow.h>
#include <signal.h>
#include <os/selectLib.h>
#include <drv/serial/pcConsole.h>
#include <drv/video/vgahw.h>
#include <util/qLib.h>
#include <util/qFifoLib.h>
#include <util/qPrioLib.h>
#include <util/qPriBmpLib.h>
#include <util/qMsgLib.h>
#include <os/pipeDrv.h>
#include <tools/moduleLib.h>
#include <net/netBufLib.h>
#include <net/mbuf.h>
#include <net/netShow.h>
#include <net/netLib.h>
#include <net/sockLib.h>
#include <ugl/ugl.h>
#include <ugl/driver/graphics/vga/udvga.h>

/*#define INCLUDE_GFX*/
#define PERIODIC_INTERVAL	(5 * sysClockRateGet() )
#define DELAY_SHORT		(sysClockRateGet() / 10)
#define DELAY_LONG		(1 * sysClockRateGet() )
#define MESSAGE_EVENT		0x02
#define SEMAPHORE_EVENT		0x04
#define MAX_MESSAGES		10
#define LOOP_PAL_LENGTH 	16

IMPORT PART_ID memHeapPartId;
IMPORT TCB_ID taskIdCurrent;
IMPORT unsigned kernTicks;
IMPORT SYMTAB_ID sysSymTable;
IMPORT BOOL kernelState;
IMPORT TCB_ID taskIdCurrent;
IMPORT void kernExit(void);
IMPORT int sysClockRateGet(void);
extern STATUS msgQEvRegister(MSG_Q_ID msgQId, u_int32_t events, u_int8_t options);
extern STATUS semEvRegister(SEM_ID semId, u_int32_t events, u_int8_t options);
extern int setjmp(jmp_buf env);
extern STATUS inet_ntoa_b();
extern STATUS remove(const char *path);

int helpDemo(void);

char bigString[] = "\n"
		   "******************************************************\n"
  		   "Suddenly I awoke. I must have falled asleep under the \n"
                   "oak, where I used to sit and dream as a child.\n"
		   "I remebered my strange dream and knew the love and \n"
		   "strength in my heart to create something from my \n"
		   "life. The world was waiting, I was reborn...\n"
  		   "******************************************************"
		   "\n";
SEM_ID sem, sem2;
int pipe0;
int created_tasks = 0;
int deleted_tasks = 0;
int switched_tasks = 0;
int swapped_tasks = 0;
int created_modules = 0;
int periodicTaskId;
int periodicValue = 0;
BOOL periodicState = FALSE;
MSG_Q_ID msgQId;
int numMsg = 0;
char buf[1024];
int var[256];

#define INCLUDE_GFX
#ifdef INCLUDE_GFX

#include "rtosball.cbm"
#include "pinball.cbm"
#include "font8x16.cfs"

UGL_DEVICE_ID gfxDevId;
UGL_DIB *pBgDib, *pFgDib;
UGL_RASTER_OP rasterOp = UGL_RASTER_OP_COPY;
BOOL doubleBuffer = TRUE;
int animTreshold = 0;

/* Locals */
LOCAL PART_ID gfxPartId;

extern int fs_test(void);

const char *TylerGajewski = "Tyler Gajewski";

int tyler(void)
{
    fs_test();
}

int createDib(void)
{
  int i, j;
  UGL_UINT8 *ptr;

  pBgDib = malloc(sizeof(UGL_DIB));
  pFgDib = malloc(sizeof(UGL_DIB));
  if (pBgDib == NULL || pFgDib == NULL) {
    printf("Error creating dib\n");
    return 1;
  }

  pBgDib->width = rtosballWidth;
  pBgDib->height = rtosballHeight;
  pBgDib->stride = rtosballWidth;
  pBgDib->imageFormat = UGL_INDEXED_8;
  pBgDib->colorFormat = UGL_DEVICE_COLOR_32;
  pBgDib->clutSize = 16;
  pBgDib->pClut = malloc(sizeof(UGL_COLOR) * 16);
  if (pBgDib->pClut == NULL) {
    printf("Error creating clut\n");
    return 1;
  }
  for (i = 0; i < 16; i++)
    ((UGL_COLOR *) pBgDib->pClut)[i] = UGL_MAKE_ARGB(1,
		    		     16 * rtosballClut[i][0] / 255,
		    		     16 * rtosballClut[i][1] / 255,
		    		     16 * rtosballClut[i][2] / 255);
  pBgDib->pData = rtosballData;

  pFgDib->width = pinballWidth;
  pFgDib->height = pinballHeight;
  pFgDib->stride = pinballWidth;
  pFgDib->imageFormat = UGL_INDEXED_8;
  pFgDib->colorFormat = UGL_DEVICE_COLOR_32;
  pFgDib->clutSize = 16;
  pFgDib->pClut = malloc(sizeof(UGL_COLOR) * 16);
  if (pFgDib->pClut == NULL) {
    printf("Error creating clut\n");
    return 1;
  }
  for (i = 0; i < 16; i++)
    ((UGL_COLOR *) pFgDib->pClut)[i] = UGL_MAKE_ARGB(1,
		    		     16 * pinballClut[i][0] / 255,
		    		     16 * pinballClut[i][1] / 255,
		    		     16 * pinballClut[i][2] / 255);
  pFgDib->pData = pinballData;

  return 0;
}

#endif /* INCLUDE_GFX */

int createHook(void)
{
  created_tasks++;
}

int deleteHook(void)
{
  deleted_tasks++;
}

int switchHook(void)
{
  switched_tasks++;
}

int swapHook(void)
{
  swapped_tasks++;
}

int moduleHook(void)
{
  created_modules++;
}

int showSysTime (void)
{
  semTake (sem, WAIT_FOREVER);
  printf("System time is: %d\n", kernTicks / sysClockRateGet() );
  semGive (sem);

  return 0;
}

int showBigString(void)
{
  semTake(sem, WAIT_FOREVER);
  printf(bigString);
  semGive(sem);

  return 0;
}

int slowFill(void)
{
  int i, j;
  semTake(sem, WAIT_FOREVER);
  for (i = 0; i < 80; i++) {
    putchar('=');
    taskDelay(DELAY_SHORT);
  }
  semGive(sem);

  return 0;
}

int sendMessage(void)
{
  if ((numMsg + 1) >= MAX_MESSAGES) {
    semTake(sem, WAIT_FOREVER);
    printf("Message queue full.\n");
    semGive(sem);

    return 1;
  }

  numMsg++;
  semTake(sem, WAIT_FOREVER);
  printf("Sending message to message queue...");
  semGive(sem);

  //msgQSend(msgQId, bigString, strlen(bigString), WAIT_FOREVER, MSG_PRI_NORMAL);
  msgQSend(msgQId, bigString, strlen(bigString), 600, MSG_PRI_NORMAL);

  semTake(sem, WAIT_FOREVER);
  printf("Message sent.\n");

  printf("%d message(s) sent to message queue.\n", numMsg);

  semGive(sem);

  return 0;
}

int receiveMessage(void)
{
  char buf[512];

  semTake(sem, WAIT_FOREVER);
  printf("Receiving message from message queue...\n");
  semGive(sem);

  msgQReceive(msgQId, buf, sizeof(bigString), WAIT_FOREVER);
  numMsg--;

  semTake(sem, WAIT_FOREVER);
  printf("Received message: ");
  semGive(sem);

  buf[strlen(bigString)]='\0';
  printf(buf);

  printf("%d message(s) left in message queue.\n", numMsg);

  return 0;
}

IMPORT char *memTop, *memTopPhys;
int showInfo(void)
{
  time_t now;
  struct tm nowDate;
  char fmt[24], str[255];

  now = time(NULL);
  localtime_r(&now, &nowDate);

  strftime(fmt, 24, "%c", &nowDate);
  strftime(str, 255, fmt, &nowDate);

  semTake(sem, WAIT_FOREVER);

  printf("Time: %s\n", str);

  printf("%d created task(s)\n"
	 "%d deleted task(s)\n"
	 "%d task switch(es)\n"
	 "%d swap(s) in to periodic task\n",
	 created_tasks,
	 deleted_tasks,
	 switched_tasks,
	 swapped_tasks);
  printf("%d created module(s)\n", created_modules);

  printf("memTop: %08x, memTopPhys: %08x\n", memTop, memTopPhys);
  printf("_end symbol: %08x\n", end);
  printf("Compile date: %s\n", COMPILE_DATE);

  semGive(sem);

  return 0;
}

int messageListener(void)
{
  u_int32_t events;

  msgQEvRegister(msgQId, MESSAGE_EVENT, EVENTS_SEND_IF_FREE);

  semTake(sem, WAIT_FOREVER);
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  printf("Waiting for message queue to get a message...\n");
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  semGive(sem);

  eventReceive(MESSAGE_EVENT, EVENTS_WAIT_ALL|EVENTS_KEEP_UNWANTED,
	       WAIT_FOREVER, &events);

  semTake(sem, WAIT_FOREVER);
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  printf("Message queue got a message.\n");
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  semGive(sem);

  return 0;
}

int msgListen(void)
{
        taskSpawn("tMsgLstnr",
		   50,
		   0,
	           DEFAULT_STACK_SIZE, (FUNCPTR) messageListener,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0);

  return 0;
}

int semaphoreListener(void)
{
  u_int32_t events;

  semEvRegister(sem2, SEMAPHORE_EVENT, EVENTS_SEND_ONCE);

  semTake(sem, WAIT_FOREVER);
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  printf("Waiting for semaphore to become free...\n");
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  semGive(sem);

  eventReceive(SEMAPHORE_EVENT, EVENTS_WAIT_ALL|EVENTS_KEEP_UNWANTED,
	       WAIT_FOREVER, &events);

  semTake(sem, WAIT_FOREVER);
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  printf("Semaphore got free.\n");
  printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  semGive(sem);

  return 0;
}

int semListen(void)
{
        taskSpawn("tSemLstnr",
		   50,
		   0,
	           DEFAULT_STACK_SIZE,
		   (FUNCPTR) semaphoreListener,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0);

  return 0;
}

int semUse(void)
{
  int i;

  semTake(sem, WAIT_FOREVER);
  printf("Take semaphore");
  semGive(sem);

  semTake(sem2, WAIT_FOREVER);

  for (i = 0; i < 3; i++) {
    semTake(sem, WAIT_FOREVER);
    putchar('.');
    semGive(sem);
    taskDelay(DELAY_LONG);
  }

  semGive(sem2);

  semTake(sem, WAIT_FOREVER);
  printf("Gave semaphore.\n");
  semGive(sem);

  return 0;
}

int testPrintf(void)
{


  return 0;
}

int testStdio(void)
{
  char str[BUFSIZ];
  int a, b, c;

  semTake(sem, WAIT_FOREVER);

  printf("Testing sprintf() function:\n"
	 "---------------------------\n");
  sprintf(str, "Unsigned: %2u\n"
	       "Hexadecimal: %08x\n"
	       "Decimal: %02d\n"
	       "String: %s\n",
	  10, 10, -10, "APA");
  printf(str);
  printf("\n");

  printf("Testing getchar(), putchar() and ungetc() functions:\n"
	 "----------------------------------------------------\n");
  printf("Enter a character: ");
  c = getchar();
  printf("Got character: ");
  putchar(c);
  putchar('\n');
  printf("Putting character back on buffer...");
  ungetc(c, stdin);
  printf("Done.\n");
  c = 0;
  c = getchar();
  printf("Got character: ");
  putchar(c);
  putchar('\n');
  printf("\n");

  printf("Testing sscanf() function:\n"
	 "---------------------------\n");
  strcpy(str, "10");
  sscanf(str, "%d", &a);
  printf("Got: %d\n\n", a);

  printf("Testing scanf() function:\n"
	 "---------------------------\n");
  printf("Enter a string, number and a hex-number (separated by spaces): ");
  scanf("%s %d %x", str, &a, &b);
  printf("Got: <string> %s, <number> %d and <hex-number> %x\n\n", str, a, b);

  semGive(sem);

  return 0;
}

int testSymbols(void)
{
  char str[256], name[256];
  int val;

  semTake(sem, WAIT_FOREVER);

  while(1) {

    printf("Enter a symbol name and value:\nsymAdd> ");
    fgets(str, 255, stdin);
    if (strlen(str) <= 1)
      break;
    sscanf(str, "%s %d", name, &val);

    if (symAdd(sysSymTable, name, (void *) val, 0x01, 0) != OK)
      fprintf(stderr, "Error - Unable to add symbol %s\n", name);
    else
      printf("Added symbol: %s = %d\n", name, val);

  }

  while (1) {

    printf("Enter symbol name to search for:\nsymFind> ");
    fgets(name, 255, stdin);
    if (strlen(name) <= 1)
      break;
    name[strlen(name) - 1] = EOS;

    if (symFindByName(sysSymTable, name, (void **) &val, NULL) != OK)
      fprintf(stderr, "Errorr - Unable to find symbol %s\n", name);
    else
      printf("Found symbol: %s = %d\n", name, val);

  }

  semGive(sem);

  return 0;
}

#ifdef INCLUDE_GFX

void setPalette(void)
{
  int i;
  UGL_ORD paletteIndex[LOOP_PAL_LENGTH];
  UGL_ARGB palette[LOOP_PAL_LENGTH];
  UGL_COLOR colors[LOOP_PAL_LENGTH];

  for (i = 0; i < LOOP_PAL_LENGTH; i++) {
    paletteIndex[i] = i;
    palette[i] = UGL_MAKE_ARGB(255,			/* ALPHA */
			       rtosballClut[i][0],	/* RED */
			       rtosballClut[i][1],	/* GREEN */
			       rtosballClut[i][2]);	/* BLUE */
  }
  uglColorAlloc(gfxDevId, palette, paletteIndex, colors, LOOP_PAL_LENGTH);
}

int newImg(UGL_DDB_ID *pDbBmpId,
	   UGL_DDB_ID *pBgBmpId,
	   UGL_DDB_ID *pFgBmpId,
	   UGL_DDB_ID *pSaveBmpId)
{
  *pDbBmpId = uglBitmapCreate(gfxDevId, UGL_NULL, UGL_DIB_INIT_VALUE,
			 14, gfxPartId);
  if (*pDbBmpId == UGL_NULL)
    return 1;

  *pBgBmpId = uglBitmapCreate(gfxDevId, pBgDib, UGL_DIB_INIT_DATA,
			 8, gfxPartId);
  if (*pBgBmpId == UGL_NULL) {
    uglBitmapDestroy(gfxDevId, *pDbBmpId, gfxPartId);
    return 1;
  }

  *pFgBmpId = uglBitmapCreate(gfxDevId, pFgDib, UGL_DIB_INIT_DATA,
			 8, gfxPartId);

  if (*pFgBmpId == UGL_NULL) {
    uglBitmapDestroy(gfxDevId, *pDbBmpId, gfxPartId);
    uglBitmapDestroy(gfxDevId, *pBgBmpId, gfxPartId);
    return 1;
  }

  *pSaveBmpId = uglBitmapCreate(gfxDevId, pFgDib, UGL_DIB_INIT_DATA,
		  		8, gfxPartId);
  if (*pSaveBmpId == UGL_NULL) {
    uglBitmapDestroy(gfxDevId, *pDbBmpId, gfxPartId);
    uglBitmapDestroy(gfxDevId, *pBgBmpId, gfxPartId);
    uglBitmapDestroy(gfxDevId, *pFgBmpId, gfxPartId);
    return 1;
  }

  return 0;
}

#define BALL_SPEED 4
BOOL firstTime = TRUE;
BOOL firstTimel = TRUE;
UGL_DDB *pDbBmp, *pBgBmp, *pFgBmp, *pSaveBmp;
UGL_DDB *pFglBmp, *pSavelBmp, *pDblBmp;
int testGfx(void)
{
  UGL_MODE gfxMode;
  struct vgaHWRec oldRegs;
  UGL_RECT dbSrcRect, srcRect, saveRect;
  UGL_POINT dbPt, pt, pt0;

  semTake(sem, WAIT_FOREVER);

  vgaSave(&oldRegs);

  if (gfxDevId == UGL_NULL) {
    printf("No compatible graphics device found.\n");
    return 1;
  }

  /* Enter video mode */
  gfxMode.Width = 640;
  gfxMode.Height = 480;
  gfxMode.Depth = 4;
  gfxMode.RefreshRate = 60;
  gfxMode.Flags = UGL_MODE_INDEXED_COLOR;

  if (uglModeSet(gfxDevId, &gfxMode) != UGL_STATUS_OK) {
    vgaRestore(&oldRegs, FALSE);
    printf("Unable to set graphics mode to 640x480 @60Hz, 16 color.\n");
    return 1;
  }

  /* Set palette */
  setPalette();

  if (firstTime == TRUE) {

    if (newImg(&pDbBmp, &pBgBmp, &pFgBmp, &pSaveBmp) != 0) {
      vgaRestore(&oldRegs, FALSE);
      printf("Error initializing images.\n");
      return 1;
    }

    firstTime = FALSE;
  }

  dbSrcRect.left = 0;
  dbSrcRect.right = 640;
  dbSrcRect.top = 0;
  dbSrcRect.bottom = 480;
  dbPt.x = 0;
  dbPt.y = 0;

  /* Blit image */
  gfxDevId->defaultGc->pDefaultBitmap = UGL_DISPLAY_ID;

  srcRect.left = 0;
  srcRect.right = pBgBmp->width;
  srcRect.top = 0;
  srcRect.bottom = pBgBmp->height;
  pt.x = 640 / 2 - pBgBmp->width / 2;
  pt.y = 480 / 2- pBgBmp->height / 2;
  if (doubleBuffer == TRUE)
    (*gfxDevId->bitmapBlt)(gfxDevId, pBgBmp, &srcRect, pDbBmp, &pt);
  else
    (*gfxDevId->bitmapBlt)(gfxDevId, pBgBmp, &srcRect, UGL_DISPLAY_ID, &pt);

  srcRect.left = 0;
  srcRect.right = pFgBmp->width;
  srcRect.top = 0;
  srcRect.bottom = pFgBmp->height;
  pt.x = 0;
  pt.y = 0;

  saveRect.left = pt.x;
  saveRect.right = pt.x + pFgBmp->width;
  saveRect.top = pt.y;
  saveRect.bottom = pt.y + pFgBmp->height;
  pt0.x = 0;
  pt0.y = 0;

  while (pt.y < 480 - pFgBmp->height) {

    /* Copy background */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pDbBmp, &saveRect, pSaveBmp, &pt0);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, UGL_DISPLAY_ID, &saveRect, pSaveBmp, &pt0);

    /* Set raster operation and draw ball */
    if (rasterOp != UGL_RASTER_OP_COPY) {
      gfxDevId->defaultGc->rasterOp = rasterOp;
      uglGcSet(gfxDevId, gfxDevId->defaultGc);
    }

    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pFgBmp, &srcRect, pDbBmp, &pt);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, pFgBmp, &srcRect, UGL_DISPLAY_ID, &pt);

    if (rasterOp != UGL_RASTER_OP_COPY) {
      gfxDevId->defaultGc->rasterOp = UGL_RASTER_OP_COPY;
      uglGcSet(gfxDevId, gfxDevId->defaultGc);
    }

    /* Draw double buffer on screen */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pDbBmp, &dbSrcRect, UGL_DISPLAY_ID, &dbPt);

    /* Erase ball */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pSaveBmp, &srcRect, pDbBmp, &pt);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, pSaveBmp, &srcRect, UGL_DISPLAY_ID, &pt);

    /* Delay */
    taskDelay(animTreshold);

    /* Move ball */
    pt.x += BALL_SPEED;
    pt.y += BALL_SPEED;
    saveRect.left += BALL_SPEED;
    saveRect.right += BALL_SPEED;
    saveRect.top += BALL_SPEED;
    saveRect.bottom += BALL_SPEED;
  }

  vgaRestore(&oldRegs, FALSE);
  vgaLoadFont(font8x16, font8x16Height);
  helpDemo();

  semGive(sem);

  return 0;
}

int freeGfx(void)
{
  if (firstTime == FALSE) {
    semTake(sem, WAIT_FOREVER);

    printf("Freeing up resources for hires gfx test:\n");

    printf("Freeing double buffer image @0x%d...", pDbBmp);
    uglBitmapDestroy(gfxDevId, pDbBmp, gfxPartId);
    printf("done.\n");

    printf("Freeing background image @0x%d...", pBgBmp);
    uglBitmapDestroy(gfxDevId, pBgBmp, gfxPartId);
    printf("done.\n");

    printf("Freeing foreground image @0x%d...", pFgBmp);
    uglBitmapDestroy(gfxDevId, pFgBmp, gfxPartId);
    printf("done.\n");

    printf("Freeing background save image @0x%d...", pSaveBmp);
    uglBitmapDestroy(gfxDevId, pSaveBmp, gfxPartId);
    printf("done.\n");

    firstTime = TRUE;

    semGive(sem);
  }
  else
    printf("Hires gfx test not run yet!\n");

  if (firstTimel == FALSE) {
    semTake(sem, WAIT_FOREVER);

    printf("Freeing up resources for lores linear gfx test:\n");

    printf("Freeing double buffer image @0x%d...", pDbBmp);
    uglBitmapDestroy(gfxDevId, pDblBmp, gfxPartId);
    printf("done.\n");

    printf("Freeing foreground image @0x%d...", pFgBmp);
    uglBitmapDestroy(gfxDevId, pFglBmp, gfxPartId);
    printf("done.\n");

    printf("Freeing background save image @0x%d...", pSaveBmp);
    uglBitmapDestroy(gfxDevId, pSavelBmp, gfxPartId);
    printf("done.\n");

    firstTimel = TRUE;

    semGive(sem);
  }
  else
    printf("Lores linear gfx test not run yet!\n");
}

int incAnimTreshold(void)
{
  semTake(sem, WAIT_FOREVER);
  ++animTreshold;
  printf("Increased animation treshold to %d\n", animTreshold);
  semGive(sem);
}

int decAnimTreshold(void)
{
  semTake(sem, WAIT_FOREVER);
  if (--animTreshold < 0)
    animTreshold = 0;
  printf("Decreased animation treshold to %d\n", animTreshold);
  semGive(sem);
}

int rasterOpCopy(void)
{
  semTake(sem, WAIT_FOREVER);
  rasterOp = UGL_RASTER_OP_COPY;
  printf("Raster operation set to COPY.\n");
  semGive(sem);
}

int rasterOpAnd(void)
{
  semTake(sem, WAIT_FOREVER);
  rasterOp = UGL_RASTER_OP_AND;
  printf("Raster operation set to AND.\n");
  semGive(sem);
}

int rasterOpOr(void)
{
  semTake(sem, WAIT_FOREVER);
  rasterOp = UGL_RASTER_OP_OR;
  printf("Raster operation set to OR.\n");
  semGive(sem);
}

int rasterOpXor(void)
{
  semTake(sem, WAIT_FOREVER);
  rasterOp = UGL_RASTER_OP_XOR;
  printf("Raster operation set to XOR.\n");
  semGive(sem);
}

int toggleDoubleBuffer(void)
{
  semTake(sem, WAIT_FOREVER);
  if (doubleBuffer == TRUE) {
    doubleBuffer = FALSE;
    printf("Double buffered animation disabled.\n");
  }
  else {
    doubleBuffer = TRUE;
    printf("Double buffered animation enabled.\n");
  }
  semGive(sem);
}

int convertTest(void)
{
  int i;
  UGL_COLOR colorArray[16];

  semTake(sem, WAIT_FOREVER);
  for (i = 0; i < 16; i++)
    printf("input[%d] = 0x%x\n", i, ((UGL_COLOR *) pBgDib->pClut)[i]);
  /*
  (*gfxDevId->colorConvert)(gfxDevId, pBgDib->pClut, UGL_DEVICE_COLOR,
		            colorArray, UGL_DEVICE_COLOR_32, 16);
  */
  for (i = 0; i < 16; i++)
    printf("output[%d] = 0x%x\n", i, colorArray[i]);
  semGive(sem);
}

unsigned actStart = 0;
unsigned visStart = 0;
void setActiveStart(unsigned offset)
{
  actStart = offset;
}

void setVisibleStart(unsigned offset)
{
  visStart = offset;
  sysOutByte(0x3d4, 0x0c);
  sysOutByte(0x3d5, visStart >> 8);
  sysOutByte(0x3d4, 0x0d);
  sysOutByte(0x3d5, visStart & 0xff);
}

void setActivePage(unsigned page)
{
  setActiveStart(page * 320 * 240 / 4);
}

void setVisiblePage(unsigned page)
{
  setVisibleStart(page * 320 * 240 / 4);
}

void setPixel(int x, int y, char c)
{
  UGL_UINT8 *dest;
  UGL_UINT8 mask;
  volatile UGL_UINT8 tmp;

  mask = 0x01 << (x & 0x03);
  dest = ((UGL_UINT8 *) 0xa0000) +
	 (x >> 2) + (y * 320/4) + actStart;

  sysOutByte(0x3c4, 0x02);
  sysOutByte(0x3c5, mask);

  tmp = *dest;
  *dest = c;
}

int testGfxLo(void)
{
  int i, j;
  UGL_MODE gfxMode;
  struct vgaHWRec oldRegs;
  UGL_DDB *pBmp;
  UGL_RECT dbSrcRect;
  UGL_POINT dbPt;

  semTake(sem, WAIT_FOREVER);

  vgaSave(&oldRegs);

  if (gfxDevId == UGL_NULL) {
    printf("No compatible graphics device found.\n");
    return 1;
  }

  /* Enter video mode */
  gfxMode.Width = 320;
  gfxMode.Height = 240;
  gfxMode.Depth = 8;
  gfxMode.RefreshRate = 60;
  gfxMode.Flags = UGL_MODE_INDEXED_COLOR;

  if (uglModeSet(gfxDevId, &gfxMode) != UGL_STATUS_OK) {
    vgaRestore(&oldRegs, FALSE);
    printf("Unable to set graphics mode to 320x240 @60Hz, 256 color.\n");
    return 1;
  }

  //setPalette();

  pBmp = uglBitmapCreate(gfxDevId, UGL_NULL, UGL_DIB_INIT_VALUE,
			 15, gfxPartId);

  //setVisiblePage(1);
  //setActivePage(0);
  for (j = 0; j < 240; j++) {
    for(i = 0; i < 320; i++) {
      uglPixelSet(gfxDevId->defaultGc, i, j, j);
    }
  }
  //setVisiblePage(0);

  taskDelay(animTreshold);

  gfxDevId->defaultGc->pDefaultBitmap = pBmp;
  uglGcSet(gfxDevId, gfxDevId->defaultGc);

  //setActivePage(1);
  for (j = 0; j < 240; j++) {
    for(i = 0; i < 320; i++) {
      uglPixelSet(gfxDevId->defaultGc, i, j, (i + j) % 255);
    }
  }
  //setVisiblePage(1);

  gfxDevId->defaultGc->pDefaultBitmap = UGL_DISPLAY_ID;
  uglGcSet(gfxDevId, gfxDevId->defaultGc);

  dbSrcRect.left = 0;
  dbSrcRect.right = 320;
  dbSrcRect.top = 0;
  dbSrcRect.bottom = 240;
  dbPt.x = 0;
  dbPt.y = 0;

  (*gfxDevId->bitmapBlt)(gfxDevId, pBmp, &dbSrcRect, UGL_DISPLAY_ID, &dbPt);

  taskDelay(animTreshold);

  vgaRestore(&oldRegs, FALSE);
  vgaLoadFont(font8x16, font8x16Height);
  helpDemo();

  semGive(sem);

  return 0;
}

void setPixelInt(int x, int y, char c)
{
  UGL_UINT8 *dest;
  UGL_UINT8 mask;
  volatile UGL_UINT8 tmp;

  mask = 0x01 << (x & 0x03);
  dest = ((UGL_UINT8 *) 0xa0000) +
	 (x >> 2) + (2 * y * 320/4);

  sysOutByte(0x3c4, 0x02);
  sysOutByte(0x3c5, mask);

  tmp = *dest;
  *dest = c;
}

int testGfxLoInt(void)
{
  int i, j;
  UGL_MODE gfxMode;
  struct vgaHWRec oldRegs;

  semTake(sem, WAIT_FOREVER);

  vgaSave(&oldRegs);

  if (gfxDevId == UGL_NULL) {
    printf("No compatible graphics device found.\n");
    return 1;
  }

  /* Enter video mode */
  gfxMode.Width = 320;
  gfxMode.Height = 240;
  gfxMode.Depth = 8;
  gfxMode.RefreshRate = 60;
  gfxMode.Flags = UGL_MODE_INDEXED_COLOR|UGL_MODE_INTERLEACE_SIMULATE;

  if (uglModeSet(gfxDevId, &gfxMode) != UGL_STATUS_OK) {
    vgaRestore(&oldRegs, FALSE);
    printf("Unable to set graphics mode to 320x240 @60Hz interleace, 256 color.\n");
    return 1;
  }

  for (j = 0; j < 240; j++) {
    for(i = 0; i < 320; i++) {
      if (i == 0) setPixelInt(i, j, 1);
      else if (i == 160) setPixelInt(i, j, 1);
      else if (i == 319) setPixelInt(i, j, 1);
      else setPixelInt(i, j, j);
    }
  }

  taskDelay(animTreshold);

  for (j = 0; j < 240; j++) {
    for(i = 0; i < 320; i++) {
      if (j == 0) setPixelInt(i, j, 1);
      else if (j == 120) setPixelInt(i, j, 1);
      else if (j == 239) setPixelInt(i, j, 1);
      else setPixelInt(i, j, (i + j) % 255);
    }
  }

  taskDelay(animTreshold);

  vgaRestore(&oldRegs, FALSE);
  vgaLoadFont(font8x16, font8x16Height);
  helpDemo();

  semGive(sem);

  return 0;
}

int newImgLoLin(UGL_DDB_ID *pDbBmpId,
	        UGL_DDB_ID *pFgBmpId,
	        UGL_DDB_ID *pSaveBmpId)
{
  *pDbBmpId = uglBitmapCreate(gfxDevId, UGL_NULL, UGL_DIB_INIT_VALUE,
			 14, gfxPartId);
  if (*pDbBmpId == UGL_NULL)
    return 1;

  *pFgBmpId = uglBitmapCreate(gfxDevId, pFgDib, UGL_DIB_INIT_DATA,
			 8, gfxPartId);

  if (*pFgBmpId == UGL_NULL) {
    uglBitmapDestroy(gfxDevId, *pDbBmpId, gfxPartId);
    return 1;
  }

  *pSaveBmpId = uglBitmapCreate(gfxDevId, pFgDib, UGL_DIB_INIT_DATA,
		  		8, gfxPartId);
  if (*pSaveBmpId == UGL_NULL) {
    uglBitmapDestroy(gfxDevId, *pDbBmpId, gfxPartId);
    uglBitmapDestroy(gfxDevId, *pFgBmpId, gfxPartId);
    return 1;
  }

  return 0;
}

int testGfxLoLin(void)
{
  int i, j;
  UGL_MODE gfxMode;
  struct vgaHWRec oldRegs;
  UGL_RECT srcRect, saveRect, dbSrcRect;
  UGL_POINT pt, pt0, dbPt;

  semTake(sem, WAIT_FOREVER);

  vgaSave(&oldRegs);

  if (gfxDevId == UGL_NULL) {
    printf("No compatible graphics device found.\n");
    return 1;
  }

  /* Enter video mode */
  gfxMode.Width = 320;
  gfxMode.Height = 200;
  gfxMode.Depth = 8;
  gfxMode.RefreshRate = 60;
  gfxMode.Flags = UGL_MODE_INDEXED_COLOR;

  if (uglModeSet(gfxDevId, &gfxMode) != UGL_STATUS_OK) {
    vgaRestore(&oldRegs, FALSE);
    printf("Unable to set graphics mode to 320x200 @60Hz, 256 color.\n");
    return 1;
  }

  setPalette();

  if (firstTimel == TRUE) {

    if (newImgLoLin(&pDblBmp, &pFglBmp, &pSavelBmp) != 0) {
      vgaRestore(&oldRegs, FALSE);
      printf("Error initializing images.\n");
      return 1;
    }

    firstTimel = FALSE;
  }

  dbSrcRect.left = 0;
  dbSrcRect.right = 320;
  dbSrcRect.top = 0;
  dbSrcRect.bottom = 200;
  dbPt.x = 0;
  dbPt.y = 0;

  if (doubleBuffer == TRUE) {
    gfxDevId->defaultGc->pDefaultBitmap = pDblBmp;
    uglGcSet(gfxDevId, gfxDevId->defaultGc);
  }

  /* Draw background */
  for (j = 0; j < 200; j++) {
    for(i = 0; i < 320; i++) {
      uglPixelSet(gfxDevId->defaultGc, i, j, (i + j) % 255);
    }
  }

  if (doubleBuffer == TRUE) {
    gfxDevId->defaultGc->pDefaultBitmap = UGL_DISPLAY_ID;
    uglGcSet(gfxDevId, gfxDevId->defaultGc);
  }

  srcRect.left = 0;
  srcRect.right = pFgDib->width;
  srcRect.top = 0;
  srcRect.bottom = pFgDib->height;
  pt.x = 0;
  pt.y = 0;

  saveRect.left = pt.x;
  saveRect.right = pt.x + pFgDib->width;
  saveRect.top = pt.y;
  saveRect.bottom = pt.y + pFgDib->height;
  pt0.x = 0;
  pt0.y = 0;

  while(pt.y < 200 - pFgDib->height) {

    /* Copy background */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pDblBmp, &saveRect, pSavelBmp, &pt0);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, UGL_DISPLAY_ID, &saveRect, pSavelBmp, &pt0);

    /* Set raster operation and draw ball */
    if (rasterOp != UGL_RASTER_OP_COPY) {
      gfxDevId->defaultGc->rasterOp = rasterOp;
      uglGcSet(gfxDevId, gfxDevId->defaultGc);
    }

    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pFglBmp, &srcRect, pDblBmp, &pt);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, pFglBmp, &srcRect, UGL_DISPLAY_ID, &pt);

    if (rasterOp != UGL_RASTER_OP_COPY) {
      gfxDevId->defaultGc->rasterOp = UGL_RASTER_OP_COPY;
      uglGcSet(gfxDevId, gfxDevId->defaultGc);
    }

    /* Draw double buffer on screen */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pDblBmp, &dbSrcRect, UGL_DISPLAY_ID, &dbPt);

    /* Erase ball */
    if (doubleBuffer == TRUE)
      (*gfxDevId->bitmapBlt)(gfxDevId, pSavelBmp, &srcRect, pDblBmp, &pt);
    else
      (*gfxDevId->bitmapBlt)(gfxDevId, pSavelBmp, &srcRect, UGL_DISPLAY_ID, &pt);

    /* Delay */
    taskDelay(animTreshold);

    /* Move ball */
    pt.x += BALL_SPEED;
    pt.y += BALL_SPEED;
    saveRect.left += BALL_SPEED;
    saveRect.right += BALL_SPEED;
    saveRect.top += BALL_SPEED;
    saveRect.bottom += BALL_SPEED;
  }

  vgaRestore(&oldRegs, FALSE);
  vgaLoadFont(font8x16, font8x16Height);
  helpDemo();

  semGive(sem);

  return 0;
}

#endif /* INCLUDE_GFX */

#include <setjmp.h>

static jmp_buf jbuf;

void second(void)
{
  printf("Begin: second()\n");
  printf("Calling longjmp()\n");
  longjmp(jbuf, 1);
  printf("End: second()\n");
}

void first(void)
{
  printf("Begin: first()\n");
  second();
  printf("End: first()\n");
}

int testJmp(void)
{
  printf("Begin: main()\n");

  if (! setjmp(jbuf) ) {
    printf("Main: setjmp() == 0\n");
    first();
  }
  else {
    printf("Main: setjmp() != 0\n");
  }

  printf("End: main()\n");

  return 0;
}

void handleKill(int signo, siginfo_t *info, void *f)
{
  printf("Caught signal sent by ");
  switch(info->si_code) {
    case SI_KILL: printf("kill()\n"); break;
    case SI_QUEUE: printf("sigqueue()\n"); break;
    default: printf("unknown\n"); break;
  }

  exit(1);
}

int installHandler(void)
{
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = handleKill;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;

  /* Install handler */
  if (sigaction(SIGCHLD, &sa, NULL) != OK) {
    printf("Error - Unable to install signal handler\n");
    return -1;
  }

  return 0;
}

int waitSignal(void)
{
  if (installHandler() != 0)
    return -1;

  printf("Wating for signal SIGCHLD (%d):\n", SIGCHLD);
  for (;;) {

    pause();
    printf(".");

  }

  printf("Out of signal wait.\n");

  return 0;
}

int selfSignal(void)
{
  if (installHandler() != 0)
    return -1;

  printf("Wating for signal SIGCHLD (%d):\n", SIGCHLD);
  raise(SIGCHLD);
  printf("Out of signal wait.\n");

  return 0;
}

int sendSignal(int taskId)
{
  kill(taskId, SIGCHLD);

  return 0;
}

int sendQSignal(int taskId, int i)
{
  union sigval value;

  value.sival_int = i;
  if ( sigqueue(taskId, SIGCHLD, value) == ERROR) {

    printf("Error sending signal.\n");
    return -1;

  }

  return 0;
}

int rsPeriodicTask(void)
{
  if (!periodicState) {
    taskResume(periodicTaskId);
    periodicState = TRUE;
  }
  else {
    taskSuspend(periodicTaskId);
    periodicState = FALSE;
  }

  return 0;
}

int prTask(void)
{
  int i, n;

  while (1) {

    taskDelay(PERIODIC_INTERVAL);

    semTake(sem, WAIT_FOREVER);
    printf("prTask: Value = %d\n", periodicValue);

    n = taskVarInfo(0, NULL, 256);
    if (n > 0) {

      printf("Got %d task variable(s)\n", n);
      for (i = 0; i < n; i++)
        printf("Task var: %d @ %x\n", taskVarGet(0, &var[i]), &var[i]);

    }

    semGive(sem);

  }

  return 0;
}

int getExcVal(int i)
{
  int val;

  val = 1000 / i;

  return val;
}

int genExc(void)
{
  int i, a;

  while (1) {

    a = 10;

    for (i = 100; i >= 0; i--)
      a += getExcVal(i);

  }

  return 0;
}

int assertExpr(int expr)
{
  assert(expr);

  return OK;
}

int testIo(void)
{
  int i;
  int fd = 0;
  char *fns[] = { "/pcConsole/0", "/pcConsole/1", "/pcConsole/2" };

  printf("stdin: %d, stdout: %d, stderr: %d\n",
	 ioGlobalStdGet(0),
	 ioGlobalStdGet(1),
	 ioGlobalStdGet(2));

  for (i = 0; i < NELEMENTS(fns); i++) {

    printf("Searching for %s...", fns[i]);
    fd = open(fns[i], O_RDWR, 0);
    printf("Found fd: %d.\n", fd);

    printf("Closing fd: %d\n", fd);
    if (close(fd) == EOF)
      printf("Error.\n");
    else
      printf("Done.\n");
  }

}

int setAbortChar(int c)
{
  printf("Setting abort character to: %d = '%c'...", c, c);
  ttyAbortSet(c);
  printf("Done.\n");

  return c;
}

int selfRestarter(void)
{
  printf("Task started.\n");

  printf("Holding on for self restart...");
  taskDelay(DELAY_LONG);
  printf("Restarting.\n");
  taskRestart(0);

  printf("Holding on for task end...");
  taskDelay(DELAY_LONG);
  printf("Task ended.\n");

  return 0;
}

int showTaskVars(int taskId)
{
  int i, n;
  TASK_VAR varList[256];

  if ( taskIdVerify(taskId) != OK ) {

    printf("usage: showTaskVars <task-id>\n");
    return -1;

  }

  n = taskVarInfo(taskId, varList, 255);
  if (n <= 0) {

    printf("No variables added.\n");
    return 0;

  }

  for (i = 0; i < n; i++)
    printf("var[%d] = %d\n", i, varList[i].value);

  printf("Total of: %d variable(s) in task.\n", n);

  return n;
}

int addTaskVars(int taskId, int n, int x0, int dx)
{
  int i, x;

  if ( (taskIdVerify(taskId) != OK) || (n <= 0) || (n >= 256) ) {

    printf("usage: addTaskVars <task-id>, <num[0-255]>, <start-value>, <delta>\n");
    return -1;

  }

  for (i = 0, x = x0; i < n; i++, x += dx) {

    var[i] = x;

    if ( taskVarAdd(taskId, &var[i]) != OK) {

      printf("Unable to add task var %d = %d @ %x\n", i, var[i], &var[i]);
      return -n;

    }

    printf("Added task var: %d = %d @ %x\n", i, var[i], &var[i]);

  }

  printf("%d task variable(s) where added.\n", n);

  return n;
}

int getTaskVar(int taskId, int *pVar)
{
  int val;
  if ( taskIdVerify(taskId) != OK ) {

    printf("usage: getTaskVar <task-id> <var-addr>\n");
    return -1;

  }

  val = taskVarGet(taskId, pVar);
  if (val == ERROR) {

    printf("Unable to find variable @ %x\n");
    return -1;

  }

  printf("Got variable: %d @ %x\n", val, pVar);

  return val;
}

int setTaskVar(int taskId, int *pVar, int value)
{
  if ( taskIdVerify(taskId) != OK ) {

    printf("usage: setTaskVar <task-id> <var-addr> <value>\n");
    return -1;

  }

  if ( taskVarSet(taskId, pVar, value) != OK ) {

    printf("Unable to find variable @ %x\n");
    return -1;

  }

  printf("Variable set: %d @ %x\n", value, pVar);

  return value;
}

int taskHiPrio(int n)
{
  int i;

  for (i = 0; i < n; i++);

  printf("taskHiPrio done.\n");

  return i;
}

int taskLoPrio(int n)
{
  int i;

  for (i = 0; i < n; i++);

  printf("taskLoPrio done.\n");

  return i;
}

int taskPrioTest(int n1, int n2, int dly)
{
  printf("High priority task treshold: %d.\n"
         "Low priority task treshold: %d.\n"
         "Delay between task starts: %d\n",
	 n1, n2, dly);

  printf("Spawning low priority task...");
        taskSpawn("tLoPrio",
		   200,
		   0,
	           DEFAULT_STACK_SIZE,
		   (FUNCPTR) taskLoPrio,
	            (ARG) n2,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0);
  printf("Done.\n");

  taskDelay(dly);

  printf("Spawning high priority task...");
        taskSpawn("tHiPrio",
		   2,
		   0,
	           DEFAULT_STACK_SIZE,
		   (FUNCPTR) taskHiPrio,
	           (ARG) n1,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0,
	            0);
  printf("Done.\n");
}

void classList(void)
{
  printf("rootClassId      : %#x\n"
	 "taskClassId      : %#x\n"
	 "memPartClassId   : %#x\n"
	 "semClassId       : %#x\n"
	 "msgQClassId      : %#x\n"
	 "vmContextClassId : %#x\n"
	 "hashClassId      : %#x\n"
	 "symTableClassId  : %#x\n"
	 "fpClassId        : %#x\n"
	 "cbioClassId      : %#x\n"
	 "qFifoClassId     : %#x\n"
	 "qPrioClassId     : %#x\n"
	 "qPriBmpClassId   : %#x\n"
	 "qMsgClassId      : %#x\n",
	 rootClassId,
	 taskClassId,
	 memPartClassId,
	 semClassId,
	 msgQClassId,
	 vmContextClassId,
	 hashClassId,
	 symTableClassId,
	 fpClassId,
	 cbioClassId,
	 qFifoClassId,
	 qPrioClassId,
	 qPriBmpClassId,
	 qMsgClassId);
}

int readSelect(int fd)
{
  char line[512];
  int result, len;
  fd_set readset;
  struct timeval tv;

  FD_ZERO(&readset);
  FD_SET(fd, &readset);

  tv.tv_sec = 10;
  tv.tv_usec = 0;

  printf("Read select on file %d...\n", fd);
  result = select(fd + 1, &readset, NULL, NULL, &tv);

  if (result > 0) {

    printf("Data is available now.\n");

    len = read(fd, line, 512);
    if (len)
      printf("Got %d bytes: %s\n", len, line);

  }
  else if (result == 0) {

    printf("No data within five seconds.\n");

  }

  else {

    printf("Error - Unable to select on file descriptor %d\n", fd);
    return -1;

  }

  return 0;
}

int demoWrite(int fd, int pos)
{
  int nBytes;
  int len;
  int currPos;

  if (pos > 0)
    lseek(fd, pos, SEEK_SET);

  if (ioctl(fd, FIOWHERE, (int) &currPos) != OK)
    currPos = 0;

  len = strlen(bigString);

  printf("Writing to file %d at position %d...", fd, currPos);
  nBytes = write(fd, bigString, len + 1);
  if (nBytes != (len + 1)) {
    printf("Error - %d of %d bytes where written\n", nBytes, len + 1);
    return -1;
  }
  printf("Done.\n");

  return 0;
}

int demoRead(int fd, int pos)
{
  char buf[512];
  int nBytes;
  int currPos;

  if (pos > 0)
    lseek(fd, pos, SEEK_SET);

  if (ioctl(fd, FIOWHERE, (int) &currPos) != OK)
    currPos = 0;

  printf("Reading from file %d at position %d...", fd, currPos);
  nBytes = read(fd, buf, 512);
  if (nBytes > 0)
    printf("Read data:\n%s\n", buf);

  return 0;
}

int netSocketNew(void)
{
  int fd;

  if ( (fd = socket(PF_UNIX, SOCK_STREAM, 0)) == ERROR ) {

    printf("Error - Unable to create socket.\n");
    return ERROR;

  }

  printf("Socket created with fd: %d\n", fd);

  return fd;
}

int netSocketDelete(int fd)
{
  if (shutdown(fd, 0x02) != OK) {

    printf("Error - Unable to shutdown socket.\n");
    return -1;

  }

  return close(fd);
}

int netSocketBind(int fd, char *name)
{
  struct sockaddr sa;

  strcpy(sa.sa_data, name);
  sa.sa_family = AF_UNIX;
  sa.sa_len = strlen(sa.sa_data);

  return bind(fd, &sa, sizeof(struct sockaddr));
}

int netSocketConnect(int fd, char *name)
{
  struct sockaddr sa;

  strcpy(sa.sa_data, name);
  sa.sa_family = AF_UNIX;
  sa.sa_len = strlen(sa.sa_data);

  return connect(fd, &sa, sizeof(struct sockaddr));
}

int netServer(char *name)
{
  struct sockaddr sa;
  int fd, con_fd, socklen;

  if ( (fd = socket(PF_UNIX, SOCK_STREAM, 0)) == ERROR ) {

    printf("netServer: Error - Unable to create socket.\n");
    return ERROR;

  }

  if ( netSocketBind(fd, name) == ERROR) {

    printf("netServer: Error - Unable to bind to address.\n");
    return ERROR;

  }

  socklen = sizeof(sa);
  listen(fd, 0);

  printf("netServer: Listening for connections.\n");
  while ( (con_fd = accept(fd, &sa, &socklen)) > 0 ) {

    printf("netServer: Got connection: with fd: %d (%s)\n",
	    con_fd, sa.sa_data);

  }
  printf("netServer: Stoped listening for connections.\n");

  return OK;
}

int netClient(char *name)
{
  int fd;

  if ( (fd = socket(PF_UNIX, SOCK_STREAM, 0)) == ERROR ) {

    printf("netClient: Error - Unable to create socket.\n");
    return ERROR;

  }

  if ( netSocketConnect(fd, name) != ERROR) {

    printf("netClient: Connected to server with fd: %d\n", fd);
    return fd;

  }

  return ERROR;
}

#include <net/radix.h>
#include <net/route.h>
#include <net/if.h>
#include <net/raw_cb.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include <netinet/tcp_var.h>
#include <netinet/ip_var.h>
#include <netinet/inetLib.h>
IMPORT struct inpcbinfo udbinfo;

int inAddRoute(char *adr, char *gate, int mask)
{
  int i, status;
  struct sockaddr_in addr;
  struct sockaddr_in gw;
  struct sockaddr_rt msk;

  addr.sin_len = sizeof(struct sockaddr_in);
  addr.sin_family = AF_INET;
  addr.sin_port = 80;
  addr.sin_addr.s_addr = inet_addr(adr);
  for (i = 0; i < 8; i++)
    addr.sin_zero[i] = 0;

  gw.sin_len = sizeof(struct sockaddr_in);
  gw.sin_family = AF_INET;
  gw.sin_port = 80;
  gw.sin_addr.s_addr = inet_addr(gate);
  for (i = 0; i < 8; i++)
    addr.sin_zero[i] = 0;

  if (mask != 0) {

    msk.srt_len = sizeof(struct sockaddr_rt);
    msk.srt_family = AF_INET;
    msk.srt_proto = SOCK_STREAM;
    msk.srt_tos = 0;
    msk.srt_addr.s_addr = mask;
    for (i = 0; i < 8; i++)
      msk.srt_zero[i] = 0;

    if ( (status = rtrequest(RTM_ADD,
			     (struct sockaddr *) &addr,
			     (struct sockaddr *) &gw,
			     (struct sockaddr *) &msk,
			     0,
			     NULL)) != 0) {

      printf("inAddRoute: Error - Unable to add route %s(%d).\n", adr, status);
      return status;

    }
    printf("inAddRoute: Added route: %s, gateway: %s, netmask: %d.\n",
	   adr, gate, mask);

  }

  else {

    if ( (status = rtrequest(RTM_ADD,
			     (struct sockaddr *) &addr,
			     (struct sockaddr *) &gw,
			     NULL,
			     0,
			     NULL)) != 0) {

      printf("inAddRoute: Error - Unable to add route %s(%d).\n", adr, status);
      return status;

    }

    printf("inAddRoute: Added route: %d, gateway: %s.\n", adr, gate);

  }

  return OK;
}

IMPORT unsigned short udp_pcbhashsize;
void inLogAddr(struct in_addr addr);
int udpCheck(void)
{
  int i;
  struct inpcb *inp;
  LIST_NODE *pNode;

  printf("UDP hashsize %d\n", udp_pcbhashsize);

  printf("UDP hash nodes:\n");
  for (inp = (struct inpcb *) LIST_HEAD(udbinfo.listhead);
       inp != NULL;
       inp = (struct inpcb *) LIST_NEXT(&inp->inp_list) ) {

    printf("Node: %#x\n", inp);
    inLogAddr(inp->inp_laddr);
    printf(":%d->", ntohs(inp->inp_lport));
    inLogAddr(inp->inp_faddr);
    printf(":%d ", ntohs(inp->inp_fport));
    printf("in bucket: %d\n", in_pcbhash(inp));

  }


  for (i = 0; i < udp_pcbhashsize; i++) {

    if (listCount(&udbinfo.hashbase[i])) {

      pNode = LIST_HEAD(&udbinfo.hashbase[i]);
      inp = (struct inpcb *) (((char *) pNode) -
				OFFSET(struct inpcb, inp_hash) );
      printf("%d [%d] @%#8x",
	     i,
	     listCount(&udbinfo.hashbase[i]),
	     inp);

      for (; pNode != NULL; pNode = LIST_NEXT(pNode))
        printf(".");

      printf("\n");

    }

  }

  return OK;
}

#define MAX_LINE 1000
int tcpServer(char *string, int port)
{
  int fd, len, con_fd, err, socklen;
  unsigned long addr;
  struct sockaddr_in sin, cliaddr;
  char buf[MAX_LINE + 1];

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("tcpServer: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  if ( (fd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR ) {

    printf("tcpServer: Error - Unable to create socket.\n");
    return ERROR;

  }

  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_len = sizeof(struct sockaddr_in);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = addr;

  err = bind(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  if (err) {

    printf("tcpServer: Error - Unable to bind to %s:%d\n", string, port);
    return err;

  }

  listen(fd, 5);
  socklen = sizeof(struct sockaddr_in);

  printf("tcpServer: Waiting for connections.\n");
  while ( (con_fd = accept(fd, (struct sockaddr *) &cliaddr, &socklen)) > 0 ) {

    printf("tcpServer: Got connection: with fd: %d\n", con_fd);

    printf("tcpServer: Waiting for data...");
    len = read(con_fd, buf, MAX_LINE);
    printf("Got data with length: %d.\n", len);

    printf("tcpServer: Echoing data...");
    len = write(con_fd, buf, len);
    printf("Done with length: %d.\n", len);

    printf("tcpServer: Closing connection...");
    close(con_fd);
    printf("Done.\n");

  }

  printf("tcpServer: Stoped listening for connections.\n");

  return OK;
}

int udpServer(char *string, int port)
{
  int fd, n, len, err;
  unsigned long addr;
  struct sockaddr_in sin, cliaddr;
  char buf[MAX_LINE + 1];

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("udpServer: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  if ( (fd = socket(PF_INET, SOCK_DGRAM, 0)) == ERROR ) {

    printf("udpServer: Error - Unable to create socket.\n");
    return ERROR;

  }

  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_len = sizeof(struct sockaddr_in);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = addr;

  err = bind(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  if (err) {

    printf("udpServer: Error - Unable to bind to %s:%d\n", string, port);
    return err;

  }

  while (1) {

    printf("udpServer: Waiting for data...");
    len = sizeof(struct sockaddr_in);
    n = 0;
    n = recvfrom(fd, buf, MAX_LINE, 0, (struct sockaddr *) &cliaddr, &len);

    printf("Got data with length: %d.\n", n);

    printf("udpServer: Echoing data...");
    sendto(fd, buf, n, 0, (struct sockaddr *) &cliaddr, len);
    printf("Done with length: %d.\n", n);

  }

  return OK;
}

int tcpClient(char *string, int port)
{
  int fd, err;
  unsigned long addr;
  struct sockaddr_in sin;

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("tcpClient: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  if ( (fd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR ) {

    printf("tcpClient: Error - Unable to create socket.\n");
    return ERROR;

  }

  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_len = sizeof(struct sockaddr_in);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = addr;

  printf("tcpClient: Connecting to: %s:%d...", string, port);

  err = connect(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  if (err) {

    printf("tcpClient: Error - Unable to connect to %s:%d\n", string, port);
    return err;
  }

  printf("tcpClient: Connected to %s:%d with fd: %d\n", string, port, fd);

  return fd;
}

int udpClient(char *string, int port)
{
  int fd, n;
  unsigned long addr;
  struct sockaddr_in sin;
  char buf[MAX_LINE + 1];

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("udpClient: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  if ( (fd = socket(PF_INET, SOCK_DGRAM, 0)) == ERROR ) {

    printf("udpClient: Error - Unable to create socket.\n");
    return ERROR;

  }

  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_len = sizeof(struct sockaddr_in);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = addr;

  printf("udpClient: Sending data...");
  sendto(fd, bigString, strlen(bigString), 0, (struct sockaddr *) &sin,
	 sizeof(struct sockaddr_in));
  printf("Done.\n");

  printf("udpClient: Receiving data...");
  n = recvfrom(fd, buf, MAX_LINE, 0, NULL, NULL);
  printf("Done.\n");

  buf[n] = '\0';

  printf("udpClient: Returned data: %s\n");

  return OK;
}

int rawSocketNew(int on)
{
  int fd;

  if ( (fd = socket(PF_INET, SOCK_RAW, 0)) == ERROR ) {

    printf("Error - Unable to create socket.\n");
    return ERROR;

  }

  if ( setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0 ) {

    printf("Error - Unable to set socket option: IP_HDRINCL\n");
    return ERROR;

  }

  printf("Socket created with fd: %d\n", fd);

  return fd;
}

int rawSocketConnect(int fd, char *string)
{
  int i;
  unsigned long addr;
  struct sockaddr_in sin;

  printf("rawSocketConnect: Connect to internet address %s\n", string);

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("rawSocketConnect: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  printf("rawSocketConnect: To integer address: %d(%#x)\n", addr, addr);

  sin.sin_family = AF_INET;
  sin.sin_len = sizeof(struct sockaddr_in);
  sin.sin_port = 0;
  sin.sin_addr.s_addr = addr;
  for (i = 0; i < 8; i++)
    sin.sin_zero[i] = 0;

  return connect(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
}

int ipRead(int fd)
{
  char str[INET_ADDR_LEN];
  char buf[512];
  int nBytes;

  printf("Reading from file %d...", fd);
  nBytes = read(fd, buf, 512);
  if (nBytes > 0) {

    printf("%d bytes received\n", nBytes);

    struct ip *ip = (struct ip *) buf;
    printf("ip_len: %d\n", ip->ip_len);

    inet_ntoa_b(ip->ip_src.s_addr, str);
    printf("ip_src: %s\n", str);

    inet_ntoa_b(ip->ip_dst.s_addr, str);
    printf("ip_dst: %s\n", str);

    if (nBytes > sizeof(struct ip))
      printf("== MESSAGE ==\n%s\n", buf + sizeof(struct ip));

  }

  return 0;
}

void inLogAddr(struct in_addr addr)
{
  char str[16];

  inet_ntoa_b(addr.s_addr, str);
  printf("%s|%#x|", str, taskIdSelf());
}

void inLogSock(struct sockaddr *sa)
{
  struct sockaddr_in *sin;
  char str[16];

  sin = (struct sockaddr_in *) sa;

  inet_ntoa_b(sin->sin_addr.s_addr, str);
  printf("%s|%#x|", str, taskIdSelf());
}

int inSetAddr(char *string)
{
  int status;
  unsigned long addr;

  printf("inSetAddr: Set to internet address %s\n", string);

  addr = inet_addr(string);
  if (addr == ERROR) {

    printf("inSetAddr: Error - Invalid internet address %s\n", string);
    return ERROR;

  }

  /* Bring up interface */
  if_up(ifunit("lo0"));

  status = ifAddrAdd("lo0", string, NULL, 0);
  if (status != OK) {

    printf("inSetAddr: Error - Unable to set interface address %s\n", string);
    return ERROR;

  }

  return OK;
}

void netTestShell(void);

#include <drv/disk/ramDrv.h>
#include <oldfs/rawFsLib.h>
#include <oldfs/rt11FsLib.h>
#include <oldfs/dosFsLib.h>
#include <oldfs/dosFsShow.h>
int ramDiskInit(char *name)
{
  int fd;
  char devname[PATH_MAX];
  BLK_DEV *pDev;
#ifdef RAWFS
  RAW_VOL_DESC *pVol;
#else
  RT_VOL_DESC *pVol;
#endif

  if (name == NULL)
    strcpy(devname, "/RAM/");
  else
    strcpy(devname, name);

  pDev = ramDevCreate(NULL, 0, 0, 0, 0);
  if (pDev == NULL) {

    fprintf(stderr, "Error - Unable to create device %s\n", devname);
    return ERROR;

  }

#ifdef RAWFS
  if (rawFsLibInit(10) != OK) {
#else
  if (rt11FsLibInit(10) != OK) {
#endif

    fprintf(stderr, "Error - Unable to initialize fs library\n");
    return ERROR;

  }


#ifdef RAWFS
  pVol = rawFsDevInit(devname, pDev);
#else
  pVol = rt11FsDevInit(devname, pDev, FALSE, 100, FALSE);
#endif
  if (pVol == NULL) {

    fprintf(stderr, "Error - Unable to initialize device %s\n", devname);
    return ERROR;

  }

  fd = open(devname, O_RDWR, 0777);
  if (fd == ERROR) {

    fprintf(stderr, "Error - Unable to open device %s.\n", devname);
    return ERROR;

  }

  if (ioctl(fd, FIODISKINIT, 0) != OK) {

    fprintf(stderr, "Error - Unable to initialize filesystem on device %s\n",
            devname);
    return ERROR;
  }

  if (close(fd) == ERROR) {

    fprintf(stderr, "Error - Unable to close device %s\n", devname);
    return ERROR;

  }

  chdir(devname);

  return OK;
}

#define DOSFS
#include <drv/fdisk/nec765Fd.h>
int floppyDiskInit(int drive, int fsType, BOOL format, BOOL lowlevel)
{
  int fd;
  char devname[PATH_MAX];
  BLK_DEV *pDev;
  DOS_VOL_DESC_ID pDosVol;
  RT_VOL_DESC *pRtVol;

  if (fdDrvInit(0x26, 0x06) != OK) {

    fprintf(stderr, "Error - Unable to initialize floppy driver\n");
    return ERROR;

  }

  sprintf(devname, "/FD%d/", drive);

  pDev = fdDevCreate(drive, 0, 0, 0);
  if (pDev == NULL) {

    fprintf(stderr, "Error - Unable to create device %s\n", devname);
    return ERROR;

  }

  switch (fsType) {

    case 1 :

    if (dosFsLibInit() != OK) {

       fprintf(stderr, "Error - Unable to initialize fs library\n");
       return ERROR;

     }

      pDosVol = dosFsDevInit(devname, pDev, 10, FALSE);
      if (pDosVol == NULL) {

        fprintf(stderr, "Error - Unable to initialize device %s\n", devname);
        return ERROR;

      }

    break;

    default:

      if (rt11FsLibInit(10) != OK) {

        fprintf(stderr, "Error - Unable to initialize fs library\n");
        return ERROR;
      }

      pRtVol = rt11FsDevInit(devname, pDev, FALSE, 100, FALSE);
      if (pRtVol == NULL) {

        fprintf(stderr, "Error - Unable to initialize device %s\n", devname);
        return ERROR;

      }

    break;

  }

  if (format) {

  printf("Formating device %s...", devname);

    fd = open(devname, O_RDWR, 0777);
    if (fd == ERROR) {

      fprintf(stderr, "Error - Unable to open device %s.\n", devname);
      return ERROR;

    }

    if (lowlevel) {

      printf("Low level...");
      if (ioctl(fd, FIODISKFORMAT, 0) != OK) {

        close(fd);
        fprintf(stderr, "Error - Unable to format device %s\n", devname);
        return ERROR;

      }
      printf("Done.");

    }

    if (ioctl(fd, FIODISKINIT, 0) != OK) {

      close(fd);
      fprintf(stderr, "Error - Unable to initialize filesystem on device %s\n",
	      devname);
      return ERROR;
    }

    if (close(fd) == ERROR) {

      fprintf(stderr, "Error - Unable to close device %s\n", devname);
      return ERROR;

    }

    printf("Done.\n");

  }

  chdir(devname);

  return OK;
}

int setLabel(char *devname, char *label)
{
  int fd;
  char name[255];

  fd = open(devname, O_RDONLY, 0777);
  if (fd == ERROR) {

    fprintf(stderr, "Error - Unable to open device %s\n", devname);
    return ERROR;

  }

  strcpy(name, label);

  if (ioctl(fd, FIOLABELSET, (int) label) == ERROR) {

    fprintf(stderr, "Error - Unable to set label\n");
    return ERROR;

  }

  close(fd);

  return OK;
}

int createFile(char *name)
{
  int fd;
  if ( (fd = creat(name, O_RDWR)) == ERROR) {

    fprintf(stderr, "Error - Unable to create file: %s\n", name);
    return ERROR;

  }

  printf("File created with fd: %d\n", fd);

  return fd;
}

int openFile(char *name)
{
  int fd;

  if ( (fd = open(name, O_RDWR, 0777)) == ERROR) {

    fprintf(stderr, "Error - Unable to open file: %s\n", name);
    return ERROR;

  }

  printf("File opened with fd: %d\n", fd);

  return fd;
}

int closeFile(int fd)
{
  if (close(fd) == ERROR) {

    fprintf(stderr, "Error - Unable to close file.\n");
    return ERROR;

  }

  printf("File closed\n");

  return OK;
}

int removeFile(char *name)
{
  if (remove(name) == ERROR) {

    fprintf(stderr, "Error - Unable to remove file %s\n", name);
    return ERROR;

  }

  printf("File %s removed\n", name);

  return OK;
}

int showFsInfo(char *devname, int level)
{
/*
  int fd;

  fd = open(devname, O_RDONLY, 0777);
  if (fd == ERROR) {

    fprintf(stderr, "Error - Unable to open device\n");
    return ERROR;

  }
  close(fd);
*/

#ifdef DOSFS
  dosFsShow(devname, level);
#endif

  return OK;
}

#include <sys/stat.h>
int fileStat(char *name)
{
  int fd;
  struct stat status;

  if (stat(name, &status) != OK) {

    fprintf(stderr, "Error - Unable to get file status for: %s\n", name);
    return ERROR;

  }

  printf("%-24s %8s\n", "name", "size");
  printf("%-24s %8d\n", name, status.st_size);

  return OK;
}

int fileRename(char *name, char *newName)
{
  int fd;

  if ( (fd = open(name, O_RDWR, 0777)) == ERROR) {

    fprintf(stderr, "Error - Unable to open file: %s\n", name);
    return ERROR;

  }

  if (ioctl(fd, FIORENAME, (int) newName) != OK) {

    fprintf(stderr, "Error - Unable to rename file: %s to %s\n", name, newName);
    return ERROR;

  }

  printf("File: %s renamed to: %s\n", name, newName);

  return OK;
}

#include <rtos/wdLib.h>
WDOG_ID wdId;
void wdFire(int arg)
{
  printf("Watchdog timer fire function with argument: %d\n", arg);
}

STATUS startWd(int secs, int arg)
{
  printf("Starting watchdog timer with delay: %d secod(s) and argument: %d...",
	 secs, arg);

  if ( wdStart(wdId, sysClockRateGet() * secs, (FUNCPTR) wdFire, (ARG) arg) ) {

    fprintf(stderr, "Error - Unable to start watchdog timer.\n");
    return ERROR;

  }

  printf("Done.\n");

  return OK;
}

int demo(void);

SYMBOL symDemo = {NULL, "_demo", demo, 0, N_TEXT | N_EXT};
void addDemo(void)
{
  symTableAdd(sysSymTable, &symDemo);
}

int helpDemo(void);
SYMBOL symTableDemo[] = {
  {NULL, "_tyler", tyler, 0, N_TEXT | N_EXT },
  {NULL, "tyler", tyler, 0, N_TEXT | N_EXT },
  {NULL, "_createHook", createHook, 0, N_TEXT | N_EXT},
  {NULL, "_deleteHook", deleteHook, 0, N_TEXT | N_EXT},
  {NULL, "_switchHook", switchHook, 0, N_TEXT | N_EXT},
  {NULL, "_swapHook", swapHook, 0, N_TEXT | N_EXT},
  {NULL, "_helpDemo", helpDemo, 0, N_TEXT | N_EXT},
  {NULL, "_showSysTime", showSysTime, 0, N_TEXT | N_EXT},
  {NULL, "_showBigString", showBigString, 0, N_TEXT | N_EXT},
  {NULL, "_slowFill", slowFill, 0, N_TEXT | N_EXT},
  {NULL, "_showInfo", showInfo, 0, N_TEXT | N_EXT},
  {NULL, "_sendMessage", sendMessage, 0, N_TEXT | N_EXT},
  {NULL, "_receiveMessage", receiveMessage, 0, N_TEXT | N_EXT},
  {NULL, "_msgListen", msgListen, 0, N_TEXT | N_EXT},
  {NULL, "_messageListener", messageListener, 0, N_TEXT | N_EXT},
  {NULL, "_semListen", semListen, 0, N_TEXT | N_EXT},
  {NULL, "_semaphoreListener", semaphoreListener, 0, N_TEXT | N_EXT},
  {NULL, "_semUse", semUse, 0, N_TEXT | N_EXT},
  {NULL, "_testStdio", testStdio, 0, N_TEXT | N_EXT},
  {NULL, "_testSymbols", testSymbols, 0, N_TEXT | N_EXT},

#ifdef INCLUDE_GFX

  {NULL, "_testGfx", testGfx, 0, N_TEXT | N_EXT},
  {NULL, "_freeGfx", freeGfx, 0, N_TEXT | N_EXT},
  {NULL, "_incAnimTreshold", incAnimTreshold, 0, N_TEXT | N_EXT},
  {NULL, "_decAnimTreshold", decAnimTreshold, 0, N_TEXT | N_EXT},
  {NULL, "_rasterOpCopy", rasterOpCopy, 0, N_TEXT | N_EXT},
  {NULL, "_rasterOpAnd", rasterOpAnd, 0, N_TEXT | N_EXT},
  {NULL, "_rasterOpOr", rasterOpOr, 0, N_TEXT | N_EXT},
  {NULL, "_rasterOpXor", rasterOpXor, 0, N_TEXT | N_EXT},
  {NULL, "_toggleDoubleBuffer", toggleDoubleBuffer, 0, N_TEXT | N_EXT},
  {NULL, "_convertTest", convertTest, 0, N_TEXT | N_EXT},
  {NULL, "_testGfxLo", testGfxLo, 0, N_TEXT | N_EXT},
  {NULL, "_testGfxLoInt", testGfxLoInt, 0, N_TEXT | N_EXT},
  {NULL, "_testGfxLoLin", testGfxLoLin, 0, N_TEXT | N_EXT},

#endif /* INCLUDE_GFX */

  {NULL, "_testJmp", testJmp, 0, N_TEXT | N_EXT},
  {NULL, "_installHandler", installHandler, 0, N_TEXT | N_EXT},
  {NULL, "_waitSignal", waitSignal, 0, N_TEXT | N_EXT},
  {NULL, "_selfSignal", selfSignal, 0, N_TEXT | N_EXT},
  {NULL, "_sendSignal", sendSignal, 0, N_TEXT | N_EXT},
  {NULL, "_sendQSignal", sendQSignal, 0, N_TEXT | N_EXT},
  {NULL, "_rsPeriodicTask", rsPeriodicTask, 0, N_TEXT | N_EXT},
  {NULL, "_genExc", genExc, 0, N_TEXT | N_EXT},
  {NULL, "_assertExpr", assertExpr, 0, N_TEXT | N_EXT},
  {NULL, "_testIo", testIo, 0, N_TEXT | N_EXT},
  {NULL, "_setAbortChar", setAbortChar, 0, N_TEXT | N_EXT},
  {NULL, "_selfRestarter", selfRestarter, 0, N_TEXT | N_EXT},
  {NULL, "_showTaskVars", showTaskVars, 0, N_TEXT | N_EXT},
  {NULL, "_addTaskVars", addTaskVars, 0, N_TEXT | N_EXT},
  {NULL, "_getTaskVar", getTaskVar, 0, N_TEXT | N_EXT},
  {NULL, "_setTaskVar", setTaskVar, 0, N_TEXT | N_EXT},
  {NULL, "_taskPrioTest", taskPrioTest, 0, N_TEXT | N_EXT},
  {NULL, "_classList", classList, 0, N_TEXT | N_EXT},
  {NULL, "_readSelect", readSelect, 0, N_TEXT | N_EXT},
  {NULL, "_demoWrite", demoWrite, 0, N_TEXT | N_EXT},
  {NULL, "_demoRead", demoRead, 0, N_TEXT | N_EXT},
  {NULL, "_netSocketNew", netSocketNew, 0, N_TEXT | N_EXT},
  {NULL, "_netSocketDelete", netSocketDelete, 0, N_TEXT | N_EXT},
  {NULL, "_netSocketBind", netSocketBind, 0, N_TEXT | N_EXT},
  {NULL, "_netSocketConnect", netSocketConnect, 0, N_TEXT | N_EXT},
  {NULL, "_netServer", netServer, 0, N_TEXT | N_EXT},
  {NULL, "_netClient", netClient, 0, N_TEXT | N_EXT},
  {NULL, "_netTestShell", netTestShell, 0, N_TEXT | N_EXT},
  {NULL, "_inAddRoute", inAddRoute, 0, N_TEXT | N_EXT},
  {NULL, "_udpCheck", udpCheck, 0, N_TEXT | N_EXT},
  {NULL, "_tcpServer", tcpServer, 0, N_TEXT | N_EXT},
  {NULL, "_udpServer", udpServer, 0, N_TEXT | N_EXT},
  {NULL, "_tcpClient", tcpClient, 0, N_TEXT | N_EXT},
  {NULL, "_udpClient", udpClient, 0, N_TEXT | N_EXT},
  {NULL, "_rawSocketNew", rawSocketNew, 0, N_TEXT | N_EXT},
  {NULL, "_rawSocketConnect", rawSocketConnect, 0, N_TEXT | N_EXT},
  {NULL, "_ipRead", ipRead, 0, N_TEXT | N_EXT},
  {NULL, "_inSetAddr", inSetAddr, 0, N_TEXT | N_EXT},
  {NULL, "_ramDiskInit", ramDiskInit, 0, N_TEXT | N_EXT},
  {NULL, "_floppyDiskInit", floppyDiskInit, 0, N_TEXT | N_EXT},
  {NULL, "_setLabel", setLabel, 0, N_TEXT | N_EXT},
  {NULL, "_createFile", createFile, 0, N_TEXT | N_EXT},
  {NULL, "_openFile", openFile, 0, N_TEXT | N_EXT},
  {NULL, "_closeFile", closeFile, 0, N_TEXT | N_EXT},
  {NULL, "_removeFile", removeFile, 0, N_TEXT | N_EXT},
  {NULL, "_showFsInfo", showFsInfo, 0, N_TEXT | N_EXT},
  {NULL, "_fileStat", fileStat, 0, N_TEXT | N_EXT},
  {NULL, "_fileRename", fileRename, 0, N_TEXT | N_EXT},
  {NULL, "_startWd", startWd, 0, N_TEXT | N_EXT},
  {NULL, "_msgQId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_sem", NULL, 0, N_DATA | N_EXT},
  {NULL, "_sem2", NULL, 0, N_DATA | N_EXT},
  {NULL, "_wdId", NULL, 0, N_DATA | N_EXT},
  {NULL, "_pipe0", NULL, 0, N_DATA | N_EXT}
};

int helpDemo(void)
{
  int i;

  semTake(sem, WAIT_FOREVER);

  for (i = 0; i < NELEMENTS(symTableDemo); i++) {

    printf("%-18s ", symTableDemo[i].name + 1);
    if ( (i % 4) == 0 ) printf("\n");

  }

  printf("\n");

  semGive(sem);

  return 0;
}

int init(void)
{
  struct tm startDate;
  struct timespec tp;
  int i;

  sem = semCreate(SEM_TYPE_MUTEX,
	          SEM_Q_PRIORITY|SEM_DELETE_SAFE);
  sem2 = semCreate(SEM_TYPE_COUNTING, 0);
  msgQId = msgQCreate(MAX_MESSAGES + 1, strlen(bigString) + 1, MSG_Q_PRIORITY);

  wdId = wdCreate();

  pipeDevCreate("/pipe/pipe0", MAX_MESSAGES, 512);
  pipe0 = open("/pipe/pipe0", O_RDWR, 0777);

  for (i = 0; i < NELEMENTS(symTableDemo); i++) {

    if (strcmp("_msgQId", symTableDemo[i].name) == 0)
      symTableDemo[i].value = (void *) msgQId;

    if (strcmp("_sem", symTableDemo[i].name) == 0)
      symTableDemo[i].value = (void *) sem;

    if (strcmp("_sem2", symTableDemo[i].name) == 0)
      symTableDemo[i].value = (void *) sem2;

    if (strcmp("_wdId", symTableDemo[i].name) == 0)
      symTableDemo[i].value = (void *) wdId;

    if (strcmp("_pipe0", symTableDemo[i].name) == 0)
      symTableDemo[i].value = (void *) pipe0;

    symTableAdd(sysSymTable, &symTableDemo[i]);

  }

#ifdef INCLUDE_GFX

  gfxPartId = memHeapPartId;
  createDib();
  gfxDevId = UGL_GRAPHICS_CREATE(0, 0, 0);

#endif /* INCLUDE_GFX */

  startDate.tm_sec = 0;
  startDate.tm_min = 56;
  startDate.tm_hour = 17;
  startDate.tm_mday = 8;
  startDate.tm_mon = 4;
  startDate.tm_year = 110;

  tp.tv_sec = mktime(&startDate);
  tp.tv_nsec = 0;
  clock_settime(CLOCK_REALTIME, &tp);

  /* Set ip address */
  inSetAddr("127.0.0.1");

  printf("Demo started, type 'helpDemo' for a list of commands\n");

  return 0;
}

int demo(void)
{
  taskCreateHookAdd((FUNCPTR) createHook);
  taskDeleteHookAdd((FUNCPTR) deleteHook);
  taskSwitchHookAdd((FUNCPTR) switchHook);
  taskSwapHookAdd((FUNCPTR) swapHook);
  moduleCreateHookAdd((FUNCPTR) moduleHook);

  taskSpawn("tInit",
	     0,
	     0,
	     DEFAULT_STACK_SIZE,
	     (FUNCPTR) init,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0);

  periodicTaskId =
  taskSpawn("tPeriodic",
	     50,
	     0,
	     DEFAULT_STACK_SIZE,
	     (FUNCPTR) prTask,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0,
	      0);
  taskSuspend(periodicTaskId);
  taskSwapHookAttach((FUNCPTR) swapHook, periodicTaskId, TRUE, FALSE);

  kernelTimeSlice(1);

  return 0;
}

#define NUM_MBLK_LOCAL 256
#define NUM_CLBLK_LOCAL 256
#define NUM_MBLK_CL_LOCAL 256
#define NET_BUF_SIZE 256

M_BLK_ID mBlkId[NUM_MBLK_LOCAL + 1];
CL_BLK_ID clBlkId[NUM_CLBLK_LOCAL + 1];
M_BLK_ID mBlkClId[NUM_MBLK_CL_LOCAL + 1];
int mBlkCount = 0, clBlkCount = 0, clBufCount = 0, mBlkClCount = 0;
int canWait = M_WAIT;

int newMblk(void)
{

  if (mBlkCount >= NUM_MBLK_LOCAL) {
    printf("Warning - No more room for mBlk.\n");
    //return 0;
  }

  if ( (mBlkId[mBlkCount] = netMblkGet(netSysPoolId, canWait, MT_DATA)) == NULL) {
    printf("Error - Unable to get mBlk.\n");
    return 1;
  }

printf("\n\n--------------------------------------------------------------------------------\n");
  printf("GOT M_BLK: %x\n", (unsigned int) mBlkId[mBlkCount]);
printf("--------------------------------------------------------------------------------\n\n");

  mBlkCount++;

  return 0;
}

void deleteMblk(void)
{
  if (mBlkCount == 0) {
    printf("Warning - No mBlk allocated.\n");
    return;
  }

  netMblkFree(netSysPoolId, mBlkId[--mBlkCount]);
}

int newClBlk(void)
{
  if (clBlkCount >= NUM_CLBLK_LOCAL) {
    printf("Warning - No more room for ClBlk.\n");
    return 0;
  }

  if ( (clBlkId[clBlkCount] = netClBlkGet(netSysPoolId, canWait)) == NULL) {
    printf("Error - Unable to get clBlk.\n");
    return 1;
  }

printf("\n\n--------------------------------------------------------------------------------\n");
  printf("GOT CL_BLK: %x\n", (unsigned int) clBlkId[clBlkCount]);
printf("--------------------------------------------------------------------------------\n\n");

  clBlkCount++;

  return 0;
}

void deleteClBlk(void)
{
  if (clBlkCount == 0) {
    printf("Warning - No clBlk allocated.\n");
    return;
  }

  netClBlkFree(netSysPoolId, clBlkId[--clBlkCount]);
}

int newTuple(void)
{
  if (mBlkClCount >= NUM_MBLK_LOCAL) {
    printf("Warning - No more room for mBlk.\n");
    return 0;
  }

  if ( (mBlkClId[mBlkClCount] = netTupleGet(netSysPoolId, NET_BUF_SIZE, canWait, MT_HEADER, TRUE)) == NULL) {
    printf("Error - Unable to get net tuple.\n");
    return 1;
  }

printf("\n\n--------------------------------------------------------------------------------\n");
  printf("GOT NET_TUPLE: %x\n", (unsigned int) mBlkClId[mBlkClCount]);
printf("--------------------------------------------------------------------------------\n\n");

  mBlkClCount++;

  return 0;
}

void deleteTuple(void)
{
  if (mBlkClCount == 0) {
    printf("Warning - No tuples allocated.\n");
    return;
  }

  netMblkClFree(mBlkClId[--mBlkClCount]);
}

void printLocalStatus(void)
{
  printf("number of Mblks created: %d\n", mBlkCount);
  printf("number of ClBlks created: %d\n", clBlkCount);
  if (canWait == M_DONTWAIT)
    printf("Will not wait for block to become free.\n");
  else
    printf("Will wait for block to become free.\n");
}

void netTestShell(void)
{
  char str[256];
  char c;

  do {

    printf("\nSelect an option: \n");
    printf("1 - New mBlk\n");
    printf("2 - Delete mBlk\n");
    printf("3 - New clBlk\n");
    printf("4 - Delete clBlk\n");
    printf("5 - New netTuple\n");
    printf("6 - Delete netTuple\n");
    printf("x - Show info\n");
    printf("c - Toggle canWait flag\n");
    printf("\n->");

    scanf("%s", str);
    c = str[0];

    switch (c) {
      case '1' : newMblk();
	         break;

      case '2' : deleteMblk();
	         break;

      case '3' : newClBlk();
	         break;

      case '4' : deleteClBlk();
	         break;

      case '5' : newTuple();
	         break;

      case '6' : deleteTuple();
	         break;

      case 'c' : if (canWait == M_DONTWAIT)
		   canWait = M_WAIT;
		 else
		   canWait = M_DONTWAIT;
	         break;

      case 'x' : netPoolShow(netSysPoolId);
		 printLocalStatus();
	         break;
    }


  } while (c != 'q');

}

