#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODES 10  

struct byLightening {
    int numb;
    struct byLightening * nextNode;
    struct byLightening * prevNode;
    };

int hitCount(void)
{
    static int hits = 0;
    return(hits++);
}

struct byLightening * nodeAdd(void)
{
    struct byLightening *node;
    node = (struct byLightening *) malloc(sizeof(struct byLightening));
    node->nextNode = NULL;
    node->numb = hitCount();
    return(node);
}

struct byLightening * build(struct byLightening *head)
{
    struct byLightening * prev, * curr;
    int i;   
    prev = head;
    for (i = 0; i < NODES; i++)
    {
        curr = nodeAdd();
        prev->nextNode = curr;
	curr->prevNode = prev;
        prev = curr;
	printf("node %d\n", curr->numb);
    }
    return(curr);
}

void calc(int x)
{
     int y = (NODES/2) % x;
     printf("calculated: %d\n", x/y);
}

void demolish(struct byLightening *head)
{
    struct byLightening * curr, * tmp;
    int j;   
    curr = head;
    for (j = 0; j < NODES; j++)
    {
	tmp = curr;
	curr = curr->prevNode;
	printf("gone %d\n", tmp->numb);
	calc(tmp->numb);
	free(tmp);
    }
    head = NULL;
}

void runLooper(void)
{
    struct byLightening *head, * tail;
    int i;
    head = nodeAdd();
    tail = build(head);
    demolish(tail);
}


