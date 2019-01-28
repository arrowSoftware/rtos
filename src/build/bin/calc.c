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

/* calc.c - Simple command line calculator */

/* Includes */
#include <stdio.h>
#include <stdlib.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */
int calcResult = 0;
char calcWelcome[] = "Welcome to calculator 1.0";

/* Functions */

/*******************************************************************************
 * calcAdd - Add two integers
 *
 * RETURNS: Result
 ******************************************************************************/

int calcAdd(int a, int b)
{
  return a + b;
}

/*******************************************************************************
 * calcSub - Subtract two integers
 *
 * RETURNS: Result
 ******************************************************************************/

int calcSub(int a, int b)
{
  return a - b;
}

/*******************************************************************************
 * calcMul - Multiply two integers
 *
 * RETURNS: Result
 ******************************************************************************/

int calcMul(int a, int b)
{
  return a * b;
}

/*******************************************************************************
 * calcDiv - Divide two integers
 *
 * RETURNS: Result
 ******************************************************************************/

int calcDiv(int a, int b)
{
  return a / b;
}

/*******************************************************************************
 *  calcLoop - Add integer in a loop
 *
 * RETURNS: Result
 ******************************************************************************/

int calcLoop(int a, int n)
{
  int sum, i;

  for (i = 0, sum = 0; i < n; i++) {

    printf(".");
    sum += a;

  }

  return sum;
}

/*******************************************************************************
 *  calcFib - Calculate fibbonaci number up to
 *
 * RETURNS: Result
 ******************************************************************************/

int calcFib(int n)
{
  if (n == 1)
    return 0;

  if (n == 2)
    return 1;

  return calcFib(n - 1) + calcFib(n - 2);
}

/*******************************************************************************
 *  calc - Calculator main function
 *
 * RETURNS: Zero or error
 ******************************************************************************/

int calc(int a, char op, int b)
{
  printf("%s\n", calcWelcome);

  switch(op) {

    case '+' : calcResult = calcAdd(a, b);
    break;

    case '-' : calcResult = calcSub(a, b);
    break;

    case '*' : calcResult = calcMul(a, b);
    break;

    case '/' : calcResult = calcDiv(a, b);
    break;

    case 'l' : calcResult = calcLoop(a, b);
    break;

    case 'f' : calcResult = calcFib(a);
    break;

    default:   printf("Unknown operator: %c\n", op);
    return 1;

  }

  printf("%d %c %d = %d\n", a, op, b, calcResult);

  return 0;
}

