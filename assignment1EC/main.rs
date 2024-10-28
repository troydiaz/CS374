/*********************************************************************** 
** Program Filename: main.rs
** Author: Troy Diaz
** Date: 10/27/24
** Description: Program that reads and parses movie information from a given
** file name. 
** Input and Output: 
**      1 - Shows movies released in a year
**      2 - Show highest rated movie for each year
**      3 - Show the title and year of movies from a language
**      4 - Quit program
*********************************************************************/

/*
*   INSTRUCTIONS ON HOW TO COMPILE, RUN, AND CHECK FOR MEMORY LEAKS!!!
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       rustc main.rs -o movies
*   Run the executable:
*       ./movies movies_info.csv
*/

use std::fs::File;
use std::io::{self, BufRead, BufReader, Write};
use std::path::Path;
use std::collections::HashMap;
use std::env;

#[derive(Debug)]
/* Struct for movies information */
struct Movie
{
    title: String,
    year: i32,
    language: String,
    rating: f64,
}

/********************************************************************* 
** Function: processFile
** Description: Linked list of movies from each line a specified file.
** Parameters: 
**      &str - A string titled movies_info.csv or any file name.
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a pointer to the start of the linked list.
*********************************************************************/
fn processFile(filePath: &str) -> io::Result<Vec<Movie>>{
    // Open file
    let file = File::open(filePath)?;
    let reader = BufReader::new(file);
    let mut movies = Vec::new();

    // Parse each line
    for line in reader.lines(){
        let line = line?;
        if let Some(movie) = createMovies(&line) {
            movies.push(movie);
        }
    }

    // Return vector
    Ok(movies)
}

/********************************************************************* 
** Function: createMovies
** Description: Parses current line which is comma delimited and creates
** a movies node
** Parameters: 
**      &str - A string titled movies_info.csv or any file name.
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a node that includes the title, year, language, and 
** rating.
*********************************************************************/
fn createMovies(line: &str) -> Option<Movie>{
    // Use comma delimiter
    let parts: Vec<&str> = line.split(',').collect();

    // Need at least four fields
    if parts.len() < 4{
        return None;
    }

    // Parse for each field
    Some(Movie{
        title: parts[0].to_string(),
        year: parts[1].parse().ok()?,
        language: parts[2].to_string(),
        rating: parts[3].parse().ok()?,
    })
}

/********************************************************************* 
** Function: searchMoviesbyLang
** Description: Displays all movies by their supported language.
** Parameters: 
**      struct movies *Movie - Pointer to the start of the linked list.
**      &str - User entered string for requested language.
** Pre-Conditions: Given a linked list and a string of characters.
** Post-Conditions: Displays all movies by language or lets user know
** that no movie supports that specified language.
*********************************************************************/
fn searchMoviesbyLang(movies: &[Movie], language: &str){
    let found_movies: Vec<_> = movies.iter()
        .filter(|m| m.language.contains(language))
        .collect();

    print!("\n");
    if found_movies.is_empty() {
        println!("No movies found in language: {}", language);
    } else {
        for movie in found_movies {
           println!("{} ({})", movie.title, movie.year);
        }
    }
    print!("\n");
}

/********************************************************************* 
** Function: searchMoviesinYear
** Description: Displays movies released in a specified year.
** Parameters: 
**      struct movies *Movie - Pointer to the start of the linked list.
**      int year - user entered integer for a year.
** Pre-Conditions: Given a linked list and a integer for year
** Post-Conditions: Displays all movies that were released in that year
** or lets user know that no movies were released in that year.
*********************************************************************/
fn searchMoviesinYear(movies: &[Movie], year: i32) {
    let found_movies: Vec<_> = movies.iter()
        .filter(|m| m.year == year)
        .collect();

    print!("\n");
    if found_movies.is_empty() {
        println!("No movies found in the year: {}", year);
    } else {
        for movie in found_movies {
            println!("Movie name: {}", movie.title);
        }
    }
    print!("\n");
}

/********************************************************************* 
** Function: searchHighestRatedinYear
** Description: Displays the highest rated movies in a range of years.
** Parameters: 
**      struct movies Movie - pointer to the start of the linked list.
** Pre-Conditions: Given a linked list.
** Post-Conditions: Displays the highest rated movie in a year. If there is only
** one movie in that year, display that one. If none, skip since
** no movies were released in that year.
*********************************************************************/
fn searchHighestRatedinYear(movies: &[Movie]){
    let mut highestRated: HashMap<i32, &Movie> = HashMap::new();

    for movie in movies {
        highestRated
            .entry(movie.year)
            .and_modify(|highest| if movie.rating > highest.rating { *highest = movie })
            .or_insert(movie);
    }

    print!("\n");
    for (_, movie) in &highestRated {
        println!("{} ({}) - {}", movie.title, movie.year, movie.rating);
    }
    print!("\n");
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <file_path>", args[0]);
        return;
    }
    let file_path = &args[1];

    let movies = match processFile(file_path) {
        Ok(movies) => movies,
        Err(_) => {
            eprintln!("Failed to read file: {}", file_path);
            return;
        }
    };

    loop {
        println!("1. Show movies released in the specified year");
        println!("2. Show highest-rated movie for each year");
        println!("3. Show movies by a specific language");
        println!("4. Quit");

        print!("Enter your choice: ");
        io::stdout().flush().expect("Failed to flush stdout");

        let mut choice = String::new();
        io::stdin().read_line(&mut choice).expect("Failed to read choice");

        match choice.trim() {
            "1" => {
                print!("Enter the year: ");
                io::stdout().flush().expect("Failed to flush stdout");
                
                let mut year_str = String::new();
                io::stdin().read_line(&mut year_str).expect("Failed to read year");
                if let Ok(year) = year_str.trim().parse() {
                    searchMoviesinYear(&movies, year);
                } else {
                    println!("Invalid year input");
                }
            }
            "2" => searchHighestRatedinYear(&movies),
            "3" => {
                print!("Enter the language: ");
                io::stdout().flush().expect("Failed to flush stdout");

                let mut language = String::new();
                io::stdin().read_line(&mut language).expect("Failed to read language");
                searchMoviesbyLang(&movies, language.trim());
            }
            "4" => {
                println!("Goodbye!");
                break;
            }
            _ => println!("Invalid choice, please try again."),
        }
    }
}