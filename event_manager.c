#include "priority_queue.h"
#include "event_manager.h"
#include "event.h"
#include "member.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define ELEMENT_NOT_FOUND -1
#define DEFAULT_EVENT_PRIORITY 1

struct EventManager_t {
    PriorityQueue Events;
    Date Date;
    PriorityQueue Members;
};

Member GetMemberById(EventManager em, int member_id) {
    Member member = pqGetFirst(em->Members);
    while (member) {
        if (memberGetId(member) == member_id) {
            return member;
        }
        member = pqGetNext(em->Members);
    }

    return NULL;
}

Event GetEventById(EventManager em, int event_id) {
    Event event = pqGetFirst(em->Events);
    while (event) {
        if (eventGetId(event) == event_id && (dateCompare(em->Date, eventGetDate(event)) <= 0)) {
            return event;
        }
        event = pqGetNext(em->Events);
    }
    return NULL;
}

EventManager createEventManager(Date date) {
    EventManager em = malloc(sizeof(*em));
    if(em == NULL){
        return NULL;
    }
    em->Date = dateCopy(date);
    em->Events = pqCreate(copyEventPQElement, freePQEvent, equalPQEvents, copyEventPriority, freePQEventPriority,
                          comparePQEventPriorities);
    em->Members = pqCreate(copyMemberPQElement, freeMemberElement, equalMemberElements, copyMemberPriority,
                           freeMemberElementPriority, compareMemberElementPriorities);
    return em;
}

void destroyEventManager(EventManager em) {
    if (em) {
        dateDestroy(em->Date);
        pqDestroy(em->Events);
        pqDestroy(em->Members);
        free(em);
    }
}

EventManagerResult checkDateId(EventManager em, Date date, int event_id) {
    if (dateCompare(em->Date, date) > 0) {
        return EM_INVALID_DATE;
    }
    if (event_id < 0) {
        return EM_INVALID_EVENT_ID;
    }
    return EM_SUCCESS;
}

bool is_str_equal(const char *str1, const char * str2) {
    if (!str1 || !str2) {
        return false;
    }
    while (*str1 != '\0' && *str1 == *str2) {
        ++str1;
        ++str2;
    }
    return *str1 == *str2;
}

