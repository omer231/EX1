#ifndef MEMBER_H_
#define MEMBER_H_
#define MEMBER_NULL_ERR -1
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "priority_queue.h"

typedef struct Member_t {
    char *MemberName;
    int member_id;
} *Member;

void destroyMember(Member member) {
    free(member->MemberName);
    free(member);
}

PQElementPriority copyMemberPriority(PQElementPriority priority) {
    PQElementPriority new_priority = malloc(sizeof(new_priority));
    if (new_priority == NULL){
        return NULL;
    }
    memcpy(new_priority, priority, sizeof(PQElementPriority));
    return new_priority;
}

void freeMemberElement(PQElement member_element) {
    destroyMember(member_element);
}

void freeMemberElementPriority(PQElementPriority priority) {
    free(priority);
}

bool equalMemberElements(PQElement member_element1, PQElement member_element2) {
    Member member1 = (Member) member_element1, member2 = (Member) member_element2;
    return member1->member_id == member2->member_id;
}

int compareMemberElementPriorities(PQElementPriority priority1, PQElementPriority priority2) {
    int *member_priority1 = priority1;
    int *member_priority2 = priority2;
    return *member_priority2 - *member_priority1;
}

Member copyMember(Member member) {
    Member member_new = malloc(sizeof(*member));
    if (member == NULL) {
        return NULL;
    }
    if (member->MemberName == NULL) {
        return NULL;
    }
    member_new->MemberName = malloc(sizeof(char) * (strlen(member->MemberName) + 1));
    strcpy(member_new->MemberName, member->MemberName);
    member_new->member_id = member->member_id;
    return member_new;
}

PQElement copyMemberPQElement(PQElement member_element) {
    return copyMember(member_element);
}

Member createMember(char *member_name, int member_id) {
    Member member = malloc(sizeof(*member));
    if (member == NULL) {
        return NULL;
    }
    member->MemberName = malloc(sizeof(char) * (strlen(member_name) + 1));
    if (member->MemberName == NULL) {
        return NULL;
    }
    strcpy(member->MemberName, member_name);
    member->member_id = member_id;
    return member;
}

int memberGetId(Member member) {
    if (member) {
        return member->member_id;
    }
    return MEMBER_NULL_ERR;
}

char *memberGetName(Member member) {
    if (member) {
        return member->MemberName;
    }
    return NULL;
}

#endif //MEMBER_H_
