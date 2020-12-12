/**
 * I want to create a list-based queue, with each node having some data and a priority
 * The order of the queue will be through the nodes, each new addition will be sorted and placed from highest
 * priority to lowest priority
 * I will remember the first and last nodes in the queue in order to compare priorities
**/

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
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
    pqNode next;
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
    pqNode iteratorP;
    int size;
    pqNode first;
};

/*
 * STATIC FUNCTIONS FOR pqNode
 */
/**
 * pqNodeIterator: Goes over the number of nodes the iterator points to, recursively
 * @param node - first node, iterating down from there
 * @param iterator - the number of nodes to go down to
 * @return
 *      if the iterator reached 0, the node that it stopped on will be returned
 *      otherwise, pqNodeIterate with the next node and iterator -1
 */
static pqNode pqNodeIterate(pqNode node, int iterator) {
    if (node == NULL) {
        return NULL;
    }
    while (iterator > 1) {
        node = (pqNode) node->next;
        return pqNodeIterate(node, iterator - 1);
    }
    return node;
}

/**
 * pqNodeCreate: Creates a new node with provided element, priority and next, each of them can be set to NULL
 * @param element
 * @param priority
 * @param next
 * @param queue - the queue we're using in order to get queue functions
 * @return
 *      NULL if memory allocation failed
 *      the new node if it didn't
 */
static pqNode pqNodeCreate(PQElement element, PQElementPriority priority, pqNode next) {
    pqNode node = malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->element = element;
    node->priority = priority;
    node->next = next;
    return node;
}

static void pqNodeFree(pqNode node, PriorityQueue queue) {
    if (node == NULL || queue == NULL) {
        return;
    }
    if (node->element != NULL) {
        queue->freeElement(node->element);
    }
    if (node->priority != NULL) {
        queue->freePriority(node->priority);
    }
    free(node);
}

/**
 * pqNodeGetElement: Retrieve the element from current node
 * @param node - the node whose element the function returns
 * @return
 *      node->element
 */
inline static PQElement pqNodeGetElement(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->element;
}

/**
 * pqNodeGetPriority: Retrieve the priority from the current node
 * @param node - the node whose priority the function returns
 * @return
 *      node->priority
 */
inline static PQElementPriority pqNodeGetPriority(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->priority;
}

/**
 * pqNodeGetNext: Retrieve the next node from the current node
 * @param node - the node whose next node the function returns
 * @return
 *      (pqNode) node->next
 */
inline static pqNode pqNodeGetNext(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->next;
}

/**
 * pqNodeCopy: Creates a copy of a single node
 * @param node
 * @param queue - the queue we're using in order to get queue functions
 * @return
 *      NULL if the node wasn't successfully created
 *      the new node otherwise
*/
static pqNode pqNodeCopy(pqNode node, PriorityQueue queue) {
    if (node == NULL || queue == NULL) {
        return NULL;
    }
    pqNode new_node = pqNodeCreate(queue->copyElement(pqNodeGetElement(node)),
                                   queue->copyPriority(pqNodeGetPriority(node)),
                                   pqNodeGetNext(node));
    if (new_node == NULL) {
        return NULL;
    }
    return new_node;
}

/**
 * pqNodeCopyFull: Creates a copy of all linked nodes, recursively
 * @param node
 * @param queue - the queue we're using in order to get queue functions
 * @return
 *      NULL if the node(s) weren't successfully created
 *      the new copied top of the node (I hope)
 */
static pqNode pqNodeCopyFull(pqNode node, PriorityQueue queue) {
    if (node == NULL || queue == NULL) {
        return NULL;
    }
    pqNode new_node = pqNodeCopy(node, queue);
    if (new_node == NULL) {
        return NULL;
    }
    new_node->next = pqNodeCopyFull(pqNodeGetNext(node), queue);
    return new_node;
}


/*
 * STATIC FUNCTIONS FOR PriorityQueue
 */
