#ifndef STDDEF_H
#define STDDEF_H

/* NULL pointer constant */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* Size type */
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef unsigned int size_t;
#endif

/* Pointer difference type */
#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
typedef int ptrdiff_t;
#endif

/* Wide character type */
#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#endif

/* Offset of member in structure */
#define offsetof(type, member) ((size_t)&((type*)0)->member)

#endif
