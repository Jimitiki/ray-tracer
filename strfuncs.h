#ifndef STRFUNCS_H_
#define STRFUNCS_H_

#include <stdlib.h> 

/**
* Similar to strsplit, but it only returns the characters before the first delimiter instance
*
* @param char * str a string that will be split
* @param char delimiter the character on which the string will be split
*
* @return char * a string; it is malloc'd
*/
char * substr(char * str, char delimiter);

/**
* Splits a string on a given delimiter and strips line breaks
* If a string contains multiples instances of the delimiter, they are all discarded.
* 
* @param char * str a string that will be split
* @param char delimiter defines on which character the string will be split
* @param int * w_count the number of new strings is stored at w
*
* @return char ** an array of strings. Length is pointed to by w_count. Each string is malloc'd as well as the array itself
*/
char ** strsplit(char * str, char delimiter, int * w_count);

/**
* Converts a string to a double.
* A wrapper function to improve the functionality of strtod, since that function returns 0.0f when it fails. (seriously...)
*
* @param char * str a string to be parsed
* @param double * d the actual value is stored in the location pointed to by d
* @param char * e holds whatever part of str couldn't be parsed. If strtod succeeds, it will simply hold '\0'
*
* @return int 0 if it fails (if *e != '\0')
*/
int strtodouble(char * str, double * d, char * e);

#endif