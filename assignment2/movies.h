/*
 * This file contains the definition of the interface for the movies list
 * you'll implement. Nothing should modify anything in this file.
 */

#ifndef _MOVIES_H
#define _MOVIES_H
/*
 * Structure used to represent a movies list.
 */
struct movies;

/********************************************************************* 
** Function: searchMoviesbyLang
** Description: Displays all movies by their supported language.
** Parameters: 
**      struct movies *head - Pointer to the start of the linked list.
**      char *language - User entered string for requested language.
** Pre-Conditions: Given a linked list and a string of characters.
** Post-Conditions: Displays all movies by language or lets user know
** that no movie supports that specified language.
*********************************************************************/
void searchMoviesbyLang(struct movies *head, char* language);

/********************************************************************* 
** Function: searchMoviesinYear
** Description: Displays movies released in a specified year.
** Parameters: 
**      struct movies *head - Pointer to the start of the linked list.
**      int year - user entered integer for a year.
** Pre-Conditions: Given a linked list and a integer for year
** Post-Conditions: Displays all movies that were released in that year
** or lets user know that no movies were released in that year.
*********************************************************************/
void searchMoviesinYear(struct movies *head, int year);

/********************************************************************* 
** Function: searchHighestRatedinYear
** Description: Displays the highest rated movies in a range of years.
** Parameters: 
**      struct movies *head - pointer to the start of the linked list.
** Pre-Conditions: Given a linked list.
** Post-Conditions: Displays the highest rated movie in a year. If there is only
** one movie in that year, display that one. If none, skip since
** no movies were released in that year.
*********************************************************************/
void searchHighestRatedinYear(struct movies *head);

/********************************************************************* 
** Function: getYear
** Description: Gets user choice of year.
** Parameters: none
** Pre-Conditions: none
** Post-Conditions: Returns an integer.
*********************************************************************/
int getYear();

/********************************************************************* 
** Function: getLang
** Description: Gets user input of language.
** Parameters: none
** Pre-Conditions: none
** Post-Conditions: Returns a string.
*********************************************************************/
char* getLang();

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
** Function: goToOption
** Description: Helper function to call functions associated with the options.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
**      int choice - User entered number for options choice.
** Pre-Conditions: Given a linked list and users choice.
** Post-Conditions: Calls functions associated with users choice.
*********************************************************************/
void goToOption(struct movies *head, int choice);

/********************************************************************* 
** Function: displayOptions
** Description: Repeatedly displays the options.
** Parameters: 
**      struct movies *head - Pointer to the start of the list
** Pre-Conditions: Given a linked list.
** Post-Conditions: Displays options until user enters 4 to exit program.
*********************************************************************/
void displayOptions(struct movies *head);

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
** Function: displayMovieCount
** Description: Displays file name and movie count.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
**      char *filePath - String that contains file name
** Pre-Conditions: Given a linked list and file name.
** Post-Conditions: Displays the file name and parsed movies.
*********************************************************************/
void displayMovieCount(struct movies *head, char *filePath);

#endif