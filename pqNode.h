#ifndef EX1_PQNODE_H
#define EX1_PQNODE_H

#include "priority_queue.h"

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
    CopyPQElement copyElement;
    FreePQElement freeElement;
    CopyPQElementPriority copyPriority;
    FreePQElementPriority freePriority;
    pqNode next;
};

/**
 * pqNodeIterator: Goes over the number of nodes the iterator points to, recursively
 * @param node - first node, iterating down from there
 * @param iterator - the number of nodes to go down to
 * @return
 *      if the iterator reached 0, the node that it stopped on will be returned
 *      otherwise, pqNodeIterate with the next node and iterator -1
 */
pqNode pqNodeIterate(pqNode node, int iterator);

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
pqNode pqNodeCreate(PQElement element, PQElementPriority priority, pqNode next,
                    CopyPQElement copy_element,
                    FreePQElement free_element,
                    CopyPQElementPriority copy_priority,
                    FreePQElementPriority free_priority);

/**
 * pqNodeFree: Free the element, priority of the specific node and free node itself
 * @param node
 */
void pqNodeFree(pqNode node);

/**
 * pqNodeGetElement: Retrieve the element from current node
 * @param node - the node whose element the function returns
 * @return
 *      node->element
 */
PQElement pqNodeGetElement(pqNode node);

/**
 * pqNodeGetPriority: Retrieve the priority from the current node
 * @param node - the node whose priority the function returns
 * @return
 *      node->priority
 */
PQElementPriority pqNodeGetPriority(pqNode node);

/**
 * pqNodeGetNext: Retrieve the next node from the current node
 * @param node - the node whose next node the function returns
 * @return
 *      (pqNode) node->next
 */
pqNode pqNodeGetNext(pqNode node);

/**
 * pqNodeCopy: Creates a copy of a single node
 * @param node
 * @param queue - the queue we're using in order to get queue functions
 * @return
 *      NULL if the node wasn't successfully created
 *      the new node otherwise
*/
pqNode pqNodeCopy(pqNode node);

/**
 * pqNodeCopyFull: Creates a copy of all linked nodes, recursively
 * @param node
 * @param queue - the queue we're using in order to get queue functions
 * @return
 *      NULL if the node(s) weren't successfully created
 *      the new copied top of the node (I hope)
 */
pqNode pqNodeCopyFull(pqNode node);


#endif //EX1_PQNODE_H
