#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include "priority_queue.h"
#include "event_manager.h"
#include "date.h"
#define LN10 2.3025850929940456840179914546844
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
    PriorityQueue member_ids;
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

void destroyString(char* c)
{
    while (!*c)
    {
        free(c);
    }
}

PQElement copyMemberId(PQElement m_id)
{
    return m_id;
}

PQElementPriority copyMemberIdPriority(PQElementPriority p)
{
    return p;
}

void freeMemberIdElement(PQElement m_id)
{
    free(m_id);
}

void freeMemberIdElementPriority(PQElementPriority p)
{
    free(p);
}

bool equalMemberIdElements(PQElement m_id1, PQElement m_id2)
{
    return m_id1== m_id2;
}

int compareMemberIdElementPriorities(PQElementPriority p1, PQElementPriority p2)
{
    return 1;
}

Event CreateEvent(char* name, Date date, int event_id)
{
    Event e = malloc(sizeof(*e));
    e->EventName = malloc(sizeof(e->EventName));
    strcpy(e->EventName, name);
    e->EventDate = dateCopy(date);
    e->event_id = event_id;
    e->member_ids = pqCreate(copyMemberId, freeMemberIdElement, equalMemberIdElements, copyMemberIdPriority, freeMemberIdElementPriority, compareMemberIdElementPriorities);
    return e;
}

Event copyEvent(Event e)
{
    Event event = malloc(sizeof(*e));
    event->EventName = malloc(sizeof(e->EventName));
    strcpy(event->EventName, e->EventName);
    event->EventDate = dateCopy(e->EventDate);
    event->event_id = e->event_id;
    event->member_ids= malloc(sizeof(e->member_ids));
    event->member_ids = pqCopy(e->member_ids);
    return event;
}

Member copyMember(Member m)
{
    Member member = malloc(sizeof(*m));
    member->MemberName = malloc(sizeof(m->MemberName));
    strcpy(member->MemberName, m->MemberName);
    member->member_id = m->member_id;
    return member;
}

void destroyMember(Member m)
{
    destroyString(m->MemberName);
    free(m->MemberName);
}

void destroyEvent(Event e)
{
    dateDestroy(e->EventDate);
    destroyString(e->EventName);
}

PQElement copyEvent1(PQElement e)
{
    return copyEvent(e);
}

PQElementPriority copyEventPriority(PQElementPriority p)
{
    return dateCopy(p);
}

void freePQElement(PQElement e)
{
    destroyEvent(e);
}

void freePQElementPriority(PQElementPriority p)
{
    dateDestroy(p);
}

bool equalPQElements(PQElement e1, PQElement e2)
{
    Event event1 = (Event)e1, event2 = (Event)e2;
    return event1->event_id==event2->event_id;
}

int comparePQElementPriorities(PQElementPriority p1, PQElementPriority p2)
{
    return dateCompare(p2,p1);
}

PQElement copyMember1(PQElement m)
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
    free(p);
}

bool equalMemberElements(PQElement m1, PQElement m2)
{
    Member member1 = (Member)m1, member2 = (Member)m2;
    return member1->member_id == member2->member_id;
}

int compareMemberElementPriorities(PQElementPriority p1, PQElementPriority p2)
{
    return 2;
}

EventManager createEventManager(Date date)
{
    EventManager em=malloc(sizeof(*em));
    em->Date=dateCopy(date);
    em->Events = pqCreate(copyEvent1, freePQElement, equalPQElements, copyEventPriority, freePQElementPriority, comparePQElementPriorities);
    em->Members = pqCreate(copyMember1, freeMemberElement, equalMemberElements, copyMemberPriority, freeMemberElementPriority, compareMemberElementPriorities);
    return em;
}