inline static PriorityQueueResult pqUpdateSizeAfterInsertAndReturnSuccess(PriorityQueue queue) {
    if (queue == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    queue->size = queue->size + 1;
    return PQ_SUCCESS;
}

inline static PriorityQueueResult pqUpdateSizeAfterRemoveAndReturnSuccess(PriorityQueue queue){
    if(queue == NULL){
        return PQ_OUT_OF_MEMORY;
    }
    queue->size = queue->size - 1;
    return PQ_SUCCESS;
}

/**
 * isPQEmpty: whether the queue is empty
 * @param queue - the queue the function checks
 * @return
 *      TRUE if the queue is empty according to pqGetSize
 *      FALSE is the queue is not empty
 */
inline static bool isPQEmpty(PriorityQueue queue) {
    return (pqGetSize(queue) == 0);
}

/**
 * pqNodeSame: Checks if the node has the element and priority specified, priority can be set to NULL and if so only
 * element will be checked
 * @param queue
 * @param element
 * @param priority
 * @param node
 * @return
 *      TRUE if the node has the element and priority/only element if priority is NULL
 *      FALSE if not or if NULL was sent as parameter in node or element
 */
static bool pqNodeSame(PriorityQueue queue, PQElement element, PQElementPriority priority, pqNode node) {
    if (element == NULL || node == NULL) {
        return false;
    }
    if (queue->equalElements(element, pqNodeGetElement(node))) {
        if (priority == NULL || queue->comparePriorities(priority, pqNodeGetPriority(node)) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * pqContainsWithPriority: Checks if a certain element with a certain priority (can be set to NULL as to ignore
 * priority) exist in queue
 * @param queue
 * @param element
 * @param priority
 * @return
 */
static bool pqContainsWithPriority(PriorityQueue queue, PQElement element, PQElementPriority priority) {
    if (queue == NULL || element == NULL) {
        return NULL;
    }
    pqNode temp = queue->first;
    int size = queue->size;
    while (size != 0) {
        if (pqNodeSame(queue, element, priority, temp) == false) {
            if (temp->next != NULL) {
                temp = (pqNode) temp->next;
                size = size - 1;
            } else {
                break;
            }
        } else {
            return true;
        }
    }
    return false;

}

/**
 * setIteratorToNULL: sets the queue's iterator to NULL
 * @param queue
 */
inline static void setIteratorToNULL(PriorityQueue queue) {
    if (queue == NULL) {
        return;
    }
    queue->iterator = ELEMENT_NOT_FOUND;
    queue->iteratorP = NULL;
}

/**
 * pqRemoveElementAndPriority: Removes a certain element with a certain priority (can be set to NULL as to be ignored)
 * @param queue
 * @param element
 * @param priority
 * @return
 */
static PriorityQueueResult pqRemoveElementAndPriority(PriorityQueue queue, PQElement element,
                                                      PQElementPriority priority) {
    if (queue == NULL || element == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    pqNode before = queue->first;
    while (before->next != NULL) {
        if (queue->equalElements(element, pqNodeGetElement(before->next))) {
            if (priority == NULL || queue->comparePriorities(priority, pqNodeGetPriority(before->next)) == 0) {
                pqNode temp = before->next;
                before->next = temp->next;
                pqNodeFree(temp, queue);
                return pqUpdateSizeAfterRemoveAndReturnSuccess(queue);
            }
        }
        before = before->next;
    }
    if (queue->size == 1) {
        pqNodeFree(before, queue);
        queue->first = NULL;
        return pqUpdateSizeAfterRemoveAndReturnSuccess(queue);
    }
    return PQ_ERROR;
}


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
    queue->iterator = ELEMENT_NOT_FOUND;
    queue->iteratorP = NULL;
    queue->size = 0;
    queue->first = NULL;
    return queue;
}

void pqDestroy(PriorityQueue queue) {
    if (queue == NULL) {
        return;
    }
    while (pqGetSize(queue) != 0) {
        pqRemove(queue);
    }
    free(queue);
}

PriorityQueue pqCopy(PriorityQueue queue) {
    if (queue == NULL) {
        return NULL;
    }
    PriorityQueue new_queue = pqCreate(queue->copyElement,
                                       queue->freeElement,
                                       queue->equalElements,
                                       queue->copyPriority,
                                       queue->freePriority,
                                       queue->comparePriorities);
    if (new_queue == NULL) {
        return NULL;
    }
    int size = queue->size;
    new_queue->size = size;
    if (size > 0) {
        pqNode copy = pqNodeCopyFull(queue->first, queue);
        new_queue->first = copy;
        size = size - 1;
    }
    setIteratorToNULL(new_queue);
    setIteratorToNULL(queue);
    return new_queue;
}

int pqGetSize(PriorityQueue queue) {
    if (queue == NULL) {
        return ELEMENT_NOT_FOUND;
    }
    return (queue->size);
}

bool pqContains(PriorityQueue queue, PQElement element) {
    if (queue == NULL || element == NULL) {
        return NULL;
    }
    return pqContainsWithPriority(queue, element, NULL);
}

PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority) {
    if (queue == NULL || element == NULL || priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    pqNode new_node = pqNodeCreate(queue->copyElement(element), queue->copyPriority(priority), NULL);
    setIteratorToNULL(queue);
    if (new_node == NULL || new_node->element == NULL || new_node->priority == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    if (queue->first == NULL) {
        queue->first = new_node;
        return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
    }
    if (queue->comparePriorities(new_node->priority, queue->first->priority) > 0) {
        new_node->next = queue->first;
        queue->first = new_node;
        return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
    }
    if (queue->comparePriorities(new_node->priority, queue->first->priority) == 0) {
        new_node->next = queue->first->next;
        queue->first->next = new_node;
        return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
    }
    pqNode temp = queue->first;
    if (temp == NULL || temp->element == NULL || temp->priority == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    if (pqNodeGetNext(queue->first) == NULL) {
        if (temp == NULL || temp->element == NULL || temp->priority == NULL) {
            return PQ_OUT_OF_MEMORY;
        }
        temp->next = new_node;
        queue->first = temp;
        return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
    }
    pqNode temp_next = pqNodeGetNext(temp);
    while (temp_next != NULL) {
        if (queue->comparePriorities(new_node->priority, pqNodeGetPriority(temp_next)) > 0) {
            new_node->next = temp_next;
            temp->next = new_node;
            return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
        }
        if (queue->comparePriorities(new_node->priority, pqNodeGetPriority(temp_next)) == 0) {
            bool next_is_also_same_priority = false;
            if (temp_next->next != NULL &&
                queue->comparePriorities(new_node->priority, pqNodeGetPriority(temp_next->next)) == 0) {
                next_is_also_same_priority = true;
            }
            if (next_is_also_same_priority == false) {
                new_node->next = temp_next->next;
                temp_next->next = new_node;
                return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
            }
        }
        temp = pqNodeGetNext(temp);
        temp_next = pqNodeGetNext(temp_next);
    }
    temp->next = new_node;
    return pqUpdateSizeAfterInsertAndReturnSuccess(queue);
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element, PQElementPriority old_priority,
                                     PQElementPriority new_priority) {
    if (queue == NULL || element == NULL || old_priority == NULL || new_priority == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    if (pqContainsWithPriority(queue, element, old_priority) == false) {
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    if (pqRemoveElementAndPriority(queue, element, old_priority) == PQ_SUCCESS) {
        return pqInsert(queue, element, new_priority);
    }
    return PQ_ERROR;
}

PriorityQueueResult pqRemove(PriorityQueue queue) {
    if (queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    if (isPQEmpty(queue) == true) {
        return PQ_SUCCESS;
    }
    pqNode temp = pqNodeGetNext(queue->first);
    pqNodeFree(queue->first, queue);
    queue->first = temp;
    setIteratorToNULL(queue);
    return pqUpdateSizeAfterRemoveAndReturnSuccess(queue);
}

PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element) {
    if (queue == NULL || element == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    setIteratorToNULL(queue);
    if (pqContains(queue, element) == true) {
        PriorityQueueResult result = pqRemoveElementAndPriority(queue, element, NULL);
        return result;
    }
    return PQ_ELEMENT_DOES_NOT_EXISTS;
}

PQElement pqGetFirst(PriorityQueue queue) {
    if (queue == NULL) {
        return NULL;
    }
    if (queue->first == NULL) {
        return NULL;
    }
    queue->iterator = 1;
    queue->iteratorP = queue->first;
    return pqNodeGetElement(queue->first);
}

PQElement pqGetNext(PriorityQueue queue) {
    if (queue == NULL) {
        return NULL;
    }
    if (queue->iteratorP == NULL) {
        return NULL;
    }
    if (queue->first->next == NULL) {
        return NULL;
    }
    queue->iterator = queue->iterator + 1;
    queue->iteratorP = queue->first->next;
    return pqNodeGetElement((pqNodeIterate(queue->first, queue->iterator)));
}

PriorityQueueResult pqClear(PriorityQueue queue) {
    if (queue == NULL) {
        return PQ_NULL_ARGUMENT;
    }
    while (pqGetSize(queue) != 0) {
        pqRemove(queue);
    }
    return PQ_SUCCESS;
}