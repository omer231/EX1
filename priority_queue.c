/**
 * I want to create a list-based queue, with each node having some data and a priority
 * The order of the queue will be through the nodes, each new addition will be sorted and placed from highest
 * priority to lowest priority
**/

#include <stdlib.h>
#include <stdbool.h>
#include "priority_queue.h"

#define ELEMENT_NOT_FOUND -1

typedef struct pqNode_t *pqNode;

/*
 * STRUCTS
 */

/**
 * Struct representing a single node inside the queue
 */
struct pqNode_t {
    PQElement *element;
    PQElementPriority *priority;
    pqNode *next;
};
/**
 * Struct representing the queue, with the first and last nodes linked, size in integer, iterator for users,
 * and all functions that the user supplies
 */
struct PriorityQueue_t {
    CopyPQElement copyElement;
    FreePQElement freeElement;
    EqualPQElements equalElements;
    CopyPQElementPriority copyPriority;
    FreePQElementPriority freePriority;
    ComparePQElementPriorities comparePriorities;
    int iterator;
    int size;
    pqNode first;
};

/*
 * STATIC FUNCTIONS FOR pqNode
 */
static pqNode pqNodeIterate(pqNode node, int iterator) {
    if (node == NULL) {
        return NULL;
    }
    while (iterator != 0) {
        node = (pqNode) node->next;
        return pqNodeIterate(node, iterator - 1);
    }
    return node;
}

/*
 * STATIC FUNCTIONS FOR PriorityQueue
 */

/*
 * PROVIDED FUNCTIONS FOR PriorityQueue
 */

PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities) {
    if (copy_element == NULL || free_element == NULL || equal_elements == NULL ||
        copy_priority == NULL || free_priority == NULL || compare_priorities == NULL) {
        return NULL;
    }
    PriorityQueue queue = malloc(sizeof(*queue));
    if (queue == NULL) {
        return NULL;
    }
    queue->copyElement = copy_element;
    queue->freeElement = free_element;
    queue->equalElements = equal_elements;
    queue->copyPriority = copy_priority;
    queue->freePriority = free_priority;
    queue->comparePriorities = compare_priorities;

    queue->iterator = 0;
    queue->size = 0;
    queue->first = NULL;
}

void pqDestroy(PriorityQueue queue) {
    if (queue == NULL) {
        return;
    }
    while (pqGetSize(queue) != 0) {
        queue->size = queue->size - 1;
        pqRemove(queue);
        pqDestroy(queue);
    }
}

PriorityQueue pqCopy(PriorityQueue queue) {

}

int pqGetSize(PriorityQueue queue) {
    if (queue == NULL) {
        return ELEMENT_NOT_FOUND;
    }
    return (queue->size);
}

bool pqContains(PriorityQueue queue, PQElement element) {

}

PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority) {
    if (queue == NULL || element == NULL || priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    pqNode new_node = malloc(sizeof(pqNode));
    if (new_node == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    new_node->element = queue->copyElement(element);
    new_node->priority = queue->copyPriority(priority);
    if (new_node->element == NULL || new_node->priority == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    if (queue->first == NULL) {
        queue->first = new_node;
        queue->size = queue->size++;
        return PQ_SUCCESS;
    }
    if (queue->comparePriorities(new_node->priority, queue->first->priority) >= 0) {
        new_node->next = (pqNode *) queue->first;
        queue->first = new_node;
        queue->size = queue->size++;
        return PQ_SUCCESS;
    }
    pqNode temp = (pqNode) queue->first->next;
    while (temp->next != NULL) {
        if (queue->comparePriorities(new_node->priority, temp->priority) >= 0) {
            pqNode tempNext = (pqNode) temp->next;
            temp->next = (pqNode *) new_node;
            new_node->next = (pqNode *) tempNext;
            queue->size = queue->size++;
            return PQ_SUCCESS;
        }
        temp = (pqNode) temp->next;
    }
    return PQ_ERROR;
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element, PQElementPriority old_priority,
                                     PQElementPriority new_priority) {

}

PriorityQueueResult pqRemove(PriorityQueue queue) {
    if (queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    queue->freeElement(queue->first->element);
    queue->freePriority(queue->first->priority);
    queue->first = (pqNode) queue->first->next;
    queue->size = queue->size - 1;
    return PQ_SUCCESS;
}

PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element) {
    if (queue == NULL || element == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    if (queue->equalElements(element, queue->first->element)) {
        return pqRemove(queue);
    }
    if (queue->first->next == NULL) {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    PriorityQueue temp = (PriorityQueue) queue;
    temp->first = (pqNode) queue->first->next;
    return pqRemoveElement(temp, element);
}

PQElement pqGetFirst(PriorityQueue queue) {
    if (queue == NULL) {
        return NULL;
    }
    if (queue->first == NULL) {
        return NULL;
    }
    queue->iterator = 1;
    return queue->first->element;
}

PQElement pqGetNext(PriorityQueue queue) {
    if (queue == NULL) {
        return NULL;
    }
    if (queue->first->next == NULL) {
        return NULL;
    }
    queue->iterator = queue->iterator++;
    return pqNodeIterate(queue->first, queue->iterator)->element;
}

PriorityQueueResult pqClear(PriorityQueue queue) {

}
