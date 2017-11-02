#ifndef FPARSER_H_
#define FPARSER_H_

#include "scene.h"

/**
* Creates a scene from a .rayTracing file
*
* @param char * file_path the location of the file
* @param scene * the output scene as described by the rayTracing file. Should not already have memory allocated
*
* @return int 0 if it fails, positive number if it succeeds
*/
int parse_file(char * file_path, scene * out_scene);

/**
* Used to check what error occured when a function fails
*
* @return char * a string containing the line number in the file that caused an error, and what specifically failed
*/
char * get_file_parse_error();

#endif
