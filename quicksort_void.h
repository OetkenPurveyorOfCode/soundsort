#pragma once
#ifndef ORDERING_DEFINED
#define ORDERING_DEFINED
typedef enum {
    Ordering_LessThan = -1,
    Ordering_Equal = 0,
    Ordering_GreaterThan = 1,
} Ordering;

void _null_write_cb(void* userdata, void* loc) {
    (void)userdata;
    (void)loc;
    return;
}
#endif

typedef ptrdiff_t ssize_t;

void quicksort_swap(void* write_cb_userdata, char* a, char* b, ssize_t elem_size, void (*write_cb)(void*, void*)) {
    for (ssize_t i = 0; i < elem_size; i++) {
        char tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
    write_cb(write_cb_userdata, (void*)a);
    write_cb(write_cb_userdata, (void*)b);
}

ssize_t partition(
    void* write_cb_userdata,
    void* cmp_cb_userdata,
    void* elems_void,
    ssize_t elem_size,
    ssize_t low,
    ssize_t high,
    void (*write_cb)(void*, void*),
    Ordering (*cmp_cb)(void*, void*, void*)
) {
    char* elems = (char*)elems_void;
    char* pivot = elems+high*elem_size;
    //quicksort_swap(write_cb_userdata, pivot, elems+high*elem_size, elem_size, write_cb);
    ssize_t i = low-1;
    for(ssize_t j = low; j < high; j++) {
        char* a = elems+j*elem_size;
        if (cmp_cb(cmp_cb_userdata, a, pivot) <= 0) {
            i += 1;
            quicksort_swap(write_cb_userdata, elems + i*elem_size, elems+ j*elem_size, elem_size, write_cb);
        }
    }
    quicksort_swap(write_cb_userdata, elems +(i+1)*elem_size, elems +high*elem_size, elem_size, write_cb);
    return i+1;
}
#include <stdlib.h>
#include <time.h>

int partition_r(
    void* write_cb_userdata,
    void* cmp_cb_userdata,
    void* elems_void,
    ssize_t elem_size,
    ssize_t low,
    ssize_t high,
    void (*write_cb)(void*, void*),
    Ordering (*cmp_cb)(void*, void*, void*)
)
{
    char* elems = (char*)elems_void;
    srand(time(NULL));
    int random = low + rand() % (high - low);
    quicksort_swap(0, elems+random*elem_size, elems+high*elem_size, elem_size, _null_write_cb);
    return partition(write_cb_userdata, cmp_cb_userdata, elems_void, elem_size, low, high, write_cb, cmp_cb);
}

void quicksort_impl(
    void* write_cb_userdata,
    void* cmp_cb_userdata,
    void* elems,
    ssize_t elem_size,
    ssize_t elem_low,
    ssize_t elem_high,
    void (*write_cb)(void*, void*),
    Ordering (*cmp_cb)(void*, void*, void*)
) {
    if (elem_low < elem_high) {
        ssize_t index = partition_r(write_cb_userdata, cmp_cb_userdata, elems, elem_size, elem_low, elem_high, write_cb, cmp_cb);
        quicksort_impl(write_cb_userdata, cmp_cb_userdata, elems, elem_size, elem_low, index-1, write_cb, cmp_cb);
        quicksort_impl(write_cb_userdata, cmp_cb_userdata, elems, elem_size, index+1, elem_high, write_cb, cmp_cb);
    }
}

void quicksort(
    void* write_cb_userdata,
    void* cmp_cb_userdata,
    void* elems,
    size_t elem_size,
    size_t elem_len,
    void (*write_cb)(void*, void*),
    Ordering (*cmp_cb)(void*, void*, void*)
) {
    if (!write_cb) {
        write_cb=_null_write_cb;
    }
    quicksort_impl(write_cb_userdata, cmp_cb_userdata, elems, elem_size, 0, (ssize_t)elem_len-1, write_cb, cmp_cb);
}

#ifdef TEST_QUICKSORT

Ordering cmp_char(void* userdata, void* a_void, void* b_void) {
    (void)userdata;
    char a = *(char*)a_void;
    char b = *(char*)b_void;
    if (a == b) return Ordering_Equal;
    else if (a < b) return Ordering_LessThan;
    else return Ordering_GreaterThan;
}

Ordering cmp_int(void* userdata, void* a_void, void* b_void) {
    (void)userdata;
    int a = *(int*)a_void;
    int b = *(int*)b_void;
    if (a == b) return Ordering_Equal;
    else if (a < b) return Ordering_LessThan;
    else return Ordering_GreaterThan;
}

int main(void) {
    char out[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    shuffle(out, 1, 20);
    printf("Shuffle\n");
    for (size_t i = 0; i < 20; i++) {
        printf("%d\n", (int)out[i]);
    }
    quicksort(0, 0, out, 1, 20, 0, cmp_char);
    printf("Sorted\n");
    for (size_t i = 0; i < 20; i++) {
        printf("%d\n", (int)out[i]);
    }

    int numbers[20] = {2, 5060, 10, 200, 1000, 3000, 54, 1400, 1, 6, 5060, 922222, 20, 90, 123};
    quicksort(0, 0, numbers, 4, 20, 0, cmp_int);
    printf("Sorted int\n");
    for (size_t i = 0; i < 20; i++) {
        printf("%d\n", (int)numbers[i]);
    }
    return 0;
}
#endif
