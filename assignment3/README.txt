/*
*   INSTRUCTIONS ON HOW TO COMPILE, RUN, AND CHECK FOR MEMORY LEAKS!!!
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       gcc -std=c99 -o smallsh main.c
*   Run the executable:
*       ./smallsh
*   Check for any memory leaks:
*       gcc -std=c99 -o smallsh main.c
*       valgrind ./smallsh
*       valgrind --leak-check=full ./smallsh
*   For testing:
*       chmod +x ./p3testscript
*       ./p3testscript 2>&1 
*       or
*       ./p3testscript 2>&1 | more
*       or
*       ./p3testscript > mytestresults 2>&1 
*/