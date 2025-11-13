#include "./include/c_map.h"

void cmap_print(cmap* self)
{
	printf("Size(%.5lli)\n", self->m_size);
	printf("Occupied(%.5lli)\n", self->_m_occupied);

  for(t_int64 index = 0; index < self->m_size; index++)
  {
		printf("Index(%lli) Hash(%lli)\n", index, self->_m_items[index]._m_hash);
  }

	printf("\n");
}

t_int64 cmap_hash(t_char* key)
{
	t_int64 hash  = 0;
	t_int64 index = 0;

	while(*key) {
		hash += (*key++ + index) << 5;
		index++;
	}

	return hash;
}

void cmap_find_item_index_by_hash(cmap* self, t_int64 hash, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;
	
	while(probe_count < self->m_size) 
	{
		if(index >= self->m_size)
		{
			index = 0;
		}
		
		if(self->_m_items[index]._m_hash == hash) 
		{
			*start = index;
			break;
		}
		
		probe_count++;
		index++;
	}
	
	if(probe_count == self->m_size)
	{
		*start = -1;
	}
}

void cmap_find_free_index(cmap_item* items, t_int64 size, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;

	while(probe_count < size)
	{
		if(index >= size)
		{
			index = 0;
		}
		
		if(!items[index]._m_hash)
		{
			*start = index;
			break;
		}

		probe_count++;
		index++;
	}

	if(probe_count == size)
	{
		*start = -1;
	}
}

t_int64 cmap_dinit(cmap* self, t_int64 size, t_int8 is_resizable)
{
	CMAP_FAIL_IF(self == NULL,             CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(size > CMAP_MAX_MAP_SIZE, CMAP_ERR_INVALID_PTR);

	self->_m_items = (cmap_item*)malloc(size * sizeof(cmap_item));
	CMAP_FAIL_IF(self->_m_items == NULL, CMAP_ERR_MEM_ALLOCATION);

	memset(self->_m_items, 0, size * sizeof(cmap_item));

	self->_m_is_resizable = is_resizable;
	self->_m_mem_loc      = CMAP_MEM_LOC_HEAP;
	self->m_size          = size;
	self->_m_occupied     = 0;
	
	return CMAP_SUCCESS;
}

t_int64 cmap_sinit(cmap* self, cmap_item* buff, t_int64 size, t_int8 is_resizable)
{
	CMAP_FAIL_IF(self == NULL || buff == NULL, CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(size > CMAP_MAX_MAP_SIZE,     CMAP_ERR_INVALID_PTR);

	self->m_size          = size;
	self->_m_items        = buff;
	self->_m_mem_loc      = CMAP_MEM_LOC_STACK;
	self->_m_occupied     = 0;
	self->_m_is_resizable = is_resizable;
		
	return CMAP_SUCCESS;
}

t_int64 cmap_set(cmap* self, t_char* key, t_any value)
{
	CMAP_FAIL_IF(self == NULL,                                                CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(*key == '\0',                                                CMAP_ERR_INVALID_KEY);
	CMAP_FAIL_IF(!self->m_size,                                               CMAP_ERR_INVALID_CMAP_SIZE);
	CMAP_FAIL_IF(self->m_size == self->_m_occupied && !self->_m_is_resizable, CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE);

	t_int64 hash = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_size;

	if(self->_m_items[index]._m_hash)
	{
		cmap_find_item_index_by_hash(self, hash, &index);
		
		if(index == -1)
		{
			index = hash % self->m_size;
			cmap_find_free_index(self->_m_items, self->m_size, &index);
			
			self->_m_items[index]._m_hash = hash;
			self->_m_items[index].m_key = key;
			self->_m_items[index].m_value = value;
			
			self->_m_occupied++;
		}
		else 
		{
			self->_m_items[index].m_value = value;
		}
	} 
	else
	{
		self->_m_items[index]._m_hash = hash;
		self->_m_items[index].m_key   = key;
		self->_m_items[index].m_value = value;
		
		self->_m_occupied++;
	}
	
	if(SHOULD_MAP_GROWTH(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, CMAP_GROWTH));
	}

	return CMAP_SUCCESS;
}

t_int64 cmap_resize(cmap* self, t_uint8 direction)
{	
	CMAP_FAIL_IF(self == NULL, CMAP_ERR_INVALID_PTR);

	t_int64    old_size  = self->m_size;
	t_int64    new_hash  = 0;
	t_int64    new_index = 0;
	cmap_item* new_items = NULL;

	self->m_size = direction == CMAP_GROWTH ? self->m_size * 2 : self->m_size / 2;
	new_items = (cmap_item*)malloc(self->m_size * sizeof(cmap_item));
	memset(new_items, 0, self->m_size * sizeof(cmap_item));

	for(t_int64 index = 0; index < old_size; index++)
	{
		if(self->_m_items[index]._m_hash)
		{
			new_hash = cmap_hash(self->_m_items[index].m_key);
			new_index = new_hash % self->m_size;
			
			if(new_items[new_index]._m_hash)
			{
				cmap_find_free_index(new_items, self->m_size, &new_index);

				if(new_index == -1)
				{
					return CMAP_ERR_CAN_NOT_FIND_FREE_INDEX;
				}
				else
				{
					CMAP_MOVE_TO_NEW_POSITION(new_items[new_index], self->_m_items[index], new_hash);
				}
			}
			else
			{
				CMAP_MOVE_TO_NEW_POSITION(new_items[new_index], self->_m_items[index], new_hash);
			}
		}
	}
	
	if(self->_m_mem_loc == CMAP_MEM_LOC_HEAP)
	{
		free(self->_m_items);
		self->_m_items = NULL;
	}
	
	self->_m_items = new_items;

	return CMAP_SUCCESS;
}

t_int64 cmap_get(cmap* self, cmap_item* item, t_char* key)
{
	CMAP_FAIL_IF(self == NULL, CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(item == NULL, CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(key  == NULL, CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(*key == '\0', CMAP_ERR_INVALID_KEY);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_size;

	if(self->_m_items[index]._m_hash == hash)
	{
		*item = self->_m_items[index];
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_CAN_NOT_FIND_ITEM;
		}
		else 
		{
			*item = self->_m_items[index];
		}
	}

	return CMAP_SUCCESS;
}

t_int64 cmap_delete(cmap* self, t_char* key)
{
	CMAP_FAIL_IF(self == NULL, CMAP_ERR_INVALID_PTR);
	CMAP_FAIL_IF(*key == '\0', CMAP_ERR_INVALID_KEY);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_size;

	if(self->_m_items[index]._m_hash == hash)
	{
		self->_m_items[index]._m_hash = 0;
		self->_m_items[index].m_key   = NULL;
		self->_m_items[index].m_value = NULL;
			
		self->_m_occupied--;
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_CAN_NOT_FIND_ITEM;
		}
		else 
		{
			self->_m_items[index]._m_hash = 0;
			self->_m_items[index].m_key   = NULL;
			self->_m_items[index].m_value = NULL;

			self->_m_occupied--;
		}
	}

	if(SHOULD_MAP_SHRINK(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, CMAP_SHRINK));
	}

	return CMAP_SUCCESS;
}