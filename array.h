#define ARRAY_INIT_CAPACITY 8
#define append(array, elem) \
    do { \
        assert((array)->len >= 0); \
        assert((array)->capacity >= 0); \
        assert((array)->len <= (array)->capacity); \
        assert(sizeof(*((array)->data)) == sizeof(elem)); \
        if (((array))->len >= (array)->capacity) { \
            (array)->capacity = (array)->capacity == 0 ? ARRAY_INIT_CAPACITY : (array)->capacity*2; \
            (array)->data = realloc((array)->data, sizeof(elem)*(array)->capacity); \
            assert((array)->data); \
        } \
        ((array))->data[((array))->len] = (elem); \
        ((array))->len += 1; \
    } while (0);

#define _EXPAND(x) macro_it ## x
#define append_many(array, ...) _append_many_(array, __COUNTER__, __VA_ARGS__) 
#define _append_many_(array, counter, ...) \
    do { \
        typeof(*(array)->data) to_append[] = {__VA_ARGS__}; \
        for (size_t _EXPAND(counter) = 0;  _EXPAND(counter) < sizeof(to_append)/sizeof(*to_append); _EXPAND(counter)++) { \
            append(array, to_append[_EXPAND(counter)]); \
        } \
    } while(0);

#define xcase break;case
#define GRID_SIZE 100
#define TILE_SIZE 50
#define _EXPAND(x) macro_it ## x
#define __EXPAND(x) _EXPAND(x)
#define UNIQUE_VARIABLE_NAME __EXPAND(__LINE__)
#define foreach(elem, elems) size_t UNIQUE_VARIABLE_NAME = 0; for(typeof(((elems)->data[0])) elem; UNIQUE_VARIABLE_NAME < (elems)->len ? (elem) = (elems)->data[UNIQUE_VARIABLE_NAME], 1 : 0; UNIQUE_VARIABLE_NAME++) 
