#pragma once

/*=======================================================================*/

#include <stdlib.h>

/*=======================================================================*/

typedef char               t_cstr_char;

typedef unsigned char      t_cstr_ret_code;
typedef unsigned char      t_cstr_boolean;

typedef unsigned char      t_cstr_uint8;
typedef signed char        t_cstr_sint8;
typedef char               t_cstr_int8;

typedef unsigned short     t_cstr_uint16;
typedef signed short       t_cstr_sint16;
typedef short              t_cstr_int16;

typedef unsigned int       t_cstr_uint32;
typedef signed int         t_cstr_sint32;
typedef int                t_cstr_int32;

typedef unsigned long long t_cstr_uint64;
typedef signed long long   t_cstr_sint64;
typedef long long          t_cstr_int64;

typedef float              t_cstr_float32;

typedef double             t_cstr_float64;
#if defined(__x86_64__) || defined(__WIN64) 
	typedef long double      t_cstr_float128;
#endif

typedef void*              t_cstr_any_ptr;
typedef void               t_cstr_none;

/*=======================================================================*/

// Boolean constants.
#define CSTR_TRUE                          (t_cstr_int8)1
#define CSTR_FALSE                         (t_cstr_int8)0
// Restrictions constants.
#define CSTR_MAX_RESIZE_FACTOR             (t_cstr_float32)1.5f // 150%
// Error constants.
#define CSTR_ERROR_NULL_PTR                (t_cstr_int8)-1
#define CSTR_ERROR_RES_FAC_OVERFLOW        (t_cstr_int8)-2
#define CSTR_ERROR_BUFFER_OVERFLOW         (t_cstr_int8)-3
#define CSTR_ERROR_VALUE_NOT_FREED         (t_cstr_int8)-4
#define CSTR_ERROR_INVALID_MEMORY_LOCATION (t_cstr_int8)-5
#define CSTR_ERROR_INVALID_POSITION        (t_cstr_int8)-6
#define CSTR_ERROR_MEMORY_ALLOCATION       (t_cstr_int8)-7
#define CSTR_ERROR_NOT_A_NUMBER            (t_cstr_int8)-8
#define CSTR_ERROR_NOT_FOUND               (t_cstr_int8)-9
// Success constants.
#define CSTR_SUCCESS_EXECUTED              (t_cstr_int8)1
// Buffer memory locations.
#define CSTR_LOCATION_HEAP                 (t_cstr_int8)0
#define CSTR_LOCATION_STACK                (t_cstr_int8)1
// Magic constants
#define CSTR_LONG_MAGIC                    (t_cstr_uint32)0x01010101L
#define CSTR_HIG_MAGIC                     (t_cstr_uint32)0x80808080L
// Chars constants.
#define CSTR_NULL_TERMINATOR               '\0'
#define CSTR_CARIAGE_RETURN                '\r'
#define CSTR_NEW_LINE                      '\n'
#define CSTR_TAB                           '\t'
// Return a specified error code when condition is true.
#define CSTR_FAIL_IF(cond, code) do { \
	if(cond) return code;               \
} while(0);
// Optimization macros.
#define CSTR_IS_EQUAL(x, y)     (((x) ^ (y)) == 0)
#define CSTR_IS_NEQUAL(x, y)    (((x) ^ (y)) != 0)
#define CSTR_UNLIKELY(x)        __builtin_expect(x, CSTR_FALSE)
#define CSTR_LIKELY(x)          __builtin_expect(x, CSTR_TRUE)

/*=======================================================================*/

typedef struct {
	t_cstr_char*   m_val;
	t_cstr_uint16  m_len;
	// Optimization options.
	// m_buff_size    is stack/heap allocated buffer, used to minimize count of reallocations.
	// m_buff_res_fac resize factor used to extend m_buff_size when needed.
	// m_buff_loc     location of buffer (stack/heap) for some function this information is needed because stack allocated memory can not be reallocated with realloc.
	t_cstr_uint16  m_buff_size;
	t_cstr_float32 m_buff_res_fac;
	t_cstr_int8    m_buff_loc;
} s_cstr_string;

typedef struct {
	t_cstr_char*  m_ptr;
	t_cstr_uint16 m_len;
} s_cstr_slice;

typedef struct {
	s_cstr_slice* m_items;
	t_cstr_uint16 m_len;
} s_cstr_slices;

/*=======================================================================*/

t_cstr_ret_code cstr_build(s_cstr_string* this, const t_cstr_char* val);
t_cstr_ret_code cstr_free(s_cstr_string* this);
t_cstr_ret_code cstr_append(s_cstr_string* this, const t_cstr_char* append_string, t_cstr_uint8 count);
t_cstr_ret_code cstr_slice(s_cstr_string* this, t_cstr_uint16 start, t_cstr_uint16 end);
t_cstr_ret_code cstr_split(s_cstr_slices* this, const t_cstr_char* to_split, t_cstr_char spliter);
t_cstr_ret_code cstr_split_free(s_cstr_slices* this);
t_cstr_ret_code cstr_cpy(t_cstr_char* dest, const t_cstr_char* src, t_cstr_uint16 len);
t_cstr_ret_code cstr_to_llong(t_cstr_int64* num, const t_cstr_char* str_num, t_cstr_uint8 str_len);
t_cstr_ret_code cstr_llong_to_str(t_cstr_char* buff, t_cstr_uint8 buff_size, t_cstr_int64 num);

t_cstr_uint16 cstr_index_of(s_cstr_string* this, t_cstr_char ch, t_cstr_uint16 start_position);
t_cstr_uint16 cstr_len(const t_cstr_char* val);

t_cstr_boolean cstr_comp(const t_cstr_char* str_one, const t_cstr_char* str_two);

/*=======================================================================*/