#ifndef MEMBER_H_
#define MEMBER_H_

#include <stdbool.h>

/** Type for defining the member */
typedef struct Member_t *Member;

/**
* copyMember: Allocates a new member.
*
* @param member - the Member we want to copy.
* @return
* 	NULL - if allocation failed or id is illegal.
* 	A new Member in case of success.
*/
Member copyMember(Member m);

/**
* destroyMember: Deallocates an existing Member.
*
* @param member - Target member to be deallocated.
*/
void destroyMember(Member m);

/**
* copyMemberPriority: Copies the member's priority.
*
* @param priority - member priority to copy.
* @return
* 	NULL - if allocation failed or @param priority is null.
* 	PQElementPriority - in case of success.
*/
PQElementPriority copyMemberPriority(PQElementPriority priority);

/**
* freeMemberElement: Deallocates the member element.
*
* @param member_element - the member element to deallocate.
*/
void freeMemberElement(PQElement member_element);

/**
* freeMemberElementPriority: Deallocates member priority.
*
* @param priority - the priority to deallocate.
*/
void freeMemberElementPriority(PQElementPriority priority);

/**
* equalMemberElements: checks if two member elements are equal.
*
* @param priority1 - the first member priority.
* @param priority2 - the second member priority.
* @return
* 	true - if the members are equal
*	false - if the members are not equal or one of them is null
*/
bool equalMemberElements(PQElement member_element1, PQElement member_element2);

/**
* compareMemberElementPriorities: Compares 2 member's priorities.
*
* @param priority1 - the priority of the first member.
* @param priority2 - the priority of the second member.
* @return
* 	the diffrance in rank between the members
*/
int compareMemberElementPriorities(PQElementPriority priority1, PQElementPriority priority2);

/**
* createMember: Allocates a new member.
*
* @param member_name - the name of the member.
* @param member_id - the id of the member.
* @return
* 	NULL - if allocation failed or id is illegal.
* 	A new Member in case of success.
*/
Member createMember(char* member_name, int member_id);

/**
* copyMember: Allocates a new member.
*
* @param member - the member to copy.
* @return
* 	NULL - if allocation failed.
* 	A new Member in case of success.
*/
Member copyMember(Member member);

/**
* copyMemberPQElement: copies a memebr element.
*
* @param member_element - the memebr element to copy.
* @return
* 	NULL - if memebr element is NULL.
* 	A new Member element in case of success.
*/
PQElement copyMemberPQElement(PQElement member_element);

/**
* memberGetId: returns the id of the member.
*
* @param member - the member we would like to get the id of.
* @return
* 	NULL - if recived member is null.
* 	The member's id in case of success.
*/
int memberGetId(Member member);

/**
* memberGetName: returns the name of the member.
*
* @param member - the member we would like to get the name of.
* @return
* 	NULL - if recived member is null.
* 	The name of the Member in case of success.
*/
char* memberGetName(Member member);

#endif //DATE_H_
