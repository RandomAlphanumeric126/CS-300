// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Name        : BinarySearchTree.cpp
// Author      : Jonathan Blue


/********************************************************************
 *    Includes
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
//#include <sstream>
//#include <iomanip>
//#include <stdexcept>
//#include <list>
 /*
      END Includes
  ********************************************************************/




 /********************************************************************
  *    Shortcut Function Declarations
  */

void LineBreak(int lineCount = 1);
void stdPrintLine(std::string string);
void stdPrint(std::string string);

/*
 *    END Shortcut Function Declarations
 ********************************************************************/




 /********************************************************************
  *    Shortcut Function Definitions
  */

  // Prints a given number of line breaks such that
  // 1 <= lineCount <= 100
void LineBreak(int lineCount) {

    if (lineCount < 1) { lineCount = 1; }
    else if (lineCount > 100) { lineCount = 100; }

    else { std::cout << std::string(lineCount, '\n'); }

}

// Prints a string using std::cout followed by a line break
void stdPrintLine(std::string string) {

    std::cout << string << std::endl;

}

// Prints a string using std::cout without a line break
void stdPrint(std::string string) {

    std::cout << string;

}

/*
 *    END Shortcut Function Definitions
 ********************************************************************/




/********************************************************************
 *    Struct Definitions
 */

struct Course {

    std::string id;
    std::string title;

    std::vector<std::string> preReqs;

    // Prints the course's id, title, and prerequisites (if any).
    void Print() {
        std::cout << id << ", " << title << std::endl;
        std::cout << "Prerequisites: ";

        // Print the course's prerequisites
        if (not preReqs.empty()) {
        
            for (int i = 0; i < preReqs.size()-1; ++i) {
                std::cout << preReqs.at(i) << ", ";
            }
            std::cout << preReqs.back() << std::endl;
        
        }

        // Course has no prerequisites
        else { std::cout << "None" << std::endl; }

    }

};

/*    END Struct Definitions
 ********************************************************************/




 /********************************************************************
  *    Class Declarations
  */

class HashTable {

private:  
    
// Define Node structs

    struct Node {

        unsigned int  key;
        Course        course;
        Node* next;


        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Course p_course) : Node()
        {
            course = p_course;
        }

        Node(Course p_course, unsigned int p_key) : Node(p_course)
        {
            key = p_key;
        }

    };

    struct HeadNode : Node {

        Node* tail;

        HeadNode() {
            key = UINT_MAX;
            next = nullptr;
            tail = nullptr;
        }

        HeadNode(Course p_course) : HeadNode()
        {
            course = p_course;
        }

        HeadNode(Course p_course, unsigned int p_key) : HeadNode(p_course)
        {
            key = p_key;
        }

    };


    // Prvate methods

    unsigned int hash(std::string stringToHash);


    // Private members

    unsigned int           tableSize = 10;
    std::vector<HeadNode>  hashTable;

public:

    // Public methods

    std::vector<Course>   CoursesAsVector    (   );
    bool                  CheckPreReqs       (   );

    Course                Search             ( std::string         p_course       );
    void                  Parse              ( std::string         p_filePath     );
    void                  Insert             ( Course              p_course       );
    void                  PrintCoursesVector ( std::vector<Course> &coursesVector );

    // Public members
    bool                   isEmpty = true;


public:
    
    // Constructors and Destructor

    HashTable  (                     );
    HashTable  ( unsigned int p_size );
    ~HashTable (                     );

};

/*
 *    END Class Declarations
 ********************************************************************/




/********************************************************************
 *    Function Declarations
 */

void                DisplayMenu         (   );
unsigned int        GetMenuChoice       (   );
std::string         GetFilePath         (   );

Course              BuildCourseFromLine (std::string string, char delimiter = ','  );
void                DoMenuChoice        (HashTable* hashTable, unsigned int choice );

void                QuickSort           ( std::vector<Course> &courses, int startIndex, int endIndex );
int                 Partition           ( std::vector<Course> &courses, int startIndex, int endIndex );

