/* This is an example of a header file for platforms/compilers that do
 * not come with stdint.h/stddef.h/stdbool.h/string.h. To use it, define
 * PB_SYSTEM_HEADER as "pb_syshdr.h", including the quotes, and add the
 * extra folder to your include path.
 *
 * It is very likely that you will need to customize this file to suit
 * your platform. For any compiler that supports C99, this file should
 * not be necessary.
 */

#ifndef _PB_SYSHDR_H_
#define _PB_SYSHDR_H_
//https://jpa.kapsi.fi/nanopb/docs/reference.html
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h> // 包含 bool 类型定义
#include <linux/limits.h> // 包含 CHAR_BIT 定义
#include <linux/stddef.h> // 包含 size_t 定义
#include <linux/string.h> // 包含 strlen, memcpy, memset 函数定义
#include <linux/ctype.h> // 包含 isdigit, isspace 等函数定义
/* stdint.h subset */

/* You will need to modify these to match the word size of your platform. */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

/* These are ok for most platforms, unless uint8_t is actually not available,
 * in which case you should give the smallest available type. */
typedef int8_t int_least8_t;
typedef uint8_t uint_least8_t;
typedef uint8_t uint_fast8_t;
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;

#define PB_NO_ENCODE_SIZE_CHECK 1

/* stddef.h subset */


/* stdbool.h subset */


/* stdlib.h subset */


/* string.h subset */



/* limits.h subset */
#define CHAR_BIT 8

#endif
