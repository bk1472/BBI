#ifndef __BBI_TYPE_H__
#define __BBI_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


typedef unsigned char		UINT08;
typedef unsigned short		UINT16;
typedef unsigned long		UINT32;
typedef unsigned long long	UINT64;

typedef signed char			SINT08;
typedef signed short		SINT16;
typedef signed long			SINT32;
typedef signed long long	SINT64;

#define	BOOLEAN				int

#ifdef FALSE
#undef FALSE
#endif

#ifdef TRUE
#undef TRUE
#endif

#define FALSE				0
#define TRUE				1

#ifdef __cplusplus
}
#endif

#endif/*__BBI_TYPE_H__*/
