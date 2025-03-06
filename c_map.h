#pragma once

/*=======================================================================*/

#include <stdlib.h>

#include "./include/c_string.h"

/*=======================================================================*/

typedef char               t_cmap_char;

typedef char               t_cmap_ret_code;
typedef unsigned char      t_cmap_boolean;

typedef unsigned char      t_cmap_uint8;
typedef signed char        t_cmap_sint8;
typedef char               t_cmap_int8;

typedef unsigned short     t_cmap_uint16;
typedef signed short       t_cmap_sint16;
typedef short              t_cmap_int16;

typedef unsigned int       t_cmap_uint32;
typedef signed int         t_cmap_sint32;
typedef int                t_cmap_int32;

typedef unsigned long long t_cmap_uint64;
typedef signed long long   t_cmap_sint64;
typedef long long          t_cmap_int64;

typedef float              t_cmap_float32;

typedef double             t_cmap_float64;
#if defined(__x86_64__) || defined(__WIN64) 
	typedef long double      t_cmap_float128;
#endif

typedef void*              t_cmap_any_ptr;
typedef void               t_cmap_none;

/*=======================================================================*/
// Restriction constants.
#define CMAP_MAX_SIZE                       (t_cmap_uint64)4000000000
#define CMAP_MAX_KEY_LENGTH                 (t_cmap_uint8)64
// Error/success constants.
#define CMAP_ERROR_MEMALLOC                 (t_cmap_int8)-1
#define CMAP_ERROR_OVERFLOW                 (t_cmap_int8)-2
#define CMAP_ERROR_ITEM_NOT_FOUND           (t_cmap_int8)-3
#define CMAP_ERROR_INVALID_RESIZE_DIRECTION (t_cmap_int8)-4
#define CMAP_ERROR_INVALID_KEY              (t_cmap_int8)-5
#define CMAP_ERROR_INVALID_MAP_SIZE         (t_cmap_int8)-6
#define CMAP_ERROR_INVALID_KEY_LENGTH       (t_cmap_int8)-7
#define CMAP_ERROR_INVALID_PTR              (t_cmap_int8)-8
#define CMAP_ITEM_FOUND                     (t_cmap_int8)-9
// Success constants.
#define CMAP_SUCCESS_EXECUTED               (t_cstr_int8)1
// Map resize constants.
#define CMAP_KEY_GROWTH_SIZE                (t_cmap_uint8)1
#define CMAP_KEY_SHRINK_SIZE                (t_cmap_uint8)2
// Map factors and size constants.
#define CMAP_SMALL_SIZE                     (t_cmap_uint16)2000
#define CMAP_SMALL_GROWTH_AT                (t_cmap_float32)0.7f
#define CMAP_SMALL_SHRINK_AT                (t_cmap_float32)0.4f
#define CMAP_MEDIUM_SIZE                    (t_cmap_uint16)20000
#define CMAP_MEDIUM_GROWTH_AT               (t_cmap_float32)0.8f
#define CMAP_MEDIUM_SHRINK_AT               (t_cmap_float32)0.5f
#define CMAP_BIG_SIZE                       (t_cmap_uint16)200000
#define CMAP_BIG_GROWTH_AT                  (t_cmap_float32)0.9f
#define CMAP_BIG_SHRINK_AT                  (t_cmap_float32)0.5f
// Type constants.
#define CMAP_TRUE                           (t_cmap_uint8)1
#define CMAP_FALSE                          (t_cmap_uint8)0
// Map characters.
#define CMAP_NULL_TERMINATOR                '\0'
// Determinate size of pointer.
#define CMAP_SIZE_OF_PTR (__UINTPTR_MAX__ / 255 % 255)

