
#define NULL ((void*)0)
#define N_UNDF 0x00 /* Undefined */
#define N_ABS  0x02 /* Absolute */
#define N_TEXT 0x04 /* Text */
#define N_DATA 0x06 /* Data */
#define N_BSS  0x08 /* Bss */
#define N_EXT  0x01 /* External or bit (global) */
#define N_TYPE 0x1e /* Mask for all types */

typedef unsigned char SYM_TYPE;
typedef struct sl_node
{
    struct sl_node *next;
} SL_NODE;
typedef struct 
{
    SL_NODE nameHashNode;
    char *name;
    void *value;
    unsigned short group;
    SYM_TYPE type;
} SYMBOL;

extern int abs();
extern int accept();
extern int actStart;
extern int addDemo();
extern int addDomain();
extern int addTaskVars();
extern int animTreshold;
extern int asprintf();
extern int __assert();
extern int assertExpr();
extern int assertLibInit();
extern int atoi();
extern int atol();
extern int bigString;
extern int bind();
extern int binit();
extern int bitblocks;
extern int blkbuf;
extern int bmap();
extern int bread();
extern int brelse();
extern int bsdSockFunc;
extern int bsdSockLibAdd();
extern int bss;
extern int buf;
extern int buf_getblk();
extern int bufHead;
extern int bwrite();
extern int cacheArchLibInit();
extern int cacheClear();
extern int cacheDataEnabled;
extern int cacheDataMode;
extern int cacheDisable();
extern int cacheDmaFlush();
extern int cacheDmaFreeFunc;
extern int cacheDmaFunc;
extern int cacheDmaInvalidate();
extern int cacheDmaIsReadCoherent();
extern int cacheDmaIsWriteCoherent();
extern int cacheDmaMallocFunc;
extern int cacheDmaPhysToVirt();
extern int cacheDmaVirtToPhys();
extern int cacheDrvFlush();
extern int cacheDrvInvalidate();
extern int cacheDrvIsReadCoherent();
extern int cacheDrvIsWriteCoherent();
extern int cacheDrvPhysToVirt();
extern int cacheDrvVirtToPhys();
extern int cacheEnable();
extern int cacheFlush();
extern int cacheFuncsSet();
extern int cacheI386Clear();
extern int cacheI386Disable();
extern int cacheI386Enable();
extern int cacheI386Flush();
extern int cacheI386Lock();
extern int cacheI386Reset();
extern int cacheI386Unlock();
extern int cacheInvalidate();
extern int cacheLib;
extern int cacheLibInit();
extern int cacheLibInstalled;
extern int cacheLock();
extern int cacheMmuAvailable;
extern int cacheNullFunc;
extern int cachePipeFlush();
extern int cacheTextUpdate();
extern int cacheUnlock();
extern int cacheUsrFlush();
extern int cacheUsrFunc;
extern int cacheUsrInvalidate();
extern int cacheUsrIsReadCoherent();
extern int cacheUsrIsWriteCoherent();
extern int calloc();
extern int canWait;
extern int cbioBlkCopy();
extern int cbioBlkRW();
extern int cbioBytesRW();
extern int cbioClassId;
extern int cbioDevCreate();
extern int cbioDevVerify();
extern int cbioIoctl();
extern int cbioLibInit();
extern int cbioLock();
extern int cbioModeGet();
extern int cbioModeSet();
extern int cbioParamsGet();
extern int cbioRdyChgdGet();
extern int cbioRdyChgdSet();
extern int cbioRecentDev;
extern int cbioShow();
extern int cbioShowInit();
extern int cbioUnlock();
extern int cbioWrapBlkDev();
extern int cd();
extern int cfree();
extern int charTable;
extern int chdir();
extern int check_file();
extern int checkStack();
extern int checksum();
extern int classCreate();
extern int classDestroy();
extern int classInit();
extern int classLibInit();
extern int classList();
extern int classShow();
extern int classShowConnect();
extern int classShowInit();
extern int clBlkCount;
extern int clBlkId;
extern int clBufCount;
extern int clDescTable;
extern int clDescTableNumEntries;
extern int clock_gettime();
extern int clockLibInit();
extern int _clockRealtime;
extern int clock_settime();
extern int close();
extern int closedir();
extern int closeFile();
extern int code();
extern int connect();
extern int connectWithTimeout();
extern int consoleFd;
extern int consoleName;
extern int convertTest();
extern int copy();
extern int creat();
extern int creatConnect();
extern int createDib();
extern int created_modules;
extern int created_tasks;
extern int createFile();
extern int createHook();
extern int __ctype;
extern int currVmContext;
extern int cwd;
extern int d();
extern int data;
extern int __daysSinceEpoch();
extern int debugLibInit();
extern int decAnimTreshold();
extern int defaultTaskId;
extern int deleteClBlk();
extern int deleted_tasks;
extern int deleteHook();
extern int deleteMblk();
extern int deleteTuple();
extern int demo();
extern int demoRead();
extern int demoWrite();
extern int devs();
extern int dirList();
extern int div();
extern int __divdi3();
extern int div_r();
extern int dllAdd();
extern int dllCount();
extern int dllGet();
extern int dllInit();
extern int dllInsert();
extern int dllRemove();
extern int dmaSetup();
extern int domaininit();
extern int domains;
extern int domainShow();
extern int dosDirHdlrsList;
extern int dosDirLibInit();
extern int dosDirSemOptions;
extern int dosFatHdlrsList;
extern int dosFatLibInit();
extern int dosFmtLibInit();
extern int dosFmtShow();
extern int dosFsDevInit();
extern int dosFsDevSemOptions;
extern int dosFsFileSemOptions;
extern int dosFsHdlrInstall();
extern int dosFsLibInit();
extern int dosFsShortSemOptions;
extern int dosFsShow();
extern int dosFsShowInit();
extern int dosFsVolDescGet();
extern int dosFsVolFormatFunc;
extern int dosFsVolLabelGet();
extern int dosFsVolUnmount();
extern int dosSuperGet();
extern int dosSuperLibInit();
extern int doubleBuffer;
extern int elfArchLibInit();
extern int end;
extern int envLibInit();
extern int envPrivateCreate();
extern int envPrivateDestroy();
extern int envShow();
extern int envShowInit();
extern int erfCategoryAllocate();
extern int erfEventRaise();
extern int erfHandlerRegister();
extern int erfHandlerUnregister();
extern int erfLibInit();
extern int erfTypeAllocate();
extern int errno;
extern int errnoGet();
extern int errnoOfTaskGet();
extern int errnoSet();
extern int eventClear();
extern int eventLibInit();
extern int eventPendQ;
extern int eventReceive();
extern int eventResourceInit();
extern int eventResourceRegister();
extern int eventResourceSend();
extern int eventResourceShow();
extern int eventResourceTerminate();
extern int eventSend();
extern int eventTaskShow();
extern int excCallTbl();
extern int excExcHandle();
extern int excIntHandle();
extern int excIntStub();
extern int excJobAdd();
extern int excLibInit();
extern int excMsgsLost;
extern int excShowInit();
extern int excStub();
extern int excTaskId;
extern int excTaskOptions;
extern int excTaskPriority;
extern int excTaskStackSize;
extern int excVecInit();
extern int execute();
extern int exit();
extern int fatClustFactor;
extern int fatSemOptions;
extern int fclose();
extern int fdDevCreate();
extern int fdDrvInit();
extern int fdIntCount;
extern int fdMutexSem;
extern int fdopen();
extern int fdprintf();
extern int fdRetry;
extern int fdSemSeconds;
extern int fdSyncSem;
extern int fdTimeout;
extern int fdWdId;
extern int fdWdSeconds;
extern int feof();
extern int ferror();
extern int fflush();
extern int ffsLsb();
extern int ffsMsb();
extern int fgetc();
extern int fgets();
extern int file;
extern int filealloc();
extern int fileclose();
extern int fileincref();
extern int fileinit();
extern int fileread();
extern int fileRename();
extern int filestat();
extern int fileStat();
extern int filewrite();
extern int fill_file();
extern int first();
extern int firstTime;
extern int firstTimel;
extern int floppyDiskInit();
extern int fnprintf();
extern int font8x16;
extern int font8x16Height;
extern int font8x16Width;
extern int fopen();
extern int fpathconf();
extern int fpClassId;
extern int fprintf();
extern int fputc();
extern int fread();
extern int free();
extern int freeblock;
extern int freeGfx();
extern int freeinode;
extern int fscanf();
extern int fsEventUtilInit();
extern int fsfd;
extern int fsPathAddedEventRaise();
extern int fsPathAddedEventSetup();
extern int fstat();
extern int fs_test();
extern int fsWaitForPath();
extern int ftruncate();
extern int _func_excBaseHook;
extern int _func_excInfoShow;
extern int _func_excIntHook;
extern int _func_excJobAdd;
extern int _func_excPanicHook;
extern int _func_logMsg;
extern int _func_selWakeupAll;
extern int _func_selWakeupListInit;
extern int _func_selWakeupListTerminate;
extern int _func_sigExcKill;
extern int _func_sigprocmask;
extern int _func_sigTimeoutRecalc;
extern int _func_symFindSymbol;
extern int _func_symNameGet;
extern int _func_symTypeGet;
extern int _func_symValueGet;
extern int _func_taskRegsShowFunc;
extern int fwrite();
extern int __generateTime();
extern int genExc();
extern int getc();
extern int getchar();
extern int getcwd();
extern int __getDay();
extern int __getDstInfo();
extern int getenv();
extern int getExcVal();
extern int __getLocale();
extern int __getMonth();
extern int getpeername();
extern int gets();
extern int getsockname();
extern int getsockopt();
extern int getTaskVar();
extern int __getTime();
extern int __getZoneInfo();
extern int gfxDevId;
extern int globalEnvNEntries;
extern int globalEnvTableSize;
extern int globalPageBlockTable;
extern int handleKill();
extern int hashClassId;
extern int hashinit();
extern int hashKeyCmp();
extern int hashKeyStringCmp();
extern int hashLibInit();
extern int hashTableCreate();
extern int hashTableDelete();
extern int hashTableDestroy();
extern int hashTableEach();
extern int hashTableFind();
extern int hashTableInit();
extern int hashTablePut();
extern int hashTableRemove();
extern int hashTableTerminate();
extern int help();
extern int helpDemo();
extern int i();
extern int i2b();
extern int ialloc();
extern int iappend();
extern int icmpCfgParams;
extern int icmp_error();
extern int icmp_input();
extern int icmpLibInit();
extern int icmpmask;
extern int icmpmaskrepl;
extern int icmpstat;
extern int idecref();
extern int ifAddrAdd();
extern int ifafree();
extern int ifa_ifwithaddr();
extern int ifa_ifwithaf();
extern int ifa_ifwithdstaddr();
extern int ifa_ifwithnet();
extern int ifa_ifwithroute();
extern int ifaof_ifpforaddr();
extern int if_attach();
extern int ifconf();
extern int if_down();
extern int ifIndexAlloc();
extern int ifIndexToIfp();
extern int ifIndexVerify();
extern int ifioctl();
extern int ifLibInit();
extern int ifnet;
extern int if_qflush();
extern int ifqmaxlen;
extern int ifShow();
extern int if_slowtimo();
extern int ifunit();
extern int if_up();
extern int iget();
extern int iincref();
extern int iinit();
extern int ilock();
extern int inAddRoute();
extern int in_addrwithif();
extern int in_broadcast();
extern int in_canforward();
extern int incAnimTreshold();
extern int in_cksum();
extern int in_control();
extern int inet_addr();
extern int inetctlerrmap;
extern int inetdomain;
extern int inetLibInit();
extern int inet_ntoa_b();
extern int inetsw;
extern int in_ifaddr;
extern int in_ifinit();
extern int in_ifscrub();
extern int in_ifwithaddr();
extern int in_interfaces;
extern int init();
extern int inLibInit();
extern int in_localaddr();
extern int inLogAddr();
extern int inLogSock();
extern int in_losing();
extern int in_netof();
extern int inode;
extern int in_pcballoc();
extern int in_pcbbind();
extern int in_pcbchghash();
extern int in_pcbconnect();
extern int in_pcbdetach();
extern int in_pcbdisconnect();
extern int in_pcbhash();
extern int in_pcbinshash();
extern int in_pcbladdr();
extern int in_pcblookup();
extern int in_pcblookuphash();
extern int in_pcbnotify();
extern int in_pcbrehash();
extern int in_pcbremhash();
extern int input_ipaddr;
extern int inSetAddr();
extern int in_setpeeraddr();
extern int in_setsockaddr();
extern int in_socktrim();
extern int insque();
extern int installHandler();
extern int intCallTbl;
extern int intCnt;
extern int intConnect();
extern int intConnectFunction();
extern int intEoi;
extern int intHandlerCreate();
extern int intIntHandle();
extern int intLevelSet();
extern int intLock();
extern int intLockLevelGet();
extern int intLockLevelSet();
extern int intLockMask;
extern int intParamTbl;
extern int intRemoveFunction();
extern int intUnlock();
extern int intVBRSet();
extern int intVecBaseGet();
extern int intVecBaseSet();
extern int intVecSet();
extern int intVecSet2();
extern int intVecSetEnt;
extern int intVecSetExit;
extern int ioConnect();
extern int ioctl();
extern int ioDefPathCat();
extern int ioDefPathGet();
extern int ioDefPathSet();
extern int ioGlobalStdGet();
extern int ioGlobalStdSet();
extern int ioHelp();
extern int iosClose();
extern int iosCreate();
extern int iosDefaultPath;
extern int iosDefaultPathSet();
extern int iosDelete();
extern int iosDevAdd();
extern int iosDevDelete();
extern int iosDevFind();
extern int iosDevList;
extern int iosDevShow();
extern int iosDrvInstall();
extern int iosDrvRemove();
extern int iosDrvTable;
extern int iosFdDevFind();
extern int iosFdFree();
extern int iosFdFreeHook;
extern int iosFdNew();
extern int iosFdNewHook;
extern int iosFdSet();
extern int iosFdTable;
extern int iosFdValue();
extern int iosIoctl();
extern int iosLibInit();
extern int iosMaxDrv;
extern int iosMaxFd;
extern int iosNextDevGet();
extern int iosOpen();
extern int iosRead();
extern int iosShowInit();
extern int iosWrite();
extern int ioTaskStdGet();
extern int ioTaskStdSet();
extern int ipCfgParams;
extern int ip_ctloutput();
extern int ip_defttl;
extern int ip_dooptions();
extern int ip_drain();
extern int ip_flags;
extern int ip_forward();
extern int ipforward_rt;
extern int ipfragttl;
extern int ip_freef();
extern int ip_freemoptions();
extern int ip_getmoptions();
extern int ip_id;
extern int ipintr();
extern int ipintrq;
extern int ipLibInit();
extern int ip_nhops;
extern int ip_optcopy();
extern int ip_output();
extern int ip_pcbopts();
extern int ipprintfs;
extern int ip_protox;
extern int ipq;
extern int ipqmaxlen;
extern int ipRead();
extern int ip_rtaddr();
extern int ip_slowtimo();
extern int ipstat;
extern int ip_stripoptions();
extern int iptime();
extern int iput();
extern int isalnum();
extern int isalpha();
extern int isatty();
extern int iscntrl();
extern int isdigit();
extern int isgraph();
extern int islower();
extern int isprint();
extern int ispunct();
extern int isspace();
extern int isupper();
extern int isxdigit();
extern int itoa();
extern int itox();
extern int itrunc();
extern int iunlock();
extern int iupdate();
extern int __julday();
extern int kbdAction;
extern int kbdEnhanced;
extern int kbdHrdInit();
extern int kbdIntr();
extern int kbdMap;
extern int kernAbsTicks;
extern int kernActiveQ;
extern int kernelInit();
extern int kernelIsIdle;
extern int kernelState;
extern int kernelTimeSlice();
extern int kernelVersion();
extern int kernExcStkCnt;
extern int kernExit();
extern int kernHookSwap();
extern int kernHookSwitch();
extern int kernInitialized;
extern int kernIntEnt();
extern int kernIntExit();
extern int kernQAdd0();
extern int kernQAdd1();
extern int kernQAdd2();
extern int kernQDoWork();
extern int kernQEmpty;
extern int kernQLibInit();
extern int kernQPanic();
extern int kernReadyBmp;
extern int kernReadyLst;
extern int kernReadyQ;
extern int kernRoundRobin;
extern int kernRoundRobinTimeSlice;
extern int kernTaskEntry();
extern int kernTaskLoadContext();
extern int kernTaskReschedule();
extern int kernTickAnnounce();
extern int kernTickLibInit();
extern int kernTickQ;
extern int kernTicks;
extern int kernVersion;
extern int kill();
extern int ld();
extern int ldCommonMatchAll;
extern int ldiv();
extern int ldiv_r();
extern int lexActions();
extern int lexClassTable;
extern int lexNclasses;
extern int lexNewLine();
extern int lexStateTable;
extern int linki();
extern int listAdd();
extern int listCount();
extern int listen();
extern int listGet();
extern int listInit();
extern int listInsert();
extern int listRemove();
extern int lkup();
extern int ll();
extern int llr();
extern int loadCommonManage();
extern int loadCommonMatch();
extern int loadElfLibInit();
extern int loadElfRelocRelEntryRead();
extern int loadFunc;
extern int loadLibInit();
extern int loadModule();
extern int loadModuleAt();
extern int loadModuleAtSym();
extern int loadModuleGet();
extern int loadSegmentsAllocate();
extern int loadSyncFunc;
extern int loattach();
extern int localtime();
extern int localtime_r();
extern int __loctime;
extern int logFdAdd();
extern int logFdDelete();
extern int logLibInit();
extern int logMsg();
extern int logTaskId;
extern int logTaskOptions;
extern int logTaskPriority;
extern int logTaskStackSize;
extern int loif;
extern int longjmp();
extern int looutput();
extern int ls();
extern int lseek();
extern int lsr();
extern int lstat();
extern int __ltostr();
extern int m();
extern int m_adj();
extern int malloc();
extern int max_hdr;
extern int max_linkhdr;
extern int max_protohdr;
extern int mb_alloc();
extern int mb_free();
extern int mBlkClCount;
extern int mBlkClId;
extern int mBlkCount;
extern int mBlkId;
extern int mbufLibInit();
extern int m_cat();
extern int mClBlkConfig;
extern int memalign();
extern int memAutosize();
extern int memccpy();
extern int memchr();
extern int memcmp();
extern int memcpy();
extern int memcpyw();
extern int memDefaultAlignment;
extern int memHeapPart;
extern int memHeapPartId;
extern int memLibInit();
extern int memmove();
extern int memPartAddToPool();
extern int memPartAlignedAlloc();
extern int memPartAlloc();
extern int memPartAllocErrorFunc;
extern int memPartBlockErrorFunc;
extern int memPartBlockValidate();
extern int memPartClassId;
extern int memPartCreate();
extern int memPartDestroy();
extern int memPartFree();
extern int memPartInit();
extern int memPartLibInit();
extern int memPartOptionsDefault;
extern int memPartOptionsSet();
extern int memPartRealloc();
extern int memPartSemInitFunc;
extern int memPartShow();
extern int memRestoreTestAddr();
extern int memset();
extern int memsetw();
extern int memShowInit();
extern int memSysPartId;
extern int memSysPartition;
extern int memTop;
extern int memTopPhys;
extern int memVerifyTestPattern();
extern int memWriteTestPattern();
extern int messageListener();
extern int m_get();
extern int m_gethdr();
extern int miscLibInit();
extern int mkdir();
extern int mkfs();
extern int mkfs_balloc();
extern int mknod();
extern int mknod1();
extern int mktime();
extern int mmuEnable();
extern int mmuEnabled;
extern int mmuLibFunctions;
extern int mmuLibInit();
extern int mmuPageBlockSize;
extern int mmuPdbrGet();
extern int mmuPdbrSet();
extern int mmuStateTransTable;
extern int mmuStateTransTableSize;
extern int mmuTlbFlush();
extern int moduleCheck();
extern int moduleClassId;
extern int moduleCreate();
extern int moduleCreateHookAdd();
extern int moduleCreateHookDelete();
extern int moduleCreateHookSemOptions;
extern int moduleDelete();
extern int moduleDestroy();
extern int moduleEach();
extern int moduleFindByGroup();
extern int moduleFindByName();
extern int moduleFindByNameAndPath();
extern int moduleFlagsGet();
extern int moduleFormatGet();
extern int moduleGroupGet();
extern int moduleHook();
extern int moduleIdFigure();
extern int moduleIdListGet();
extern int moduleInfoGet();
extern int moduleInit();
extern int moduleLibInit();
extern int moduleListSemOptions;
extern int moduleNameGet();
extern int moduleSegAdd();
extern int moduleSegEach();
extern int moduleSegFirst();
extern int moduleSegGet();
extern int moduleSegInfoGet();
extern int moduleSegNext();
extern int moduleSegSemOptions;
extern int moduleShow();
extern int moduleShowInit();
extern int moduleTerminate();
extern int mountBufsCreate();
extern int mountCreate();
extern int mountLock();
extern int mountUnlock();
extern int m_prepend();
extern int m_pullup();
extern int msgListen();
extern int msgQClassId;
extern int msgQCreate();
extern int msgQDelete();
extern int msgQDestroy();
extern int msgQEvRegister();
extern int msgQEvUnregister();
extern int msgQId;
extern int msgQInfoGet();
extern int msgQInit();
extern int msgQLibInit();
extern int msgQNumMsgs();
extern int msgQReceive();
extern int msgQSend();
extern int msgQShow();
extern int msgQShowInit();
extern int msgQTerminate();
extern int mutextOptionsLogLib;
extern int namei();
extern int namelessPrefix;
extern int nblocks;
extern int netBufCollectFuncSet();
extern int netBufLibInit();
extern int netClBlkFree();
extern int netClBlkGet();
extern int netClFree();
extern int netClient();
extern int netClPoolIdGet();
extern int netClusterGet();
extern int netJobAdd();
extern int netLibInit();
extern int netLibInstalled;
extern int netMblkChainDup();
extern int netMblkClChainFree();
extern int netMblkClFree();
extern int netMblkClGet();
extern int netMblkFree();
extern int netMblkGet();
extern int netPoolDelete();
extern int netPoolInit();
extern int netPoolShow();
extern int netServer();
extern int netSocketBind();
extern int netSocketConnect();
extern int netSocketDelete();
extern int netSocketNew();
extern int netSysDpoolId;
extern int netSysPoolId;
extern int netTask();
extern int netTaskId;
extern int netTaskOptions;
extern int netTaskPriority;
extern int netTaskSemId;
extern int netTaskStackSize;
extern int netTestShell();
extern int netTupleGet();
extern int newClBlk();
extern int newImg();
extern int newImgLoLin();
extern int newMblk();
extern int newTuple();
extern int ninodes;
extern int numMsg;
extern int objAlloc();
extern int objAllocPad();
extern int objCoreInit();
extern int objCoreTerminate();
extern int objFree();
extern int objShow();
extern int ofile;
extern int open();
extern int openConnect();
extern int opendir();
extern int openFile();
extern int openInternal();
extern int panic();
extern int panicHookFunc;
extern int panicSuspend;
extern int pathCondense();
extern int pathconf();
extern int pathCwdLen();
extern int pathLibInit();
extern int pathPrependCwd();
extern int pathSplit();
extern int pause();
extern int pBgBmp;
extern int pBgDib;
extern int pbuf;
extern int pc();
extern int pcConDev;
extern int pcConDevCreate();
extern int pcConDrvInit();
extern int pcConDrvNumber();
extern int pDbBmp;
extern int pDblBmp;
extern int period();
extern int periodicState;
extern int periodicTaskId;
extern int periodicValue;
extern int periodRun();
extern int pfctlinput();
extern int pffindproto();
extern int pffindtype();
extern int pFgBmp;
extern int pFgDib;
extern int pFglBmp;
extern int pinballClut;
extern int pinballData;
extern int pinballHeight;
extern int pinballWidth;
extern int pinode;
extern int pipe0;
extern int pipeDevCreate();
extern int pipeDevDelete();
extern int pipeDrvInit();
extern int pipeDrvMsgQOptions;
extern int pKernExcStkBase;
extern int pKernExcStkEnd;
extern int _pNetBufCollect;
extern int _pNetPoolFuncTable;
extern int ppGlobalEnviron;
extern int print_blk();
extern int printErr();
extern int printExc();
extern int printf();
extern int print_inode();
extern int printLocalStatus();
extern int printLogo();
extern int pRootMemStart;
extern int pRootMount;
extern int pRootSyncer;
extern int prTask();
extern int pSaveBmp;
extern int pSavelBmp;
extern int pSockFdMap;
extern int pSysGdt;
extern int putc();
extern int putchar();
extern int putenv();
extern int puts();
extern int pwd();
extern int qAdvance();
extern int qCreate();
extern int qDestroy();
extern int __qdivrem();
extern int qEach();
extern int qExpired();
extern int qFifoClassId;
extern int qFirst();
extern int qGet();
extern int qInfo();
extern int qInit();
extern int qKey();
extern int qMove();
extern int qMsgClassId;
extern int qMsgGet();
extern int qMsgPut();
extern int qMsgTerminate();
extern int qOffset();
extern int qPriBmpClassId;
extern int qPrioClassId;
extern int qPut();
extern int qRemove();
extern int qTerminate();
extern int radixLibInit();
extern int raise();
extern int ramDevCreate();
extern int ramDiskInit();
extern int ramdsk_buf;
extern int ramdsk_deinit();
extern int ramdsk_init();
extern int ramdsk_rw();
extern int rand();
extern int _randseed;
extern int rasterOp;
extern int rasterOpAnd();
extern int rasterOpCopy();
extern int rasterOpOr();
extern int rasterOpXor();
extern int raw_attach();
extern int raw_detach();
extern int raw_disconnect();
extern int raw_input();
extern int rawipCfgParams;
extern int rawIpLibInit();
extern int rawLibInit();
extern int rawList;
extern int raw_recvspace;
extern int raw_sendspace;
extern int rawSocketConnect();
extern int rawSocketNew();
extern int raw_usrreq();
extern int read();
extern int readdir();
extern int readi();
extern int readlink();
extern int readSelect();
extern int realloc();
extern int receiveMessage();
extern int recv();
extern int recvfrom();
extern int recvmsg();
extern int redirInFd;
extern int redirOutFd;
extern int remove();
extern int removeConnect();
extern int removeFile();
extern int remque();
extern int repeat();
extern int repeatRun();
extern int restartTaskName;
extern int restartTaskOptions;
extern int restartTaskPriority;
extern int restartTaskStackSize;
extern int rewinddir();
extern int ringBufferGet();
extern int ringBufferPut();
extern int ringCreate();
extern int ringDelete();
extern int ringFlush();
extern int ringFreeBytes();
extern int ringIsEmpty();
extern int ringIsFull();
extern int ringMoveAhead();
extern int ringNBytes();
extern int ringPutAhead();
extern int rinode();
extern int rip_ctloutput();
extern int rip_input();
extern int rip_output();
extern int rip_usrreq();
extern int rmdir();
extern int rn_addmask();
extern int rn_addroute();
extern int rn_delete();
extern int rn_inithead();
extern int rn_insert();
extern int rn_lookup();
extern int rn_match();
extern int rn_newpair();
extern int rn_refines();
extern int rn_search();
extern int rn_search_m();
extern int rn_walktree();
extern int rn_walktree_from();
extern int rootClassId;
extern int rootMemNBytes;
extern int rootTaskId;
extern int route_cb;
extern int routeLibInit();
extern int routeShow();
extern int rsect();
extern int rsPeriodicTask();
extern int rt11FsDevInit();
extern int rt11FsFdMutexOptions;
extern int rt11FsLibInit();
extern int rt11FsMaxFiles;
extern int rt11FsReadyChange();
extern int rt11FsVolMutexOptions;
extern int rtalloc();
extern int rtalloc1();
extern int rtfree();
extern int rtIfaceMsgHook;
extern int rtinit();
extern int rtioctl();
extern int rtMissMsgHook;
extern int rtNewAddrMsgHook;
extern int rtosballClut;
extern int rtosballData;
extern int rtosballHeight;
extern int rtosballWidth;
extern int rtosDelay();
extern int rtosDelete();
extern int rtosPendQFlush();
extern int rtosPendQGet();
extern int rtosPendQPut();
extern int rtosPendQRemove();
extern int rtosPendQTerminate();
extern int rtosPendQWithHandlerPut();
extern int rtosPrioritySet();
extern int rtosReadyQPut();
extern int rtosReadyQRemove();
extern int rtosResume();
extern int rtosSemDelete();
extern int rtosSpawn();
extern int rtosSuspend();
extern int rtosTickAnnounce();
extern int rtosUndelay();
extern int rtosWdCancel();
extern int rtosWdStart();
extern int rtredirect();
extern int rtrequest();
extern int rtrequestAddEqui();
extern int rt_setgate();
extern int rt_tables;
extern int sb;
extern int sbappend();
extern int sbappendaddr();
extern int sbappendrecord();
extern int sbcompress();
extern int sbdrop();
extern int sbdroprecord();
extern int sbflush();
extern int sb_max;
extern int sbrelease();
extern int sbreserve();
extern int sbseldequeue();
extern int sbwait();
extern int sbwakeup();
extern int scanf();
extern int __sclose();
extern int second();
extern int select();
extern int selectLibInit();
extern int selectLibInitDelete();
extern int selfRestarter();
extern int selfSignal();
extern int selNodeAdd();
extern int selNodeDelete();
extern int selWakeup();
extern int selWakeupAll();
extern int selWakeupListInit();
extern int selWakeupListLen();
extern int selWakeupListTerminate();
extern int selWakeupType();
extern int sem;
extern int sem2;
extern int semaphoreListener();
extern int semBCoreInit();
extern int semBCreate();
extern int semBGive();
extern int semBGiveDefer();
extern int semBInit();
extern int semBLibInit();
extern int semBTake();
extern int semCCoreInit();
extern int semCCreate();
extern int semCGive();
extern int semCGiveDefer();
extern int semCInit();
extern int semClass;
extern int semClassId;
extern int semCLibInit();
extern int semCreate();
extern int semCTake();
extern int semDelete();
extern int semDestroy();
extern int semEvRegister();
extern int semEvUnregister();
extern int semFlush();
extern int semFlushDeferTable;
extern int semFlushTable;
extern int semGive();
extern int semGiveDeferTable;
extern int semGiveTable;
extern int semInfo();
extern int semInit();
extern int semInvalid();
extern int semLibInit();
extern int semListen();
extern int semMCoreInit();
extern int semMCreate();
extern int semMGive();
extern int semMGiveForce();
extern int semMInit();
extern int semMLibInit();
extern int semMTake();
extern int semQFlush();
extern int semQFlushDefer();
extern int semQInit();
extern int semReaderTake();
extern int semRWCreate();
extern int semRWDowngrade();
extern int semRWInit();
extern int semRWLibInit();
extern int semRWUpgrade();
extern int semRWUpgradeForce();
extern int semShow();
extern int semShowInit();
extern int semTake();
extern int semTakeTable;
extern int semTerminate();
extern int semUse();
extern int semWriterTake();
extern int send();
extern int sendMessage();
extern int sendmsg();
extern int sendQSignal();
extern int sendSignal();
extern int sendto();
extern int setAbortChar();
extern int setActivePage();
extern int setActiveStart();
extern int setjmp();
extern int _setjmpSetup();
extern int setLabel();
extern int setPalette();
extern int setPixel();
extern int setPixelInt();
extern int setsockopt();
extern int setTaskVar();
extern int setvbuf();
extern int setVisiblePage();
extern int setVisibleStart();
extern int __sflags();
extern int __sflush();
extern int __sfvwrite();
extern int shell();
extern int shellLibInit();
extern int shellLock();
extern int shellLogin();
extern int shellLoginInstall();
extern int shellLogout();
extern int shellLogoutInstall();
extern int shellSpawn();
extern int shellTaskId;
extern int shellTaskName;
extern int shellTaskOptions;
extern int shellTaskPriority;
extern int shellTaskStackSize;
extern int show();
extern int showBigString();
extern int showFsInfo();
extern int showInfo();
extern int showSysTime();
extern int showTaskVars();
extern int shutdown();
extern int sigaction();
extern int sigaddset();
extern int _sigCtxLoad();
extern int _sigCtxRetValueSet();
extern int _sigCtxSave();
extern int _sigCtxSetup();
extern int _sigCtxStackEnd();
extern int sigdelset();
extern int sigemptyset();
extern int _sigfaulttable;
extern int sigfillset();
extern int sigismember();
extern int sigLibInit();
extern int signal();
extern int sigPendDestroy();
extern int sigpending();
extern int sigPendInit();
extern int sigPendKill();
extern int sigprocmask();
extern int sigqueue();
extern int sigqueueInit();
extern int sigreturn();
extern int sigsetjmp();
extern int sigsuspend();
extern int size;
extern int sleep();
extern int sllAdd();
extern int sllCount();
extern int sllEach();
extern int sllGet();
extern int sllInit();
extern int sllInsert();
extern int sllPrevious();
extern int sllPutAtHead();
extern int sllPutAtTail();
extern int sllRemove();
extern int slowFill();
extern int __smakebuf();
extern int snprintf();
extern int soabort();
extern int soaccept();
extern int sobind();
extern int socantrcvmore();
extern int socantsendmore();
extern int socket();
extern int sockFdtosockFunc();
extern int sockLibInit();
extern int sockMapAdd();
extern int soclose();
extern int soconnect();
extern int soconnect2();
extern int socreate();
extern int sodisconnect();
extern int sofree();
extern int sogetopt();
extern int sohasoutofband();
extern int soisconnected();
extern int soisconnecting();
extern int soisdisconnected();
extern int soisdisconnecting();
extern int soLibInit();
extern int solisten();
extern int somaxconn;
extern int sonewconn();
extern int soo_ioctl();
extern int soo_unselect();
extern int soqinsque();
extern int soqremque();
extern int soreceive();
extern int soreserve();
extern int sorflush();
extern int sosend();
extern int sosetopt();
extern int soshutdown();
extern int sowakeup();
extern int sp();
extern int splimp();
extern int splMutexOptions;
extern int splnet();
extern int splTaskId;
extern int splx();
extern int sprintf();
extern int spTaskOptions;
extern int spTaskPriority;
extern int spTaskStackSize;
extern int srand();
extern int __sread();
extern int __srefill();
extern int __srget();
extern int sscanf();
extern int __sseek();
extern int standTable;
extern int standTableInit();
extern int standTableSize;
extern int startWd();
extern int stat();
extern int stati();
extern int __stderr();
extern int __stdin();
extern int stdioFp();
extern int stdioFpCreate();
extern int stdioFpDestroy();
extern int stdioFpInit();
extern int stdioLibInit();
extern int __stdout();
extern int strcat();
extern int strchr();
extern int strcmp();
extern int strcpy();
extern int strcspn();
extern int strdup();
extern int strftime();
extern int strlen();
extern int strncat();
extern int strncmp();
extern int strncpy();
extern int strpbrk();
extern int strrchr();
extern int strspn();
extern int strstr();
extern int strtok();
extern int strtok_r();
extern int _strto_l();
extern int strtol();
extern int strtoul();
extern int __submore();
extern int subnetsarelocal;
extern int swapHook();
extern int swapped_tasks;
extern int __swbuf();
extern int switched_tasks;
extern int switchHook();
extern int __swrite();
extern int __swsetup();
extern int symAdd();
extern int symCreate();
extern int symDemo;
extern int symDestroy();
extern int symEach();
extern int symFindByName();
extern int symFindByNameAndType();
extern int symFindSymbol();
extern int symGroupDefault;
extern int symInit();
extern int symLibInit();
extern int symlink();
extern int symNameGet();
extern int symRemove();
extern int symShow();
extern int symShowInit();
extern int symTableAdd();
extern int symTableClassId;
extern int symTableCreate();
extern int symTableDelete();
extern int symTableDemo;
extern int symTableDestroy();
extern int symTableInit();
extern int symTableRemove();
extern int symTableTerminate();
extern int symTypeGet();
extern int symValueGet();
extern int sys_chdir();
extern int sysClDescTable;
extern int sysClDescTableNumEntries;
extern int sysClockConnect();
extern int sysClockDisable();
extern int sysClockEnable();
extern int sysClockInt();
extern int sysClockRateGet();
extern int sysClockRateSet();
extern int sys_close();
extern int sysCsExc;
extern int sysCsInt;
extern int sysCsSuper;
extern int sysDelay();
extern int sys_dup();
extern int sysFdBuf;
extern int sysFdBufSize;
extern int sys_fstat();
extern int sysGdt();
extern int sysGdtr();
extern int sysHwInit();
extern int sysHwInit0();
extern int sysHwInit2();
extern int sysInByte();
extern int sysInit();
extern int sysInLong();
extern int sysInLongString();
extern int sysIntDisablePIC();
extern int sysIntEnablePIC();
extern int sysIntIdtType;
extern int sysIntInitPIC();
extern int sysIntLevel();
extern int sysIntLock();
extern int sysIntMask1;
extern int sysIntMask2;
extern int sysIntUnlock();
extern int sysInWord();
extern int sysInWordString();
extern int sys_link();
extern int sysLoadGdt();
extern int sysMclBlkConfig;
extern int sysMemTop();
extern int sys_mkdir();
extern int sys_mknod();
extern int sys_open();
extern int sysOutByte();
extern int sysOutLong();
extern int sysOutLongString();
extern int sysOutWord();
extern int sysOutWordString();
extern int sysPhysMemDesc;
extern int sysPhysMemDescNumEntries;
extern int sysPhysMemTop();
extern int sys_read();
extern int sysReboot();
extern int sysSymTable;
extern int sys_unlink();
extern int sysVectorIRQ0;
extern int sysWait();
extern int sys_write();
extern int taskActivate();
extern int taskArgGet();
extern int taskArgSet();
extern int taskClassId;
extern int taskCreat();
extern int taskCreateHookAdd();
extern int taskCreateHookDelete();
extern int taskCreateHooks;
extern int taskCreateHookShow();
extern int taskDelay();
extern int taskDelete();
extern int taskDeleteForce();
extern int taskDeleteHookAdd();
extern int taskDeleteHookDelete();
extern int taskDeleteHooks;
extern int taskDeleteHookShow();
extern int taskDestroy();
extern int taskExit();
extern int taskHiPrio();
extern int taskHookLibInit();
extern int taskHookShowInit();
extern int taskIdCurrent;
extern int taskIdDefault();
extern int taskIdFigure();
extern int taskIdle();
extern int taskIdListGet();
extern int taskIdListSort();
extern int taskIdSelf();
extern int taskIdVerify();
extern int taskInfoGet();
extern int taskInit();
extern int taskLibInit();
extern int taskLock();
extern int taskLoPrio();
extern int taskName();
extern int taskNameToId();
extern int taskOptionsSet();
extern int taskOptionsString();
extern int taskPriorityGet();
extern int taskPrioritySet();
extern int taskPrioTest();
extern int taskRegName;
extern int taskRegsFmt;
extern int taskRegsFormat;
extern int taskRegsGet();
extern int taskRegsInit();
extern int taskRegsShow();
extern int taskRestart();
extern int taskResume();
extern int taskRetValueSet();
extern int taskSafe();
extern int taskShow();
extern int taskShowInit();
extern int taskSpawn();
extern int taskStackAllot();
extern int taskStatusString();
extern int taskSuspend();
extern int taskSwapHookAdd();
extern int taskSwapHookAttach();
extern int taskSwapHookDelete();
extern int taskSwapHookDetach();
extern int taskSwapHooks;
extern int taskSwapHookShow();
extern int taskSwapReference;
extern int taskSwitchHookAdd();
extern int taskSwitchHookDelete();
extern int taskSwitchHooks;
extern int taskSwitchHookShow();
extern int taskTcb();
extern int taskTerminate();
extern int taskUndelay();
extern int taskUnlock();
extern int taskUnsafe();
extern int taskVarAdd();
extern int taskVarDelete();
extern int taskVarGet();
extern int taskVarInfo();
extern int taskVarLibInit();
extern int taskVarSet();
extern int tcbinfo;
extern int tcp_attach();
extern int tcp_backoff;
extern int tcp_canceltimers();
extern int tcpCfgParams;
extern int tcpClient();
extern int tcp_close();
extern int tcp_ctlinput();
extern int tcp_ctloutput();
extern int tcp_disconnect();
extern int tcp_dooptions();
extern int tcp_do_rfc1323;
extern int tcp_drain();
extern int tcp_drop();
extern int tcp_fasttimo();
extern int tcp_input();
extern int tcp_iss;
extern int tcp_keepcnt;
extern int tcp_keepidle;
extern int tcp_keepinit;
extern int tcp_keepintvl;
extern int tcp_last_inpcb;
extern int tcpLibInit();
extern int tcp_maxidle;
extern int tcp_maxpersistidle;
extern int tcp_msl;
extern int tcp_mss();
extern int tcp_mssdflt;
extern int tcp_newtcpcb();
extern int tcp_notify();
extern int tcp_now;
extern int tcp_outflags;
extern int tcp_output();
extern int tcpOutRsts;
extern int tcp_pcbhashsize;
extern int tcp_pulloutofband();
extern int tcp_quench();
extern int tcpRandFunc;
extern int tcp_reass();
extern int tcp_recvspace;
extern int tcp_respond();
extern int tcprexmtthresh;
extern int tcp_rttdflt;
extern int tcp_saveti;
extern int tcp_sendspace;
extern int tcpServer();
extern int tcp_setpersist();
extern int tcp_slowtimo();
extern int tcpstat;
extern int tcp_template();
extern int tcp_timers();
extern int tcp_totbackoff;
extern int tcp_usrclosed();
extern int tcp_usrreq();
extern int tcp_xmit_timer();
extern int td();
extern int test_blkdev();
extern int test_filename();
extern int testGfx();
extern int testGfxLo();
extern int testGfxLoInt();
extern int testGfxLoLin();
extern int test_inode();
extern int testIo();
extern int testJmp();
extern int testPrintf();
extern int testStdio();
extern int testSymbols();
extern int ti();
extern int tick64Get();
extern int tick64Set();
extern int tickGet();
extern int tickSet();
extern int time();
extern int timeLibInit();
extern int __tmNormalize();
extern int __tmValidate();
extern int toggleDoubleBuffer();
extern int tolower();
extern int toupper();
extern int tr();
extern int ts();
extern int ttyAbortFuncSet();
extern int ttyAbortSet();
extern int ttyBackSpaceSet();
extern int ttyDeleteLineSet();
extern int ttyDevInit();
extern int ttyDevRemove();
extern int ttyEOFSet();
extern int ttyIntRd();
extern int ttyIntTx();
extern int ttyIoctl();
extern int ttyMonitorTrapSet();
extern int ttyRead();
extern int ttyWrite();
extern int tyler();
extern int TylerGajewski;
extern int udbinfo;
extern int __udivdi3();
extern int udpCfgParams;
extern int udpCheck();
extern int udpClient();
extern int udp_ctlinput();
extern int udpDoCkSumRcv;
extern int udpDoCkSumSnd;
extern int udp_in;
extern int udp_input();
extern int udp_last_inpcb;
extern int udpLibInit();
extern int udp_output();
extern int udp_pcbhashsize;
extern int udpServer();
extern int udpstat;
extern int udp_usrreq();
extern int uglBitmapCreate();
extern int uglBitmapDestroy();
extern int uglClutGet();
extern int uglClutSet();
extern int uglColorAlloc();
extern int uglColorAllocExt();
extern int uglColorFree();
extern int uglCommonClutAlloc();
extern int uglCommonClutCreate();
extern int uglCommonClutDestroy();
extern int uglCommonClutFree();
extern int uglCommonClutGet();
extern int uglCommonClutMapNearest();
extern int uglCommonClutSet();
extern int uglGcCopy();
extern int uglGcCreate();
extern int uglGcDestroy();
extern int uglGcSet();
extern int uglGeneric8BitBitmapBlt();
extern int uglGeneric8BitBitmapCreate();
extern int uglGeneric8BitBitmapDestroy();
extern int uglGeneric8BitBitmapWrite();
extern int uglGeneric8BitPixelSet();
extern int uglGenericClipDdb();
extern int uglGenericClipDibToDdb();
extern int uglGenericClutCreate();
extern int uglGenericClutDestroy();
extern int uglGenericClutGet();
extern int uglGenericClutMapNearest();
extern int uglGenericClutSet();
extern int uglGenericColorAllocIndexed();
extern int uglGenericColorFreeIndexed();
extern int uglGenericGcCopy();
extern int uglGenericGcCreate();
extern int uglGenericGcDestroy();
extern int uglGenericGcSet();
extern int uglGenericModeFind();
extern int uglGraphicsDevDestroy();
extern int uglModeAvailGet();
extern int uglModeSet();
extern int uglOsLock();
extern int uglOsLockCreate();
extern int uglOsLockDestroy();
extern int uglOsUnLock();
extern int uglPixelSet();
extern int uglUgiDevDeinit();
extern int uglUgiDevInit();
extern int uglVga4BitColorConvert();
extern int uglVgaBitmapBlt();
extern int uglVgaBitmapCreate();
extern int uglVgaBitmapDestroy();
extern int uglVgaBitmapWrite();
extern int uglVgaBltPlane();
extern int uglVgaClutGet();
extern int uglVgaClutSet();
extern int uglVgaDevCreate();
extern int uglVgaDevDestroy();
extern int uglVgaGcSet();
extern int uglVgaInfo();
extern int uglVgaPixelSet();
extern int uglVgaXBitmapBlt();
extern int uglVgaXBitmapCreate();
extern int uglVgaXBitmapDestroy();
extern int uglVgaXBitmapWrite();
extern int uglVgaXPixelSet();
extern int uiomove();
extern int __ultostr();
extern int __umoddi3();
extern int ungetc();
extern int unixDomain;
extern int unixDomainLibInit();
extern int unixDomainName;
extern int unixDomainProto;
extern int unlinki();
extern int usedblocks;
extern int usrClock();
extern int usrFsLibInit();
extern int usrInit();
extern int usrKernelInit();
extern int usrLibInit();
extern int usrMmuInit();
extern int usrNetInit();
extern int usrPageCheck();
extern int usrPageInit();
extern int usrPageNumLinesSet();
extern int usrPageReset();
extern int usrRoot();
extern int valloc();
extern int var;
extern int vdprintf();
extern int version();
extern int vfnprintf();
extern int vfprintf();
extern int vfscanf();
extern int vfsops;
extern int vgaANSIPalette;
extern int vgaBlankScreen();
extern int vgaDACLoad();
extern int vgaHrdInit();
extern int vgaInitMode();
extern int vgaLoadFont();
extern int vgaLoadPalette();
extern int vgaPutPixel();
extern int vgaRefreshArea();
extern int vgaRestore();
extern int vgaSave();
extern int vgaSaveScreen();
extern int vgaWriteString();
extern int visStart;
extern int vmContextClassId;
extern int vmContextCreate();
extern int vmContextDestroy();
extern int vmContextInit();
extern int vmCurrentSet();
extern int vmEnable();
extern int vmGlobalMap();
extern int vmGlobalMapInit();
extern int vmLibInfo;
extern int vmLibInit();
extern int vmMap();
extern int vmPageSizeGet();
extern int vmStateGet();
extern int vmStateSet();
extern int vmTranslate();
extern int vnodeLock();
extern int vnodesCreate();
extern int vnodesDelete();
extern int vnodeUnlock();
extern int vprintf();
extern int vscanf();
extern int vsnprintf();
extern int vsprintf();
extern int waitSignal();
extern int wakeup();
extern int wdCancel();
extern int wdClassId;
extern int wdCreate();
extern int wdDelete();
extern int wdDestroy();
extern int wdFire();
extern int wdId;
extern int wdInit();
extern int wdir();
extern int wdLibInit();
extern int wdShow();
extern int wdShowInit();
extern int wdStart();
extern int wdTerminate();
extern int __weekOfYear();
extern int winode();
extern int write();
extern int writei();
extern int wsect();
extern int xbdAttach();
extern int xbdBlockSize();
extern int xbdDevName();
extern int xbdDump();
extern int xbdEventCategory;
extern int xbdEventMediaChanged;
extern int xbdEventPrimaryInsert;
extern int xbdEventRemove;
extern int xbdEventSecondaryInsert;
extern int xbdEventSoftInsert;
extern int xbdIoctl();
extern int xbdLibInit();
extern int xbdNBlocks();
extern int xbdStrategy();
extern int xint();
extern int xshort();
extern int yychar;
extern int yylex();
extern int yylval;
extern int yynerrs;
extern int yyparse();
extern int yystart();
extern int zeroes;
extern int zeroin_addr;

