/*
 * Copyright (c) 2010 Alexandre Devert <marmakoide@yahoo.fr>
 *
 * ESKit is free software; you can redistribute it and/or modify it under the 
 * terms of the MIT license. See LICENSE for details.
 */

#ifndef ESKIT_MACROS_H
#define ESKIT_MACROS_H



#define swap(a, b, tmp) (tmp) = (a); (a) = (b); (b) = (tmp)

#define new(type) (type*)malloc(sizeof(type))

#define newArray(type, n) (type*)malloc((n) * sizeof(type))



/* 
   Declare unused function parameters 
 */

#ifdef ESKIT_UNUSED
#elif defined(__GNUC__)
  #define ESKIT_UNUSED(x) ESKIT_UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
  #define ESKIT_UNUSED(x) /*@unused@*/ x
#else
  #define ESKIT_UNUSED(x) x
#endif



#endif /* ESKIT_MACROS_H */