/*
 *    END Function Declarations
 ********************************************************************/




/********************************************************************
 *    Class Definitions
 */


HashTable::HashTable() {
    hashTable.resize(tableSize);
    //std::cout << "Size 1 " << tableSize << std::endl;
}

HashTable::HashTable(unsigned int size) {
    tableSize = size;
    hashTable.resize(tableSize);
    //std::cout << "Size 2 " << tableSize << std::endl;
}

HashTable::~HashTable() {
    hashTable.erase(hashTable.begin());
}


// Parse course data from input file, line by line, and
// insert each course into the hash table
void HashTable::Parse(std::string filePath) {

    std::ifstream file;
    std::string   string;
    Course        tempCourse;


    file.open(filePath);

    stdPrint(" - Reading file " + filePath + ": ");

    // in case of failbit
    if (!file.good()) {
		file.close();
        throw std::runtime_error("FAILED\n   Unable to parse the given file.");
        return;
    }
    else {
        stdPrintLine("PASSED");
    }

    // Iterate down the file
    while (std::getline(file, string)) {

        if (string == "") { continue; }  // bypass empty lines

        // Build the course and insert into the table
        tempCourse = BuildCourseFromLine(string);
        this->Insert(tempCourse);

    }

    file.close();
}

// Insert a given course into the hash table
void   HashTable::Insert(Course course) {

    // Initialize worker vars
    unsigned int hashKey  = hash(course.id);
    HeadNode* scannerNode = &hashTable.at(hashKey);

    // No matching key exists in the hashtable; assign a head node
    if (scannerNode->key == UINT_MAX) {
        HeadNode newHead = HeadNode(course, hashKey);
        hashTable.at(hashKey) = newHead;

        // Hash table is no longer empty, set the flag (used in DoMenuChoice case 1)
        if (this->isEmpty) { this->isEmpty = false; }

    }

    // Collision occurs at hasKey index
    // Send the new node to the end of the linked list
    else {

        Node* newNode = new Node(course, hashKey);
        
        // Append the first collision
        if (scannerNode->tail == nullptr) {
            scannerNode->next = newNode;
            scannerNode->tail = newNode;
        }

        // Append further collisions
        else {
            scannerNode->tail->next = newNode;
            scannerNode->tail = newNode;
        }

    }

}


// Dump all courses to a vector
std::vector<Course> HashTable::CoursesAsVector() {

    std::vector<Course> coursesAsVector;

    // For each headnode in the hashtable
    for ( auto i: hashTable) {

        if (i.key != UINT_MAX) {

            // Append head nodes to return vector
            coursesAsVector.push_back(i.course);

            // Append collisions to return vector
            Node* collision = collision = &i;
            
            while (collision->next != nullptr) {

                collision = collision->next;
                coursesAsVector.push_back(collision->course);

            }

        }

    }

    return coursesAsVector;

}

void HashTable::PrintCoursesVector(std::vector<Course> &coursesVector) {

    for (auto i: coursesVector) {
        stdPrintLine(i.id);
    }

}


// Search the hash table for a given course id
Course HashTable::Search(std::string courseID) {

    // Declare worker vars
    Course       course;
    unsigned int tempKey     = hash(courseID);
    Node*        currentNode = &(hashTable.at(tempKey));

    // Course does not exist
    //if (currentNode == nullptr) { return course; }

    // Iterate through the HashTable
    while (currentNode != nullptr) {

        // Course found, hold on to it
        if (currentNode->course.id == courseID) {

            course = currentNode->course;
            break;

        }

        // Move through the collisions if there are any
        currentNode = currentNode->next;
    }

    // No match found
    if (course.id == "") { 

        LineBreak();
        stdPrintLine("No match found for the ID \"" + courseID + "\"");

    }


    return course;
}

// Calculate a non-negative hash number from a given string
unsigned int HashTable::hash(std::string stringToHash) {

    unsigned int key = 0;

    for (int i = 0; i < stringToHash.size(); ++i) {
        key += stringToHash[i];
    }

    key %= tableSize;

    return key;
}

