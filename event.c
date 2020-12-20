#ifndef EVENT_H_
#define EVENT_H_
#define EVENT_NULL_ERR -1
#include "date.h"
#include <string.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "member.h"

typedef struct Event_t {
    char *EventName;
    Date EventDate;
    int event_id;
    PriorityQueue Members;
} *Event;

Event copyEvent(Event event) {
    Event event_new = malloc(sizeof(*event));
    if (event_new == NULL) {
        return NULL;
    }
    event_new->EventName = malloc(sizeof(char) * (strlen(event->EventName) + 1));
    if (event_new->EventName == NULL) {
        return NULL;
    }
    strcpy(event_new->EventName, event->EventName);
    event_new->EventDate = dateCopy(event->EventDate);
    event_new->event_id = event->event_id;
    event_new->Members = pqCopy(event->Members);
    return event_new;
}

void destroyEvent(Event event) {
    dateDestroy(event->EventDate);
    pqDestroy(event->Members);
    free(event->EventName);
    free(event);
}

PQElement copyEventPQElement(PQElement event) {
    return copyEvent(event);
}

PQElementPriority copyEventPriority(PQElementPriority priority) {
    PQElementPriority new_priority = malloc(sizeof(new_priority));
    if (new_priority == NULL) {
        return NULL;
    }
    memcpy(new_priority, priority, sizeof(PQElementPriority));
    return new_priority;
}

void freePQEvent(PQElement event) {
    destroyEvent(event);
}

void freePQEventPriority(PQElementPriority priority) {
    free(priority);
}

bool equalPQEvents(PQElement element1, PQElement element2) {
    Event event1 = (Event) element1, event2 = (Event) element2;
    return event1->event_id == event2->event_id;
}

int comparePQEventPriorities(PQElementPriority priority1, PQElementPriority priority2) {
    int *event_priority1 = priority1;
    int *event_priority2 = priority2;
    return *event_priority2 - *event_priority1;
}

Event createEvent(char *name, Date date, int event_id) {
    Event event = malloc(sizeof(*event));
    if (event == NULL) {
        return NULL;
    }
    event->EventName = malloc(sizeof(char) * (strlen(name) + 1));
    if (event->EventName == NULL) {
        return NULL;
    }
    strcpy(event->EventName, name);
    event->EventDate = dateCopy(date);
    event->event_id = event_id;
    event->Members = pqCreate(copyMemberPQElement, freeMemberElement, equalMemberElements, copyMemberPriority,
                              freeMemberElementPriority, compareMemberElementPriorities);
    return event;
}

char *eventGetName(Event event) {
    if (event)
    {
        return event->EventName;
    }
    return NULL;
}

Date eventGetDate(Event event) {
    if (event)
    {
        return event->EventDate;
    }
    return NULL;
}

int eventGetId(Event event) {
    if (event)
    {
        return event->event_id;
    }
    return EVENT_NULL_ERR;
}

PriorityQueue eventGetMembers(Event event) {
    if (event)
    {
        return event->Members;
    }
    return NULL;
}


#endif //EVENT_H_
