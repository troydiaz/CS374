/*
 * This file contains the definition of the interface for the movies list
 * you'll implement. Nothing should modify anything in this file.
 */

#ifndef _MOVIES_H
#define _MOVIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movies.h"
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>

/*
 * Structure used to represent a movies list.
 */
struct movies;

/********************************************************************* 
** Function: checkPrefix
** Description: Function to check if a string has the required prefix
** Parameters: 
**      char *fileName - string to represent a given file name.
** Pre-Conditions: Given a file name, check prefix and suffix.
** Post-Conditions: None
*********************************************************************/
int checkPrefix(char *fileName);

/********************************************************************* 
** Function: getLargestFile
** Description: Iterates through a working directory to find the largest file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void getLargestFile();

/********************************************************************* 
** Function: getSmallestFile
** Description: Iterates through a working directory to find the smallest file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void getSmallestFile();

/********************************************************************* 
** Function: clearInputBuffer
** Description: Clears input buffer for next user entered value.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void clearInputBuffer();

/********************************************************************* 
** Function: getSpecificFile
** Description: Searches for user specified file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void getSpecificFile();

/********************************************************************* 
** Function: createDir
** Description: Creates a new directory in the working directory.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void createDir(struct movies *head);

/********************************************************************* 
** Function: createFiles
** Description: Creates files in newly created directory. Each file is
** data about the movies' year.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
**      char *dirName - string to represent a given directory name.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void createFiles(struct movies *head, char* dirName);

/********************************************************************* 
** Function: freeList 
** Description: Function to free all nodes in a linked list.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
** Pre-Conditions: Given a linked list
** Post-Conditions: Frees nodes associated with movies LL.
*********************************************************************/
void freeList(struct movies *head);

/********************************************************************* 
** Function: createMovies
** Description: Parses current line which is comma delimited and creates
** a movies node
** Parameters: 
**      char *currLine - A string containing a line from the csv file
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a node that includes the title, year, language, and 
** rating.
*********************************************************************/
struct movies *createMovies(char *currLine);

/********************************************************************* 
** Function: processFile
** Description: Linked list of movies from each line a specified file.
** Parameters: 
**      char *filePath - A string titled movies_info.csv or any file name.
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a pointer to the start of the linked list.
*********************************************************************/
struct movies *processFile(char *filePath);

/********************************************************************* 
** Function: selectFileOptions
** Description: Options to parse file information.
**      1) getLargestFile() - Traverse directory to find largest file. Parse movie information
**          and create a new directory to create new files for each movies' year.
**      2) getSmallestFile() - Traverse directory to find smallest file. Parse movie information
**          and create a new directory to create new files for each movies' year.
**      3) getSpecificFile() - Search directory for a user specified file. Parse movie info and
**          create a new directory to create new files for each movies' year.
**      4) Exit - return to main menu.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void selectFileOptions();

/********************************************************************* 
** Function: displayOptions
** Description: Options to either select a file to process or exit the program.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void displayOptions();


#endif