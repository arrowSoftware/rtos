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

/* qClass.h - Queue class */

#ifndef _qClass_h
#define _qClass_h

#include <rtos.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef struct qClass {
  FUNCPTR createMethod;
  FUNCPTR initMethod;
  FUNCPTR destroyMethod;
  FUNCPTR terminateMethod;
  FUNCPTR putMethod;
  FUNCPTR getMethod;
  FUNCPTR removeMethod;
  FUNCPTR moveMethod;
  FUNCPTR advanceMethod;
  FUNCPTR expiredMethod;
  FUNCPTR keyMethod;
  FUNCPTR offsetMethod;
  FUNCPTR infoMethod;
  FUNCPTR eachMethod;
  struct qClass *obj;
} Q_CLASS;

typedef Q_CLASS *Q_CLASS_ID;

/******************************************************************************
* Q_CLASS_VERIFY - Validate class
*
* RETURNS: OK or ERROR
******************************************************************************/

#define Q_CLASS_VERIFY(pQClass)						\
  (									\
    ( ((Q_CLASS *)(pQClass))->obj == (Q_CLASS *) pQClass) ?		\
      OK								\
    :									\
      ERROR								\
  )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _qClass_h */

