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

/* etherLib.c - Ethernet hook functions */

#include <rtos.h>
#include <rtos/listLib.h>
#include <net/mbuf.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#include <net/netBufLib.h>

/******************************************************************************
* etherOutput - Send out packet on ethernet interface
*
* RETURNS: OK or ERROR
******************************************************************************/

STATUS etherOutput(struct ifnet *pIf, struct ether_header *pEh, char *pData)
{
  /* Not implemented yet */
  return ERROR;
}

