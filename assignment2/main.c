/*********************************************************************** 
** Program Filename: main.c
** Author: Troy Diaz
** Date: 10/7/24
** Description: 
** Input: 
** Output: 
*********************************************************************/

// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

/*
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       make clean && make
*   Run the executable:
*       ./main movies_info.csv
*/

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_info.csv\n");
        return EXIT_FAILURE;
    }
    struct movies *list = processFile(argv[1]);

    displayMovieCount(list, argv[1]);
    
    displayOptions(list);

    freeList(list);

    return EXIT_SUCCESS;
}