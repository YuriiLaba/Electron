#include "stm32f0xx.h"
#include "stdio.h"

//a dynamically growing array
typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, int element);
void freeArray(Array *a);