void destroyEventManager(EventManager em)
{
    if (em)
    {
        dateDestroy(em->Date);

    }
    /*
    PQ_FOREACH(Event, e, em->Events)
    {
         destroyEvent(e);
    }
    PQ_FOREACH(Member, m, em->Members)
    {
        destroyMember(m);
    }
    */
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

int equal(const char* s1, const char* s2)
{
    if (!s1 || !s2)
    {
        return 0;
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
            if (equal(event->EventName, event_name) == 1 && (dateCompare(date, event->EventDate) == 0))
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
    pqInsert(em->Events, e, e->EventDate);
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
        if (event->event_id == event_id)
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
    e->EventDate=dateCopy(new_date);
    return EM_SUCCESS;
}

Member CreateMember(char* member_name, int member_id)
{
    Member m = malloc(sizeof(*m));
    m->MemberName = malloc(sizeof(m->MemberName));
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
    return EM_SUCCESS;
}

Member GetMemberById(EventManager em, int member_id)
{
    Member m=pqGetFirst(em->Members);
    while(m->member_id!=member_id)
    {
        if(m->member_id==member_id)
        {
            return m;
        }
        m=pqGetNext(em->Members);
    }
    
    return NULL;
}

EventManagerResult checkMemberEvent(int member_id, int event_id, Event e, Member m)
{
    if(member_id<0)
    {
        return EM_INVALID_EVENT_ID;
    }
    if(event_id<0)
    {
        return EM_INVALID_MEMBER_ID;
    }
    if(!e)
    {
        return EM_EVENT_NOT_EXISTS;
    }
    if(!m)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }
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
    Event e=GetEventById(em, event_id);
    if (!e)
    {
        return EM_EVENT_ID_NOT_EXISTS;
    }
    int *m_id = pqGetFirst(e->member_ids);
    int* tmp = &member_id;
    while (m_id)
    {
        if (*m_id == member_id)
        {
            return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
        }
        m_id = pqGetNext(e->member_ids);
    }
    pqInsert(e->member_ids, tmp, tmp);
    return EM_SUCCESS;
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
    Member member = pqGetFirst(em->Members);
    while (member)
    {
        if (member->member_id == member_id)
        {
            found = true;
        }
        member = pqGetNext(em->Members);
    }
    if (!found)
    {
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    int* m_id = pqGetFirst(e->member_ids);
    while (m_id)
    {
        if (*m_id == member_id)
        {
            pqRemoveElement(e->member_ids, &member_id);
            return EM_SUCCESS;
        }
        m_id = pqGetNext(e->member_ids);
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

char* emGetNextEvent(EventManager em)
{
    if (!em)
    {
        return NULL;
    }
    Event event = pqGetFirst(em->Events), e=copyEvent(event);
    while(event)
    {
        if (dateCompare(e->EventDate, event->EventDate) > 0)
        {
            e = copyEvent(event);
        }
        event = pqGetNext(em->Events);
    }
    return e->EventName;
   
}

double ln(double x)
{
    double old_sum = 0.0;
    double xmlxpl = (x - 1) / (x + 1);
    double xmlxpl_2 = xmlxpl * xmlxpl;
    double denom = 1.0;
    double frac = xmlxpl;
    double term = frac;
    double sum = term;

    while (sum != old_sum)
    {
        old_sum = sum;
        denom += 2.0;
        frac *= xmlxpl_2;
        sum += frac / denom;
    }
    return 2.0 * sum;
}

double log10(double x) {
    return ln(x) / LN10;
}

char* toArray(int number)
{
    int n = log10(number) + 1;
    int i;
    char* numberArray = calloc(n, sizeof(char));
    for (i = n - 1; i >= 0; --i, number /= 10)
    {
        numberArray[i] = (number % 10) + '0';
    }
    return numberArray;
}

char* dateToStr(Date date)
{
    char* buffer = malloc(sizeof(char) * 10);
    //sprintf(buffer, "%s%s%s", toArray(date->day), toArray(date->month), toArray(date->year));
    return buffer;
}

void emPrintAllEvents(EventManager em, const char* file_name)
{
    Event pointer=pqGetFirst(em->Events);
    char *s;
    //order the events by date
    while(!pointer)
    {
        pointer=pqGetNext(em->Events);
        s=strcat(s, pointer->EventName);
        s=strcat(s, ",");
        s=strcat(s, dateToStr(pointer->EventDate));
        /*
        PQ_FOREACH(int, member_id, pointer->member_ids)
        {
            s=strcat(s, ",");
            s=strcat(s, toArray(member_id));
        }
        */
        s=strcat(s, "\n");
    }
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "%s\n", s);
    fclose(fp); 
}

int memberEventCount(EventManager em, Member m)
{
    int count=0;
    /*
    PQ_FOREACH(Event, e, em->Events)
    {
        PQ_FOREACH(int, member_id, e->member_ids)
        {
            if(member_id==m->member_id)
            {
                count++;
            }
        }
    }
    */
    return count;
}

void emPrintAllResponsibleMembers(EventManager em, const char* file_name)
{
    Member pointer=pqGetFirst(em->Members);
    char *s;
    //order members by no. of events
    while(!pointer)
    {
        pointer=pqGetNext(em->Members);
        s=strcat(s, pointer->MemberName);
        s=strcat(s, ",");
        s=strcat(s, toArray(memberEventCount(em,pointer)));
        s=strcat(s, "\n");
    }
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "%s\n", s);
    fclose(fp); 
}

#endif //EVENT_MANAGER_H
