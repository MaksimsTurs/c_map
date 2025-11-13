#include "c_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	t_char*   m_key;
	t_any     m_value;
	t_int64   _m_hash;
} cmap_item;

typedef struct {
	t_int64    m_size;
	t_int64    _m_occupied;
	t_int8     _m_is_resizable;
	t_int8     _m_mem_loc;
	cmap_item* _m_items;
} cmap;

#define CMAP_MAX_MAP_SIZE CTYPE_INT64_MAX - 10

#define CMAP_MEM_LOC_STACK 1
#define CMAP_MEM_LOC_HEAP  2

#define CMAP_GROWTH 1
#define CMAP_SHRINK 0

#define CMAP_ERR_INVALID_PTR                   -1
#define CMAP_ERR_INVALID_CMAP_SIZE             -2
#define CMAP_ERR_INVALID_KEY                   -3
#define CMAP_ERR_MEM_ALLOCATION                -4
#define CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE -5
#define CMAP_ERR_CAN_NOT_FIND_FREE_INDEX       -6
#define CMAP_ERR_CAN_NOT_FIND_ITEM             -7

#define CMAP_SUCCESS          0

#define CMAP_FAIL_IF(cond, code) do { \
	if(cond) { \
		return code; \
	} \
} while(0); 
#define CMAP_MOVE_TO_NEW_POSITION(new_item, old_item, new_hash) do { \
	new_item.m_key   = old_item.m_key; \
	new_item.m_value = old_item.m_value; \
	new_item._m_hash = new_hash; \
} while(0);
#define CMAP_SAFE_CALL(expr_res) do { \
	t_int64 res = expr_res; \
	if(res < 0) { return res; } \
} while(0);

#define SHOULD_MAP_GROWTH(map) map->_m_is_resizable && (((t_float32)(map)->_m_occupied)) / (map)->m_size >= 0.75f
#define SHOULD_MAP_SHRINK(map) map->_m_is_resizable && (((t_float32)(map)->_m_occupied)) / (map)->m_size <= 0.50f
#define CMAP_CONCAT_STRING(...) __VA_ARGS__

void cmap_print(cmap* self);

t_int64 cmap_sinit(cmap* self, cmap_item* buff, t_int64 size, t_int8 should_expand);
t_int64 cmap_dinit(cmap* self, t_int64 size, t_int8 should_expand);
t_int64 cmap_set(cmap* self, t_char* key, t_any value);
t_int64 cmap_get(cmap* self, cmap_item* item, t_char* key);
t_int64 cmap_delete(cmap* self, t_char* key);

void    cmap_find_item_index_by_hash(cmap* self, t_int64 hash, t_int64* start);
void    cmap_find_free_index(cmap_item* items, t_int64 size, t_int64* index);
t_int64 cmap_resize(cmap* self, t_uint8 direction);

t_int64 cmap_hash(t_char* key);
