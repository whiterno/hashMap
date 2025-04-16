#ifndef LIST_H
#define LIST_H

#include "hash_map.h"

const int LIST_BASE_CAPACITY = 4;

List listCtor();
int pushListElem(List* list, data_t data);
int popListElem(List* list, data_t data);
int searchElement(List* list, data_t data);

void debugPrint(List* list);

#endif
