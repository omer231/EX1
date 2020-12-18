#ifndef EVENT_H_
#define EVENT_H_
#include "date.h"

/** Type for defining the event */
typedef struct Event_t *Event;

/**
* destroyEvent: Deallocates an event.
*
* @param event - the Event we want to deallocate.
*/
void destroyEvent(Event event);

/**
* createEvent: Allocates a new event.
*
* @param name - the name of the event we want to create.
* @param date - the date of the event we want to create.
* @param event_id - the id of the event we want to create.
* @return
* 	NULL - if allocation failed or id is illegal.
* 	A new event in case of success.
*/
Event createEvent(char* name, Date date, int event_id);

/**
* copyEvent: Allocates a new event from provided event.
*
* @param event - the event we want to copy.
* @return
* 	NULL - if allocation failed.
* 	A new event in case of success.
*/
Event copyEvent(Event event);

/**
* copyMemberPQElement: copies a event element.
*
* @param member_element - the event element to copy.
* @return
* 	NULL - if event element is NULL.
* 	A new event element in case of success.
*/
PQElement copyEventPQElement(PQElement event_element);

/**
* copyEventPriority: Copies the event's priority.
*
* @param priority - event priority to copy.
* @return
* 	NULL - if allocation failed or @param priority is null.
* 	PQElementPriority - in case of success.
*/
PQElementPriority copyEventPriority(PQElementPriority event_priority);

/**
* freeEventElement: Deallocates the event element.
*
* @param event_element - the event element to deallocate.
*/
void freePQEvent(PQElement event_element);

/**
* freeEventElement: Deallocates the event pririty.
*
* @param priority - the event element priority to deallocate.
*/
void freePQEventPriority(PQElementPriority priority);

/**
* equalEventElements: checks if two event elements are equal.
*
* @param priority1 - the first event priority.
* @param priority2 - the second event priority.
* @return
* 	true - if the events are equal
*	false - if the events are not equal or one of them is null
*/
bool equalPQEvents(PQElement event1, PQElement event2);

/**
* compareEventElementPriorities: Compares 2 event's priorities.
*
* @param priority1 - the priority of the first event.
* @param priority2 - the priority of the second event.
* @return
* 	the diffrance in rank between the events
*/
int comparePQEventPriorities(PQElementPriority priority1, PQElementPriority priority2);

/**
* eventGetName: Get the name of the provided event.
*
* @param event - the event we want to get the name of.
* @return
* 	NULL - the event provided is null.
* 	The name of the event in case of sucess.
*/
char* eventGetName(Event event);

/**
* eventGetDate: Get the id of the provided event.
*
* @param event - the event we want to get the id of.
* @return
* 	NULL - the event provided is null.
* 	The Date of the event in case of sucess.
*/
Date eventGetDate(Event event);

/**
* eventGetId: Get the id of the provided event.
*
* @param event - the event we want to get the id of.
* @return
* 	-1 - the event is null.
* 	The id of the event in case of sucess.
*/
int eventGetId(Event event);

/**
* eventGetMembers: Get the PriorityQueue of members inside the event.
*
* @param event - the event we want to get the PriorityQueue of members from.
* @return
* 	NULL - if the provided event is null.
* 	The pointer for the event's PriorityQueue of members in case of sucess.
*/
PriorityQueue eventGetMembers(Event event);

/**
* eventSetDate: Sets a different date for the provided event.
*
* @param event - the event we want to change the date of.
*/
void eventSetDate(Event event, Date date);

#endif //EVENT_H_
