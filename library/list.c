#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "list.h"

const double RESIZE_FACTOR = 2;

typedef struct list {
    void **data;
    size_t size;
    size_t capacity;
    free_func_t element_free;
} list_t;

list_t *list_init(size_t initial_size, free_func_t list_element_free){
    list_t *new_list = malloc(sizeof(list_t));
    assert(new_list != NULL);
    new_list->data = malloc(initial_size * sizeof(void *));
    assert(new_list->data != NULL);
    new_list->size = 0;
    new_list->capacity = initial_size;
    new_list->element_free = list_element_free;
    return new_list;
}

void list_free(list_t *list){
    if (list->element_free != NULL){
        for (size_t i = 0; i < list->size; i++){
            list->element_free(list->data[i]);
        }
    }
    free(list->data);
    free(list);
}

size_t list_size(list_t *list){
    return list->size;
}

void *list_get(list_t *list, size_t index){
    assert(index < list->size);
    return list->data[index];
}

void *list_remove(list_t *list, size_t index){
    assert(list->size != 0);
    assert(index < list->size);
    void *removed = (list->data)[index];
    (list->size)--;
    if (list->size == 0){
        size_t capacity = list->capacity;
        free_func_t ele_free = list->element_free;
        list = list_init(capacity, ele_free);
        return removed;
    }
    for(size_t i = index + 1; i <= list->size; i++){
        list->data[i - 1] = list->data[i];
    }
    return removed;
}

void list_resize(list_t *list){
    if (list->capacity == 0){
        list->capacity = 1;
    }
    void **new = malloc(sizeof(void *) * RESIZE_FACTOR * list->capacity);
    for (int i = 0; i < list->size; i++){
        new[i] = list->data[i];
    }
    free(list->data);
    list->data = new;
    list->capacity *= RESIZE_FACTOR;
}

void list_add(list_t *list, void *value){
    assert(value != NULL);
    if (list->size >= list->capacity){
        list_resize(list);
    }
    size_t current_size = list->size;
    (list->data)[current_size] = value;
    (list->size)++;
}
