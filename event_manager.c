#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include "date.h"
#include "priority_queue.h"
#include "date.c"
#include <stdio.h>


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


EventManager createEventManager(Date date)
{
    EventManager em;
    em->Date=dateCopy(date);
    return em;
}

void destroyString(char *c)
{
    while(!*c)
    {
        free(c);
    }
}

void destroyMember(Member m)
{
    destroyString(m->MemberName);
}

void destroyEvent(Event e)
{
    free(e->event_id);
    dateDestroy(e->EventDate);
    destroyString(e->EventName);
    PQ_FOREACH(int, member_id, e->member_ids)
    {
        free(member_id);
    }
}

void destroyEventManager(EventManager em)
{
    dateDestroy(em->Date);
    PQ_FOREACH(Event, e, em->Events)
    {
         destroyEvent(e);
    }
    PQ_FOREACH(Member, m, em->Members)
    {
        destroyMember(m);
    }
}

Event CreateEvent(char* name, Date date, int event_id, PriorityQueue CommissionMembers)
{
    Event e;
    e->EventName=name;
    e->EventDate=dateCopy(date);
    e->event_id=event_id;
    e->member_ids=pqCopy(CommissionMembers);
    return e;
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

EventManagerResult checkDateIdName(EventManager em, Date date, int event_id, char* event_name)
{
    PQ_FOREACH(Event, e, em->Events)
    {
        if((strcmp(e->EventName, event_name)==0)&&(dateCompare(date, e->EventDate)==0))
        {
            return EM_EVENT_ALREADY_EXISTS;
        }
    }
    return checkDateId(em, date, event_id);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id)
{
    if(!(em&&event_name&&date&&event_id))
    {
        return EM_NULL_ARGUMENT;
    }
    EventManagerResult result=checkDateIdName(em, date, event_id, event_name);
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    PQ_FOREACH(Event, e, em->Events)
    {
        if(e->event_id==event_id)
        {
            return EM_EVENT_ALREADY_EXISTS;
        }
    }
    Event e=CreateEvent(event_name, date, event_id, NULL);
    if(e==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    pqInsert(em->Events, e, e->EventDate);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id)
{
    if(!(em&&event_name&&event_name&&days&&event_id))
    {
        return EM_NULL_ARGUMENT;
    }
    PQ_FOREACH(Event, e, em->Events)
    {
        if(e->event_id==event_id)
        {
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
    }
    Date date=dateCopy(em->Date);
    while(days>0)
    {
        days--;
        dateTick(date);
    }
    EventManagerResult result=checkDateIdName(em, date, event_id, event_name);
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    return emAddEventByDate(em, event_name, date, event_id);
}

Event GetEventById(EventManager em, int event_id)
{
    Event e=pqGetFirst(em->Events);
    while(e->event_id!=event_id)
    {
        e=pqGetNext(em->Events);
    }
    if(e->event_id==event_id)
    {
        return e;
    }
    return NULL;
}

EventManagerResult emRemoveEvent(EventManager em, int event_id)
{
    if(!(em&&&event_id))
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
    destroyEvent(e);
    return EM_SUCCESS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date)
{
    if(!(em&&event_id&&new_date))
    {
        return EM_NULL_ARGUMENT;
    }
    Event e=GetEventById(em, event_id);
    if(!e)
    {
        return EM_EVENT_NOT_EXISTS;
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
    Member m;
    memcpy(m->MemberName, member_name, strlen(member_name));
    m->member_id=member_id;
    return m;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id)
{
    if(!(em&&member_name&&member_id))
    {
        return EM_NULL_ARGUMENT;
    }
    if(member_id<0)
    {
        return EM_INVALID_MEMBER_ID;
    }
    PQ_FOREACH(Member, member, em->Members)
    {
        if(member_id==member->member_id)
        {
            return EM_MEMBER_ID_ALREADY_EXISTS;
        }
    }
    Member m=CreateMember(member_name, member_id);
    if(m==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    pqInsert(em->Members, m, NULL);
    return EM_SUCCESS;
}

Member GetMemberById(EventManager em, int member_id)
{
    Member m=pqGetFirst(em->Members);
    while(m->member_id!=member_id)
    {
        m=pqGetNext(em->Members);
    }
    if(m->member_id==member_id)
    {
        return m;
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
    if(!(em&&member_id&&event_id))
    {
        return EM_NULL_ARGUMENT;
    }
    Event e=GetEventById(em, event_id);
    Member m=GetMemberById(em, member_id);
    if(e==NULL||m==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    EventManagerResult result=checkMemberEvent(member_id, event_id, e, m);
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    bool found=false;
    PQ_FOREACH(int, member_id, e->member_ids)
    {
        if(member_id==m->member_id)
        {
            found=true;
        }
    }
    if(found)
    {
        return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
    }
    pqInsert(e->member_ids, m->member_id, NULL);
    destroyEvent(e);
    destroyMember(m);
    return EM_SUCCESS;
}



EventManagerResult emRemoveMemberFromEvent (EventManager em, int member_id, int event_id)
{
    if(!(em&&member_id&&event_id))
    {
        return EM_NULL_ARGUMENT;
    }
    Event e=GetEventById(em, event_id);
    Member m=GetMemberById(em, member_id);
    if(e==NULL||m==NULL)
    {
        return EM_OUT_OF_MEMORY;
    }
    EventManagerResult result=checkMemberEvent(member_id, event_id, e, m);
    if(result!=EM_SUCCESS)
    {
        return result;
    }
    bool found=false;
    int *id;
    PQ_FOREACH(int, member_id, e->member_ids)
    {
        if(member_id==m->member_id)
        {
            found=true;
            id=m->member_id;
        }
    }
    if(!found)
    {
        return EM_EVENT_AND_MEMBER_NOT_LINKED;
    }
    pqRemoveElement(e->member_ids,id);
    destroyEvent(e);
    destroyMember(m);
    return EM_SUCCESS;
}

EventManagerResult emTick(EventManager em, int days)
{
    if(!(em&&days))
    {
        return EM_NULL_ARGUMENT;
    }
    if(days<0)
    {
        return EM_INVALID_DATE;
    }
    while(days>0)
    {
        days--;
        dateTick(em->Date);
    }
    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em)
{
    int count=0;
    PQ_FOREACH(Event, e, em->Events)
    {
        if(dateCompare(em->Date, e->EventDate)<0)
        {
            count++;
        }
    }
    return count;
}

Event copyEvent(Event e)
{
    Event event;
    event->EventDate=dateCopy(e->EventDate);
    event->event_id=e->event_id;
    event->member_ids=pqCopy(e->member_ids);
    e->EventName=strcpy(event->EventName, e->EventName);
    return event;
}

char* emGetNextEvent(EventManager em)
{
    Event e=pqGetFirst(em->Events);
    PQ_FOREACH(Event, event, em->Events)
    {
        if(dateCompare(e->EventDate, event->EventDate)>0&&dateCompare(em->Date, event->EventDate)<0)
        {
            e=copyEvent(event);
        }
    }
    destroyEvent(e);
    return e;
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
        PQ_FOREACH(int, member_id, pointer->member_ids)
        {
            s=strcat(s, ",");
            s=strcat(s, toArray(member_id));
        }
        s=strcat(s, "\n");
    }
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "%s\n", s);
    fclose(fp); 
}

int memberEventCount(EventManager em, Member m)
{
    int count=0;
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
        s=strcat(s, dateToStr(memberEventCount(em,pointer)));
        s=strcat(s, "\n");
    }
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "%s\n", s);
    fclose(fp); 
}

#endif //EVENT_MANAGER_H