// Verify that all prerequisites for each course in the table
// exist as a course object somewhere within the table
bool HashTable::CheckPreReqs() {

    Course currentCourse;
    Node*  currentNode;

    // Iterate through all headnodes in the hash table
    for (unsigned int i = 0; i < tableSize; ++i) {

        currentNode   = &hashTable.at(i);
        currentCourse = currentNode->course;

        if (currentNode->key == UINT_MAX        // Node has no course data
        or  currentCourse.preReqs.empty()) {    // OR Course has no preReqs

            continue;

        }

        // Scanning the preReqs vector and searching the table for matches
        for (int j = 0; j < currentCourse.preReqs.size(); ++j) {

            std::string reqCourse = currentCourse.preReqs.at(j);

            // Course not found
            if (Search(reqCourse).id == "") {

                LineBreak();

                stdPrintLine("Identified a missing course: " + reqCourse);
                stdPrintLine("Required for " + currentCourse.id);

                LineBreak();

                return false;
            }

        }

    }

    return true;

}

/*
 *    END Class Definitions
 ********************************************************************/




/********************************************************************
*    Function Definitions
*/

// Displays the main menu
void DisplayMenu() {

    std::string border = std::string(26, '=');

    LineBreak();

    stdPrintLine(border);
    stdPrintLine("| Main Menu:             |");
    stdPrintLine("| 1: Load a file         |");
    stdPrintLine("| 2: Print all courses   |");
    stdPrintLine("| 3: Search for a course |");
    stdPrintLine("| 9: Exit program        |");
    stdPrintLine(border);

    LineBreak();

}

// Prompt the user to input a menu selection
unsigned int GetMenuChoice() {

    unsigned int choice;
    std::string alert = "Please enter a valid number.";

    printf("What would you like to do? ");
    std::cin >> choice;

    // Clear cin of multiple keystrokes to prevent repeated calls
    std::cin.clear(); std::cin.ignore(INT_MAX, '\n');

    LineBreak();

    return choice;

}

// Prompt the user to input a file location
std::string GetFilePath() {

    std::string filePath;

    stdPrint("Enter file path: ");
    std::cin >> filePath;
    LineBreak();

    return filePath;

}

// Builds a Course object from a line of text
// @param string: A delimiter separated string containing course data
// @param delimiter: Character that separates each element. Uses comma by default.
Course BuildCourseFromLine(std::string string, char delimiter) {

    std::string tempString = "";
    int         elements = 0;
    Course      tempCourse;

    std::vector<std::string> tempVector;

    // Iterate to the end of the string
    for (int i = 0; i <= string.length(); ++i) {

        // Build each delimiter separated element
        if (string[i] == delimiter || i == string.length()) {

            // Only act if the element is not empty
            if (tempString != "") {
                tempVector.push_back(tempString);
                tempString = "";
                elements++;
            }

            continue;

        }
        tempString += string[i];

    }

    // Bad file formatting
    if (elements < 2) {
        stdPrintLine("ERROR: Improper file formatting. Please check the input file and try again.");
        return Course();
    }

    // Build Course from vector
    tempCourse.id = tempVector[0];
    tempCourse.title = tempVector[1];

    // Build the Course pre-requisistes vector
    for (int i = 2; i < elements; ++i) {
        tempCourse.preReqs.push_back(tempVector[i]);
    }

    return tempCourse;
}


