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

/* signal.h - Signal header */

#ifndef _signal_h
#define _signal_h

#include <sigevent.h>

#define SIGHUP		1		/* Hangup */
#define SIGINT		2		/* Interrupt */
#define SIGQUIT		3		/* Quit */
#define SIGILL		4		/* Illegal instruction, not reset */
#define SIGTRAP		5		/* Trace trap, no reset */
#define SIGABRT		6		/* Abort, SIGIOT */
#define SIGEMT		7		/* EMT instruction */
#define SIGFPE		8		/* Floating point exception */
#define SIGKILL		9		/* Kill */
#define SIGBUS		10		/* Buss error */
#define SIGSEGV		11		/* Segment violation */
#define SIGFMT		12		/* Stack format error */
#define SIGPIPE		13		/* Write on pipe with no listener */
#define SIGALRM		14		/* Alarm */
#define SIGTERM		15		/* Termination signal sent from kill */
#define SIGCNCL		16		/* Thread cancel */
#define SIGSTOP		17		/* Sendabe stop from tty */
#define SIGSTP		18		/* Stop from tty */
#define SIGCONT		19		/* Continue */
#define SIGCHLD		20		/* Child exit */
#define SIGTTIN		21		/* Background tty read */
#define SIGTTOU		22		/* Background tty output */

#define SIGRTMIN	23		/* Realtime signals start */
#define SIGRTMAX	29		/* Realtime signals end */

#define SIGUSR1		30		/* User defined */
#define SIGUSR2		31		/* User defined */

#define _NSIGS		SIGUSR2		/* Max number of signal types */

/* Flags set in sigaction */
#define SA_NOCLDSTOP	0x0001		/* Do no genereate SIGCHLD */
#define SA_SIGINFO	0x0002		/* Pass extra siginfo structure */
#define SA_ONSTACK	0x0004		/* Run on sigstack */
#define SA_INTERRUPT	0x0008		/* Don't restart function */
#define SA_RESETHAND	0x0010		/* Reset the handler */

/* Used in sigprocmask */
#define SIG_BLOCK	1		/* Block signal */
#define SIG_UNBLOCK	2		/* Unblock signal */
#define SIG_SETMASK	3		/* Set signal mask */

/* Used in siginfo */
#define SI_SYNC		0		/* Hadrware generated */
#define SI_KILL		1		/* From kill() */
#define SI_QUEUE	2		/* From sigqueue() */
#define SI_TIMER	3		/* Timer expiration */
#define SI_ASYNCIO	4		/* Completion of asunc I/O */
#define SI_MESGQ	5		/* Message arrival */

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define SIG_ERR		(void(*) (int)) -1
#define SIG_DFL		(void(*) (int)) 0
#define SIG_IGN		(void(*) (int)) 1

/* Typedefs */
typedef unsigned long sigset_t;
typedef unsigned char sig_atomic_t;

typedef struct siginfo {
  int			si_signo;
  int			si_code;
  union sigval		si_value;
} siginfo_t;

/* Structs */
struct sigcontext;

struct sigaction {
  union {
    void		(*__sa_handler)(int);
    void		(*__sa_sigaction)(int, siginfo_t *, void *);
  } sa_u;
  sigset_t		sa_mask;
  int			sa_flags;
};

/* Macros */
#define sa_handler	sa_u.__sa_handler
#define sa_sigaction	sa_u.__sa_sigaction
#define sigmask(m)	( 1 << ((m) - 1) )

/* Functions */
IMPORT void ( *signal(int signo, void (*handler)()) )();
IMPORT int sigprocmask(int what, const sigset_t *set, sigset_t *oldset);
IMPORT int sigaction(int signo, struct sigaction *act,
		        struct sigaction *oact);
IMPORT int sigemptyset(sigset_t *set);
IMPORT int sigfillset(sigset_t *set);
IMPORT int sigaddset(sigset_t *set, int signo);
IMPORT int sigdelset(sigset_t *set, int signo);
IMPORT int sigismember(sigset_t *set, int signo);
IMPORT int sigpending(sigset_t *set);
IMPORT int sigsuspend(sigset_t *set);
IMPORT int pause(void);
IMPORT int kill(int taskId, int signo);
IMPORT int raise(int signo);
IMPORT void sigreturn(struct sigcontext *scp);
IMPORT int sigqueue(int taskId, int signo, const union sigval value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _signal_h */