// Set size of bytes in the memory block to zero.
#define CMAP_MEMSET(ptr, size)                          do { \
	while(size--) *(ptr + size) = NULL;                        \
} while(0);
// Return a specified error code when condition is true.
#define CMAP_FAIL_IF(cond, err_code)                    do { \
	if(cond) return err_code;                                  \
} while(0);
// Because iteration can start from any location, 
// set hash to 0 to iterate from the start again.
#define CMAP_RESET_HASH_IF_BIGGER_THAN_SIZE(hash, size) do { \
	if((hash + 1) >= size) hash = 0;                           \
} while(0);
// Depending on the map size, either Quadratic or Linear probe algorithm is used.
#define CMAP_USE_HASH_ALGORITHM(hash, index, size) do { \
	if(size <= CMAP_SMALL_SIZE) {                         \
		hash += 1;                                          \
	}	else {                                              \
		hash = (hash + (index ^ 2)) % size;                 \
		index += 1;                                         \
	}	                                                    \
} while(0);
// Returns exec_code when result of x execution is not equal to success code.
#define CMAP_SAFE_CALL(x)                          do {    \
	t_cmap_ret_code exec_code = (x);                         \
	if(exec_code != CMAP_SUCCESS_EXECUTED) return exec_code; \
} while(0);
// Get map shrink facotr.
#define CMAP_GET_SHRINK_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_SHRINK_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_SHRINK_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_SHRINK_AT  : CMAP_SMALL_SHRINK_AT
// Get map growth factor.
#define CMAP_GET_GROWTH_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_GROWTH_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_GROWTH_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_GROWTH_AT  : CMAP_SMALL_GROWTH_AT
// When occupied used for x map will be become smaller, when size than bigger.
#define CMAP_GET_PRIME_FROM(x)               (x * 2) >= CMAP_MAX_SIZE ? CMAP_MAX_SIZE : x * 2
// Check is map to small and need growth.
#define CMAP_IS_MAP_TO_SMALL(occupied, size) (((t_cmap_float32)occupied) / size) >= (CMAP_GET_GROWTH_FACTOR(size))
// Check is map to big and need shrink.
#define CMAP_IS_MAP_TO_BIG(occupied, size)   (((t_cmap_float32)occupied) / size) <= (CMAP_GET_SHRINK_FACTOR(size))
// Optimization macros.
#define CMAP_IS_EQUAL(x, y)                  (((x) ^ (y)) == 0)
#define CMAP_IS_NEQUAL(x, y)                 (((x) ^ (y)) != 0)
#define CMAP_UNLIKELY(x)                      __builtin_expect(x, CMAP_FALSE)
#define CMAP_LIKELY(x)                       __builtin_expect(x, CMAP_TRUE)

/*=======================================================================*/

typedef struct {
	t_cmap_char*   m_key;
	t_cmap_any_ptr m_value;
} s_cmap_item;

typedef struct {
	t_cmap_uint64 m_len;
	t_cmap_uint64 m_occupied;
	s_cmap_item** m_items;
} s_cmap_map;

/*=======================================================================*/

/* Hashing and Collision handle functions */
static inline __attribute__((always_inline))
t_cmap_ret_code cmap_gen_hash(t_cmap_uint64* const hash, const t_cmap_char* key, t_cmap_uint64 size);
static inline __attribute__((always_inline))
t_cmap_ret_code cmap_find_hash(t_cmap_uint64* const hash, const s_cmap_item** items, t_cmap_uint64 occupied, t_cmap_uint64 size);
static inline __attribute__((always_inline))
t_cmap_ret_code cmap_find_hash_key(t_cmap_uint64* const hash, const t_cmap_char* key, const s_cmap_item** items, t_cmap_uint64 occupied, t_cmap_uint64 size);
/* Helper functions */
static inline __attribute__((always_inline))
t_cmap_ret_code cmap_resize(s_cmap_map* this, t_cmap_uint8 direction);
/* API functions */
t_cmap_ret_code cmap_init(s_cmap_map* this, t_cmap_uint64 size);
t_cmap_ret_code cmap_set(s_cmap_map* this, const t_cmap_char* key, const t_cmap_any_ptr value, t_cmap_uint16 value_size);
t_cmap_ret_code cmap_delete(s_cmap_map* this);
t_cmap_ret_code cmap_get(const s_cmap_item** item, const t_cmap_char* key, s_cmap_map map);
t_cmap_ret_code cmap_delete_item(const t_cmap_uint64* hash, const t_cmap_char* key, s_cmap_map* const this);

/*=======================================================================*/