#ifndef STDINT_H
#define STDINT_H

/* Exact-width integer types */
typedef signed char        int8_t;
typedef short int          int16_t;
typedef int                int32_t;
typedef long long int      int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

/* Minimum-width integer types */
typedef signed char        int_least8_t;
typedef short int          int_least16_t;
typedef int                int_least32_t;
typedef long long int      int_least64_t;

typedef unsigned char      uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int       uint_least32_t;
typedef unsigned long long uint_least64_t;

/* Fastest minimum-width integer types */
typedef signed char        int_fast8_t;
typedef short int          int_fast16_t;
typedef int                int_fast32_t;
typedef long long int      int_fast64_t;

typedef unsigned char      uint_fast8_t;
typedef unsigned short int uint_fast16_t;
typedef unsigned int       uint_fast32_t;
typedef unsigned long long uint_fast64_t;

/* Integer types capable of holding object pointers */
typedef int                intptr_t;
typedef unsigned int       uintptr_t;

/* Greatest-width integer types */
typedef long long int      intmax_t;
typedef unsigned long long uintmax_t;

/* Limits of exact-width integer types */
#define INT8_MIN   (-128)
#define INT16_MIN  (-32767-1)
#define INT32_MIN  (-2147483647-1)
#define INT64_MIN  (-9223372036854775807LL-1)

#define INT8_MAX   (127)
#define INT16_MAX  (32767)
#define INT32_MAX  (2147483647)
#define INT64_MAX  (9223372036854775807LL)

#define UINT8_MAX  (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (18446744073709551615ULL)

/* Limits of minimum-width integer types */
#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN

#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX

#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN   INT8_MIN
#define INT_FAST16_MIN  INT16_MIN
#define INT_FAST32_MIN  INT32_MIN
#define INT_FAST64_MIN  INT64_MIN

#define INT_FAST8_MAX   INT8_MAX
#define INT_FAST16_MAX  INT16_MAX
#define INT_FAST32_MAX  INT32_MAX
#define INT_FAST64_MAX  INT64_MAX

#define UINT_FAST8_MAX  UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* Limits of integer types capable of holding object pointers */
#define INTPTR_MIN  INT32_MIN
#define INTPTR_MAX  INT32_MAX
#define UINTPTR_MAX UINT32_MAX

/* Limits of greatest-width integer types */
#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* Limits of other integer types */
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#define SIZE_MAX UINT32_MAX

#define WCHAR_MIN 0
#define WCHAR_MAX UINT16_MAX

#define WINT_MIN 0
#define WINT_MAX UINT32_MAX

/* Macros for integer constants */
#define INT8_C(value)   ((int8_t)(value))
#define INT16_C(value)  ((int16_t)(value))
#define INT32_C(value)  ((int32_t)(value))
#define INT64_C(value)  ((int64_t)(value##LL))

#define UINT8_C(value)  ((uint8_t)(value##U))
#define UINT16_C(value) ((uint16_t)(value##U))
#define UINT32_C(value) ((uint32_t)(value##U))
#define UINT64_C(value) ((uint64_t)(value##ULL))

#define INTMAX_C(value)  INT64_C(value)
#define UINTMAX_C(value) UINT64_C(value)

#endif 
