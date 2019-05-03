#ifndef _config_h_
#define _config_h_

#define VM_INVALID_ADDR -1
#define VM_PAGE_SIZE    PAGE_SIZE_4KB
#define VM_PAGE_OFFSET  VM_PAGE_SIZE

#define VM_STATE_MASK_FOR_ALL (VM_STATE_MASK_VALID | \
                               VM_STATE_MASK_WRITABLE | \
                               VM_STATE_MASK_CACHEABLE)

#define VM_STATE_FOR_IO (VM_STATE_VALID | \
                         VM_STATE_WRITABLE | \
                         VM_STATE_NOT_CACHEABLE)

#define VM_STATE_FOR_MEM_OS (VM_STATE_VALID | \
                             VM_STATE_WRITABLE | \
                             VM_STATE_CACHEABLE)

#define VM_STATE_FOR_MEM_APPLICATION (VM_STATE_VALID | \
                                      VM_STATE_WRITABLE | \
                                      VM_STATE_CACHEABLE)

#define USER_RESERVED_MEM     0x00100000
#define LOCAL_MEM_LOCAL_ADRS  0x00100000
#define LOCAL_MEM_SIZE_OS     0x00300000
#define SYSTEM_RAM_SIZE       0x00900000
#define FREE_MEM_START_ADRS   0x00400000
#define LOCAL_MEM_SIZE        (SYSTEM_RAM_SIZE - LOCAL_MEM_LOCAL_ADRS)

/* Options */
#define LOCAL_MEM_AUTOSIZE

/* PC interrupt controller related */
#define PIC_REG_ADDR_INTERVAL 1

/* PC timer related */
#define PIT0_INT_LVL          0x00
#define PIT_BASE_ADR          0x40
#define PIT_REG_ADDR_INTERVAL 1
#define PIT_CLOCK             1193180
#define SYS_CLOCK_RATE_MIN    19
#define SYS_CLOCK_RATE_MAX    8192
#define DIAG_CTRL             0x61

/* PC vga related */
#define COLOR         1
#define VGA_MEM_BASE  (u_int8_t *) 0xb8000
#define VGA_SEL_REG   (u_int8_t *) 0x3d4
#define VGA_VAL_REG   (u_int8_t *) 0x3d5
#define DEFAULT_FG    ATRB_FG_BRIGHTWHITE
#define DEFAULT_BG    ATRB_BG_BLACK
#define DEFAULT_ATR   DEFAULT_FG | DEFAULT_BG
#define COLOR_MODE    COLOR
#define CTRL_MEM_BASE VGA_MEM_BASE
#define CTRL_SEL_REG  VGA_SEL_REG
#define CTRL_VAL_REG  VGA_VAL_REG
#define CHR           2

/* PC keyboard related */
#define KBD_INT_LVL  0x01
#define COMMAND_8042 0x64
#define DATA_8042    0x60
#define STATUS_8042  COMMAND_8042
#define	COMMAND_8048 0x61
#define DATA_8048    0x60
#define STATUS_8048  COMMAND_8048

#define NUM_TTY           2
#define CONSOLE_TTY       0
#define CONSOLE_BUAD_RATE 9600

#define PC_CONSOLE         0
#define N_VIRTUAL_CONSOLES 3

#define UNUSED_ISA_IO_ADDRESS 0x84

#define BEEP_PITCH_L 1280
#define BEEP_TIME_L  18
#define BEEP_PITCH_S 1280
#define BEEP_TIME_S  9

/* Pc floppy related */
#define FD_DMA_BUF_ADDR (0x2000 + (VM_PAGE_OFFSET * 2))
#define FD_DMA_BUF_SIZE (0x1000)

/* Kernel configuration */
#define INT_LOCK_LEVEL          0x0 /* Interrupt lockout level */
#define ROOT_STACK_SIZE         10000   /* Root task stack size */
#define SHELL_STACK_SIZE        20000   /* Shell stack size */
#define ISR_STACK_SIZE          2000    /* Interrupt stack size */
#define VEC_BASE_ADRS           ((char *) LOCAL_MEM_LOCAL_ADRS)

#define SYS_CLOCK_RATE          60  /* Ticks per second */
#define NUM_DRIVERS         20  /* Maximum drivers */
#define NUM_FILES           50  /* Maximum open files */
#define MAX_LOG_MSGS            50  /* Maximum of log messages */
#define NUM_SIGNAL_QUEUES       16  /* Numboer of signal queues */
#define SYM_TABLE_HASH_SIZE_LOG2    8   /* Symbol table log 2 size  */

#define USER_I_CACHE_MODE       CACHE_WRITETROUGH
#define USER_D_CACHE_MODE       CACHE_WRITETROUGH
#define USER_I_CACHE_ENABLE
#define USER_D_CACHE_ENABLE

/* Low memory layout */
#define GDT_BASE_OFFSET         0x1000

//#define schedLib_PORTABLE
#define INCLUDE_SHOW_ROUTINES
#define INCLUDE_LOGGING
//#define INCLUDE_LOG_STARTUP

#define INCLUDE_WDOG
#define INCLUDE_TIME
#define INCLUDE_ENV_VARS
#define ENV_VAR_USE_HOOKS       TRUE
#define INCLUDE_ASSERT
#define INCLUDE_LOADER
#define INCLUDE_ELF
#define INCLUDE_FILESYSTEMS
#define INCLUDE_NET

/* usrKernel.c includes */
#define INCLUDE_TASK_HOOKS
#define INCLUDE_SEM_BINARY
#define INCLUDE_SEM_MUTEX
#define INCLUDE_SEM_COUNTING
#define INCLUDE_SEM_RW
#define INCLUDE_rtos_EVENTS
#define INCLUDE_MSG_Q
#define INCLUDE_CONSTANT_RDY_Q

/* usrConfig.c includes */
#define INCLUDE_MEM_MGR_FULL
#define INCLUDE_CACHE_SUPPORT
#define INCLUDE_MMU
#define INCLUDE_TASK_VAR
#define INCLUDE_SELECT
#define INCLUDE_IO_SYSTEM

/*#define INCLUDE_TTY_DRV*/
#define INCLUDE_PC_CONSOLE
#define INCLUDE_SYM_TBL
#define INCLUDE_STDIO
#define INCLUDE_EXC_HANDELING
#define INCLUDE_EXC_TASK
#define INCLUDE_EXC_SHOW
#define INCLUDE_SIGNALS
#define INCLUDE_DEBUG
#define INCLUDE_PIPES
#define INCLUDE_POSIX_SIGNALS
#define INCLUDE_CBIO
#define INCLUDE_STANDALONE_SYM_TBL
#define INCLUDE_SHELL
#define INCLUDE_USR_LIB
#define INCLUDE_DEMO

#endif /* _config_h */

