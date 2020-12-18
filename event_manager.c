#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include "priority_queue.h"
#include "event_manager.h"
#include "event.h"
#include "member.h"
#include "date.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct EventManager_t{
    PriorityQueue Events;
    Date Date;
    PriorityQueue Members;
} *EventManager;

typedef enum EventManagerResult_t {
    EM_SUCCESS,
    EM_OUT_OF_MEMORY,
    EM_NULL_ARGUMENT,
    EM_INVALID_DATE,
    EM_INVALID_EVENT_ID,
    EM_EVENT_ALREADY_EXISTS,
    EM_EVENT_ID_ALREADY_EXISTS,
    EM_EVENT_NOT_EXISTS,
    EM_EVENT_ID_NOT_EXISTS,
    EM_INVALID_MEMBER_ID,
    EM_MEMBER_ID_ALREADY_EXISTS,
    EM_MEMBER_ID_NOT_EXISTS,
    EM_EVENT_AND_MEMBER_ALREADY_LINKED,
    EM_EVENT_AND_MEMBER_NOT_LINKED,
    EM_ERROR
} EventManagerResult;

Member GetMemberById(EventManager em, int member_id)
{
    Member member = pqGetFirst(em->Members);
    while (memberGetId(member) != member_id)
    {
        if (memberGetId(member)== member_id)
        {
            return member;
        }
        member = pqGetNext(em->Members);
    }

    return NULL;
}

Event GetEventById(EventManager em, int event_id)
{
    Event event = pqGetFirst(em->Events);
    while (event)
    {
        if (eventGetId(event) == event_id && (dateCompare(em->Date, eventGetDate(event)) <= 0))
        {
            return event;
        }
        event = pqGetNext(em->Events);
    }
    return NULL;
}

EventManager createEventManager(Date date)
{
    EventManager em=malloc(sizeof(*em));
    em->Date=dateCopy(date);
    em->Events = pqCreate(copyEventPQElement, freePQEvent, equalPQEvents, copyEventPriority, freePQEventPriority, comparePQEventPriorities);
    em->Members = pqCreate(copyMemberPQElement, freeMemberElement, equalMemberElements, copyMemberPriority, freeMemberElementPriority, compareMemberElementPriorities);
    return em;
}

void destroyEventManager(EventManager em)
{
    if (em)
    {
        dateDestroy(em->Date);
        pqDestroy(em->Events);
        pqDestroy(em->Members);
        free(em);
    }

}

EventManagerResult checkDateId(EventManager em, Date date, int event_id)
{
    if(dateCompare(em->Date, date)>0)
    {
        return EM_INVALID_DATE;
    }
    if(event_id<0)
    {
        return EM_INVALID_EVENT_ID;
    }
    return EM_SUCCESS;
}

bool equal(const char* s1, const char* s2)
{
    if (!s1 || !s2)
    {
        return false;
    }
    while (*s1 != '\0' && *s1 == *s2)
    {
        ++s1; ++s2;
    }

    return *s1 == *s2;
}

