/*
 * DoublyLinkedList.c
 *
 *  Created on: Feb 21, 2016
 *      Author: Jake
 */
#include "DoublyLinkedList.h"
#include <stdlib.h>
#include <stdio.h>



/*
 *Allocates memory to an empty list
 */
DList *memAlloc()
{
    return calloc(1, sizeof(DList));
}

/*
 * freeing the nodes
 */
void destroyFromList(DList *d)
{
	pageNode *pNode=NULL;
	pNode=d->head;
	while(pNode!=NULL)
	{
		if(pNode->prev)
		{
			free(pNode->value);
		}
	pNode=pNode->next;
	}

}
/*
 * Adding value to head of the list
 */
void addToHead(DList *list, void *value)
{
    pageNode *node = calloc(1, sizeof(pageNode));
    node->value = value;

    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        list->size=1;
        return;
    }
    else
    {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->size++;
    return;
}
/*
 * Removes an element from the head
 */
void *removeNode(DList *list, pageNode *node)
{
    if(node==NULL)
    {
    	printf("Value to be inserted not initialized\n");
    	return NULL;
    }

    if(list->head==NULL) {
	printf("Empty List\n");
	return NULL;
    }

    if(node == list->head && list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
    }
	else if(node == list->head) {
        list->head = node->next;
        list->head->prev = NULL;
    }
	else if (node == list->tail)
	{
        list->tail = node->prev;
        list->tail->next = NULL;
    }
	else
	{
		node->prev->next = node->next;
        node->next->prev = node->prev;

    }
    list->size--;
    free(node);

    return node->value;
}
/*
 * Adding elements to tail
 */
void addToTail(DList *list, void *value)
{
    pageNode *node = calloc(1, sizeof(pageNode));
    node->value = value;

    if(list->tail == NULL)
    {
        list->head = node;
        list->tail = node;
        list->size=1;
        return;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->size++;
return;
}
