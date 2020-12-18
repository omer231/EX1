#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include "priority_queue.h"
#include "event_manager.h"
#include "date.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


typedef struct Member_t{
    char* MemberName;
    int member_id;
} *Member;

typedef struct Event_t{
    char* EventName;
    Date EventDate;
    int event_id;
    PriorityQueue Members;
} *Event;

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

Member copyMember(Member m)
{
    Member member = malloc(sizeof(*m));
    member->MemberName = malloc(sizeof(char)*(strlen(m->MemberName)+1));
    strcpy(member->MemberName, m->MemberName);
    member->member_id = m->member_id;
    return member;
}

void destroyMember(Member m)
{
    free(m->MemberName);
    free(m);
}

void destroyEvent(Event e)
{
    dateDestroy(e->EventDate);
    pqDestroy(e->Members);
    free(e->EventName);
    free(e);
}

PQElement copyMemberPQ(PQElement m)
{
    return copyMember(m);
}

PQElementPriority copyMemberPriority(PQElementPriority p)
{
    return p;
}

void freeMemberElement(PQElement m)
{
    destroyMember(m);
}

void freeMemberElementPriority(PQElementPriority p)
{
    
}

bool equalMemberElements(PQElement m1, PQElement m2)
{
    Member member1 = (Member)m1, member2 = (Member)m2;
    return member1->member_id == member2->member_id;
}

int compareMemberElementPriorities(PQElementPriority p1, PQElementPriority p2)
{
    int* m1 = p1;
    int* m2 = p2;
    return *m2-*m1;
}

Event CreateEvent(char* name, Date date, int event_id)
{
    Event e = malloc(sizeof(*e));
    e->EventName = malloc(sizeof(char)*(strlen(name)+1));
    strcpy(e->EventName, name);
    e->EventDate = dateCopy(date);
    e->event_id = event_id;
    e->Members = pqCreate(copyMemberPQ, freeMemberElement, equalMemberElements, copyMemberPriority, freeMemberElementPriority, compareMemberElementPriorities);
    return e;
}

Event copyEvent(Event e)
{
    Event event = malloc(sizeof(*e));
    event->EventName = malloc(sizeof(char)*(strlen(e->EventName)+1));
    strcpy(event->EventName, e->EventName);
    event->EventDate = dateCopy(e->EventDate);
    event->event_id = e->event_id;
    event->Members = pqCopy(e->Members);
    return event;
}

PQElement copyEventPQ(PQElement e)
{
    return copyEvent(e);
}

PQElementPriority copyEventPriority(PQElementPriority p)
{
    return p;
}

void freePQElement(PQElement e)
{
    destroyEvent(e);
}

void freePQElementPriority(PQElementPriority p)
{
   
}

bool equalPQElements(PQElement e1, PQElement e2)
{
    Event event1 = (Event)e1, event2 = (Event)e2;
    return event1->event_id==event2->event_id;
}

int comparePQElementPriorities(PQElementPriority p1, PQElementPriority p2)
{
    int* e1 = p1;
    int* e2 = p2;
    return *e2 - *e1;
}

