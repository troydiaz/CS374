/*********************************************************************** 
** Program Filename: main.c
** Author: Troy Diaz
** Date: 10/19/24
** Description: Program that reads and parses movie information from a 
** working directory. Creates a new directory and writes files about that
** files movie->year data. 
** Input and Output: 
**      1 - Select file to process.
**          1 - Pick the largest file to process.
**          2 - Pick the smallest file to process.
**          3 - Specificy the name of file to process.
**          4 - Go back to main menu.
**      2 - Quit program.
*********************************************************************/

// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
// #define _GNU_SOURCE

/*
*   INSTRUCTIONS ON HOW TO COMPILE, RUN, AND CHECK FOR MEMORY LEAKS!!!
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       make clean && make
*   Run the executable:
*       ./main
*   Check for any memory leaks:
*       make clean && make
*       valgrind --leak-check=full ./main
*   To check for directories:
*       Each directory will be named "diaztr.movies.<random_num>"
*       And in each directory there will be file(s) named "<year>.txt"
*   For testing:
*       movies_1.csv is the largest file
*       movies_2.csv is the smallest file
*       specific.csv is for testing getSpecificFile()
*   Remove junk from your directory:
*       To remove all directories/files made from this program 
*       use this command rm -rf diaztr.movies.*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movies.h"
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    
    displayOptions();

    return EXIT_SUCCESS;
}