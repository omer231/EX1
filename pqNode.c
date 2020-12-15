#include "pqNode.h"
#include <stdlib.h>


pqNode pqNodeIterate(pqNode node, int iterator) {
    if (node == NULL) {
        return NULL;
    }
    while (iterator > 1) {
        node = (pqNode) node->next;
        return pqNodeIterate(node, iterator - 1);
    }
    return node;
}

pqNode pqNodeCreate(PQElement element, PQElementPriority priority, pqNode next,
                    CopyPQElement copy_element,
                    FreePQElement free_element,
                    CopyPQElementPriority copy_priority,
                    FreePQElementPriority free_priority) {
    pqNode node = malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->element = element;
    node->priority = priority;
    node->next = next;
    node->copyElement = copy_element;
    node->freeElement = free_element;
    node->copyPriority = copy_priority;
    node->freePriority = free_priority;
    return node;
}

void pqNodeFree(pqNode node) {
    if (node == NULL) {
        return;
    }
    if (node->element != NULL) {
        node->freeElement(node->element);
    }
    if (node->priority != NULL) {
        node->freePriority(node->priority);
    }
    free(node);
}

inline PQElement pqNodeGetElement(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->element;
}

inline PQElementPriority pqNodeGetPriority(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->priority;
}

inline pqNode pqNodeGetNext(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    return node->next;
}

pqNode pqNodeCopy(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    pqNode new_node = pqNodeCreate(node->copyElement(pqNodeGetElement(node)),
                                   node->copyPriority(pqNodeGetPriority(node)),
                                   pqNodeGetNext(node),
                                   node->copyElement, node->freeElement,
                                   node->copyPriority, node->freePriority);
    if (new_node == NULL) {
        return NULL;
    }
    return new_node;
}

pqNode pqNodeCopyFull(pqNode node) {
    if (node == NULL) {
        return NULL;
    }
    pqNode new_node = pqNodeCopy(node);
    if (new_node == NULL) {
        return NULL;
    }
    new_node->next = pqNodeCopyFull(pqNodeGetNext(node));
    return new_node;
}