EventManager createEventManager(Date date)
{
    EventManager em=malloc(sizeof(*em));
    em->Date=dateCopy(date);
    em->Events = pqCreate(copyEventPQ, freePQElement, equalPQElements, copyEventPriority, freePQElementPriority, comparePQElementPriorities);
    em->Members = pqCreate(copyMemberPQ, freeMemberElement, equalMemberElements, copyMemberPriority, freeMemberElementPriority, compareMemberElementPriorities);
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
            if (equal(event->EventName, event_name)&& (dateCompare(date, event->EventDate) == 0))
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
        if (event->event_id == event_id)
        {
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
        event = pqGetNext(em->Events);
    }
    Event e=CreateEvent(event_name, date, event_id);
    if(e==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    int priority = pqGetSize(em->Events);
    pqInsert(em->Events, e, &priority);
    destroyEvent(e);
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

Event GetEventById(EventManager em, int event_id)
{
    Event event = pqGetFirst(em->Events);
    while (event)
    {
        if (event->event_id == event_id&& (dateCompare(em->Date, event->EventDate) <= 0))
        {
            return event;
        }
        event = pqGetNext(em->Events);
    }
    return NULL;
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
    Event e=GetEventById(em, event_id);
    if(!e)
    {
        return EM_EVENT_NOT_EXISTS;
    }
    pqRemoveElement(em->Events, e);
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
    Event e=GetEventById(em, event_id);
    if(!e)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    EventManagerResult result=checkDateIdName(em, new_date, event_id, e->EventName);
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    dateDestroy(e->EventDate);
    e->EventDate=dateCopy(new_date);
    return EM_SUCCESS;
}

Member CreateMember(char* member_name, int member_id)
{
    Member m = malloc(sizeof(*m));
    m->MemberName = malloc(sizeof(char)*(strlen(member_name)+1));
    strcpy(m->MemberName, member_name);
    m->member_id=member_id;
    return m;
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
        if (member->member_id == member_id)
        {
            return EM_MEMBER_ID_ALREADY_EXISTS;
        }
        member = pqGetNext(em->Members);
    }
    Member m=CreateMember(member_name, member_id);
    if(!m)
    {
        return EM_OUT_OF_MEMORY;
    }
    pqInsert(em->Members, m, &member_id);
    destroyMember(m);
    return EM_SUCCESS;
}

Member GetMemberById(EventManager em, int member_id)
{
    Member m=pqGetFirst(em->Members);
    while(m)
    {
        if(m->member_id==member_id)
        {
            return m;
        }
        m=pqGetNext(em->Members);
    }
    
    return NULL;
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
    Event e=GetEventById(em, event_id);
    if (!e)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member m = pqGetFirst(e->Members);
    while (m)
    {
        if (m->member_id == member_id)
        {
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
        m = pqGetNext(e->Members);
    }
    m = pqGetFirst(em->Members);
    while (m)
    {
        if (m->member_id == member_id)
        {
            pqInsert(e->Members, m, &m->member_id);
            return EM_SUCCESS;
        }
        m = pqGetNext(em->Members);
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
    Event e = GetEventById(em, event_id);
    if (!e)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    bool found = false;
    Member m = pqGetFirst(em->Members);
    while (m)
    {
        if (m->member_id == member_id)
        {
            found = true;
        }
        m = pqGetNext(em->Members);
    }
    if (!found)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    m = pqGetFirst(e->Members);
    while (m)
    {
        if (m->member_id == member_id)
        {
            pqRemoveElement(e->Members, m);
            return EM_SUCCESS;
        }
        m = pqGetNext(e->Members);
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
        if (dateCompare(em->Date, event->EventDate) <= 0)
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
        if (pqContains(event->Members, m)&& (dateCompare(em->Date, event->EventDate) <= 0))
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
    int min = -1, min_id = event->event_id;
    bool changed = false;
    while (event)
    {
        int tmp = dateCompare(event->EventDate, date);
        if ((tmp < min || min == -1) && tmp >= 0)
        {
            min = tmp;
            min_id = event->event_id;
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
    return e->EventName;
}

void emPrintAllEvents(EventManager em, const char* file_name)
{
    PriorityQueue Events = pqCopy(em->Events);
    Event e;
    char str[40000];
    int pos = 0;
    int day, month, year;
    Member m;
    int size = pqGetSize(em->Events);
    for(int i=0; i<size; i++)
    {
        e = GetEventById(em, emGetNextEventId(Events, em->Date));
        if (e&&dateCompare(em->Date, e->EventDate) <= 0)
        {
            dateGet(e->EventDate, &day, &month, &year);
            pos += sprintf(&str[pos], "%s,", e->EventName);
            pos += sprintf(&str[pos], "%d.%d.%d", day, month, year);
            m = pqGetFirst(e->Members);
            while (m)
            {
                pos += sprintf(&str[pos], ",%s", m->MemberName);
                m = pqGetNext(e->Members);
            }
            pos += sprintf(&str[pos], "\n");
        }
        pqRemoveElement(Events, e);
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
    int max = 0, max_id = member->member_id;
    while (member)
    {
        int tmp = getEventsAmountByMember(em, member);
        if (tmp > max)
        {
            max = tmp;
            max_id = member->member_id;
        }
        member = pqGetNext(Members);
    }
    return max_id;
}

void emPrintAllResponsibleMembers(EventManager em, const char* file_name)
{
    Member m=NULL;
    PriorityQueue Members = pqCopy(em->Members);
    char str[4000];
    int pos = 0, count = 0;
    int size = pqGetSize(em->Members);
    for (int i = 0; i < size; i++)
    {
        m = GetMemberById(em , emGetNextMemberId(em, Members));
        
        if (m)
        {
            count = getEventsAmountByMember(em, m);
            if (count > 0)
            {
                pos += sprintf(&str[pos], "%s,%d\n", m->MemberName, count);
            }
        }
        pqRemoveElement(Members, m);
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