SYMBOL symTblAll[1770] =
{
    {NULL, "_abs", (char*)abs, 0, (N_TEXT | N_EXT)},
    {NULL, "_accept", (char*)accept, 0, (N_TEXT | N_EXT)},
    {NULL, "_actStart", (char*)&actStart, 0, (N_BSS | N_EXT)},
    {NULL, "_addDemo", (char*)addDemo, 0, (N_TEXT | N_EXT)},
    {NULL, "_addDomain", (char*)addDomain, 0, (N_TEXT | N_EXT)},
    {NULL, "_addTaskVars", (char*)addTaskVars, 0, (N_TEXT | N_EXT)},
    {NULL, "_animTreshold", (char*)&animTreshold, 0, (N_BSS | N_EXT)},
    {NULL, "_asprintf", (char*)asprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "___assert", (char*)__assert, 0, (N_TEXT | N_EXT)},
    {NULL, "_assertExpr", (char*)assertExpr, 0, (N_TEXT | N_EXT)},
    {NULL, "_assertLibInit", (char*)assertLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_atoi", (char*)atoi, 0, (N_TEXT | N_EXT)},
    {NULL, "_atol", (char*)atol, 0, (N_TEXT | N_EXT)},
    {NULL, "_bigString", (char*)&bigString, 0, (N_DATA | N_EXT)},
    {NULL, "_bind", (char*)bind, 0, (N_TEXT | N_EXT)},
    {NULL, "_binit", (char*)binit, 0, (N_TEXT | N_EXT)},
    {NULL, "_bitblocks", (char*)&bitblocks, 0, (N_BSS | N_EXT)},
    {NULL, "_blkbuf", (char*)&blkbuf, 0, (N_BSS | N_EXT)},
    {NULL, "_bmap", (char*)bmap, 0, (N_TEXT | N_EXT)},
    {NULL, "_bread", (char*)bread, 0, (N_TEXT | N_EXT)},
    {NULL, "_brelse", (char*)brelse, 0, (N_TEXT | N_EXT)},
    {NULL, "_bsdSockFunc", (char*)&bsdSockFunc, 0, (N_DATA | N_EXT)},
    {NULL, "_bsdSockLibAdd", (char*)bsdSockLibAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_bss", (char*)&bss, 0, (N_BSS | N_EXT)},
    {NULL, "_buf", (char*)&buf, 0, (N_BSS | N_EXT)},
    {NULL, "_buf_getblk", (char*)buf_getblk, 0, (N_TEXT | N_EXT)},
    {NULL, "_bufHead", (char*)&bufHead, 0, (N_BSS | N_EXT)},
    {NULL, "_bwrite", (char*)bwrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheArchLibInit", (char*)cacheArchLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheClear", (char*)cacheClear, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDataEnabled", (char*)&cacheDataEnabled, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheDataMode", (char*)&cacheDataMode, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheDisable", (char*)cacheDisable, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaFlush", (char*)cacheDmaFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaFreeFunc", (char*)&cacheDmaFreeFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheDmaFunc", (char*)&cacheDmaFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheDmaInvalidate", (char*)cacheDmaInvalidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaIsReadCoherent", (char*)cacheDmaIsReadCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaIsWriteCoherent", (char*)cacheDmaIsWriteCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaMallocFunc", (char*)&cacheDmaMallocFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheDmaPhysToVirt", (char*)cacheDmaPhysToVirt, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDmaVirtToPhys", (char*)cacheDmaVirtToPhys, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvFlush", (char*)cacheDrvFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvInvalidate", (char*)cacheDrvInvalidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvIsReadCoherent", (char*)cacheDrvIsReadCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvIsWriteCoherent", (char*)cacheDrvIsWriteCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvPhysToVirt", (char*)cacheDrvPhysToVirt, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheDrvVirtToPhys", (char*)cacheDrvVirtToPhys, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheEnable", (char*)cacheEnable, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheFlush", (char*)cacheFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheFuncsSet", (char*)cacheFuncsSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Clear", (char*)cacheI386Clear, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Disable", (char*)cacheI386Disable, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Enable", (char*)cacheI386Enable, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Flush", (char*)cacheI386Flush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Lock", (char*)cacheI386Lock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Reset", (char*)cacheI386Reset, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheI386Unlock", (char*)cacheI386Unlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheInvalidate", (char*)cacheInvalidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheLib", (char*)&cacheLib, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheLibInit", (char*)cacheLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheLibInstalled", (char*)&cacheLibInstalled, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheLock", (char*)cacheLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheMmuAvailable", (char*)&cacheMmuAvailable, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheNullFunc", (char*)&cacheNullFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_cachePipeFlush", (char*)cachePipeFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheTextUpdate", (char*)cacheTextUpdate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheUnlock", (char*)cacheUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheUsrFlush", (char*)cacheUsrFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheUsrFunc", (char*)&cacheUsrFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_cacheUsrInvalidate", (char*)cacheUsrInvalidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheUsrIsReadCoherent", (char*)cacheUsrIsReadCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_cacheUsrIsWriteCoherent", (char*)cacheUsrIsWriteCoherent, 0, (N_TEXT | N_EXT)},
    {NULL, "_calloc", (char*)calloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_canWait", (char*)&canWait, 0, (N_BSS | N_EXT)},
    {NULL, "_cbioBlkCopy", (char*)cbioBlkCopy, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioBlkRW", (char*)cbioBlkRW, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioBytesRW", (char*)cbioBytesRW, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioClassId", (char*)&cbioClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_cbioDevCreate", (char*)cbioDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioDevVerify", (char*)cbioDevVerify, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioIoctl", (char*)cbioIoctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioLibInit", (char*)cbioLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioLock", (char*)cbioLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioModeGet", (char*)cbioModeGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioModeSet", (char*)cbioModeSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioParamsGet", (char*)cbioParamsGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioRdyChgdGet", (char*)cbioRdyChgdGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioRdyChgdSet", (char*)cbioRdyChgdSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioRecentDev", (char*)&cbioRecentDev, 0, (N_BSS | N_EXT)},
    {NULL, "_cbioShow", (char*)cbioShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioShowInit", (char*)cbioShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioUnlock", (char*)cbioUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_cbioWrapBlkDev", (char*)cbioWrapBlkDev, 0, (N_TEXT | N_EXT)},
    {NULL, "_cd", (char*)cd, 0, (N_TEXT | N_EXT)},
    {NULL, "_cfree", (char*)cfree, 0, (N_TEXT | N_EXT)},
    {NULL, "_charTable", (char*)&charTable, 0, (N_DATA | N_EXT)},
    {NULL, "_chdir", (char*)chdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_check_file", (char*)check_file, 0, (N_TEXT | N_EXT)},
    {NULL, "_checkStack", (char*)checkStack, 0, (N_TEXT | N_EXT)},
    {NULL, "_checksum", (char*)checksum, 0, (N_TEXT | N_EXT)},
    {NULL, "_classCreate", (char*)classCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_classDestroy", (char*)classDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_classInit", (char*)classInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_classLibInit", (char*)classLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_classList", (char*)classList, 0, (N_TEXT | N_EXT)},
    {NULL, "_classShow", (char*)classShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_classShowConnect", (char*)classShowConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_classShowInit", (char*)classShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_clBlkCount", (char*)&clBlkCount, 0, (N_BSS | N_EXT)},
    {NULL, "_clBlkId", (char*)&clBlkId, 0, (N_BSS | N_EXT)},
    {NULL, "_clBufCount", (char*)&clBufCount, 0, (N_BSS | N_EXT)},
    {NULL, "_clDescTable", (char*)&clDescTable, 0, (N_DATA | N_EXT)},
    {NULL, "_clDescTableNumEntries", (char*)&clDescTableNumEntries, 0, (N_BSS | N_EXT)},
    {NULL, "_clock_gettime", (char*)clock_gettime, 0, (N_TEXT | N_EXT)},
    {NULL, "_clockLibInit", (char*)clockLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "__clockRealtime", (char*)&_clockRealtime, 0, (N_BSS | N_EXT)},
    {NULL, "_clock_settime", (char*)clock_settime, 0, (N_TEXT | N_EXT)},
    {NULL, "_close", (char*)close, 0, (N_TEXT | N_EXT)},
    {NULL, "_closedir", (char*)closedir, 0, (N_TEXT | N_EXT)},
    {NULL, "_closeFile", (char*)closeFile, 0, (N_TEXT | N_EXT)},
    {NULL, "_code", (char*)code, 0, (N_TEXT | N_EXT)},
    {NULL, "_connect", (char*)connect, 0, (N_TEXT | N_EXT)},
    {NULL, "_connectWithTimeout", (char*)connectWithTimeout, 0, (N_TEXT | N_EXT)},
    {NULL, "_consoleFd", (char*)&consoleFd, 0, (N_BSS | N_EXT)},
    {NULL, "_consoleName", (char*)&consoleName, 0, (N_BSS | N_EXT)},
    {NULL, "_convertTest", (char*)convertTest, 0, (N_TEXT | N_EXT)},
    {NULL, "_copy", (char*)copy, 0, (N_TEXT | N_EXT)},
    {NULL, "_creat", (char*)creat, 0, (N_TEXT | N_EXT)},
    {NULL, "_creatConnect", (char*)creatConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_createDib", (char*)createDib, 0, (N_TEXT | N_EXT)},
    {NULL, "_created_modules", (char*)&created_modules, 0, (N_BSS | N_EXT)},
    {NULL, "_created_tasks", (char*)&created_tasks, 0, (N_BSS | N_EXT)},
    {NULL, "_createFile", (char*)createFile, 0, (N_TEXT | N_EXT)},
    {NULL, "_createHook", (char*)createHook, 0, (N_TEXT | N_EXT)},
    {NULL, "___ctype", (char*)&__ctype, 0, (N_DATA | N_EXT)},
    {NULL, "_currVmContext", (char*)&currVmContext, 0, (N_BSS | N_EXT)},
    {NULL, "_cwd", (char*)&cwd, 0, (N_BSS | N_EXT)},
    {NULL, "_d", (char*)d, 0, (N_TEXT | N_EXT)},
    {NULL, "_data", (char*)&data, 0, (N_DATA | N_EXT)},
    {NULL, "___daysSinceEpoch", (char*)__daysSinceEpoch, 0, (N_TEXT | N_EXT)},
    {NULL, "_debugLibInit", (char*)debugLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_decAnimTreshold", (char*)decAnimTreshold, 0, (N_TEXT | N_EXT)},
    {NULL, "_defaultTaskId", (char*)&defaultTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_deleteClBlk", (char*)deleteClBlk, 0, (N_TEXT | N_EXT)},
    {NULL, "_deleted_tasks", (char*)&deleted_tasks, 0, (N_BSS | N_EXT)},
    {NULL, "_deleteHook", (char*)deleteHook, 0, (N_TEXT | N_EXT)},
    {NULL, "_deleteMblk", (char*)deleteMblk, 0, (N_TEXT | N_EXT)},
    {NULL, "_deleteTuple", (char*)deleteTuple, 0, (N_TEXT | N_EXT)},
    {NULL, "_demo", (char*)demo, 0, (N_TEXT | N_EXT)},
    {NULL, "_demoRead", (char*)demoRead, 0, (N_TEXT | N_EXT)},
    {NULL, "_demoWrite", (char*)demoWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_devs", (char*)devs, 0, (N_TEXT | N_EXT)},
    {NULL, "_dirList", (char*)dirList, 0, (N_TEXT | N_EXT)},
    {NULL, "_div", (char*)div, 0, (N_TEXT | N_EXT)},
    {NULL, "___divdi3", (char*)__divdi3, 0, (N_TEXT | N_EXT)},
    {NULL, "_div_r", (char*)div_r, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllAdd", (char*)dllAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllCount", (char*)dllCount, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllGet", (char*)dllGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllInit", (char*)dllInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllInsert", (char*)dllInsert, 0, (N_TEXT | N_EXT)},
    {NULL, "_dllRemove", (char*)dllRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_dmaSetup", (char*)dmaSetup, 0, (N_TEXT | N_EXT)},
    {NULL, "_domaininit", (char*)domaininit, 0, (N_TEXT | N_EXT)},
    {NULL, "_domains", (char*)&domains, 0, (N_BSS | N_EXT)},
    {NULL, "_domainShow", (char*)domainShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosDirHdlrsList", (char*)&dosDirHdlrsList, 0, (N_BSS | N_EXT)},
    {NULL, "_dosDirLibInit", (char*)dosDirLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosDirSemOptions", (char*)&dosDirSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_dosFatHdlrsList", (char*)&dosFatHdlrsList, 0, (N_BSS | N_EXT)},
    {NULL, "_dosFatLibInit", (char*)dosFatLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFmtLibInit", (char*)dosFmtLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFmtShow", (char*)dosFmtShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsDevInit", (char*)dosFsDevInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsDevSemOptions", (char*)&dosFsDevSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_dosFsFileSemOptions", (char*)&dosFsFileSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_dosFsHdlrInstall", (char*)dosFsHdlrInstall, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsLibInit", (char*)dosFsLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsShortSemOptions", (char*)&dosFsShortSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_dosFsShow", (char*)dosFsShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsShowInit", (char*)dosFsShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsVolDescGet", (char*)dosFsVolDescGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsVolFormatFunc", (char*)&dosFsVolFormatFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_dosFsVolLabelGet", (char*)dosFsVolLabelGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosFsVolUnmount", (char*)dosFsVolUnmount, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosSuperGet", (char*)dosSuperGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_dosSuperLibInit", (char*)dosSuperLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_doubleBuffer", (char*)&doubleBuffer, 0, (N_DATA | N_EXT)},
    {NULL, "_elfArchLibInit", (char*)elfArchLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_end", (char*)&end, 0, (N_BSS | N_EXT)},
    {NULL, "_envLibInit", (char*)envLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_envPrivateCreate", (char*)envPrivateCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_envPrivateDestroy", (char*)envPrivateDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_envShow", (char*)envShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_envShowInit", (char*)envShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfCategoryAllocate", (char*)erfCategoryAllocate, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfEventRaise", (char*)erfEventRaise, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfHandlerRegister", (char*)erfHandlerRegister, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfHandlerUnregister", (char*)erfHandlerUnregister, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfLibInit", (char*)erfLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_erfTypeAllocate", (char*)erfTypeAllocate, 0, (N_TEXT | N_EXT)},
    {NULL, "_errno", (char*)&errno, 0, (N_BSS | N_EXT)},
    {NULL, "_errnoGet", (char*)errnoGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_errnoOfTaskGet", (char*)errnoOfTaskGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_errnoSet", (char*)errnoSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventClear", (char*)eventClear, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventLibInit", (char*)eventLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventPendQ", (char*)&eventPendQ, 0, (N_BSS | N_EXT)},
    {NULL, "_eventReceive", (char*)eventReceive, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventResourceInit", (char*)eventResourceInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventResourceRegister", (char*)eventResourceRegister, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventResourceSend", (char*)eventResourceSend, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventResourceShow", (char*)eventResourceShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventResourceTerminate", (char*)eventResourceTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventSend", (char*)eventSend, 0, (N_TEXT | N_EXT)},
    {NULL, "_eventTaskShow", (char*)eventTaskShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_excCallTbl", (char*)excCallTbl, 0, (N_TEXT | N_EXT)},
    {NULL, "_excExcHandle", (char*)excExcHandle, 0, (N_TEXT | N_EXT)},
    {NULL, "_excIntHandle", (char*)excIntHandle, 0, (N_TEXT | N_EXT)},
    {NULL, "_excIntStub", (char*)excIntStub, 0, (N_TEXT | N_EXT)},
    {NULL, "_excJobAdd", (char*)excJobAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_excLibInit", (char*)excLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_excMsgsLost", (char*)&excMsgsLost, 0, (N_BSS | N_EXT)},
    {NULL, "_excShowInit", (char*)excShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_excStub", (char*)excStub, 0, (N_TEXT | N_EXT)},
    {NULL, "_excTaskId", (char*)&excTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_excTaskOptions", (char*)&excTaskOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_excTaskPriority", (char*)&excTaskPriority, 0, (N_BSS | N_EXT)},
    {NULL, "_excTaskStackSize", (char*)&excTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_excVecInit", (char*)excVecInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_execute", (char*)execute, 0, (N_TEXT | N_EXT)},
    {NULL, "_exit", (char*)exit, 0, (N_TEXT | N_EXT)},
    {NULL, "_fatClustFactor", (char*)&fatClustFactor, 0, (N_DATA | N_EXT)},
    {NULL, "_fatSemOptions", (char*)&fatSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_fclose", (char*)fclose, 0, (N_TEXT | N_EXT)},
    {NULL, "_fdDevCreate", (char*)fdDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_fdDrvInit", (char*)fdDrvInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_fdIntCount", (char*)&fdIntCount, 0, (N_BSS | N_EXT)},
    {NULL, "_fdMutexSem", (char*)&fdMutexSem, 0, (N_BSS | N_EXT)},
    {NULL, "_fdopen", (char*)fdopen, 0, (N_TEXT | N_EXT)},
    {NULL, "_fdprintf", (char*)fdprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_fdRetry", (char*)&fdRetry, 0, (N_DATA | N_EXT)},
    {NULL, "_fdSemSeconds", (char*)&fdSemSeconds, 0, (N_DATA | N_EXT)},
    {NULL, "_fdSyncSem", (char*)&fdSyncSem, 0, (N_BSS | N_EXT)},
    {NULL, "_fdTimeout", (char*)&fdTimeout, 0, (N_DATA | N_EXT)},
    {NULL, "_fdWdId", (char*)&fdWdId, 0, (N_BSS | N_EXT)},
    {NULL, "_fdWdSeconds", (char*)&fdWdSeconds, 0, (N_DATA | N_EXT)},
    {NULL, "_feof", (char*)feof, 0, (N_TEXT | N_EXT)},
    {NULL, "_ferror", (char*)ferror, 0, (N_TEXT | N_EXT)},
    {NULL, "_fflush", (char*)fflush, 0, (N_TEXT | N_EXT)},
    {NULL, "_ffsLsb", (char*)ffsLsb, 0, (N_TEXT | N_EXT)},
    {NULL, "_ffsMsb", (char*)ffsMsb, 0, (N_TEXT | N_EXT)},
    {NULL, "_fgetc", (char*)fgetc, 0, (N_TEXT | N_EXT)},
    {NULL, "_fgets", (char*)fgets, 0, (N_TEXT | N_EXT)},
    {NULL, "_file", (char*)&file, 0, (N_BSS | N_EXT)},
    {NULL, "_filealloc", (char*)filealloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileclose", (char*)fileclose, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileincref", (char*)fileincref, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileinit", (char*)fileinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileread", (char*)fileread, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileRename", (char*)fileRename, 0, (N_TEXT | N_EXT)},
    {NULL, "_filestat", (char*)filestat, 0, (N_TEXT | N_EXT)},
    {NULL, "_fileStat", (char*)fileStat, 0, (N_TEXT | N_EXT)},
    {NULL, "_filewrite", (char*)filewrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_fill_file", (char*)fill_file, 0, (N_TEXT | N_EXT)},
    {NULL, "_first", (char*)first, 0, (N_TEXT | N_EXT)},
    {NULL, "_firstTime", (char*)&firstTime, 0, (N_DATA | N_EXT)},
    {NULL, "_firstTimel", (char*)&firstTimel, 0, (N_DATA | N_EXT)},
    {NULL, "_floppyDiskInit", (char*)floppyDiskInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_fnprintf", (char*)fnprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_font8x16", (char*)&font8x16, 0, (N_DATA | N_EXT)},
    {NULL, "_font8x16Height", (char*)&font8x16Height, 0, (N_DATA | N_EXT)},
    {NULL, "_font8x16Width", (char*)&font8x16Width, 0, (N_DATA | N_EXT)},
    {NULL, "_fopen", (char*)fopen, 0, (N_TEXT | N_EXT)},
    {NULL, "_fpathconf", (char*)fpathconf, 0, (N_TEXT | N_EXT)},
    {NULL, "_fpClassId", (char*)&fpClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_fprintf", (char*)fprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_fputc", (char*)fputc, 0, (N_TEXT | N_EXT)},
    {NULL, "_fread", (char*)fread, 0, (N_TEXT | N_EXT)},
    {NULL, "_free", (char*)free, 0, (N_TEXT | N_EXT)},
    {NULL, "_freeblock", (char*)&freeblock, 0, (N_BSS | N_EXT)},
    {NULL, "_freeGfx", (char*)freeGfx, 0, (N_TEXT | N_EXT)},
    {NULL, "_freeinode", (char*)&freeinode, 0, (N_DATA | N_EXT)},
    {NULL, "_fscanf", (char*)fscanf, 0, (N_TEXT | N_EXT)},
    {NULL, "_fsEventUtilInit", (char*)fsEventUtilInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_fsfd", (char*)&fsfd, 0, (N_BSS | N_EXT)},
    {NULL, "_fsPathAddedEventRaise", (char*)fsPathAddedEventRaise, 0, (N_TEXT | N_EXT)},
    {NULL, "_fsPathAddedEventSetup", (char*)fsPathAddedEventSetup, 0, (N_TEXT | N_EXT)},
    {NULL, "_fstat", (char*)fstat, 0, (N_TEXT | N_EXT)},
    {NULL, "_fs_test", (char*)fs_test, 0, (N_TEXT | N_EXT)},
    {NULL, "_fsWaitForPath", (char*)fsWaitForPath, 0, (N_TEXT | N_EXT)},
    {NULL, "_ftruncate", (char*)ftruncate, 0, (N_TEXT | N_EXT)},
    {NULL, "__func_excBaseHook", (char*)&_func_excBaseHook, 0, (N_BSS | N_EXT)},
    {NULL, "__func_excInfoShow", (char*)&_func_excInfoShow, 0, (N_BSS | N_EXT)},
    {NULL, "__func_excIntHook", (char*)&_func_excIntHook, 0, (N_BSS | N_EXT)},
    {NULL, "__func_excJobAdd", (char*)&_func_excJobAdd, 0, (N_BSS | N_EXT)},
    {NULL, "__func_excPanicHook", (char*)&_func_excPanicHook, 0, (N_BSS | N_EXT)},
    {NULL, "__func_logMsg", (char*)&_func_logMsg, 0, (N_BSS | N_EXT)},
    {NULL, "__func_selWakeupAll", (char*)&_func_selWakeupAll, 0, (N_BSS | N_EXT)},
    {NULL, "__func_selWakeupListInit", (char*)&_func_selWakeupListInit, 0, (N_BSS | N_EXT)},
    {NULL, "__func_selWakeupListTerminate", (char*)&_func_selWakeupListTerminate, 0, (N_BSS | N_EXT)},
    {NULL, "__func_sigExcKill", (char*)&_func_sigExcKill, 0, (N_BSS | N_EXT)},
    {NULL, "__func_sigprocmask", (char*)&_func_sigprocmask, 0, (N_BSS | N_EXT)},
    {NULL, "__func_sigTimeoutRecalc", (char*)&_func_sigTimeoutRecalc, 0, (N_BSS | N_EXT)},
    {NULL, "__func_symFindSymbol", (char*)&_func_symFindSymbol, 0, (N_BSS | N_EXT)},
    {NULL, "__func_symNameGet", (char*)&_func_symNameGet, 0, (N_BSS | N_EXT)},
    {NULL, "__func_symTypeGet", (char*)&_func_symTypeGet, 0, (N_BSS | N_EXT)},
    {NULL, "__func_symValueGet", (char*)&_func_symValueGet, 0, (N_BSS | N_EXT)},
    {NULL, "__func_taskRegsShowFunc", (char*)&_func_taskRegsShowFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_fwrite", (char*)fwrite, 0, (N_TEXT | N_EXT)},
    {NULL, "___generateTime", (char*)__generateTime, 0, (N_TEXT | N_EXT)},
    {NULL, "_genExc", (char*)genExc, 0, (N_TEXT | N_EXT)},
    {NULL, "_getc", (char*)getc, 0, (N_TEXT | N_EXT)},
    {NULL, "_getchar", (char*)getchar, 0, (N_TEXT | N_EXT)},
    {NULL, "_getcwd", (char*)getcwd, 0, (N_TEXT | N_EXT)},
    {NULL, "___getDay", (char*)__getDay, 0, (N_TEXT | N_EXT)},
    {NULL, "___getDstInfo", (char*)__getDstInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_getenv", (char*)getenv, 0, (N_TEXT | N_EXT)},
    {NULL, "_getExcVal", (char*)getExcVal, 0, (N_TEXT | N_EXT)},
    {NULL, "___getLocale", (char*)__getLocale, 0, (N_TEXT | N_EXT)},
    {NULL, "___getMonth", (char*)__getMonth, 0, (N_TEXT | N_EXT)},
    {NULL, "_getpeername", (char*)getpeername, 0, (N_TEXT | N_EXT)},
    {NULL, "_gets", (char*)gets, 0, (N_TEXT | N_EXT)},
    {NULL, "_getsockname", (char*)getsockname, 0, (N_TEXT | N_EXT)},
    {NULL, "_getsockopt", (char*)getsockopt, 0, (N_TEXT | N_EXT)},
    {NULL, "_getTaskVar", (char*)getTaskVar, 0, (N_TEXT | N_EXT)},
    {NULL, "___getTime", (char*)__getTime, 0, (N_TEXT | N_EXT)},
    {NULL, "___getZoneInfo", (char*)__getZoneInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_gfxDevId", (char*)&gfxDevId, 0, (N_BSS | N_EXT)},
    {NULL, "_globalEnvNEntries", (char*)&globalEnvNEntries, 0, (N_BSS | N_EXT)},
    {NULL, "_globalEnvTableSize", (char*)&globalEnvTableSize, 0, (N_BSS | N_EXT)},
    {NULL, "_globalPageBlockTable", (char*)&globalPageBlockTable, 0, (N_BSS | N_EXT)},
    {NULL, "_handleKill", (char*)handleKill, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashClassId", (char*)&hashClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_hashinit", (char*)hashinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashKeyCmp", (char*)hashKeyCmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashKeyStringCmp", (char*)hashKeyStringCmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashLibInit", (char*)hashLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableCreate", (char*)hashTableCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableDelete", (char*)hashTableDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableDestroy", (char*)hashTableDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableEach", (char*)hashTableEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableFind", (char*)hashTableFind, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableInit", (char*)hashTableInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTablePut", (char*)hashTablePut, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableRemove", (char*)hashTableRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_hashTableTerminate", (char*)hashTableTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_help", (char*)help, 0, (N_TEXT | N_EXT)},
    {NULL, "_helpDemo", (char*)helpDemo, 0, (N_TEXT | N_EXT)},
    {NULL, "_i", (char*)i, 0, (N_TEXT | N_EXT)},
    {NULL, "_i2b", (char*)i2b, 0, (N_TEXT | N_EXT)},
    {NULL, "_ialloc", (char*)ialloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_iappend", (char*)iappend, 0, (N_TEXT | N_EXT)},
    {NULL, "_icmpCfgParams", (char*)&icmpCfgParams, 0, (N_BSS | N_EXT)},
    {NULL, "_icmp_error", (char*)icmp_error, 0, (N_TEXT | N_EXT)},
    {NULL, "_icmp_input", (char*)icmp_input, 0, (N_TEXT | N_EXT)},
    {NULL, "_icmpLibInit", (char*)icmpLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_icmpmask", (char*)&icmpmask, 0, (N_BSS | N_EXT)},
    {NULL, "_icmpmaskrepl", (char*)&icmpmaskrepl, 0, (N_BSS | N_EXT)},
    {NULL, "_icmpstat", (char*)&icmpstat, 0, (N_BSS | N_EXT)},
    {NULL, "_idecref", (char*)idecref, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifAddrAdd", (char*)ifAddrAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifafree", (char*)ifafree, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifa_ifwithaddr", (char*)ifa_ifwithaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifa_ifwithaf", (char*)ifa_ifwithaf, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifa_ifwithdstaddr", (char*)ifa_ifwithdstaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifa_ifwithnet", (char*)ifa_ifwithnet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifa_ifwithroute", (char*)ifa_ifwithroute, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifaof_ifpforaddr", (char*)ifaof_ifpforaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_if_attach", (char*)if_attach, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifconf", (char*)ifconf, 0, (N_TEXT | N_EXT)},
    {NULL, "_if_down", (char*)if_down, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifIndexAlloc", (char*)ifIndexAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifIndexToIfp", (char*)ifIndexToIfp, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifIndexVerify", (char*)ifIndexVerify, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifioctl", (char*)ifioctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifLibInit", (char*)ifLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifnet", (char*)&ifnet, 0, (N_BSS | N_EXT)},
    {NULL, "_if_qflush", (char*)if_qflush, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifqmaxlen", (char*)&ifqmaxlen, 0, (N_BSS | N_EXT)},
    {NULL, "_ifShow", (char*)ifShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_if_slowtimo", (char*)if_slowtimo, 0, (N_TEXT | N_EXT)},
    {NULL, "_ifunit", (char*)ifunit, 0, (N_TEXT | N_EXT)},
    {NULL, "_if_up", (char*)if_up, 0, (N_TEXT | N_EXT)},
    {NULL, "_iget", (char*)iget, 0, (N_TEXT | N_EXT)},
    {NULL, "_iincref", (char*)iincref, 0, (N_TEXT | N_EXT)},
    {NULL, "_iinit", (char*)iinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ilock", (char*)ilock, 0, (N_TEXT | N_EXT)},
    {NULL, "_inAddRoute", (char*)inAddRoute, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_addrwithif", (char*)in_addrwithif, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_broadcast", (char*)in_broadcast, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_canforward", (char*)in_canforward, 0, (N_TEXT | N_EXT)},
    {NULL, "_incAnimTreshold", (char*)incAnimTreshold, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_cksum", (char*)in_cksum, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_control", (char*)in_control, 0, (N_TEXT | N_EXT)},
    {NULL, "_inet_addr", (char*)inet_addr, 0, (N_TEXT | N_EXT)},
    {NULL, "_inetctlerrmap", (char*)&inetctlerrmap, 0, (N_DATA | N_EXT)},
    {NULL, "_inetdomain", (char*)&inetdomain, 0, (N_DATA | N_EXT)},
    {NULL, "_inetLibInit", (char*)inetLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_inet_ntoa_b", (char*)inet_ntoa_b, 0, (N_TEXT | N_EXT)},
    {NULL, "_inetsw", (char*)&inetsw, 0, (N_DATA | N_EXT)},
    {NULL, "_in_ifaddr", (char*)&in_ifaddr, 0, (N_BSS | N_EXT)},
    {NULL, "_in_ifinit", (char*)in_ifinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_ifscrub", (char*)in_ifscrub, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_ifwithaddr", (char*)in_ifwithaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_interfaces", (char*)&in_interfaces, 0, (N_BSS | N_EXT)},
    {NULL, "_init", (char*)init, 0, (N_TEXT | N_EXT)},
    {NULL, "_inLibInit", (char*)inLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_localaddr", (char*)in_localaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_inLogAddr", (char*)inLogAddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_inLogSock", (char*)inLogSock, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_losing", (char*)in_losing, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_netof", (char*)in_netof, 0, (N_TEXT | N_EXT)},
    {NULL, "_inode", (char*)&inode, 0, (N_BSS | N_EXT)},
    {NULL, "_in_pcballoc", (char*)in_pcballoc, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbbind", (char*)in_pcbbind, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbchghash", (char*)in_pcbchghash, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbconnect", (char*)in_pcbconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbdetach", (char*)in_pcbdetach, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbdisconnect", (char*)in_pcbdisconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbhash", (char*)in_pcbhash, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbinshash", (char*)in_pcbinshash, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbladdr", (char*)in_pcbladdr, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcblookup", (char*)in_pcblookup, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcblookuphash", (char*)in_pcblookuphash, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbnotify", (char*)in_pcbnotify, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbrehash", (char*)in_pcbrehash, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_pcbremhash", (char*)in_pcbremhash, 0, (N_TEXT | N_EXT)},
    {NULL, "_input_ipaddr", (char*)&input_ipaddr, 0, (N_BSS | N_EXT)},
    {NULL, "_inSetAddr", (char*)inSetAddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_setpeeraddr", (char*)in_setpeeraddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_setsockaddr", (char*)in_setsockaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_in_socktrim", (char*)in_socktrim, 0, (N_TEXT | N_EXT)},
    {NULL, "_insque", (char*)insque, 0, (N_TEXT | N_EXT)},
    {NULL, "_installHandler", (char*)installHandler, 0, (N_TEXT | N_EXT)},
    {NULL, "_intCallTbl", (char*)&intCallTbl, 0, (N_BSS | N_EXT)},
    {NULL, "_intCnt", (char*)&intCnt, 0, (N_BSS | N_EXT)},
    {NULL, "_intConnect", (char*)intConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_intConnectFunction", (char*)intConnectFunction, 0, (N_TEXT | N_EXT)},
    {NULL, "_intEoi", (char*)&intEoi, 0, (N_BSS | N_EXT)},
    {NULL, "_intHandlerCreate", (char*)intHandlerCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_intIntHandle", (char*)intIntHandle, 0, (N_TEXT | N_EXT)},
    {NULL, "_intLevelSet", (char*)intLevelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intLock", (char*)intLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_intLockLevelGet", (char*)intLockLevelGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intLockLevelSet", (char*)intLockLevelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intLockMask", (char*)&intLockMask, 0, (N_BSS | N_EXT)},
    {NULL, "_intParamTbl", (char*)&intParamTbl, 0, (N_BSS | N_EXT)},
    {NULL, "_intRemoveFunction", (char*)intRemoveFunction, 0, (N_TEXT | N_EXT)},
    {NULL, "_intUnlock", (char*)intUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVBRSet", (char*)intVBRSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVecBaseGet", (char*)intVecBaseGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVecBaseSet", (char*)intVecBaseSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVecSet", (char*)intVecSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVecSet2", (char*)intVecSet2, 0, (N_TEXT | N_EXT)},
    {NULL, "_intVecSetEnt", (char*)&intVecSetEnt, 0, (N_BSS | N_EXT)},
    {NULL, "_intVecSetExit", (char*)&intVecSetExit, 0, (N_BSS | N_EXT)},
    {NULL, "_ioConnect", (char*)ioConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioctl", (char*)ioctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioDefPathCat", (char*)ioDefPathCat, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioDefPathGet", (char*)ioDefPathGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioDefPathSet", (char*)ioDefPathSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioGlobalStdGet", (char*)ioGlobalStdGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioGlobalStdSet", (char*)ioGlobalStdSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioHelp", (char*)ioHelp, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosClose", (char*)iosClose, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosCreate", (char*)iosCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDefaultPath", (char*)&iosDefaultPath, 0, (N_BSS | N_EXT)},
    {NULL, "_iosDefaultPathSet", (char*)iosDefaultPathSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDelete", (char*)iosDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDevAdd", (char*)iosDevAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDevDelete", (char*)iosDevDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDevFind", (char*)iosDevFind, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDevList", (char*)&iosDevList, 0, (N_BSS | N_EXT)},
    {NULL, "_iosDevShow", (char*)iosDevShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDrvInstall", (char*)iosDrvInstall, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDrvRemove", (char*)iosDrvRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosDrvTable", (char*)&iosDrvTable, 0, (N_BSS | N_EXT)},
    {NULL, "_iosFdDevFind", (char*)iosFdDevFind, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosFdFree", (char*)iosFdFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosFdFreeHook", (char*)&iosFdFreeHook, 0, (N_BSS | N_EXT)},
    {NULL, "_iosFdNew", (char*)iosFdNew, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosFdNewHook", (char*)&iosFdNewHook, 0, (N_BSS | N_EXT)},
    {NULL, "_iosFdSet", (char*)iosFdSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosFdTable", (char*)&iosFdTable, 0, (N_BSS | N_EXT)},
    {NULL, "_iosFdValue", (char*)iosFdValue, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosIoctl", (char*)iosIoctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosLibInit", (char*)iosLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosMaxDrv", (char*)&iosMaxDrv, 0, (N_BSS | N_EXT)},
    {NULL, "_iosMaxFd", (char*)&iosMaxFd, 0, (N_BSS | N_EXT)},
    {NULL, "_iosNextDevGet", (char*)iosNextDevGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosOpen", (char*)iosOpen, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosRead", (char*)iosRead, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosShowInit", (char*)iosShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_iosWrite", (char*)iosWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioTaskStdGet", (char*)ioTaskStdGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ioTaskStdSet", (char*)ioTaskStdSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ipCfgParams", (char*)&ipCfgParams, 0, (N_DATA | N_EXT)},
    {NULL, "_ip_ctloutput", (char*)ip_ctloutput, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_defttl", (char*)&ip_defttl, 0, (N_DATA | N_EXT)},
    {NULL, "_ip_dooptions", (char*)ip_dooptions, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_drain", (char*)ip_drain, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_flags", (char*)&ip_flags, 0, (N_DATA | N_EXT)},
    {NULL, "_ip_forward", (char*)ip_forward, 0, (N_TEXT | N_EXT)},
    {NULL, "_ipforward_rt", (char*)&ipforward_rt, 0, (N_BSS | N_EXT)},
    {NULL, "_ipfragttl", (char*)&ipfragttl, 0, (N_DATA | N_EXT)},
    {NULL, "_ip_freef", (char*)ip_freef, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_freemoptions", (char*)ip_freemoptions, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_getmoptions", (char*)ip_getmoptions, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_id", (char*)&ip_id, 0, (N_BSS | N_EXT)},
    {NULL, "_ipintr", (char*)ipintr, 0, (N_TEXT | N_EXT)},
    {NULL, "_ipintrq", (char*)&ipintrq, 0, (N_BSS | N_EXT)},
    {NULL, "_ipLibInit", (char*)ipLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_nhops", (char*)&ip_nhops, 0, (N_BSS | N_EXT)},
    {NULL, "_ip_optcopy", (char*)ip_optcopy, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_output", (char*)ip_output, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_pcbopts", (char*)ip_pcbopts, 0, (N_TEXT | N_EXT)},
    {NULL, "_ipprintfs", (char*)&ipprintfs, 0, (N_BSS | N_EXT)},
    {NULL, "_ip_protox", (char*)&ip_protox, 0, (N_BSS | N_EXT)},
    {NULL, "_ipq", (char*)&ipq, 0, (N_BSS | N_EXT)},
    {NULL, "_ipqmaxlen", (char*)&ipqmaxlen, 0, (N_DATA | N_EXT)},
    {NULL, "_ipRead", (char*)ipRead, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_rtaddr", (char*)ip_rtaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_ip_slowtimo", (char*)ip_slowtimo, 0, (N_TEXT | N_EXT)},
    {NULL, "_ipstat", (char*)&ipstat, 0, (N_BSS | N_EXT)},
    {NULL, "_ip_stripoptions", (char*)ip_stripoptions, 0, (N_TEXT | N_EXT)},
    {NULL, "_iptime", (char*)iptime, 0, (N_TEXT | N_EXT)},
    {NULL, "_iput", (char*)iput, 0, (N_TEXT | N_EXT)},
    {NULL, "_isalnum", (char*)isalnum, 0, (N_TEXT | N_EXT)},
    {NULL, "_isalpha", (char*)isalpha, 0, (N_TEXT | N_EXT)},
    {NULL, "_isatty", (char*)isatty, 0, (N_TEXT | N_EXT)},
    {NULL, "_iscntrl", (char*)iscntrl, 0, (N_TEXT | N_EXT)},
    {NULL, "_isdigit", (char*)isdigit, 0, (N_TEXT | N_EXT)},
    {NULL, "_isgraph", (char*)isgraph, 0, (N_TEXT | N_EXT)},
    {NULL, "_islower", (char*)islower, 0, (N_TEXT | N_EXT)},
    {NULL, "_isprint", (char*)isprint, 0, (N_TEXT | N_EXT)},
    {NULL, "_ispunct", (char*)ispunct, 0, (N_TEXT | N_EXT)},
    {NULL, "_isspace", (char*)isspace, 0, (N_TEXT | N_EXT)},
    {NULL, "_isupper", (char*)isupper, 0, (N_TEXT | N_EXT)},
    {NULL, "_isxdigit", (char*)isxdigit, 0, (N_TEXT | N_EXT)},
    {NULL, "_itoa", (char*)itoa, 0, (N_TEXT | N_EXT)},
    {NULL, "_itox", (char*)itox, 0, (N_TEXT | N_EXT)},
    {NULL, "_itrunc", (char*)itrunc, 0, (N_TEXT | N_EXT)},
    {NULL, "_iunlock", (char*)iunlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_iupdate", (char*)iupdate, 0, (N_TEXT | N_EXT)},
    {NULL, "___julday", (char*)__julday, 0, (N_TEXT | N_EXT)},
    {NULL, "_kbdAction", (char*)&kbdAction, 0, (N_DATA | N_EXT)},
    {NULL, "_kbdEnhanced", (char*)&kbdEnhanced, 0, (N_DATA | N_EXT)},
    {NULL, "_kbdHrdInit", (char*)kbdHrdInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kbdIntr", (char*)kbdIntr, 0, (N_TEXT | N_EXT)},
    {NULL, "_kbdMap", (char*)&kbdMap, 0, (N_DATA | N_EXT)},
    {NULL, "_kernAbsTicks", (char*)&kernAbsTicks, 0, (N_BSS | N_EXT)},
    {NULL, "_kernActiveQ", (char*)&kernActiveQ, 0, (N_BSS | N_EXT)},
    {NULL, "_kernelInit", (char*)kernelInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernelIsIdle", (char*)&kernelIsIdle, 0, (N_BSS | N_EXT)},
    {NULL, "_kernelState", (char*)&kernelState, 0, (N_BSS | N_EXT)},
    {NULL, "_kernelTimeSlice", (char*)kernelTimeSlice, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernelVersion", (char*)kernelVersion, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernExcStkCnt", (char*)&kernExcStkCnt, 0, (N_BSS | N_EXT)},
    {NULL, "_kernExit", (char*)kernExit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernHookSwap", (char*)kernHookSwap, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernHookSwitch", (char*)kernHookSwitch, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernInitialized", (char*)&kernInitialized, 0, (N_BSS | N_EXT)},
    {NULL, "_kernIntEnt", (char*)kernIntEnt, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernIntExit", (char*)kernIntExit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQAdd0", (char*)kernQAdd0, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQAdd1", (char*)kernQAdd1, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQAdd2", (char*)kernQAdd2, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQDoWork", (char*)kernQDoWork, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQEmpty", (char*)&kernQEmpty, 0, (N_BSS | N_EXT)},
    {NULL, "_kernQLibInit", (char*)kernQLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernQPanic", (char*)kernQPanic, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernReadyBmp", (char*)&kernReadyBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_kernReadyLst", (char*)&kernReadyLst, 0, (N_BSS | N_EXT)},
    {NULL, "_kernReadyQ", (char*)&kernReadyQ, 0, (N_BSS | N_EXT)},
    {NULL, "_kernRoundRobin", (char*)&kernRoundRobin, 0, (N_BSS | N_EXT)},
    {NULL, "_kernRoundRobinTimeSlice", (char*)&kernRoundRobinTimeSlice, 0, (N_BSS | N_EXT)},
    {NULL, "_kernTaskEntry", (char*)kernTaskEntry, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernTaskLoadContext", (char*)kernTaskLoadContext, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernTaskReschedule", (char*)kernTaskReschedule, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernTickAnnounce", (char*)kernTickAnnounce, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernTickLibInit", (char*)kernTickLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_kernTickQ", (char*)&kernTickQ, 0, (N_BSS | N_EXT)},
    {NULL, "_kernTicks", (char*)&kernTicks, 0, (N_BSS | N_EXT)},
    {NULL, "_kernVersion", (char*)&kernVersion, 0, (N_DATA | N_EXT)},
    {NULL, "_kill", (char*)kill, 0, (N_TEXT | N_EXT)},
    {NULL, "_ld", (char*)ld, 0, (N_TEXT | N_EXT)},
    {NULL, "_ldCommonMatchAll", (char*)&ldCommonMatchAll, 0, (N_DATA | N_EXT)},
    {NULL, "_ldiv", (char*)ldiv, 0, (N_TEXT | N_EXT)},
    {NULL, "_ldiv_r", (char*)ldiv_r, 0, (N_TEXT | N_EXT)},
    {NULL, "_lexActions", (char*)lexActions, 0, (N_TEXT | N_EXT)},
    {NULL, "_lexClassTable", (char*)&lexClassTable, 0, (N_DATA | N_EXT)},
    {NULL, "_lexNclasses", (char*)&lexNclasses, 0, (N_DATA | N_EXT)},
    {NULL, "_lexNewLine", (char*)lexNewLine, 0, (N_TEXT | N_EXT)},
    {NULL, "_lexStateTable", (char*)&lexStateTable, 0, (N_DATA | N_EXT)},
    {NULL, "_linki", (char*)linki, 0, (N_TEXT | N_EXT)},
    {NULL, "_listAdd", (char*)listAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_listCount", (char*)listCount, 0, (N_TEXT | N_EXT)},
    {NULL, "_listen", (char*)listen, 0, (N_TEXT | N_EXT)},
    {NULL, "_listGet", (char*)listGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_listInit", (char*)listInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_listInsert", (char*)listInsert, 0, (N_TEXT | N_EXT)},
    {NULL, "_listRemove", (char*)listRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_lkup", (char*)lkup, 0, (N_TEXT | N_EXT)},
    {NULL, "_ll", (char*)ll, 0, (N_TEXT | N_EXT)},
    {NULL, "_llr", (char*)llr, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadCommonManage", (char*)loadCommonManage, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadCommonMatch", (char*)loadCommonMatch, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadElfLibInit", (char*)loadElfLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadElfRelocRelEntryRead", (char*)loadElfRelocRelEntryRead, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadFunc", (char*)&loadFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_loadLibInit", (char*)loadLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadModule", (char*)loadModule, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadModuleAt", (char*)loadModuleAt, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadModuleAtSym", (char*)loadModuleAtSym, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadModuleGet", (char*)loadModuleGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadSegmentsAllocate", (char*)loadSegmentsAllocate, 0, (N_TEXT | N_EXT)},
    {NULL, "_loadSyncFunc", (char*)&loadSyncFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_loattach", (char*)loattach, 0, (N_TEXT | N_EXT)},
    {NULL, "_localtime", (char*)localtime, 0, (N_TEXT | N_EXT)},
    {NULL, "_localtime_r", (char*)localtime_r, 0, (N_TEXT | N_EXT)},
    {NULL, "___loctime", (char*)&__loctime, 0, (N_DATA | N_EXT)},
    {NULL, "_logFdAdd", (char*)logFdAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_logFdDelete", (char*)logFdDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_logLibInit", (char*)logLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_logMsg", (char*)logMsg, 0, (N_TEXT | N_EXT)},
    {NULL, "_logTaskId", (char*)&logTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_logTaskOptions", (char*)&logTaskOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_logTaskPriority", (char*)&logTaskPriority, 0, (N_BSS | N_EXT)},
    {NULL, "_logTaskStackSize", (char*)&logTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_loif", (char*)&loif, 0, (N_BSS | N_EXT)},
    {NULL, "_longjmp", (char*)longjmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_looutput", (char*)looutput, 0, (N_TEXT | N_EXT)},
    {NULL, "_ls", (char*)ls, 0, (N_TEXT | N_EXT)},
    {NULL, "_lseek", (char*)lseek, 0, (N_TEXT | N_EXT)},
    {NULL, "_lsr", (char*)lsr, 0, (N_TEXT | N_EXT)},
    {NULL, "_lstat", (char*)lstat, 0, (N_TEXT | N_EXT)},
    {NULL, "___ltostr", (char*)__ltostr, 0, (N_TEXT | N_EXT)},
    {NULL, "_m", (char*)m, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_adj", (char*)m_adj, 0, (N_TEXT | N_EXT)},
    {NULL, "_malloc", (char*)malloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_max_hdr", (char*)&max_hdr, 0, (N_BSS | N_EXT)},
    {NULL, "_max_linkhdr", (char*)&max_linkhdr, 0, (N_BSS | N_EXT)},
    {NULL, "_max_protohdr", (char*)&max_protohdr, 0, (N_BSS | N_EXT)},
    {NULL, "_mb_alloc", (char*)mb_alloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_mb_free", (char*)mb_free, 0, (N_TEXT | N_EXT)},
    {NULL, "_mBlkClCount", (char*)&mBlkClCount, 0, (N_BSS | N_EXT)},
    {NULL, "_mBlkClId", (char*)&mBlkClId, 0, (N_BSS | N_EXT)},
    {NULL, "_mBlkCount", (char*)&mBlkCount, 0, (N_BSS | N_EXT)},
    {NULL, "_mBlkId", (char*)&mBlkId, 0, (N_BSS | N_EXT)},
    {NULL, "_mbufLibInit", (char*)mbufLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_cat", (char*)m_cat, 0, (N_TEXT | N_EXT)},
    {NULL, "_mClBlkConfig", (char*)&mClBlkConfig, 0, (N_DATA | N_EXT)},
    {NULL, "_memalign", (char*)memalign, 0, (N_TEXT | N_EXT)},
    {NULL, "_memAutosize", (char*)memAutosize, 0, (N_TEXT | N_EXT)},
    {NULL, "_memccpy", (char*)memccpy, 0, (N_TEXT | N_EXT)},
    {NULL, "_memchr", (char*)memchr, 0, (N_TEXT | N_EXT)},
    {NULL, "_memcmp", (char*)memcmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_memcpy", (char*)memcpy, 0, (N_TEXT | N_EXT)},
    {NULL, "_memcpyw", (char*)memcpyw, 0, (N_TEXT | N_EXT)},
    {NULL, "_memDefaultAlignment", (char*)&memDefaultAlignment, 0, (N_DATA | N_EXT)},
    {NULL, "_memHeapPart", (char*)&memHeapPart, 0, (N_BSS | N_EXT)},
    {NULL, "_memHeapPartId", (char*)&memHeapPartId, 0, (N_DATA | N_EXT)},
    {NULL, "_memLibInit", (char*)memLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_memmove", (char*)memmove, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartAddToPool", (char*)memPartAddToPool, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartAlignedAlloc", (char*)memPartAlignedAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartAlloc", (char*)memPartAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartAllocErrorFunc", (char*)&memPartAllocErrorFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_memPartBlockErrorFunc", (char*)&memPartBlockErrorFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_memPartBlockValidate", (char*)memPartBlockValidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartClassId", (char*)&memPartClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_memPartCreate", (char*)memPartCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartDestroy", (char*)memPartDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartFree", (char*)memPartFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartInit", (char*)memPartInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartLibInit", (char*)memPartLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartOptionsDefault", (char*)&memPartOptionsDefault, 0, (N_DATA | N_EXT)},
    {NULL, "_memPartOptionsSet", (char*)memPartOptionsSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartRealloc", (char*)memPartRealloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_memPartSemInitFunc", (char*)&memPartSemInitFunc, 0, (N_DATA | N_EXT)},
    {NULL, "_memPartShow", (char*)memPartShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_memRestoreTestAddr", (char*)memRestoreTestAddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_memset", (char*)memset, 0, (N_TEXT | N_EXT)},
    {NULL, "_memsetw", (char*)memsetw, 0, (N_TEXT | N_EXT)},
    {NULL, "_memShowInit", (char*)memShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_memSysPartId", (char*)&memSysPartId, 0, (N_DATA | N_EXT)},
    {NULL, "_memSysPartition", (char*)&memSysPartition, 0, (N_BSS | N_EXT)},
    {NULL, "_memTop", (char*)&memTop, 0, (N_BSS | N_EXT)},
    {NULL, "_memTopPhys", (char*)&memTopPhys, 0, (N_BSS | N_EXT)},
    {NULL, "_memVerifyTestPattern", (char*)memVerifyTestPattern, 0, (N_TEXT | N_EXT)},
    {NULL, "_memWriteTestPattern", (char*)memWriteTestPattern, 0, (N_TEXT | N_EXT)},
    {NULL, "_messageListener", (char*)messageListener, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_get", (char*)m_get, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_gethdr", (char*)m_gethdr, 0, (N_TEXT | N_EXT)},
    {NULL, "_miscLibInit", (char*)miscLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_mkdir", (char*)mkdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_mkfs", (char*)mkfs, 0, (N_TEXT | N_EXT)},
    {NULL, "_mkfs_balloc", (char*)mkfs_balloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_mknod", (char*)mknod, 0, (N_TEXT | N_EXT)},
    {NULL, "_mknod1", (char*)mknod1, 0, (N_TEXT | N_EXT)},
    {NULL, "_mktime", (char*)mktime, 0, (N_TEXT | N_EXT)},
    {NULL, "_mmuEnable", (char*)mmuEnable, 0, (N_TEXT | N_EXT)},
    {NULL, "_mmuEnabled", (char*)&mmuEnabled, 0, (N_BSS | N_EXT)},
    {NULL, "_mmuLibFunctions", (char*)&mmuLibFunctions, 0, (N_BSS | N_EXT)},
    {NULL, "_mmuLibInit", (char*)mmuLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_mmuPageBlockSize", (char*)&mmuPageBlockSize, 0, (N_BSS | N_EXT)},
    {NULL, "_mmuPdbrGet", (char*)mmuPdbrGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_mmuPdbrSet", (char*)mmuPdbrSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_mmuStateTransTable", (char*)&mmuStateTransTable, 0, (N_BSS | N_EXT)},
    {NULL, "_mmuStateTransTableSize", (char*)&mmuStateTransTableSize, 0, (N_BSS | N_EXT)},
    {NULL, "_mmuTlbFlush", (char*)mmuTlbFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleCheck", (char*)moduleCheck, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleClassId", (char*)&moduleClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_moduleCreate", (char*)moduleCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleCreateHookAdd", (char*)moduleCreateHookAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleCreateHookDelete", (char*)moduleCreateHookDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleCreateHookSemOptions", (char*)&moduleCreateHookSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_moduleDelete", (char*)moduleDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleDestroy", (char*)moduleDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleEach", (char*)moduleEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleFindByGroup", (char*)moduleFindByGroup, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleFindByName", (char*)moduleFindByName, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleFindByNameAndPath", (char*)moduleFindByNameAndPath, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleFlagsGet", (char*)moduleFlagsGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleFormatGet", (char*)moduleFormatGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleGroupGet", (char*)moduleGroupGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleHook", (char*)moduleHook, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleIdFigure", (char*)moduleIdFigure, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleIdListGet", (char*)moduleIdListGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleInfoGet", (char*)moduleInfoGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleInit", (char*)moduleInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleLibInit", (char*)moduleLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleListSemOptions", (char*)&moduleListSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_moduleNameGet", (char*)moduleNameGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegAdd", (char*)moduleSegAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegEach", (char*)moduleSegEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegFirst", (char*)moduleSegFirst, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegGet", (char*)moduleSegGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegInfoGet", (char*)moduleSegInfoGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegNext", (char*)moduleSegNext, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleSegSemOptions", (char*)&moduleSegSemOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_moduleShow", (char*)moduleShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleShowInit", (char*)moduleShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_moduleTerminate", (char*)moduleTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_mountBufsCreate", (char*)mountBufsCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_mountCreate", (char*)mountCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_mountLock", (char*)mountLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_mountUnlock", (char*)mountUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_prepend", (char*)m_prepend, 0, (N_TEXT | N_EXT)},
    {NULL, "_m_pullup", (char*)m_pullup, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgListen", (char*)msgListen, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQClassId", (char*)&msgQClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_msgQCreate", (char*)msgQCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQDelete", (char*)msgQDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQDestroy", (char*)msgQDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQEvRegister", (char*)msgQEvRegister, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQEvUnregister", (char*)msgQEvUnregister, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQId", (char*)&msgQId, 0, (N_BSS | N_EXT)},
    {NULL, "_msgQInfoGet", (char*)msgQInfoGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQInit", (char*)msgQInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQLibInit", (char*)msgQLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQNumMsgs", (char*)msgQNumMsgs, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQReceive", (char*)msgQReceive, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQSend", (char*)msgQSend, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQShow", (char*)msgQShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQShowInit", (char*)msgQShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_msgQTerminate", (char*)msgQTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_mutextOptionsLogLib", (char*)&mutextOptionsLogLib, 0, (N_DATA | N_EXT)},
    {NULL, "_namei", (char*)namei, 0, (N_TEXT | N_EXT)},
    {NULL, "_namelessPrefix", (char*)&namelessPrefix, 0, (N_DATA | N_EXT)},
    {NULL, "_nblocks", (char*)&nblocks, 0, (N_DATA | N_EXT)},
    {NULL, "_netBufCollectFuncSet", (char*)netBufCollectFuncSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netBufLibInit", (char*)netBufLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClBlkFree", (char*)netClBlkFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClBlkGet", (char*)netClBlkGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClFree", (char*)netClFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClient", (char*)netClient, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClPoolIdGet", (char*)netClPoolIdGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netClusterGet", (char*)netClusterGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netJobAdd", (char*)netJobAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_netLibInit", (char*)netLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_netLibInstalled", (char*)&netLibInstalled, 0, (N_BSS | N_EXT)},
    {NULL, "_netMblkChainDup", (char*)netMblkChainDup, 0, (N_TEXT | N_EXT)},
    {NULL, "_netMblkClChainFree", (char*)netMblkClChainFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_netMblkClFree", (char*)netMblkClFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_netMblkClGet", (char*)netMblkClGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netMblkFree", (char*)netMblkFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_netMblkGet", (char*)netMblkGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_netPoolDelete", (char*)netPoolDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_netPoolInit", (char*)netPoolInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_netPoolShow", (char*)netPoolShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_netServer", (char*)netServer, 0, (N_TEXT | N_EXT)},
    {NULL, "_netSocketBind", (char*)netSocketBind, 0, (N_TEXT | N_EXT)},
    {NULL, "_netSocketConnect", (char*)netSocketConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_netSocketDelete", (char*)netSocketDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_netSocketNew", (char*)netSocketNew, 0, (N_TEXT | N_EXT)},
    {NULL, "_netSysDpoolId", (char*)&netSysDpoolId, 0, (N_DATA | N_EXT)},
    {NULL, "_netSysPoolId", (char*)&netSysPoolId, 0, (N_DATA | N_EXT)},
    {NULL, "_netTask", (char*)netTask, 0, (N_TEXT | N_EXT)},
    {NULL, "_netTaskId", (char*)&netTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_netTaskOptions", (char*)&netTaskOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_netTaskPriority", (char*)&netTaskPriority, 0, (N_DATA | N_EXT)},
    {NULL, "_netTaskSemId", (char*)&netTaskSemId, 0, (N_DATA | N_EXT)},
    {NULL, "_netTaskStackSize", (char*)&netTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_netTestShell", (char*)netTestShell, 0, (N_TEXT | N_EXT)},
    {NULL, "_netTupleGet", (char*)netTupleGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_newClBlk", (char*)newClBlk, 0, (N_TEXT | N_EXT)},
    {NULL, "_newImg", (char*)newImg, 0, (N_TEXT | N_EXT)},
    {NULL, "_newImgLoLin", (char*)newImgLoLin, 0, (N_TEXT | N_EXT)},
    {NULL, "_newMblk", (char*)newMblk, 0, (N_TEXT | N_EXT)},
    {NULL, "_newTuple", (char*)newTuple, 0, (N_TEXT | N_EXT)},
    {NULL, "_ninodes", (char*)&ninodes, 0, (N_DATA | N_EXT)},
    {NULL, "_numMsg", (char*)&numMsg, 0, (N_BSS | N_EXT)},
    {NULL, "_objAlloc", (char*)objAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_objAllocPad", (char*)objAllocPad, 0, (N_TEXT | N_EXT)},
    {NULL, "_objCoreInit", (char*)objCoreInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_objCoreTerminate", (char*)objCoreTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_objFree", (char*)objFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_objShow", (char*)objShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_ofile", (char*)&ofile, 0, (N_BSS | N_EXT)},
    {NULL, "_open", (char*)open, 0, (N_TEXT | N_EXT)},
    {NULL, "_openConnect", (char*)openConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_opendir", (char*)opendir, 0, (N_TEXT | N_EXT)},
    {NULL, "_openFile", (char*)openFile, 0, (N_TEXT | N_EXT)},
    {NULL, "_openInternal", (char*)openInternal, 0, (N_TEXT | N_EXT)},
    {NULL, "_panic", (char*)panic, 0, (N_TEXT | N_EXT)},
    {NULL, "_panicHookFunc", (char*)&panicHookFunc, 0, (N_BSS | N_EXT)},
    {NULL, "_panicSuspend", (char*)&panicSuspend, 0, (N_DATA | N_EXT)},
    {NULL, "_pathCondense", (char*)pathCondense, 0, (N_TEXT | N_EXT)},
    {NULL, "_pathconf", (char*)pathconf, 0, (N_TEXT | N_EXT)},
    {NULL, "_pathCwdLen", (char*)pathCwdLen, 0, (N_TEXT | N_EXT)},
    {NULL, "_pathLibInit", (char*)pathLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_pathPrependCwd", (char*)pathPrependCwd, 0, (N_TEXT | N_EXT)},
    {NULL, "_pathSplit", (char*)pathSplit, 0, (N_TEXT | N_EXT)},
    {NULL, "_pause", (char*)pause, 0, (N_TEXT | N_EXT)},
    {NULL, "_pBgBmp", (char*)&pBgBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pBgDib", (char*)&pBgDib, 0, (N_BSS | N_EXT)},
    {NULL, "_pbuf", (char*)&pbuf, 0, (N_BSS | N_EXT)},
    {NULL, "_pc", (char*)pc, 0, (N_TEXT | N_EXT)},
    {NULL, "_pcConDev", (char*)&pcConDev, 0, (N_BSS | N_EXT)},
    {NULL, "_pcConDevCreate", (char*)pcConDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_pcConDrvInit", (char*)pcConDrvInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_pcConDrvNumber", (char*)pcConDrvNumber, 0, (N_TEXT | N_EXT)},
    {NULL, "_pDbBmp", (char*)&pDbBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pDblBmp", (char*)&pDblBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_period", (char*)period, 0, (N_TEXT | N_EXT)},
    {NULL, "_periodicState", (char*)&periodicState, 0, (N_BSS | N_EXT)},
    {NULL, "_periodicTaskId", (char*)&periodicTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_periodicValue", (char*)&periodicValue, 0, (N_BSS | N_EXT)},
    {NULL, "_periodRun", (char*)periodRun, 0, (N_TEXT | N_EXT)},
    {NULL, "_pfctlinput", (char*)pfctlinput, 0, (N_TEXT | N_EXT)},
    {NULL, "_pffindproto", (char*)pffindproto, 0, (N_TEXT | N_EXT)},
    {NULL, "_pffindtype", (char*)pffindtype, 0, (N_TEXT | N_EXT)},
    {NULL, "_pFgBmp", (char*)&pFgBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pFgDib", (char*)&pFgDib, 0, (N_BSS | N_EXT)},
    {NULL, "_pFglBmp", (char*)&pFglBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pinballClut", (char*)&pinballClut, 0, (N_DATA | N_EXT)},
    {NULL, "_pinballData", (char*)&pinballData, 0, (N_DATA | N_EXT)},
    {NULL, "_pinballHeight", (char*)&pinballHeight, 0, (N_DATA | N_EXT)},
    {NULL, "_pinballWidth", (char*)&pinballWidth, 0, (N_DATA | N_EXT)},
    {NULL, "_pinode", (char*)&pinode, 0, (N_BSS | N_EXT)},
    {NULL, "_pipe0", (char*)&pipe0, 0, (N_BSS | N_EXT)},
    {NULL, "_pipeDevCreate", (char*)pipeDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_pipeDevDelete", (char*)pipeDevDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_pipeDrvInit", (char*)pipeDrvInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_pipeDrvMsgQOptions", (char*)&pipeDrvMsgQOptions, 0, (N_BSS | N_EXT)},
    {NULL, "_pKernExcStkBase", (char*)&pKernExcStkBase, 0, (N_BSS | N_EXT)},
    {NULL, "_pKernExcStkEnd", (char*)&pKernExcStkEnd, 0, (N_BSS | N_EXT)},
    {NULL, "__pNetBufCollect", (char*)&_pNetBufCollect, 0, (N_BSS | N_EXT)},
    {NULL, "__pNetPoolFuncTable", (char*)&_pNetPoolFuncTable, 0, (N_DATA | N_EXT)},
    {NULL, "_ppGlobalEnviron", (char*)&ppGlobalEnviron, 0, (N_BSS | N_EXT)},
    {NULL, "_print_blk", (char*)print_blk, 0, (N_TEXT | N_EXT)},
    {NULL, "_printErr", (char*)printErr, 0, (N_TEXT | N_EXT)},
    {NULL, "_printExc", (char*)printExc, 0, (N_TEXT | N_EXT)},
    {NULL, "_printf", (char*)printf, 0, (N_TEXT | N_EXT)},
    {NULL, "_print_inode", (char*)print_inode, 0, (N_TEXT | N_EXT)},
    {NULL, "_printLocalStatus", (char*)printLocalStatus, 0, (N_TEXT | N_EXT)},
    {NULL, "_printLogo", (char*)printLogo, 0, (N_TEXT | N_EXT)},
    {NULL, "_pRootMemStart", (char*)&pRootMemStart, 0, (N_BSS | N_EXT)},
    {NULL, "_pRootMount", (char*)&pRootMount, 0, (N_BSS | N_EXT)},
    {NULL, "_pRootSyncer", (char*)&pRootSyncer, 0, (N_BSS | N_EXT)},
    {NULL, "_prTask", (char*)prTask, 0, (N_TEXT | N_EXT)},
    {NULL, "_pSaveBmp", (char*)&pSaveBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pSavelBmp", (char*)&pSavelBmp, 0, (N_BSS | N_EXT)},
    {NULL, "_pSockFdMap", (char*)&pSockFdMap, 0, (N_BSS | N_EXT)},
    {NULL, "_pSysGdt", (char*)&pSysGdt, 0, (N_DATA | N_EXT)},
    {NULL, "_putc", (char*)putc, 0, (N_TEXT | N_EXT)},
    {NULL, "_putchar", (char*)putchar, 0, (N_TEXT | N_EXT)},
    {NULL, "_putenv", (char*)putenv, 0, (N_TEXT | N_EXT)},
    {NULL, "_puts", (char*)puts, 0, (N_TEXT | N_EXT)},
    {NULL, "_pwd", (char*)pwd, 0, (N_TEXT | N_EXT)},
    {NULL, "_qAdvance", (char*)qAdvance, 0, (N_TEXT | N_EXT)},
    {NULL, "_qCreate", (char*)qCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_qDestroy", (char*)qDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "___qdivrem", (char*)__qdivrem, 0, (N_TEXT | N_EXT)},
    {NULL, "_qEach", (char*)qEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_qExpired", (char*)qExpired, 0, (N_TEXT | N_EXT)},
    {NULL, "_qFifoClassId", (char*)&qFifoClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_qFirst", (char*)qFirst, 0, (N_TEXT | N_EXT)},
    {NULL, "_qGet", (char*)qGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_qInfo", (char*)qInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_qInit", (char*)qInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_qKey", (char*)qKey, 0, (N_TEXT | N_EXT)},
    {NULL, "_qMove", (char*)qMove, 0, (N_TEXT | N_EXT)},
    {NULL, "_qMsgClassId", (char*)&qMsgClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_qMsgGet", (char*)qMsgGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_qMsgPut", (char*)qMsgPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_qMsgTerminate", (char*)qMsgTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_qOffset", (char*)qOffset, 0, (N_TEXT | N_EXT)},
    {NULL, "_qPriBmpClassId", (char*)&qPriBmpClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_qPrioClassId", (char*)&qPrioClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_qPut", (char*)qPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_qRemove", (char*)qRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_qTerminate", (char*)qTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_radixLibInit", (char*)radixLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_raise", (char*)raise, 0, (N_TEXT | N_EXT)},
    {NULL, "_ramDevCreate", (char*)ramDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_ramDiskInit", (char*)ramDiskInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ramdsk_buf", (char*)&ramdsk_buf, 0, (N_BSS | N_EXT)},
    {NULL, "_ramdsk_deinit", (char*)ramdsk_deinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ramdsk_init", (char*)ramdsk_init, 0, (N_TEXT | N_EXT)},
    {NULL, "_ramdsk_rw", (char*)ramdsk_rw, 0, (N_TEXT | N_EXT)},
    {NULL, "_rand", (char*)rand, 0, (N_TEXT | N_EXT)},
    {NULL, "__randseed", (char*)&_randseed, 0, (N_DATA | N_EXT)},
    {NULL, "_rasterOp", (char*)&rasterOp, 0, (N_DATA | N_EXT)},
    {NULL, "_rasterOpAnd", (char*)rasterOpAnd, 0, (N_TEXT | N_EXT)},
    {NULL, "_rasterOpCopy", (char*)rasterOpCopy, 0, (N_TEXT | N_EXT)},
    {NULL, "_rasterOpOr", (char*)rasterOpOr, 0, (N_TEXT | N_EXT)},
    {NULL, "_rasterOpXor", (char*)rasterOpXor, 0, (N_TEXT | N_EXT)},
    {NULL, "_raw_attach", (char*)raw_attach, 0, (N_TEXT | N_EXT)},
    {NULL, "_raw_detach", (char*)raw_detach, 0, (N_TEXT | N_EXT)},
    {NULL, "_raw_disconnect", (char*)raw_disconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_raw_input", (char*)raw_input, 0, (N_TEXT | N_EXT)},
    {NULL, "_rawipCfgParams", (char*)&rawipCfgParams, 0, (N_DATA | N_EXT)},
    {NULL, "_rawIpLibInit", (char*)rawIpLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_rawLibInit", (char*)rawLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_rawList", (char*)&rawList, 0, (N_BSS | N_EXT)},
    {NULL, "_raw_recvspace", (char*)&raw_recvspace, 0, (N_BSS | N_EXT)},
    {NULL, "_raw_sendspace", (char*)&raw_sendspace, 0, (N_BSS | N_EXT)},
    {NULL, "_rawSocketConnect", (char*)rawSocketConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_rawSocketNew", (char*)rawSocketNew, 0, (N_TEXT | N_EXT)},
    {NULL, "_raw_usrreq", (char*)raw_usrreq, 0, (N_TEXT | N_EXT)},
    {NULL, "_read", (char*)read, 0, (N_TEXT | N_EXT)},
    {NULL, "_readdir", (char*)readdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_readi", (char*)readi, 0, (N_TEXT | N_EXT)},
    {NULL, "_readlink", (char*)readlink, 0, (N_TEXT | N_EXT)},
    {NULL, "_readSelect", (char*)readSelect, 0, (N_TEXT | N_EXT)},
    {NULL, "_realloc", (char*)realloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_receiveMessage", (char*)receiveMessage, 0, (N_TEXT | N_EXT)},
    {NULL, "_recv", (char*)recv, 0, (N_TEXT | N_EXT)},
    {NULL, "_recvfrom", (char*)recvfrom, 0, (N_TEXT | N_EXT)},
    {NULL, "_recvmsg", (char*)recvmsg, 0, (N_TEXT | N_EXT)},
    {NULL, "_redirInFd", (char*)&redirInFd, 0, (N_DATA | N_EXT)},
    {NULL, "_redirOutFd", (char*)&redirOutFd, 0, (N_DATA | N_EXT)},
    {NULL, "_remove", (char*)remove, 0, (N_TEXT | N_EXT)},
    {NULL, "_removeConnect", (char*)removeConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_removeFile", (char*)removeFile, 0, (N_TEXT | N_EXT)},
    {NULL, "_remque", (char*)remque, 0, (N_TEXT | N_EXT)},
    {NULL, "_repeat", (char*)repeat, 0, (N_TEXT | N_EXT)},
    {NULL, "_repeatRun", (char*)repeatRun, 0, (N_TEXT | N_EXT)},
    {NULL, "_restartTaskName", (char*)&restartTaskName, 0, (N_DATA | N_EXT)},
    {NULL, "_restartTaskOptions", (char*)&restartTaskOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_restartTaskPriority", (char*)&restartTaskPriority, 0, (N_BSS | N_EXT)},
    {NULL, "_restartTaskStackSize", (char*)&restartTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_rewinddir", (char*)rewinddir, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringBufferGet", (char*)ringBufferGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringBufferPut", (char*)ringBufferPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringCreate", (char*)ringCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringDelete", (char*)ringDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringFlush", (char*)ringFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringFreeBytes", (char*)ringFreeBytes, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringIsEmpty", (char*)ringIsEmpty, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringIsFull", (char*)ringIsFull, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringMoveAhead", (char*)ringMoveAhead, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringNBytes", (char*)ringNBytes, 0, (N_TEXT | N_EXT)},
    {NULL, "_ringPutAhead", (char*)ringPutAhead, 0, (N_TEXT | N_EXT)},
    {NULL, "_rinode", (char*)rinode, 0, (N_TEXT | N_EXT)},
    {NULL, "_rip_ctloutput", (char*)rip_ctloutput, 0, (N_TEXT | N_EXT)},
    {NULL, "_rip_input", (char*)rip_input, 0, (N_TEXT | N_EXT)},
    {NULL, "_rip_output", (char*)rip_output, 0, (N_TEXT | N_EXT)},
    {NULL, "_rip_usrreq", (char*)rip_usrreq, 0, (N_TEXT | N_EXT)},
    {NULL, "_rmdir", (char*)rmdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_addmask", (char*)rn_addmask, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_addroute", (char*)rn_addroute, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_delete", (char*)rn_delete, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_inithead", (char*)rn_inithead, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_insert", (char*)rn_insert, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_lookup", (char*)rn_lookup, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_match", (char*)rn_match, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_newpair", (char*)rn_newpair, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_refines", (char*)rn_refines, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_search", (char*)rn_search, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_search_m", (char*)rn_search_m, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_walktree", (char*)rn_walktree, 0, (N_TEXT | N_EXT)},
    {NULL, "_rn_walktree_from", (char*)rn_walktree_from, 0, (N_TEXT | N_EXT)},
    {NULL, "_rootClassId", (char*)&rootClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_rootMemNBytes", (char*)&rootMemNBytes, 0, (N_BSS | N_EXT)},
    {NULL, "_rootTaskId", (char*)&rootTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_route_cb", (char*)&route_cb, 0, (N_BSS | N_EXT)},
    {NULL, "_routeLibInit", (char*)routeLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_routeShow", (char*)routeShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_rsect", (char*)rsect, 0, (N_TEXT | N_EXT)},
    {NULL, "_rsPeriodicTask", (char*)rsPeriodicTask, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt11FsDevInit", (char*)rt11FsDevInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt11FsFdMutexOptions", (char*)&rt11FsFdMutexOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_rt11FsLibInit", (char*)rt11FsLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt11FsMaxFiles", (char*)&rt11FsMaxFiles, 0, (N_BSS | N_EXT)},
    {NULL, "_rt11FsReadyChange", (char*)rt11FsReadyChange, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt11FsVolMutexOptions", (char*)&rt11FsVolMutexOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_rtalloc", (char*)rtalloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtalloc1", (char*)rtalloc1, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtfree", (char*)rtfree, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtIfaceMsgHook", (char*)&rtIfaceMsgHook, 0, (N_BSS | N_EXT)},
    {NULL, "_rtinit", (char*)rtinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtioctl", (char*)rtioctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtMissMsgHook", (char*)&rtMissMsgHook, 0, (N_BSS | N_EXT)},
    {NULL, "_rtNewAddrMsgHook", (char*)&rtNewAddrMsgHook, 0, (N_BSS | N_EXT)},
    {NULL, "_rtosballClut", (char*)&rtosballClut, 0, (N_DATA | N_EXT)},
    {NULL, "_rtosballData", (char*)&rtosballData, 0, (N_DATA | N_EXT)},
    {NULL, "_rtosballHeight", (char*)&rtosballHeight, 0, (N_DATA | N_EXT)},
    {NULL, "_rtosballWidth", (char*)&rtosballWidth, 0, (N_DATA | N_EXT)},
    {NULL, "_rtosDelay", (char*)rtosDelay, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosDelete", (char*)rtosDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQFlush", (char*)rtosPendQFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQGet", (char*)rtosPendQGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQPut", (char*)rtosPendQPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQRemove", (char*)rtosPendQRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQTerminate", (char*)rtosPendQTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPendQWithHandlerPut", (char*)rtosPendQWithHandlerPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosPrioritySet", (char*)rtosPrioritySet, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosReadyQPut", (char*)rtosReadyQPut, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosReadyQRemove", (char*)rtosReadyQRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosResume", (char*)rtosResume, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosSemDelete", (char*)rtosSemDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosSpawn", (char*)rtosSpawn, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosSuspend", (char*)rtosSuspend, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosTickAnnounce", (char*)rtosTickAnnounce, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosUndelay", (char*)rtosUndelay, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosWdCancel", (char*)rtosWdCancel, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtosWdStart", (char*)rtosWdStart, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtredirect", (char*)rtredirect, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtrequest", (char*)rtrequest, 0, (N_TEXT | N_EXT)},
    {NULL, "_rtrequestAddEqui", (char*)rtrequestAddEqui, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt_setgate", (char*)rt_setgate, 0, (N_TEXT | N_EXT)},
    {NULL, "_rt_tables", (char*)&rt_tables, 0, (N_BSS | N_EXT)},
    {NULL, "_sb", (char*)&sb, 0, (N_BSS | N_EXT)},
    {NULL, "_sbappend", (char*)sbappend, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbappendaddr", (char*)sbappendaddr, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbappendrecord", (char*)sbappendrecord, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbcompress", (char*)sbcompress, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbdrop", (char*)sbdrop, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbdroprecord", (char*)sbdroprecord, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbflush", (char*)sbflush, 0, (N_TEXT | N_EXT)},
    {NULL, "_sb_max", (char*)&sb_max, 0, (N_DATA | N_EXT)},
    {NULL, "_sbrelease", (char*)sbrelease, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbreserve", (char*)sbreserve, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbseldequeue", (char*)sbseldequeue, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbwait", (char*)sbwait, 0, (N_TEXT | N_EXT)},
    {NULL, "_sbwakeup", (char*)sbwakeup, 0, (N_TEXT | N_EXT)},
    {NULL, "_scanf", (char*)scanf, 0, (N_TEXT | N_EXT)},
    {NULL, "___sclose", (char*)__sclose, 0, (N_TEXT | N_EXT)},
    {NULL, "_second", (char*)second, 0, (N_TEXT | N_EXT)},
    {NULL, "_select", (char*)select, 0, (N_TEXT | N_EXT)},
    {NULL, "_selectLibInit", (char*)selectLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_selectLibInitDelete", (char*)selectLibInitDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_selfRestarter", (char*)selfRestarter, 0, (N_TEXT | N_EXT)},
    {NULL, "_selfSignal", (char*)selfSignal, 0, (N_TEXT | N_EXT)},
    {NULL, "_selNodeAdd", (char*)selNodeAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_selNodeDelete", (char*)selNodeDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeup", (char*)selWakeup, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeupAll", (char*)selWakeupAll, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeupListInit", (char*)selWakeupListInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeupListLen", (char*)selWakeupListLen, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeupListTerminate", (char*)selWakeupListTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_selWakeupType", (char*)selWakeupType, 0, (N_TEXT | N_EXT)},
    {NULL, "_sem", (char*)&sem, 0, (N_BSS | N_EXT)},
    {NULL, "_sem2", (char*)&sem2, 0, (N_BSS | N_EXT)},
    {NULL, "_semaphoreListener", (char*)semaphoreListener, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBCoreInit", (char*)semBCoreInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBCreate", (char*)semBCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBGive", (char*)semBGive, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBGiveDefer", (char*)semBGiveDefer, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBInit", (char*)semBInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBLibInit", (char*)semBLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semBTake", (char*)semBTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCCoreInit", (char*)semCCoreInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCCreate", (char*)semCCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCGive", (char*)semCGive, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCGiveDefer", (char*)semCGiveDefer, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCInit", (char*)semCInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semClass", (char*)&semClass, 0, (N_BSS | N_EXT)},
    {NULL, "_semClassId", (char*)&semClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_semCLibInit", (char*)semCLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCreate", (char*)semCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semCTake", (char*)semCTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_semDelete", (char*)semDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_semDestroy", (char*)semDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_semEvRegister", (char*)semEvRegister, 0, (N_TEXT | N_EXT)},
    {NULL, "_semEvUnregister", (char*)semEvUnregister, 0, (N_TEXT | N_EXT)},
    {NULL, "_semFlush", (char*)semFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_semFlushDeferTable", (char*)&semFlushDeferTable, 0, (N_DATA | N_EXT)},
    {NULL, "_semFlushTable", (char*)&semFlushTable, 0, (N_DATA | N_EXT)},
    {NULL, "_semGive", (char*)semGive, 0, (N_TEXT | N_EXT)},
    {NULL, "_semGiveDeferTable", (char*)&semGiveDeferTable, 0, (N_DATA | N_EXT)},
    {NULL, "_semGiveTable", (char*)&semGiveTable, 0, (N_DATA | N_EXT)},
    {NULL, "_semInfo", (char*)semInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_semInit", (char*)semInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semInvalid", (char*)semInvalid, 0, (N_TEXT | N_EXT)},
    {NULL, "_semLibInit", (char*)semLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semListen", (char*)semListen, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMCoreInit", (char*)semMCoreInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMCreate", (char*)semMCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMGive", (char*)semMGive, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMGiveForce", (char*)semMGiveForce, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMInit", (char*)semMInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMLibInit", (char*)semMLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semMTake", (char*)semMTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_semQFlush", (char*)semQFlush, 0, (N_TEXT | N_EXT)},
    {NULL, "_semQFlushDefer", (char*)semQFlushDefer, 0, (N_TEXT | N_EXT)},
    {NULL, "_semQInit", (char*)semQInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semReaderTake", (char*)semReaderTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWCreate", (char*)semRWCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWDowngrade", (char*)semRWDowngrade, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWInit", (char*)semRWInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWLibInit", (char*)semRWLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWUpgrade", (char*)semRWUpgrade, 0, (N_TEXT | N_EXT)},
    {NULL, "_semRWUpgradeForce", (char*)semRWUpgradeForce, 0, (N_TEXT | N_EXT)},
    {NULL, "_semShow", (char*)semShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_semShowInit", (char*)semShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_semTake", (char*)semTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_semTakeTable", (char*)&semTakeTable, 0, (N_DATA | N_EXT)},
    {NULL, "_semTerminate", (char*)semTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_semUse", (char*)semUse, 0, (N_TEXT | N_EXT)},
    {NULL, "_semWriterTake", (char*)semWriterTake, 0, (N_TEXT | N_EXT)},
    {NULL, "_send", (char*)send, 0, (N_TEXT | N_EXT)},
    {NULL, "_sendMessage", (char*)sendMessage, 0, (N_TEXT | N_EXT)},
    {NULL, "_sendmsg", (char*)sendmsg, 0, (N_TEXT | N_EXT)},
    {NULL, "_sendQSignal", (char*)sendQSignal, 0, (N_TEXT | N_EXT)},
    {NULL, "_sendSignal", (char*)sendSignal, 0, (N_TEXT | N_EXT)},
    {NULL, "_sendto", (char*)sendto, 0, (N_TEXT | N_EXT)},
    {NULL, "_setAbortChar", (char*)setAbortChar, 0, (N_TEXT | N_EXT)},
    {NULL, "_setActivePage", (char*)setActivePage, 0, (N_TEXT | N_EXT)},
    {NULL, "_setActiveStart", (char*)setActiveStart, 0, (N_TEXT | N_EXT)},
    {NULL, "_setjmp", (char*)setjmp, 0, (N_TEXT | N_EXT)},
    {NULL, "__setjmpSetup", (char*)_setjmpSetup, 0, (N_TEXT | N_EXT)},
    {NULL, "_setLabel", (char*)setLabel, 0, (N_TEXT | N_EXT)},
    {NULL, "_setPalette", (char*)setPalette, 0, (N_TEXT | N_EXT)},
    {NULL, "_setPixel", (char*)setPixel, 0, (N_TEXT | N_EXT)},
    {NULL, "_setPixelInt", (char*)setPixelInt, 0, (N_TEXT | N_EXT)},
    {NULL, "_setsockopt", (char*)setsockopt, 0, (N_TEXT | N_EXT)},
    {NULL, "_setTaskVar", (char*)setTaskVar, 0, (N_TEXT | N_EXT)},
    {NULL, "_setvbuf", (char*)setvbuf, 0, (N_TEXT | N_EXT)},
    {NULL, "_setVisiblePage", (char*)setVisiblePage, 0, (N_TEXT | N_EXT)},
    {NULL, "_setVisibleStart", (char*)setVisibleStart, 0, (N_TEXT | N_EXT)},
    {NULL, "___sflags", (char*)__sflags, 0, (N_TEXT | N_EXT)},
    {NULL, "___sflush", (char*)__sflush, 0, (N_TEXT | N_EXT)},
    {NULL, "___sfvwrite", (char*)__sfvwrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_shell", (char*)shell, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLibInit", (char*)shellLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLock", (char*)shellLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLogin", (char*)shellLogin, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLoginInstall", (char*)shellLoginInstall, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLogout", (char*)shellLogout, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellLogoutInstall", (char*)shellLogoutInstall, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellSpawn", (char*)shellSpawn, 0, (N_TEXT | N_EXT)},
    {NULL, "_shellTaskId", (char*)&shellTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_shellTaskName", (char*)&shellTaskName, 0, (N_DATA | N_EXT)},
    {NULL, "_shellTaskOptions", (char*)&shellTaskOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_shellTaskPriority", (char*)&shellTaskPriority, 0, (N_DATA | N_EXT)},
    {NULL, "_shellTaskStackSize", (char*)&shellTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_show", (char*)show, 0, (N_TEXT | N_EXT)},
    {NULL, "_showBigString", (char*)showBigString, 0, (N_TEXT | N_EXT)},
    {NULL, "_showFsInfo", (char*)showFsInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_showInfo", (char*)showInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_showSysTime", (char*)showSysTime, 0, (N_TEXT | N_EXT)},
    {NULL, "_showTaskVars", (char*)showTaskVars, 0, (N_TEXT | N_EXT)},
    {NULL, "_shutdown", (char*)shutdown, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigaction", (char*)sigaction, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigaddset", (char*)sigaddset, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigCtxLoad", (char*)_sigCtxLoad, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigCtxRetValueSet", (char*)_sigCtxRetValueSet, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigCtxSave", (char*)_sigCtxSave, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigCtxSetup", (char*)_sigCtxSetup, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigCtxStackEnd", (char*)_sigCtxStackEnd, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigdelset", (char*)sigdelset, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigemptyset", (char*)sigemptyset, 0, (N_TEXT | N_EXT)},
    {NULL, "__sigfaulttable", (char*)&_sigfaulttable, 0, (N_DATA | N_EXT)},
    {NULL, "_sigfillset", (char*)sigfillset, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigismember", (char*)sigismember, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigLibInit", (char*)sigLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_signal", (char*)signal, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigPendDestroy", (char*)sigPendDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigpending", (char*)sigpending, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigPendInit", (char*)sigPendInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigPendKill", (char*)sigPendKill, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigprocmask", (char*)sigprocmask, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigqueue", (char*)sigqueue, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigqueueInit", (char*)sigqueueInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigreturn", (char*)sigreturn, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigsetjmp", (char*)sigsetjmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_sigsuspend", (char*)sigsuspend, 0, (N_TEXT | N_EXT)},
    {NULL, "_size", (char*)&size, 0, (N_DATA | N_EXT)},
    {NULL, "_sleep", (char*)sleep, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllAdd", (char*)sllAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllCount", (char*)sllCount, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllEach", (char*)sllEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllGet", (char*)sllGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllInit", (char*)sllInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllInsert", (char*)sllInsert, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllPrevious", (char*)sllPrevious, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllPutAtHead", (char*)sllPutAtHead, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllPutAtTail", (char*)sllPutAtTail, 0, (N_TEXT | N_EXT)},
    {NULL, "_sllRemove", (char*)sllRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_slowFill", (char*)slowFill, 0, (N_TEXT | N_EXT)},
    {NULL, "___smakebuf", (char*)__smakebuf, 0, (N_TEXT | N_EXT)},
    {NULL, "_snprintf", (char*)snprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_soabort", (char*)soabort, 0, (N_TEXT | N_EXT)},
    {NULL, "_soaccept", (char*)soaccept, 0, (N_TEXT | N_EXT)},
    {NULL, "_sobind", (char*)sobind, 0, (N_TEXT | N_EXT)},
    {NULL, "_socantrcvmore", (char*)socantrcvmore, 0, (N_TEXT | N_EXT)},
    {NULL, "_socantsendmore", (char*)socantsendmore, 0, (N_TEXT | N_EXT)},
    {NULL, "_socket", (char*)socket, 0, (N_TEXT | N_EXT)},
    {NULL, "_sockFdtosockFunc", (char*)sockFdtosockFunc, 0, (N_TEXT | N_EXT)},
    {NULL, "_sockLibInit", (char*)sockLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sockMapAdd", (char*)sockMapAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_soclose", (char*)soclose, 0, (N_TEXT | N_EXT)},
    {NULL, "_soconnect", (char*)soconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_soconnect2", (char*)soconnect2, 0, (N_TEXT | N_EXT)},
    {NULL, "_socreate", (char*)socreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_sodisconnect", (char*)sodisconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_sofree", (char*)sofree, 0, (N_TEXT | N_EXT)},
    {NULL, "_sogetopt", (char*)sogetopt, 0, (N_TEXT | N_EXT)},
    {NULL, "_sohasoutofband", (char*)sohasoutofband, 0, (N_TEXT | N_EXT)},
    {NULL, "_soisconnected", (char*)soisconnected, 0, (N_TEXT | N_EXT)},
    {NULL, "_soisconnecting", (char*)soisconnecting, 0, (N_TEXT | N_EXT)},
    {NULL, "_soisdisconnected", (char*)soisdisconnected, 0, (N_TEXT | N_EXT)},
    {NULL, "_soisdisconnecting", (char*)soisdisconnecting, 0, (N_TEXT | N_EXT)},
    {NULL, "_soLibInit", (char*)soLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_solisten", (char*)solisten, 0, (N_TEXT | N_EXT)},
    {NULL, "_somaxconn", (char*)&somaxconn, 0, (N_DATA | N_EXT)},
    {NULL, "_sonewconn", (char*)sonewconn, 0, (N_TEXT | N_EXT)},
    {NULL, "_soo_ioctl", (char*)soo_ioctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_soo_unselect", (char*)soo_unselect, 0, (N_TEXT | N_EXT)},
    {NULL, "_soqinsque", (char*)soqinsque, 0, (N_TEXT | N_EXT)},
    {NULL, "_soqremque", (char*)soqremque, 0, (N_TEXT | N_EXT)},
    {NULL, "_soreceive", (char*)soreceive, 0, (N_TEXT | N_EXT)},
    {NULL, "_soreserve", (char*)soreserve, 0, (N_TEXT | N_EXT)},
    {NULL, "_sorflush", (char*)sorflush, 0, (N_TEXT | N_EXT)},
    {NULL, "_sosend", (char*)sosend, 0, (N_TEXT | N_EXT)},
    {NULL, "_sosetopt", (char*)sosetopt, 0, (N_TEXT | N_EXT)},
    {NULL, "_soshutdown", (char*)soshutdown, 0, (N_TEXT | N_EXT)},
    {NULL, "_sowakeup", (char*)sowakeup, 0, (N_TEXT | N_EXT)},
    {NULL, "_sp", (char*)sp, 0, (N_TEXT | N_EXT)},
    {NULL, "_splimp", (char*)splimp, 0, (N_TEXT | N_EXT)},
    {NULL, "_splMutexOptions", (char*)&splMutexOptions, 0, (N_DATA | N_EXT)},
    {NULL, "_splnet", (char*)splnet, 0, (N_TEXT | N_EXT)},
    {NULL, "_splTaskId", (char*)&splTaskId, 0, (N_BSS | N_EXT)},
    {NULL, "_splx", (char*)splx, 0, (N_TEXT | N_EXT)},
    {NULL, "_sprintf", (char*)sprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_spTaskOptions", (char*)&spTaskOptions, 0, (N_BSS | N_EXT)},
    {NULL, "_spTaskPriority", (char*)&spTaskPriority, 0, (N_DATA | N_EXT)},
    {NULL, "_spTaskStackSize", (char*)&spTaskStackSize, 0, (N_DATA | N_EXT)},
    {NULL, "_srand", (char*)srand, 0, (N_TEXT | N_EXT)},
    {NULL, "___sread", (char*)__sread, 0, (N_TEXT | N_EXT)},
    {NULL, "___srefill", (char*)__srefill, 0, (N_TEXT | N_EXT)},
    {NULL, "___srget", (char*)__srget, 0, (N_TEXT | N_EXT)},
    {NULL, "_sscanf", (char*)sscanf, 0, (N_TEXT | N_EXT)},
    {NULL, "___sseek", (char*)__sseek, 0, (N_TEXT | N_EXT)},
    {NULL, "_standTable", (char*)&standTable, 0, (N_DATA | N_EXT)},
    {NULL, "_standTableInit", (char*)standTableInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_standTableSize", (char*)&standTableSize, 0, (N_BSS | N_EXT)},
    {NULL, "_startWd", (char*)startWd, 0, (N_TEXT | N_EXT)},
    {NULL, "_stat", (char*)stat, 0, (N_TEXT | N_EXT)},
    {NULL, "_stati", (char*)stati, 0, (N_TEXT | N_EXT)},
    {NULL, "___stderr", (char*)__stderr, 0, (N_TEXT | N_EXT)},
    {NULL, "___stdin", (char*)__stdin, 0, (N_TEXT | N_EXT)},
    {NULL, "_stdioFp", (char*)stdioFp, 0, (N_TEXT | N_EXT)},
    {NULL, "_stdioFpCreate", (char*)stdioFpCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_stdioFpDestroy", (char*)stdioFpDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_stdioFpInit", (char*)stdioFpInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_stdioLibInit", (char*)stdioLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "___stdout", (char*)__stdout, 0, (N_TEXT | N_EXT)},
    {NULL, "_strcat", (char*)strcat, 0, (N_TEXT | N_EXT)},
    {NULL, "_strchr", (char*)strchr, 0, (N_TEXT | N_EXT)},
    {NULL, "_strcmp", (char*)strcmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_strcpy", (char*)strcpy, 0, (N_TEXT | N_EXT)},
    {NULL, "_strcspn", (char*)strcspn, 0, (N_TEXT | N_EXT)},
    {NULL, "_strdup", (char*)strdup, 0, (N_TEXT | N_EXT)},
    {NULL, "_strftime", (char*)strftime, 0, (N_TEXT | N_EXT)},
    {NULL, "_strlen", (char*)strlen, 0, (N_TEXT | N_EXT)},
    {NULL, "_strncat", (char*)strncat, 0, (N_TEXT | N_EXT)},
    {NULL, "_strncmp", (char*)strncmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_strncpy", (char*)strncpy, 0, (N_TEXT | N_EXT)},
    {NULL, "_strpbrk", (char*)strpbrk, 0, (N_TEXT | N_EXT)},
    {NULL, "_strrchr", (char*)strrchr, 0, (N_TEXT | N_EXT)},
    {NULL, "_strspn", (char*)strspn, 0, (N_TEXT | N_EXT)},
    {NULL, "_strstr", (char*)strstr, 0, (N_TEXT | N_EXT)},
    {NULL, "_strtok", (char*)strtok, 0, (N_TEXT | N_EXT)},
    {NULL, "_strtok_r", (char*)strtok_r, 0, (N_TEXT | N_EXT)},
    {NULL, "__strto_l", (char*)_strto_l, 0, (N_TEXT | N_EXT)},
    {NULL, "_strtol", (char*)strtol, 0, (N_TEXT | N_EXT)},
    {NULL, "_strtoul", (char*)strtoul, 0, (N_TEXT | N_EXT)},
    {NULL, "___submore", (char*)__submore, 0, (N_TEXT | N_EXT)},
    {NULL, "_subnetsarelocal", (char*)&subnetsarelocal, 0, (N_DATA | N_EXT)},
    {NULL, "_swapHook", (char*)swapHook, 0, (N_TEXT | N_EXT)},
    {NULL, "_swapped_tasks", (char*)&swapped_tasks, 0, (N_BSS | N_EXT)},
    {NULL, "___swbuf", (char*)__swbuf, 0, (N_TEXT | N_EXT)},
    {NULL, "_switched_tasks", (char*)&switched_tasks, 0, (N_BSS | N_EXT)},
    {NULL, "_switchHook", (char*)switchHook, 0, (N_TEXT | N_EXT)},
    {NULL, "___swrite", (char*)__swrite, 0, (N_TEXT | N_EXT)},
    {NULL, "___swsetup", (char*)__swsetup, 0, (N_TEXT | N_EXT)},
    {NULL, "_symAdd", (char*)symAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_symCreate", (char*)symCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_symDemo", (char*)&symDemo, 0, (N_DATA | N_EXT)},
    {NULL, "_symDestroy", (char*)symDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_symEach", (char*)symEach, 0, (N_TEXT | N_EXT)},
    {NULL, "_symFindByName", (char*)symFindByName, 0, (N_TEXT | N_EXT)},
    {NULL, "_symFindByNameAndType", (char*)symFindByNameAndType, 0, (N_TEXT | N_EXT)},
    {NULL, "_symFindSymbol", (char*)symFindSymbol, 0, (N_TEXT | N_EXT)},
    {NULL, "_symGroupDefault", (char*)&symGroupDefault, 0, (N_BSS | N_EXT)},
    {NULL, "_symInit", (char*)symInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_symLibInit", (char*)symLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_symlink", (char*)symlink, 0, (N_TEXT | N_EXT)},
    {NULL, "_symNameGet", (char*)symNameGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_symRemove", (char*)symRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_symShow", (char*)symShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_symShowInit", (char*)symShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableAdd", (char*)symTableAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableClassId", (char*)&symTableClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_symTableCreate", (char*)symTableCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableDelete", (char*)symTableDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableDemo", (char*)&symTableDemo, 0, (N_DATA | N_EXT)},
    {NULL, "_symTableDestroy", (char*)symTableDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableInit", (char*)symTableInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableRemove", (char*)symTableRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTableTerminate", (char*)symTableTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_symTypeGet", (char*)symTypeGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_symValueGet", (char*)symValueGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_chdir", (char*)sys_chdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClDescTable", (char*)&sysClDescTable, 0, (N_DATA | N_EXT)},
    {NULL, "_sysClDescTableNumEntries", (char*)&sysClDescTableNumEntries, 0, (N_BSS | N_EXT)},
    {NULL, "_sysClockConnect", (char*)sysClockConnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClockDisable", (char*)sysClockDisable, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClockEnable", (char*)sysClockEnable, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClockInt", (char*)sysClockInt, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClockRateGet", (char*)sysClockRateGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysClockRateSet", (char*)sysClockRateSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_close", (char*)sys_close, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysCsExc", (char*)&sysCsExc, 0, (N_DATA | N_EXT)},
    {NULL, "_sysCsInt", (char*)&sysCsInt, 0, (N_DATA | N_EXT)},
    {NULL, "_sysCsSuper", (char*)&sysCsSuper, 0, (N_DATA | N_EXT)},
    {NULL, "_sysDelay", (char*)sysDelay, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_dup", (char*)sys_dup, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysFdBuf", (char*)&sysFdBuf, 0, (N_DATA | N_EXT)},
    {NULL, "_sysFdBufSize", (char*)&sysFdBufSize, 0, (N_DATA | N_EXT)},
    {NULL, "_sys_fstat", (char*)sys_fstat, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysGdt", (char*)sysGdt, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysGdtr", (char*)sysGdtr, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysHwInit", (char*)sysHwInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysHwInit0", (char*)sysHwInit0, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysHwInit2", (char*)sysHwInit2, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInByte", (char*)sysInByte, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInit", (char*)sysInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInLong", (char*)sysInLong, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInLongString", (char*)sysInLongString, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntDisablePIC", (char*)sysIntDisablePIC, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntEnablePIC", (char*)sysIntEnablePIC, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntIdtType", (char*)&sysIntIdtType, 0, (N_DATA | N_EXT)},
    {NULL, "_sysIntInitPIC", (char*)sysIntInitPIC, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntLevel", (char*)sysIntLevel, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntLock", (char*)sysIntLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysIntMask1", (char*)&sysIntMask1, 0, (N_BSS | N_EXT)},
    {NULL, "_sysIntMask2", (char*)&sysIntMask2, 0, (N_BSS | N_EXT)},
    {NULL, "_sysIntUnlock", (char*)sysIntUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInWord", (char*)sysInWord, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysInWordString", (char*)sysInWordString, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_link", (char*)sys_link, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysLoadGdt", (char*)sysLoadGdt, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysMclBlkConfig", (char*)&sysMclBlkConfig, 0, (N_DATA | N_EXT)},
    {NULL, "_sysMemTop", (char*)sysMemTop, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_mkdir", (char*)sys_mkdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_mknod", (char*)sys_mknod, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_open", (char*)sys_open, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysOutByte", (char*)sysOutByte, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysOutLong", (char*)sysOutLong, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysOutLongString", (char*)sysOutLongString, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysOutWord", (char*)sysOutWord, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysOutWordString", (char*)sysOutWordString, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysPhysMemDesc", (char*)&sysPhysMemDesc, 0, (N_DATA | N_EXT)},
    {NULL, "_sysPhysMemDescNumEntries", (char*)&sysPhysMemDescNumEntries, 0, (N_BSS | N_EXT)},
    {NULL, "_sysPhysMemTop", (char*)sysPhysMemTop, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_read", (char*)sys_read, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysReboot", (char*)sysReboot, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysSymTable", (char*)&sysSymTable, 0, (N_BSS | N_EXT)},
    {NULL, "_sys_unlink", (char*)sys_unlink, 0, (N_TEXT | N_EXT)},
    {NULL, "_sysVectorIRQ0", (char*)&sysVectorIRQ0, 0, (N_DATA | N_EXT)},
    {NULL, "_sysWait", (char*)sysWait, 0, (N_TEXT | N_EXT)},
    {NULL, "_sys_write", (char*)sys_write, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskActivate", (char*)taskActivate, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskArgGet", (char*)taskArgGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskArgSet", (char*)taskArgSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskClassId", (char*)&taskClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_taskCreat", (char*)taskCreat, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskCreateHookAdd", (char*)taskCreateHookAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskCreateHookDelete", (char*)taskCreateHookDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskCreateHooks", (char*)&taskCreateHooks, 0, (N_BSS | N_EXT)},
    {NULL, "_taskCreateHookShow", (char*)taskCreateHookShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDelay", (char*)taskDelay, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDelete", (char*)taskDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDeleteForce", (char*)taskDeleteForce, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDeleteHookAdd", (char*)taskDeleteHookAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDeleteHookDelete", (char*)taskDeleteHookDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDeleteHooks", (char*)&taskDeleteHooks, 0, (N_BSS | N_EXT)},
    {NULL, "_taskDeleteHookShow", (char*)taskDeleteHookShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskDestroy", (char*)taskDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskExit", (char*)taskExit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskHiPrio", (char*)taskHiPrio, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskHookLibInit", (char*)taskHookLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskHookShowInit", (char*)taskHookShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdCurrent", (char*)&taskIdCurrent, 0, (N_BSS | N_EXT)},
    {NULL, "_taskIdDefault", (char*)taskIdDefault, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdFigure", (char*)taskIdFigure, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdle", (char*)taskIdle, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdListGet", (char*)taskIdListGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdListSort", (char*)taskIdListSort, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdSelf", (char*)taskIdSelf, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskIdVerify", (char*)taskIdVerify, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskInfoGet", (char*)taskInfoGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskInit", (char*)taskInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskLibInit", (char*)taskLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskLock", (char*)taskLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskLoPrio", (char*)taskLoPrio, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskName", (char*)taskName, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskNameToId", (char*)taskNameToId, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskOptionsSet", (char*)taskOptionsSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskOptionsString", (char*)taskOptionsString, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskPriorityGet", (char*)taskPriorityGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskPrioritySet", (char*)taskPrioritySet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskPrioTest", (char*)taskPrioTest, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskRegName", (char*)&taskRegName, 0, (N_DATA | N_EXT)},
    {NULL, "_taskRegsFmt", (char*)&taskRegsFmt, 0, (N_DATA | N_EXT)},
    {NULL, "_taskRegsFormat", (char*)&taskRegsFormat, 0, (N_DATA | N_EXT)},
    {NULL, "_taskRegsGet", (char*)taskRegsGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskRegsInit", (char*)taskRegsInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskRegsShow", (char*)taskRegsShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskRestart", (char*)taskRestart, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskResume", (char*)taskResume, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskRetValueSet", (char*)taskRetValueSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSafe", (char*)taskSafe, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskShow", (char*)taskShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskShowInit", (char*)taskShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSpawn", (char*)taskSpawn, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskStackAllot", (char*)taskStackAllot, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskStatusString", (char*)taskStatusString, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSuspend", (char*)taskSuspend, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapHookAdd", (char*)taskSwapHookAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapHookAttach", (char*)taskSwapHookAttach, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapHookDelete", (char*)taskSwapHookDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapHookDetach", (char*)taskSwapHookDetach, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapHooks", (char*)&taskSwapHooks, 0, (N_BSS | N_EXT)},
    {NULL, "_taskSwapHookShow", (char*)taskSwapHookShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwapReference", (char*)&taskSwapReference, 0, (N_BSS | N_EXT)},
    {NULL, "_taskSwitchHookAdd", (char*)taskSwitchHookAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwitchHookDelete", (char*)taskSwitchHookDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskSwitchHooks", (char*)&taskSwitchHooks, 0, (N_BSS | N_EXT)},
    {NULL, "_taskSwitchHookShow", (char*)taskSwitchHookShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskTcb", (char*)taskTcb, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskTerminate", (char*)taskTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskUndelay", (char*)taskUndelay, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskUnlock", (char*)taskUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskUnsafe", (char*)taskUnsafe, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarAdd", (char*)taskVarAdd, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarDelete", (char*)taskVarDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarGet", (char*)taskVarGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarInfo", (char*)taskVarInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarLibInit", (char*)taskVarLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_taskVarSet", (char*)taskVarSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcbinfo", (char*)&tcbinfo, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_attach", (char*)tcp_attach, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_backoff", (char*)&tcp_backoff, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_canceltimers", (char*)tcp_canceltimers, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcpCfgParams", (char*)&tcpCfgParams, 0, (N_DATA | N_EXT)},
    {NULL, "_tcpClient", (char*)tcpClient, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_close", (char*)tcp_close, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_ctlinput", (char*)tcp_ctlinput, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_ctloutput", (char*)tcp_ctloutput, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_disconnect", (char*)tcp_disconnect, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_dooptions", (char*)tcp_dooptions, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_do_rfc1323", (char*)&tcp_do_rfc1323, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_drain", (char*)tcp_drain, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_drop", (char*)tcp_drop, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_fasttimo", (char*)tcp_fasttimo, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_input", (char*)tcp_input, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_iss", (char*)&tcp_iss, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_keepcnt", (char*)&tcp_keepcnt, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_keepidle", (char*)&tcp_keepidle, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_keepinit", (char*)&tcp_keepinit, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_keepintvl", (char*)&tcp_keepintvl, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_last_inpcb", (char*)&tcp_last_inpcb, 0, (N_BSS | N_EXT)},
    {NULL, "_tcpLibInit", (char*)tcpLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_maxidle", (char*)&tcp_maxidle, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_maxpersistidle", (char*)&tcp_maxpersistidle, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_msl", (char*)&tcp_msl, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_mss", (char*)tcp_mss, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_mssdflt", (char*)&tcp_mssdflt, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_newtcpcb", (char*)tcp_newtcpcb, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_notify", (char*)tcp_notify, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_now", (char*)&tcp_now, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_outflags", (char*)&tcp_outflags, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_output", (char*)tcp_output, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcpOutRsts", (char*)&tcpOutRsts, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_pcbhashsize", (char*)&tcp_pcbhashsize, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_pulloutofband", (char*)tcp_pulloutofband, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_quench", (char*)tcp_quench, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcpRandFunc", (char*)&tcpRandFunc, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_reass", (char*)tcp_reass, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_recvspace", (char*)&tcp_recvspace, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_respond", (char*)tcp_respond, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcprexmtthresh", (char*)&tcprexmtthresh, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_rttdflt", (char*)&tcp_rttdflt, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_saveti", (char*)&tcp_saveti, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_sendspace", (char*)&tcp_sendspace, 0, (N_DATA | N_EXT)},
    {NULL, "_tcpServer", (char*)tcpServer, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_setpersist", (char*)tcp_setpersist, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_slowtimo", (char*)tcp_slowtimo, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcpstat", (char*)&tcpstat, 0, (N_BSS | N_EXT)},
    {NULL, "_tcp_template", (char*)tcp_template, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_timers", (char*)tcp_timers, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_totbackoff", (char*)&tcp_totbackoff, 0, (N_DATA | N_EXT)},
    {NULL, "_tcp_usrclosed", (char*)tcp_usrclosed, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_usrreq", (char*)tcp_usrreq, 0, (N_TEXT | N_EXT)},
    {NULL, "_tcp_xmit_timer", (char*)tcp_xmit_timer, 0, (N_TEXT | N_EXT)},
    {NULL, "_td", (char*)td, 0, (N_TEXT | N_EXT)},
    {NULL, "_test_blkdev", (char*)test_blkdev, 0, (N_TEXT | N_EXT)},
    {NULL, "_test_filename", (char*)test_filename, 0, (N_TEXT | N_EXT)},
    {NULL, "_testGfx", (char*)testGfx, 0, (N_TEXT | N_EXT)},
    {NULL, "_testGfxLo", (char*)testGfxLo, 0, (N_TEXT | N_EXT)},
    {NULL, "_testGfxLoInt", (char*)testGfxLoInt, 0, (N_TEXT | N_EXT)},
    {NULL, "_testGfxLoLin", (char*)testGfxLoLin, 0, (N_TEXT | N_EXT)},
    {NULL, "_test_inode", (char*)test_inode, 0, (N_TEXT | N_EXT)},
    {NULL, "_testIo", (char*)testIo, 0, (N_TEXT | N_EXT)},
    {NULL, "_testJmp", (char*)testJmp, 0, (N_TEXT | N_EXT)},
    {NULL, "_testPrintf", (char*)testPrintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_testStdio", (char*)testStdio, 0, (N_TEXT | N_EXT)},
    {NULL, "_testSymbols", (char*)testSymbols, 0, (N_TEXT | N_EXT)},
    {NULL, "_ti", (char*)ti, 0, (N_TEXT | N_EXT)},
    {NULL, "_tick64Get", (char*)tick64Get, 0, (N_TEXT | N_EXT)},
    {NULL, "_tick64Set", (char*)tick64Set, 0, (N_TEXT | N_EXT)},
    {NULL, "_tickGet", (char*)tickGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_tickSet", (char*)tickSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_time", (char*)time, 0, (N_TEXT | N_EXT)},
    {NULL, "_timeLibInit", (char*)timeLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "___tmNormalize", (char*)__tmNormalize, 0, (N_TEXT | N_EXT)},
    {NULL, "___tmValidate", (char*)__tmValidate, 0, (N_TEXT | N_EXT)},
    {NULL, "_toggleDoubleBuffer", (char*)toggleDoubleBuffer, 0, (N_TEXT | N_EXT)},
    {NULL, "_tolower", (char*)tolower, 0, (N_TEXT | N_EXT)},
    {NULL, "_toupper", (char*)toupper, 0, (N_TEXT | N_EXT)},
    {NULL, "_tr", (char*)tr, 0, (N_TEXT | N_EXT)},
    {NULL, "_ts", (char*)ts, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyAbortFuncSet", (char*)ttyAbortFuncSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyAbortSet", (char*)ttyAbortSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyBackSpaceSet", (char*)ttyBackSpaceSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyDeleteLineSet", (char*)ttyDeleteLineSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyDevInit", (char*)ttyDevInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyDevRemove", (char*)ttyDevRemove, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyEOFSet", (char*)ttyEOFSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyIntRd", (char*)ttyIntRd, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyIntTx", (char*)ttyIntTx, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyIoctl", (char*)ttyIoctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyMonitorTrapSet", (char*)ttyMonitorTrapSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyRead", (char*)ttyRead, 0, (N_TEXT | N_EXT)},
    {NULL, "_ttyWrite", (char*)ttyWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_tyler", (char*)tyler, 0, (N_TEXT | N_EXT)},
    {NULL, "_TylerGajewski", (char*)&TylerGajewski, 0, (N_DATA | N_EXT)},
    {NULL, "_udbinfo", (char*)&udbinfo, 0, (N_BSS | N_EXT)},
    {NULL, "___udivdi3", (char*)__udivdi3, 0, (N_TEXT | N_EXT)},
    {NULL, "_udpCfgParams", (char*)&udpCfgParams, 0, (N_DATA | N_EXT)},
    {NULL, "_udpCheck", (char*)udpCheck, 0, (N_TEXT | N_EXT)},
    {NULL, "_udpClient", (char*)udpClient, 0, (N_TEXT | N_EXT)},
    {NULL, "_udp_ctlinput", (char*)udp_ctlinput, 0, (N_TEXT | N_EXT)},
    {NULL, "_udpDoCkSumRcv", (char*)&udpDoCkSumRcv, 0, (N_BSS | N_EXT)},
    {NULL, "_udpDoCkSumSnd", (char*)&udpDoCkSumSnd, 0, (N_BSS | N_EXT)},
    {NULL, "_udp_in", (char*)&udp_in, 0, (N_BSS | N_EXT)},
    {NULL, "_udp_input", (char*)udp_input, 0, (N_TEXT | N_EXT)},
    {NULL, "_udp_last_inpcb", (char*)&udp_last_inpcb, 0, (N_BSS | N_EXT)},
    {NULL, "_udpLibInit", (char*)udpLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_udp_output", (char*)udp_output, 0, (N_TEXT | N_EXT)},
    {NULL, "_udp_pcbhashsize", (char*)&udp_pcbhashsize, 0, (N_DATA | N_EXT)},
    {NULL, "_udpServer", (char*)udpServer, 0, (N_TEXT | N_EXT)},
    {NULL, "_udpstat", (char*)&udpstat, 0, (N_BSS | N_EXT)},
    {NULL, "_udp_usrreq", (char*)udp_usrreq, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglBitmapCreate", (char*)uglBitmapCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglBitmapDestroy", (char*)uglBitmapDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglClutGet", (char*)uglClutGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglClutSet", (char*)uglClutSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglColorAlloc", (char*)uglColorAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglColorAllocExt", (char*)uglColorAllocExt, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglColorFree", (char*)uglColorFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutAlloc", (char*)uglCommonClutAlloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutCreate", (char*)uglCommonClutCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutDestroy", (char*)uglCommonClutDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutFree", (char*)uglCommonClutFree, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutGet", (char*)uglCommonClutGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutMapNearest", (char*)uglCommonClutMapNearest, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglCommonClutSet", (char*)uglCommonClutSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGcCopy", (char*)uglGcCopy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGcCreate", (char*)uglGcCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGcDestroy", (char*)uglGcDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGcSet", (char*)uglGcSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGeneric8BitBitmapBlt", (char*)uglGeneric8BitBitmapBlt, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGeneric8BitBitmapCreate", (char*)uglGeneric8BitBitmapCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGeneric8BitBitmapDestroy", (char*)uglGeneric8BitBitmapDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGeneric8BitBitmapWrite", (char*)uglGeneric8BitBitmapWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGeneric8BitPixelSet", (char*)uglGeneric8BitPixelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClipDdb", (char*)uglGenericClipDdb, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClipDibToDdb", (char*)uglGenericClipDibToDdb, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClutCreate", (char*)uglGenericClutCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClutDestroy", (char*)uglGenericClutDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClutGet", (char*)uglGenericClutGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClutMapNearest", (char*)uglGenericClutMapNearest, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericClutSet", (char*)uglGenericClutSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericColorAllocIndexed", (char*)uglGenericColorAllocIndexed, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericColorFreeIndexed", (char*)uglGenericColorFreeIndexed, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericGcCopy", (char*)uglGenericGcCopy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericGcCreate", (char*)uglGenericGcCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericGcDestroy", (char*)uglGenericGcDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericGcSet", (char*)uglGenericGcSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGenericModeFind", (char*)uglGenericModeFind, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglGraphicsDevDestroy", (char*)uglGraphicsDevDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglModeAvailGet", (char*)uglModeAvailGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglModeSet", (char*)uglModeSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglOsLock", (char*)uglOsLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglOsLockCreate", (char*)uglOsLockCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglOsLockDestroy", (char*)uglOsLockDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglOsUnLock", (char*)uglOsUnLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglPixelSet", (char*)uglPixelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglUgiDevDeinit", (char*)uglUgiDevDeinit, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglUgiDevInit", (char*)uglUgiDevInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVga4BitColorConvert", (char*)uglVga4BitColorConvert, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaBitmapBlt", (char*)uglVgaBitmapBlt, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaBitmapCreate", (char*)uglVgaBitmapCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaBitmapDestroy", (char*)uglVgaBitmapDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaBitmapWrite", (char*)uglVgaBitmapWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaBltPlane", (char*)uglVgaBltPlane, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaClutGet", (char*)uglVgaClutGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaClutSet", (char*)uglVgaClutSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaDevCreate", (char*)uglVgaDevCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaDevDestroy", (char*)uglVgaDevDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaGcSet", (char*)uglVgaGcSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaInfo", (char*)uglVgaInfo, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaPixelSet", (char*)uglVgaPixelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaXBitmapBlt", (char*)uglVgaXBitmapBlt, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaXBitmapCreate", (char*)uglVgaXBitmapCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaXBitmapDestroy", (char*)uglVgaXBitmapDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaXBitmapWrite", (char*)uglVgaXBitmapWrite, 0, (N_TEXT | N_EXT)},
    {NULL, "_uglVgaXPixelSet", (char*)uglVgaXPixelSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_uiomove", (char*)uiomove, 0, (N_TEXT | N_EXT)},
    {NULL, "___ultostr", (char*)__ultostr, 0, (N_TEXT | N_EXT)},
    {NULL, "___umoddi3", (char*)__umoddi3, 0, (N_TEXT | N_EXT)},
    {NULL, "_ungetc", (char*)ungetc, 0, (N_TEXT | N_EXT)},
    {NULL, "_unixDomain", (char*)&unixDomain, 0, (N_BSS | N_EXT)},
    {NULL, "_unixDomainLibInit", (char*)unixDomainLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_unixDomainName", (char*)&unixDomainName, 0, (N_DATA | N_EXT)},
    {NULL, "_unixDomainProto", (char*)&unixDomainProto, 0, (N_BSS | N_EXT)},
    {NULL, "_unlinki", (char*)unlinki, 0, (N_TEXT | N_EXT)},
    {NULL, "_usedblocks", (char*)&usedblocks, 0, (N_BSS | N_EXT)},
    {NULL, "_usrClock", (char*)usrClock, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrFsLibInit", (char*)usrFsLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrInit", (char*)usrInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrKernelInit", (char*)usrKernelInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrLibInit", (char*)usrLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrMmuInit", (char*)usrMmuInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrNetInit", (char*)usrNetInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrPageCheck", (char*)usrPageCheck, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrPageInit", (char*)usrPageInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrPageNumLinesSet", (char*)usrPageNumLinesSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrPageReset", (char*)usrPageReset, 0, (N_TEXT | N_EXT)},
    {NULL, "_usrRoot", (char*)usrRoot, 0, (N_TEXT | N_EXT)},
    {NULL, "_valloc", (char*)valloc, 0, (N_TEXT | N_EXT)},
    {NULL, "_var", (char*)&var, 0, (N_BSS | N_EXT)},
    {NULL, "_vdprintf", (char*)vdprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_version", (char*)version, 0, (N_TEXT | N_EXT)},
    {NULL, "_vfnprintf", (char*)vfnprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vfprintf", (char*)vfprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vfscanf", (char*)vfscanf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vfsops", (char*)&vfsops, 0, (N_DATA | N_EXT)},
    {NULL, "_vgaANSIPalette", (char*)&vgaANSIPalette, 0, (N_DATA | N_EXT)},
    {NULL, "_vgaBlankScreen", (char*)vgaBlankScreen, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaDACLoad", (char*)vgaDACLoad, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaHrdInit", (char*)vgaHrdInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaInitMode", (char*)vgaInitMode, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaLoadFont", (char*)vgaLoadFont, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaLoadPalette", (char*)vgaLoadPalette, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaPutPixel", (char*)vgaPutPixel, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaRefreshArea", (char*)vgaRefreshArea, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaRestore", (char*)vgaRestore, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaSave", (char*)vgaSave, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaSaveScreen", (char*)vgaSaveScreen, 0, (N_TEXT | N_EXT)},
    {NULL, "_vgaWriteString", (char*)vgaWriteString, 0, (N_TEXT | N_EXT)},
    {NULL, "_visStart", (char*)&visStart, 0, (N_BSS | N_EXT)},
    {NULL, "_vmContextClassId", (char*)&vmContextClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_vmContextCreate", (char*)vmContextCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmContextDestroy", (char*)vmContextDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmContextInit", (char*)vmContextInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmCurrentSet", (char*)vmCurrentSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmEnable", (char*)vmEnable, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmGlobalMap", (char*)vmGlobalMap, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmGlobalMapInit", (char*)vmGlobalMapInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmLibInfo", (char*)&vmLibInfo, 0, (N_BSS | N_EXT)},
    {NULL, "_vmLibInit", (char*)vmLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmMap", (char*)vmMap, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmPageSizeGet", (char*)vmPageSizeGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmStateGet", (char*)vmStateGet, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmStateSet", (char*)vmStateSet, 0, (N_TEXT | N_EXT)},
    {NULL, "_vmTranslate", (char*)vmTranslate, 0, (N_TEXT | N_EXT)},
    {NULL, "_vnodeLock", (char*)vnodeLock, 0, (N_TEXT | N_EXT)},
    {NULL, "_vnodesCreate", (char*)vnodesCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_vnodesDelete", (char*)vnodesDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_vnodeUnlock", (char*)vnodeUnlock, 0, (N_TEXT | N_EXT)},
    {NULL, "_vprintf", (char*)vprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vscanf", (char*)vscanf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vsnprintf", (char*)vsnprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_vsprintf", (char*)vsprintf, 0, (N_TEXT | N_EXT)},
    {NULL, "_waitSignal", (char*)waitSignal, 0, (N_TEXT | N_EXT)},
    {NULL, "_wakeup", (char*)wakeup, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdCancel", (char*)wdCancel, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdClassId", (char*)&wdClassId, 0, (N_DATA | N_EXT)},
    {NULL, "_wdCreate", (char*)wdCreate, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdDelete", (char*)wdDelete, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdDestroy", (char*)wdDestroy, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdFire", (char*)wdFire, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdId", (char*)&wdId, 0, (N_BSS | N_EXT)},
    {NULL, "_wdInit", (char*)wdInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdir", (char*)wdir, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdLibInit", (char*)wdLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdShow", (char*)wdShow, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdShowInit", (char*)wdShowInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdStart", (char*)wdStart, 0, (N_TEXT | N_EXT)},
    {NULL, "_wdTerminate", (char*)wdTerminate, 0, (N_TEXT | N_EXT)},
    {NULL, "___weekOfYear", (char*)__weekOfYear, 0, (N_TEXT | N_EXT)},
    {NULL, "_winode", (char*)winode, 0, (N_TEXT | N_EXT)},
    {NULL, "_write", (char*)write, 0, (N_TEXT | N_EXT)},
    {NULL, "_writei", (char*)writei, 0, (N_TEXT | N_EXT)},
    {NULL, "_wsect", (char*)wsect, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdAttach", (char*)xbdAttach, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdBlockSize", (char*)xbdBlockSize, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdDevName", (char*)xbdDevName, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdDump", (char*)xbdDump, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdEventCategory", (char*)&xbdEventCategory, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdEventMediaChanged", (char*)&xbdEventMediaChanged, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdEventPrimaryInsert", (char*)&xbdEventPrimaryInsert, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdEventRemove", (char*)&xbdEventRemove, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdEventSecondaryInsert", (char*)&xbdEventSecondaryInsert, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdEventSoftInsert", (char*)&xbdEventSoftInsert, 0, (N_DATA | N_EXT)},
    {NULL, "_xbdIoctl", (char*)xbdIoctl, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdLibInit", (char*)xbdLibInit, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdNBlocks", (char*)xbdNBlocks, 0, (N_TEXT | N_EXT)},
    {NULL, "_xbdStrategy", (char*)xbdStrategy, 0, (N_TEXT | N_EXT)},
    {NULL, "_xint", (char*)xint, 0, (N_TEXT | N_EXT)},
    {NULL, "_xshort", (char*)xshort, 0, (N_TEXT | N_EXT)},
    {NULL, "_yychar", (char*)&yychar, 0, (N_BSS | N_EXT)},
    {NULL, "_yylex", (char*)yylex, 0, (N_TEXT | N_EXT)},
    {NULL, "_yylval", (char*)&yylval, 0, (N_BSS | N_EXT)},
    {NULL, "_yynerrs", (char*)&yynerrs, 0, (N_BSS | N_EXT)},
    {NULL, "_yyparse", (char*)yyparse, 0, (N_TEXT | N_EXT)},
    {NULL, "_yystart", (char*)yystart, 0, (N_TEXT | N_EXT)},
    {NULL, "_zeroes", (char*)&zeroes, 0, (N_BSS | N_EXT)},
    {NULL, "_zeroin_addr", (char*)&zeroin_addr, 0, (N_BSS | N_EXT)},
};
int symTblAllSize = 1770;
