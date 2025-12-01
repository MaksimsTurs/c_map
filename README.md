# C Map

C Map is a very simple Hash Table implemintation in C.

## API
`cmap_sinit`: Initialize a stack based Map that work with the buffer that **must** be allocated on the stack, when the Map does not have more place and `is_resizable` parameters is `0` the `cmap_set` function will return an error code and do nothing. By passing a `1` the Map will switch from stack based to heap based and use heap as a storage, returns a error code. 

`cmap_dinit`: Initialize a heap based Map, when the Map does not have more place and to `is_resizable` parameters was passed a `0` the `cmap_set` function will return a error code and do nothing. By passing a `1` the map will expand, returns error code.

`cmap_set`: Set a new value or overwrite the old value when the existing key was passed, returns error code.

`cmap_get`: Save a map item data in the `cmap_item` structure, return error code when item with the passed key does not exist, returns error code.

`cmap_delete`: Remove single item from the map, returns error code.

`cmap_has`: Checks if element exist in the map.

`cmap_print`: For debbuging purposes, print all elements in the map.

## Example
```c
#include "../include/c_map.h"

int main(void)
{
  cmap _1 = {0};
  cmap _2 = {0};

  cmap_item __1 = {0};
  cmap_item __2 = {0};

  cmap_item buff[4] = {0};

  cmap_sinit(&_1, buff, sizeof(buff) / sizeof(cmap_item), 1);
  cmap_dinit(&_2, sizeof(buff) / sizeof(cmap_item),  1);
  
  cmap_set(&_1, "First:Name", "Max");
  cmap_set(&_2, "First:Name", "Max");
  
  cmap_set(&_1, "Second:Name", "Musterman");
  cmap_set(&_2, "Second:Name", "Musterman");

  if(cmap_has(&_1, "First:Name"))
  {
    // do stuff...
  }
  if(cmap_has(&_2, "Second:Name"))
  {
    // do stuff...
  }

  cmap_print(&_1);
  cmap_print(&_2);

  cmap_get(&_1, &__1, "First:Name");
  cmap_get(&_2, &__2, "Second:Name");

  printf("Item from __1 map: %lli %s %s\n", __1._m_hash, __1.m_key, (t_char*)__1.m_value);
  printf("Item from __2 map: %lli %s %s\n", __2._m_hash, __2.m_key, (t_char*)__2.m_value);
  printf("\n");

  cmap_delete(&_1, "First:Name");
  cmap_delete(&_2, "First:Name");

  cmap_print(&_1);
  cmap_print(&_2);

  return 0;
}
```