// Calls the appropriate functions to perform the desired task
void DoMenuChoice(HashTable* hashTable, unsigned int choice) {

    std::string filePath;
    Course courseToSearch;
    std::vector<Course> courses;

    switch (choice)
    {
    case 1: // Load file

        /* BROKEN: Ran out of time to get this working */
        /*
        // works fine all the way through (best as I can
        // follow in the debugger) but when it returns
        // to restart the main() running loop the fresh
        // hash table created here 'disappears' and is
        // replaced by an empty one.

        if (not hashTable->isEmpty) {
            char input;

            stdPrintLine("Course data already exists.");
            stdPrintLine("Continuing will remove existing data.");
            stdPrint("Are you sure you wish to continue? (Y/N): ");
            std::cin >> input;

            if (toupper(input) == 'N') {
                stdPrintLine("Loading cancelled.");
                return;
            }

            else if (toupper(input) == 'Y') {
                delete hashTable;
                hashTable = new HashTable();
            }
        }
        */
        

        // Load and parse a given file path
        try {
            hashTable->Parse(GetFilePath());
        }

        catch (const std::exception& e) {
            stdPrintLine(e.what());
            return;
        }

        // Print an error if any prerequisites do not exist in the hash table
        stdPrint(" - Verifying prerequisites: ");
        if (hashTable->CheckPreReqs() == false) {
            stdPrintLine("FAILED");
            stdPrintLine("Check for prerequisite availablity: FAILED"    );
            stdPrint    ("Please check that the input file is formatted ");
            stdPrintLine("properly and contains all necessary courses.");

            return;
        }
        else { stdPrintLine("PASSED"); }
        

        break;


    case 2:  // Print all courses

        // Hashtable has not had any data inserted
        if (hashTable->isEmpty) { 
            stdPrintLine(" - List of courses is currently empty.");
        }
        
        // Do the thing
        else {

            stdPrintLine("Printing ID's for all courses in sorted order:");
            stdPrintLine(std::string(50, '-'));

            courses = hashTable->CoursesAsVector();
            QuickSort(courses, 0, (int)courses.size() - 1);
            hashTable->PrintCoursesVector(courses);

        }

        break;


    case 3:  // Search for a course

        // Hashtable has not had any data inserted
        if (hashTable->isEmpty) {
            stdPrintLine(" - List of courses is currently empty.");
        }

        // Do the thing
        else {

            // Prompt for user input
            stdPrint("What course do you want to know about? ");
            std::cin >> courseToSearch.id;

            LineBreak();

            // Convert input to upper case for searching (Credit: https://stackoverflow.com/a/23418474)
            transform(courseToSearch.id.begin(), courseToSearch.id.end(), courseToSearch.id.begin(), ::toupper);

            // Find the course and print its data if found
            courseToSearch = hashTable->Search(courseToSearch.id);
            if (courseToSearch.id != "") { courseToSearch.Print(); }

        }

        break;


    case 9:  // Exit program

        stdPrintLine("Thank you for using the course planner!");

        break;


    default:

        stdPrintLine("Invalid selection");

        break;
    }

}


// Sort a vector of courses into alphanumeric order by course id
void QuickSort(std::vector<Course> &courses, int startIndex, int endIndex) {

    if (startIndex < endIndex) {
        
        int pivotIndex = Partition(courses, startIndex, endIndex);

        QuickSort(courses, startIndex,     pivotIndex);
        QuickSort(courses, pivotIndex + 1, endIndex  );

    }

}


int Partition(std::vector<Course> &courses, int startIndex, int endIndex) {

    int midPoint = ( (endIndex - startIndex) / 2 ) + startIndex;
    Course pivot = courses.at(midPoint  );

    bool running = true;
    while (running) {
    
        while (courses.at(startIndex).id < pivot.id) {
            startIndex++; 
        }
        while (pivot.id < courses.at(endIndex).id) {
            endIndex--; 
        }
        if (startIndex >= endIndex) { 
            running = false; 
        }
        else {
            std::swap(courses.at(startIndex), courses.at(endIndex));
            startIndex++;
            endIndex--;
        }

    }



    return endIndex;
}


/*
 *    END Funtion Definitions
 ********************************************************************/



int main()
{
    
    bool running    = true;
    HashTable* hashTable = new HashTable();

    std::cout << "Welcome to the course planner." << std::endl;

    while (running) {

        int menuChoice = NULL;

        DisplayMenu();
        menuChoice = GetMenuChoice();

        if (menuChoice == 9) { running = false; } // Exit program

        DoMenuChoice(hashTable, menuChoice);
    }


    return 0;
}