EventManagerResult checkDateIdName(EventManager em, Date date, int event_id, char* event_name)
{
    EventManagerResult result=checkDateId(em, date, event_id);
    if (result != EM_SUCCESS)
    {
        return result;
    }
    else
    {
        Event event = pqGetFirst(em->Events);
        while (event)
        {
            if (equal(eventGetName(event), event_name)&& (dateCompare(date, eventGetDate(event)) == 0))
            {
                return EM_EVENT_ALREADY_EXISTS;
            }
            event = pqGetNext(em->Events);
        }
    }
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id)
{
    if(!(em&&event_name&&date))
    {
        return EM_NULL_ARGUMENT;
    }
    EventManagerResult result = checkDateIdName(em, date, event_id, event_name);
    if (result != EM_SUCCESS)
    {
        return result;
    }
    Event event = pqGetFirst(em->Events);
    while (event)
    {
        if (eventGetId(event) == event_id)
        {
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
        event = pqGetNext(em->Events);
    }
    Event new_event=createEvent(event_name, date, event_id);
    if(new_event ==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    int priority = pqGetSize(em->Events);
    pqInsert(em->Events, new_event, &priority);
    destroyEvent(new_event);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id)
{
    if (!(em && event_name))
    {
        return EM_NULL_ARGUMENT;
    }
    if (days < 0)
    {
        return EM_INVALID_DATE;
    }
    Date date=dateCopy(em->Date);
    while(days>0)
    {
        days--;
        dateTick(date);
    }
    EventManagerResult result= emAddEventByDate(em, event_name, date, event_id);
    dateDestroy(date);
    return result;
}

EventManagerResult emRemoveEvent(EventManager em, int event_id)
{
    if(!em)
    {
        return EM_NULL_ARGUMENT;
    }
    if(event_id<0)
    {
        return EM_INVALID_EVENT_ID;
    }
    Event event=GetEventById(em, event_id);
    if(!event)
    {
        return EM_EVENT_NOT_EXISTS;
    }
    pqRemoveElement(em->Events, event);
    return EM_SUCCESS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date)
{
    if(!(em&&event_id&&new_date))
    {
        return EM_NULL_ARGUMENT;
    }
    if (dateCompare(em->Date, new_date) > 0)
    {
        return EM_INVALID_DATE;
    }
    Event event =GetEventById(em, event_id);
    if(!event)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    EventManagerResult result=checkDateIdName(em, new_date, event_id, eventGetName(event));
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    dateDestroy(eventGetDate(event));
    eventSetDate(event, dateCopy(new_date));
    return EM_SUCCESS;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id)
{
    if(!(em&&member_name))
    {
        return EM_NULL_ARGUMENT;
    }
    if(member_id<0)
    {
        return EM_INVALID_MEMBER_ID;
    }
    Member member = pqGetFirst(em->Members);
    while (member)
    {
        if (memberGetId(member) == member_id)
        {
            return EM_MEMBER_ID_ALREADY_EXISTS;
        }
        member = pqGetNext(em->Members);
    }
    Member new_member=createMember(member_name, member_id);
    if(!new_member)
    {
        return EM_OUT_OF_MEMORY;
    }
    pqInsert(em->Members, new_member, &member_id);
    destroyMember(new_member);
    return EM_SUCCESS;
}

EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id)
{
    if(!em)
    {
        return EM_NULL_ARGUMENT;
    }
    if (member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }
    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }
    Event event=GetEventById(em, event_id);
    if (!event)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member member = pqGetFirst(eventGetMembers(event));
    while (member)
    {
        if (memberGetId(member) == member_id)
        {
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
        member = pqGetNext(eventGetMembers(event));
    }
    member = pqGetFirst(em->Members);
    while (member)
    {
        if (memberGetId(member) == member_id)
        {
            pqInsert(eventGetMembers(event), member, &member_id);
            return EM_SUCCESS;
        }
        member = pqGetNext(em->Members);
    }
    return EM_MEMBER_ID_NOT_EXISTS;
}

EventManagerResult emRemoveMemberFromEvent(EventManager em, int member_id, int event_id)
{
    if (!em)
    {
        return EM_NULL_ARGUMENT;
    }
    if (member_id < 0)
    {
        return EM_INVALID_MEMBER_ID;
    }
    if (event_id < 0)
    {
        return EM_INVALID_EVENT_ID;
    }
    Event event = GetEventById(em, event_id);
    if (!event)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    bool found = false;
    Member member = pqGetFirst(em->Members);
    while (member)
    {
        if (memberGetId(member) == member_id)
        {
            found = true;
        }
        member = pqGetNext(em->Members);
    }
    if (!found)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    member = pqGetFirst(eventGetMembers(event));
    while (member)
    {
        if (memberGetId(member) == member_id)
        {
            pqRemoveElement(eventGetMembers(event), member);
            return EM_SUCCESS;
        }
        member = pqGetNext(eventGetMembers(event));
    }
    return EM_EVENT_AND_MEMBER_NOT_LINKED;
}

EventManagerResult emTick(EventManager em, int days)
{
    if (!em )
    {
        return EM_NULL_ARGUMENT;
    }
    if (days<=0)
    {
        return EM_INVALID_DATE;
    }
    while (days > 0)
    {
        days--;
        dateTick(em->Date);
    }
    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em)
{
    if (!em)
    {
        return -1;
    }
    int count = 0;
    Event event = pqGetFirst(em->Events);
    while (event)
    {
        if (dateCompare(em->Date, eventGetDate(event)) <= 0)
        {
            count++;
        }
        event = pqGetNext(em->Events);
    }
    return count;
}

int getEventsAmountByMember(EventManager em, Member m)
{
    if (!em)
    {
        return -1;
    }
    int count = 0;
    Event event = pqGetFirst(em->Events);
    while (event)
    {
        if (pqContains(eventGetMembers(event), m)&& (dateCompare(em->Date, eventGetDate(event)) <= 0))
        {
            count++;
        }
        event = pqGetNext(em->Events);
    }
    return count;
}

int emGetNextEventId(PriorityQueue Events, Date date)
{
    Event event = pqGetFirst(Events);
    int min = -1, min_id = eventGetId(event);
    bool changed = false;
    while (event)
    {
        int tmp = dateCompare(eventGetDate(event), date);
        if ((tmp < min || min == -1) && tmp >= 0)
        {
            min = tmp;
            min_id = eventGetId(event);
            changed = true;
        }
        event = pqGetNext(Events);
    }
    if (!changed)
    {
        return -1;
    }
    return min_id;
}

char* emGetNextEvent(EventManager em)
{
    if (!em)
    {
        return NULL;
    }
    int id = emGetNextEventId(em->Events, em->Date);
    if (id == -1)
    {
        return NULL;
    }
    Event e = GetEventById(em, id);
    return eventGetName(e);
}

void emPrintAllEvents(EventManager em, const char* file_name)
{
    PriorityQueue Events = pqCopy(em->Events);
    Event event;
    char str[40000];
    int pos = 0;
    int day, month, year;
    Member member;
    int size = pqGetSize(em->Events);
    for(int i=0; i<size; i++)
    {
        event = GetEventById(em, emGetNextEventId(Events, em->Date));
        if (event&&dateCompare(em->Date, eventGetDate(event)) <= 0)
        {
            dateGet(eventGetDate(event), &day, &month, &year);
            pos += sprintf(&str[pos], "%s,", eventGetName(event));
            pos += sprintf(&str[pos], "%d.%d.%d", day, month, year);
            member = pqGetFirst(eventGetMembers(event));
            while (member)
            {
                pos += sprintf(&str[pos], ",%s", memberGetName(member));
                member = pqGetNext(eventGetMembers(event));
            }
            pos += sprintf(&str[pos], "\n");
        }
        pqRemoveElement(Events, event);
    }
    FILE* fp = fopen(file_name, "w");
    if (pos != 0)
    {
        fprintf(fp, "%s", str);
    }
    fclose(fp);
    pqDestroy(Events);
}

int emGetNextMemberId(EventManager em, PriorityQueue Members)
{
    Member member = pqGetFirst(Members);
    int max = 0, max_id = memberGetId(member);
    while (member)
    {
        int tmp = getEventsAmountByMember(em, member);
        if (tmp > max)
        {
            max = tmp;
            max_id = memberGetId(member);
        }
        member = pqGetNext(Members);
    }
    return max_id;
}

void emPrintAllResponsibleMembers(EventManager em, const char* file_name)
{
    Member member =NULL;
    PriorityQueue Members = pqCopy(em->Members);
    char str[4000];
    int pos = 0, count = 0;
    int size = pqGetSize(em->Members);
    for (int i = 0; i < size; i++)
    {
        member = GetMemberById(em , emGetNextMemberId(em, Members));
        
        if (member)
        {
            count = getEventsAmountByMember(em, member);
            if (count > 0)
            {
                pos += sprintf(&str[pos], "%s,%d\n", memberGetName(member), count);
            }
        }
        pqRemoveElement(Members, member);
    }
    FILE* fp = fopen(file_name, "w");
    if (pos != 0)
    {
        fprintf(fp, "%s", str);
    }
    fclose(fp);
    pqDestroy(Members);
}

#endif //EVENT_MANAGER_H
