/*
 * DoublyLinkedList.h
 *
 *  Created on: Feb 22, 2016
 *      Author: Jake
 */

#ifndef DOUBLYLINKEDLIST_H_
#define DOUBLYLINKEDLIST_H_

#include <stdlib.h>
#include <stdio.h>

extern struct pageNode;
extern struct DList;

typedef struct pageNode {
    struct pageNode *next;
    struct pageNode *prev;
    void *value;
    int rank;

} pageNode;

typedef struct DList {
	int max_rank_lru;
	struct pageNode *cur;
    struct pageNode *head;
    struct pageNode *tail;
    int size;
} DList;

/*
 *Allocates memory to an empty list
 */
DList *memAlloc();

/*
 * Adding value to head of the list
 */
void addToHead(DList *list, void *value);

/*
 * Removes an element from the head
 */
void *removeFromTail(DList *list, pageNode *node);

/*
 * Adding elements to tail
 */
void addToTail(DList *list, void *value);

/*
 * freeing the nodes
 */
void destroyFromList(DList *d);

#endif /* DOUBLYLINKEDLIST_H_ */
