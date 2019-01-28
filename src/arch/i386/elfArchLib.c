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

/* elfArchLib.c - Elf loadable code support */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/types.h>
#include <rtos.h>
#include <rtos/errnoLib.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <tools/moduleLib.h>
#include <tools/loadLib.h>
#include <tools/private/loadElfLibP.h>
#include <tools/loadElfLib.h>
#include <arch/elfArchLib.h>

/* Defines */

/* Macros */
#define MREAD32(pAddr, off)		off = *((u_int32_t *) pAddr);
#define MWRITE32(pAddr, val)		*((u_int32_t *) pAddr) = val;
#define MWRITE16(pAddr, val)		*((u_int16_t *) pAddr = val;

/* Imports */
IMPORT int loadElfRelocRelEntryRead(int fd, int pos, Elf32_Rel *pRel);

/* Locals */
LOCAL BOOL elfArchVerify(u_int32_t type, BOOL *sdaUsed);
LOCAL STATUS elfSegReloc(int fd,
			 MODULE_ID moduleId,
			 int loadFlag,
			 int posCurrRelCmd,
			 Elf32_Shdr *pSectHeaderTable,
			 Elf32_Shdr *pRelHeader,
			 SECT_ADDR *pSectAddr,
			 SYM_INFO_TABLE symInfoTable,
			 Elf32_Sym *pSymArray,
			 SYMTAB_ID symTable,
			 SEG_INFO *pSegInfo);
LOCAL STATUS elfRelocEntry(void *pRelAddr,
			   Elf32_Rel *pRelCmd,
			   void *pSymAddr);
LOCAL STATUS elfAddr32Reloc(void *pRelAddr, void *pSymAddr);
LOCAL STATUS elfAddrPc32Reloc(void *pRelAddr, void *pSymAddr);

/* Globals */

/* Functions */

/*******************************************************************************
 * elfArchLibInit - Initialize elf arch library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS elfArchLibInit(FUNCPTR *pElfVerifyFunc, FUNCPTR *pElfSegRelFunc)
{
  *pElfVerifyFunc = (FUNCPTR) elfArchVerify;
  *pElfSegRelFunc = (FUNCPTR) elfSegReloc;

  return OK;
}

/*******************************************************************************
 * elfArchVerify - Verify target machine
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL elfArchVerify(u_int32_t type, BOOL *sdaUsed)
{
  BOOL ret;

  *sdaUsed = FALSE;

  if (type != EM_ARCH_MACHINE) {

    ret = FALSE;
    errnoSet(S_loadElfLib_HDR_READ);

  }

  else {

    ret = TRUE;

  }

  return ret;
}

/*******************************************************************************
 * elfSegReloc - Relocate
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS elfSegReloc(int fd,
			 MODULE_ID moduleId,
			 int loadFlag,
			 int posCurrRelCmd,
			 Elf32_Shdr *pSectHeaderTable,
			 Elf32_Shdr *pRelHeader,
			 SECT_ADDR *pSectAddr,
			 SYM_INFO_TABLE symInfoTable,
			 Elf32_Sym *pSymArray,
			 SYMTAB_ID symTable,
			 SEG_INFO *pSegInfo)
{
  Elf32_Rel relCmd;
  u_int32_t i, relNum;
  void *pRelAddr, *pSymAddr;
  STATUS status;

  /* Inititialize locals */
  status = OK;

  if (pRelHeader->sh_type != SHT_REL) {

    fprintf(stderr, "Relocation sections of type %d are not supported.\n");
    errnoSet(S_loadElfLib_RELA_SECTION);
    return ERROR;

  }

  if (pRelHeader->sh_entsize != RELSZ) {

    fprintf(stderr, "Wrong relocation entry size.\n");
    errnoSet(S_loadElfLib_RELOC);
    return ERROR;

  }

  /* Calculate size */
  relNum = pRelHeader->sh_size / pRelHeader->sh_entsize;

  /* For all relocation entries */
  for (i = 0; i < relNum; i++) {

    posCurrRelCmd = loadElfRelocRelEntryRead(fd, posCurrRelCmd, &relCmd);
    if (posCurrRelCmd == ERROR) {

      errnoSet(S_loadElfLib_READ_SECTIONS);
      return ERROR;

    }

    /* Get entry in symbol table */
    pSymAddr = symInfoTable[ELF32_R_SYM(relCmd.r_info)].pAddr;
    if (pSymAddr == NULL)
      continue;

    /* Calculate relocation address */
    pRelAddr = (void *) ((Elf32_Addr) *pSectAddr + relCmd.r_offset);

    /* Relocate */
    if (elfRelocEntry(pRelAddr, &relCmd, pSymAddr) != OK)
      status = ERROR;

  } /* End for all relocation entries */

  return status;
}

/*******************************************************************************
 * elfRelocEntry - Relocate current entry
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS elfRelocEntry(void *pRelAddr,
			   Elf32_Rel *pRelCmd,
			   void *pSymAddr)
{
  /* Select relocation type */
  switch (ELF32_R_TYPE(pRelCmd->r_info)) {

    case R_386_NONE:

#ifdef DEBUG_ELF
     printf("No relocation [%#x, %#x]: ", pRelAddr, pSymAddr);
#endif

    break;

    case R_386_32:

#ifdef DEBUG_ELF
     printf("Direct 32-bit relocation[%#x, %#x]: ", pRelAddr, pSymAddr);
#endif

      if (elfAddr32Reloc(pRelAddr, pSymAddr) != OK)
        return ERROR;

    break;

    case R_386_PC32:

#ifdef DEBUG_ELF
     printf("PC relative 32-bit relocation[%#x, %#x]: ", pRelAddr, pSymAddr);
#endif

      if (elfAddrPc32Reloc(pRelAddr, pSymAddr) != OK)
        return ERROR;

    break;

    default:

      fprintf(stderr, "Unknown relocation type %d\n",
	      ELF32_R_TYPE(pRelCmd->r_info));
      errnoSet(S_loadElfLib_UNRECOGNIZED_RELOCENTRY);
      return ERROR;

    break;

  } /* End select relocation type */

#ifdef DEBUG_ELF
   printf("Done.\n");
#endif

  return OK;
}

/*******************************************************************************
 * elfAddr32Reloc - Perform 32-bit absoulte relocation
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS elfAddr32Reloc(void *pRelAddr, void *pSymAddr)
{
  u_int32_t val, off;

  /* Read offset */
  MREAD32(pRelAddr, off);

#ifdef DEBUG_ELF
  printf("%#x ", off);
#endif

  /* Add offset to symbol address */
  val = (u_int32_t) pSymAddr + off;

#ifdef DEBUG_ELF
  printf("%#x ", val);
#endif

  /* Write address */
  MWRITE32(pRelAddr, val);

  return OK;
}

/*******************************************************************************
 * elfAddrPc32Reloc - Perform 32-bit pc relative relocation
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS elfAddrPc32Reloc(void *pRelAddr, void *pSymAddr)
{
  u_int32_t val, off;

  /* Read offset */
  MREAD32(pRelAddr, off);

#ifdef DEBUG_ELF
  printf("%#x ", off);
#endif

  /* Add offset to symbol address and strubstract relocation address */
  val = (u_int32_t) pSymAddr + off - (u_int32_t) pRelAddr;

#ifdef DEBUG_ELF
  printf("%#x ", val);
#endif

  /* Write address */
  MWRITE32(pRelAddr, val);

  return OK;
}

