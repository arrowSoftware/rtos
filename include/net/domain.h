/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* domain.h - Domain header file */

#ifndef _domain_h
#define _domain_h

#include <rtos.h>
#include <sys/socket.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <net/protosw.h>

/* Structs */
struct domain {
  int			dom_family;		/* Domain family */
  char			*dom_name;		/* Domain name */

  FUNCPTR		dom_init;		/* Initialize domain */
  FUNCPTR		dom_externalize;	/* Externalize right */
  FUNCPTR		dom_dispose;		/* Dispose external right */

  struct protosw	*dom_protosw;		/* Protocol switch table */
  struct protosw	*dom_nprotosw;
  struct domain		*dom_next;		/* Next domain */

  FUNCPTR		dom_rtattach;		/* Initialize routing table */

  int			dom_rtoffset;		/* Arg to reattach */
  int			dom_maxrtkey;		/* For routing layer */
};

/* Functions */
IMPORT STATUS addDomain(struct domain *domain);
IMPORT void domaininit(void);
IMPORT struct protosw* pffindtype(int family, int type);
IMPORT struct protosw* pffindproto(int family, int protocol, int type);
IMPORT void pfctlinput(int cmd, struct sockaddr *sa);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _domain_h */

