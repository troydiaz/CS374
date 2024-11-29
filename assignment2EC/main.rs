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

use std::fs::{self, File};
use std::io::{self, BufRead, Write};
use std::os::unix::fs::PermissionsExt;
use std::path::{Path, PathBuf};

struct movies {
    title: String,
    year: i32,
    language: String,
    rating: f64,
    next: Option<Box<movies>>,
}

/********************************************************************* 
** Function: checkPrefix
** Description: Function to check if a string has the required prefix
** Parameters: 
**      &str fileName - string to represent a given file name.
** Pre-Conditions: Given a file name, check prefix and suffix.
** Post-Conditions: None
*********************************************************************/
fn checkPrefix(fileName: &str) -> bool {
    fileName.starts_with("movies_")
}

/********************************************************************* 
** Function: getLargestFile
** Description: Iterates through a working directory to find the largest file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn getLargestFile() -> Option<PathBuf> {
    let mut largestFile = None;
    let mut maxSize = 0;

    // Iterate through each file in current directory
    for entry in fs::read_dir(".").unwrap() {
        if let Ok(entry) = entry {
            let fileName = entry.file_name();
            let fileNameStr = fileName.to_str().unwrap();
            
            // If prefix matches
            if checkPrefix(fileNameStr) {
                if let Ok(metadata) = entry.metadata() {
                    let size = metadata.len();
                    // Update the largest file 
                    if size > maxSize {
                        maxSize = size;
                        largestFile = Some(entry.path());
                    }
                }
            }
        }
    }

    if let Some(filePath) = &largestFile {
        println!("The file name: {}", filePath.display());
        println!("The size in bytes: {}\n", maxSize);
    } else {
        println!("No files were found!\n");
    }

    largestFile
}

/********************************************************************* 
** Function: getSmallestFile
** Description: Iterates through a working directory to find the smallest file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn getSmallestFile() -> Option<PathBuf> {
    let mut smallestFile = None;
    let mut minSize = u64::MAX;

    // Iterate through all files in current directory
    for entry in fs::read_dir(".").unwrap() {
        if let Ok(entry) = entry {
            let fileName = entry.file_name();
            let fileNameStr = fileName.to_str().unwrap();

            // If prefix matches
            if checkPrefix(fileNameStr) {
                if let Ok(metadata) = entry.metadata() {
                    let size = metadata.len();
                    // Update smallest file
                    if size < minSize {
                        minSize = size;
                        smallestFile = Some(entry.path());
                    }
                }
            }
        }
    }

    if let Some(filePath) = &smallestFile {
        println!("The file name: {}", filePath.display());
        println!("The size in bytes: {}\n", minSize);
    } else {
        println!("No files were found!\n");
    }

    smallestFile
}

/********************************************************************* 
** Function: clearInputBuffer
** Description: Clears input buffer for next user entered value.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn clearInputBuffer() {
    let _ = io::stdin().lock().lines().next();
}

/********************************************************************* 
** Function: getSpecificFile
** Description: Searches for user specified file.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: Returns path to specified file.
*********************************************************************/
fn getSpecificFile() -> Option<PathBuf> {
    println!("Enter the complete file name: ");
    let mut fileName = String::new();
    io::stdin().read_line(&mut fileName).unwrap();
    let fileName = fileName.trim();

    let filePath = Path::new(fileName);

    // Return file path if it exists
    if filePath.exists() {
        println!("Now processing the chosen file named {}", fileName);
        Some(filePath.to_path_buf())
    // Return none if doesn't
    } else {
        println!("The file {} was not found. Try again.\n", fileName);
        None
    }
}

/********************************************************************* 
** Function: createDir
** Description: Creates a new directory in the working directory.
** Parameters: 
**      Vec<movies> head - Pointer to the start of the list.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn createDir(head: Vec<movies>) {
    let dirName = String::from("diaztr.movies.12345");
    if fs::create_dir(&dirName).is_ok() {
        println!("Created directory with name {}\n", dirName);
        createFiles(head, &dirName);
    } else {
        println!("Error: directory with that name already exists!\n");
    }
}

/********************************************************************* 
** Function: createFiles
** Description: Creates files in newly created directory. Each file is
** data about the movies' year.
** Parameters: 
**      Vec<movies> head - Pointer to the start of the list.
**      &str dirName - string to represent a given directory name.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn createFiles(movies: Vec<movies>, dirName: &str) {
    for movie in movies {
        // Create files with year as title
        let filePath = format!("{}/{}.txt", dirName, movie.year);
        if let Ok(mut file) = File::create(&filePath) {
            // write titles to file
            writeln!(file, "{}", movie.title).unwrap();
            // set permissions to be owner-modify only, group read only
            let permissions = fs::Permissions::from_mode(0o640);
            fs::set_permissions(&filePath, permissions).unwrap();
        } else {
            println!("Error opening file: {}", filePath);
        }
    }
}

/********************************************************************* 
** Function: processFile
** Description: Linked list of movies from each line a specified file.
** Parameters: 
**      &Path filePath - A string titled movies_info.csv or any file name.
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a pointer to the start of the linked list.
*********************************************************************/
fn processFile(filePath: &Path) -> Vec<movies> {
    // Create list of movie data
    let mut head = Vec::new();
    if let Ok(file) = File::open(filePath) {
        let reader = io::BufReader::new(file);
        for line in reader.lines() {
            if let Ok(line) = line {
                let fields: Vec<&str> = line.split(',').collect();
                if fields.len() == 4 {
                    // Fill each movie fields
                    let title = fields[0].to_string();
                    let year = fields[1].parse().unwrap_or(0);
                    let language = fields[2].to_string();
                    let rating = fields[3].parse().unwrap_or(0.0);
                    head.push(movies {
                        title,
                        year,
                        language,
                        rating,
                        next: None,
                    });
                }
            }
        }
    }
    // Return pointer to list of movies
    head
}

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
fn selectFileOptions() {
    let mut choice = 0;
    while choice != 4 {
        println!("Enter 1 to pick the largest file");
        println!("Enter 2 to pick the smallest file");
        println!("Enter 3 to specify the name of a file");
        println!("Enter 4 to go back to the main menu\n");

        print!("Your choice: ");
        io::stdout().flush().unwrap(); 
        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        choice = input.trim().parse().unwrap_or(0);

        match choice {
            1 => {
                if let Some(filePath) = getLargestFile() {
                    let head = processFile(&filePath);
                    createDir(head);
                }
            }
            2 => {
                if let Some(filePath) = getSmallestFile() {
                    let head = processFile(&filePath);
                    createDir(head);
                }
            }
            3 => {
                if let Some(filePath) = getSpecificFile() {
                    let head = processFile(&filePath);
                    createDir(head);
                }
            }
            4 => break,
            _ => println!("Invalid choice. Choose between 1-4."),
        }
    }
}

/********************************************************************* 
** Function: displayOptions
** Description: Options to either select a file to process or exit the program.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
fn displayOptions() {
    let mut choice = 0;
    while choice != 2 {
        println!("1. Select file to process");
        println!("2. Exit the program\n");

        print!("Your choice: "); 
        io::stdout().flush().unwrap(); 
        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        choice = input.trim().parse().unwrap_or(0);

        match choice {
            1 => selectFileOptions(),
            2 => println!("Goodbye!"),
            _ => println!("Invalid choice. Choose between 1-2."),
        }
    }
}

fn main() {
    displayOptions();
}
