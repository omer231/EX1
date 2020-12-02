#include <stdlib.h>
#include <stdbool.h>
#include "date.h"

#define MIN_DAY 1
#define DAYS_IN_MONTH 30
#define MIN_MONTH 1
#define MONTHS_IN_YEAR 12
#define DAYS_IN_YEAR 365

/** Type for defining the date */

typedef struct Date_t 
{
    int day;
    int month;
    int year;
} *Date;

/**
* dateCreate: Allocates a new date.
*
* @param day - the day of the date.
* @param month - the month of the date.
* @param year - the year of the date.
* @return
* 	NULL - if allocation failed or date is illegal.
* 	A new Date in case of success.
*/

Date dateCreate(int day, int month, int year)
{
    if(!(day>=MIN_DAY&&day<=DAYS_IN_MONTH&&month>=MIN_MONTH&&month<=MONTHS_IN_YEAR))
    {
        return NULL;
    }
    Date d;
    d->day=day;
    d->month=month;
    d->year=year;
    return d;
}

/**
* dateDestroy: Deallocates an existing Date.
*
* @param date - Target date to be deallocated. If priority queue is NULL nothing will be done
*/
void dateDestroy(Date date)
{
    free(date);
}

/**
* dateCopy: Creates a copy of target Date.
*
* @param date - Target Date.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A Date containing the same elements as date otherwise.
*/
Date dateCopy(Date date)
{
    Date d;
    memcpy(d,date);
    return d;
}

/**
* dateGet: Returns the day, month and year of a date
*
* @param date - Target Date
* @param day - the pointer to assign to day of the date into.
* @param month - the pointer to assign to month of the date into.
* @param year - the pointer to assign to year of the date into.
*
* @return
* 	false if one of pointers is NULL.
* 	Otherwise true and the date is assigned to the pointers.
*/
bool dateGet(Date date, int* day, int* month, int* year)
{
    if(!date->day&&!date->month&&!date->year)
    {
        day=date->day;
        month=date->month;
        year=date->year;
        return true;
    }
    return false;
}

/**
* dateCompare: compares to dates and return which comes first
*
* @return
* 		A negative integer if date1 occurs first;
* 		0 if they're equal or one of the given dates is NULL;
*		A positive integer if date1 arrives after date2.
*/

int timediff(Date date1, Date date2)
{
    return (date2->year-date1->year)*DAYS_IN_YEAR+(date2->month-date1->month)*DAYS_IN_MONTH+(date2->day-date1->day);
}

int dateCompare(Date date1, Date date2)
{
    return timediff(date1, date2);
}

/**
* dateTick: increases the date by one day, if date is NULL should do nothing.
*
* @param date - Target Date
*
*/
void dateTick(Date date)
{
    if(date->day==DAYS_IN_MONTH)
    {
        date->day=MIN_DAY;
        if(date->month==MONTHS_IN_YEAR)
        {
            date->month=MIN_MONTH;
            date->year++;
        }
        else
        {
            date->month++;
        }
    }
    else
    {
        date->day++;
    }
    
}