EventManagerResult checkDateIdName(EventManager em, Date date, int event_id, char *event_name) {
    EventManagerResult result = checkDateId(em, date, event_id);
    if (result != EM_SUCCESS) {
        return result;
    } else {
        Event event = pqGetFirst(em->Events);
        while (event) {
            if (is_str_equal(eventGetName(event), event_name) && (dateCompare(date, eventGetDate(event)) == 0)) {
                return EM_EVENT_ALREADY_EXISTS;
            }
            event = pqGetNext(em->Events);
        }
    }
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDate(EventManager em, char *event_name, Date date, int event_id) {
    if (!(em && event_name && date)) {
        return EM_NULL_ARGUMENT;
    }
    EventManagerResult result = checkDateIdName(em, date, event_id, event_name);
    if (result != EM_SUCCESS) {
        return result;
    }
    Event event = pqGetFirst(em->Events);
    while (event) {
        if (eventGetId(event) == event_id) {
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
        event = pqGetNext(em->Events);
    }
    Event new_event = createEvent(event_name, date, event_id);
    if (new_event == NULL) {
        return EM_OUT_OF_MEMORY;
    }
    int priority = DEFAULT_EVENT_PRIORITY;
    pqInsert(em->Events, new_event, &priority);
    destroyEvent(new_event);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char *event_name, int days, int event_id) {
    if (!(em && event_name)) {
        return EM_NULL_ARGUMENT;
    }
    if (days < 0) {
        return EM_INVALID_DATE;
    }
    Date date = dateCopy(em->Date);
    while (days > 0) {
        days--;
        dateTick(date);
    }
    EventManagerResult result = emAddEventByDate(em, event_name, date, event_id);
    dateDestroy(date);
    return result;
}

EventManagerResult emRemoveEvent(EventManager em, int event_id) {
    if (!em) {
        return EM_NULL_ARGUMENT;
    }
    if (event_id < 0) {
        return EM_INVALID_EVENT_ID;
    }
    Event event = GetEventById(em, event_id);
    if (!event) {
        return EM_EVENT_NOT_EXISTS;
    }
    pqRemoveElement(em->Events, event);
    return EM_SUCCESS;
}

EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id) {
    if (!em) {
        return EM_NULL_ARGUMENT;
    }
    if (member_id < 0) {
        return EM_INVALID_MEMBER_ID;
    }
    if (event_id < 0) {
        return EM_INVALID_EVENT_ID;
    }
    Event event = GetEventById(em, event_id);
    if (!event) {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member member = pqGetFirst(eventGetMembers(event));
    while (member) {
        if (memberGetId(member) == member_id) {
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
        member = pqGetNext(eventGetMembers(event));
    }
    member = pqGetFirst(em->Members);
    while (member) {
        if (memberGetId(member) == member_id) {
            PriorityQueueResult result =pqInsert(eventGetMembers(event), member, &member_id);
            if (result == PQ_OUT_OF_MEMORY)
            {
                return EM_OUT_OF_MEMORY;
            }
            return EM_SUCCESS;
        }
        member = pqGetNext(em->Members);
    }
    return EM_MEMBER_ID_NOT_EXISTS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date)
{
    if (!(em && event_id && new_date))
    {
        return EM_NULL_ARGUMENT;
    }
    if (dateCompare(em->Date, new_date) > 0)
    {
        return EM_INVALID_DATE;
    }
    Event event = GetEventById(em, event_id);
    if (!event)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    EventManagerResult result = checkDateIdName(em, new_date, event_id, eventGetName(event));
    if (result != EM_SUCCESS)
    {
        return result;
    }
    PriorityQueue Members = pqCopy(eventGetMembers(event));
    if (!Members)
    {
        return EM_OUT_OF_MEMORY;
    }
    char* name = malloc(sizeof(char) * (strlen(eventGetName(event)) + 1));
    if (!name)
    {
        return EM_OUT_OF_MEMORY;
    }
    strcpy(name, eventGetName(event));
    emRemoveEvent(em, event_id);
    emAddEventByDate(em, name, new_date, event_id);
    Member member = pqGetFirst(Members);
    while (member)
    {
        emAddMemberToEvent(em, memberGetId(member), event_id);
        member = pqGetNext(Members);
    }
    pqDestroy(Members);
    free(name);
    return EM_SUCCESS;
}

EventManagerResult emAddMember(EventManager em, char *member_name, int member_id) {
    if (!(em && member_name)) {
        return EM_NULL_ARGUMENT;
    }
    if (member_id < 0) {
        return EM_INVALID_MEMBER_ID;
    }
    Member member = pqGetFirst(em->Members);
    while (member) {
        if (memberGetId(member) == member_id) {
            return EM_MEMBER_ID_ALREADY_EXISTS;
        }
        member = pqGetNext(em->Members);
    }
    Member new_member = createMember(member_name, member_id);
    if (!new_member) {
        return EM_OUT_OF_MEMORY;
    }
    pqInsert(em->Members, new_member, &member_id);
    destroyMember(new_member);
    return EM_SUCCESS;
}

EventManagerResult emRemoveMemberFromEvent(EventManager em, int member_id, int event_id) {
    if (!em) {
        return EM_NULL_ARGUMENT;
    }
    if (event_id < 0) {
        return EM_INVALID_EVENT_ID;
    }
    if (member_id < 0) {
        return EM_INVALID_MEMBER_ID;
    }
        Event event = GetEventById(em, event_id);
    if (!event) {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    bool found = false;
    Member member = pqGetFirst(em->Members);
    while (member) {
        if (memberGetId(member) == member_id) {
            found = true;
        }
        member = pqGetNext(em->Members);
    }
    if (!found) {
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    member = pqGetFirst(eventGetMembers(event));
    while (member) {
        if (memberGetId(member) == member_id) {
            pqRemoveElement(eventGetMembers(event), member);
            return EM_SUCCESS;

        }
        member = pqGetNext(eventGetMembers(event));
    }
    return EM_EVENT_AND_MEMBER_NOT_LINKED;
}

EventManagerResult emTick(EventManager em, int days) {
    if (!em) {
        return EM_NULL_ARGUMENT;
    }
    if (days <= 0) {
        return EM_INVALID_DATE;
    }
    while (days > 0) {
        days--;
        dateTick(em->Date);
    }
    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em) {
    if (!em) {
        return ELEMENT_NOT_FOUND;
    }
    int count = 0;
    Event event = pqGetFirst(em->Events);
    while (event) {
        if (dateCompare(em->Date, eventGetDate(event)) <= 0) {
            count++;
        }
        event = pqGetNext(em->Events);
    }
    return count;
}

int getEventsAmountByMember(EventManager em, Member m) {
    if (!em) {
        return ELEMENT_NOT_FOUND;
    }
    int count = 0;
    Event event = pqGetFirst(em->Events);
    while (event) {
        if (pqContains(eventGetMembers(event), m) && (dateCompare(em->Date, eventGetDate(event)) <= 0)) {
            count++;
        }
        event = pqGetNext(em->Events);
    }
    return count;
}

int emGetNextEventId(PriorityQueue Events, Date date) {
    if (!Events || !date) {
        return ELEMENT_NOT_FOUND;
    }
    Event event = pqGetFirst(Events);
    if (!event) {
        return ELEMENT_NOT_FOUND;
    }
    int min = 0, min_id = eventGetId(event);
    bool found = false;
    while (event) {
        int days_diff = dateCompare(eventGetDate(event), date);
        if ((days_diff < min || !found) && days_diff >= 0) {
            min = days_diff;
            min_id = eventGetId(event);
            found = true;
        }
        event = pqGetNext(Events);
    }
    if (!found) {
        return ELEMENT_NOT_FOUND;
    }
    return min_id;
}

char *emGetNextEvent(EventManager em) {
    if (!em) {
        return NULL;
    }
    int id = emGetNextEventId(em->Events, em->Date);
    if (id == ELEMENT_NOT_FOUND) {
        return NULL;
    }
    Event event = GetEventById(em, id);
    if (!event) {
        return NULL;
    }
    return eventGetName(event);
}

void emPrintAllEvents(EventManager em, const char *file_name) {
    PriorityQueue Events = pqCopy(em->Events);
    Event event;
    char str[40000];
    int pos = 0;
    int day, month, year;
    Member member;
    int size = pqGetSize(em->Events);
    for (int i = 0; i < size; i++) {
        event = GetEventById(em, emGetNextEventId(Events, em->Date));
        if (event && dateCompare(em->Date, eventGetDate(event)) <= 0) {
            dateGet(eventGetDate(event), &day, &month, &year);
            pos += sprintf(&str[pos], "%s,", eventGetName(event));
            pos += sprintf(&str[pos], "%d.%d.%d", day, month, year);
            member = pqGetFirst(eventGetMembers(event));
            while (member) {
                pos += sprintf(&str[pos], ",%s", memberGetName(member));
                member = pqGetNext(eventGetMembers(event));
            }
            pos += sprintf(&str[pos], "\n");
        }
        pqRemoveElement(Events, event);
    }
    FILE *fp = fopen(file_name, "w");
    if (pos != 0) {
        fprintf(fp, "%s", str);
    }
    fclose(fp);
    pqDestroy(Events);
}

int emGetNextMemberId(EventManager em, PriorityQueue Members) {
    Member member = pqGetFirst(Members);
    int max = 0, max_id = memberGetId(member);
    while (member) {
        int events_amount = getEventsAmountByMember(em, member);
        if (events_amount > max) {
            max = events_amount;
            max_id = memberGetId(member);
        }
        member = pqGetNext(Members);
    }
    return max_id;
}

void emPrintAllResponsibleMembers(EventManager em, const char *file_name) {
    Member member = NULL;
    PriorityQueue Members = pqCopy(em->Members);
    char str[4000];
    int pos = 0, count = 0;
    int size = pqGetSize(em->Members);
    for (int i = 0; i < size; i++) {
        member = GetMemberById(em, emGetNextMemberId(em, Members));
        if (member) {
            count = getEventsAmountByMember(em, member);
            if (count > 0) {
                pos += sprintf(&str[pos], "%s,%d\n", memberGetName(member), count);
            }
        }
        pqRemoveElement(Members, member);
    }
    FILE *fp = fopen(file_name, "w");
    if (pos != 0) {
        fprintf(fp, "%s", str);
    }
    fclose(fp);
    pqDestroy(Members);
}

