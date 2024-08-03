#pragma once
#include <stdint.h>

void reverse(void* elems_void, size_t elem_size, size_t elem_count) {
    char* elems = (char*)elems_void;
    void* tmp = malloc(elem_size);
    size_t i = 0;
    size_t j = elem_count-1;
    while (i < j) {
        memcpy(tmp,                 elems+i*elem_size,  elem_size);
        memcpy(elems+i*elem_size,   elems+j*elem_size,  elem_size);
        memcpy(elems+j*elem_size,   tmp,                elem_size);
        i++;
        j--;
    }
    free(tmp);
}
