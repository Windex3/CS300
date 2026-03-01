#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cctype>
#include <limits>

using namespace std;

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

//Node used in the bst to store Course objects
struct TreeNode {
    Course course;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
    private:
        TreeNode* root;
    
    public:
        BinarySearchTree() : root(nullptr) {}

        void Insert(const Course& course);
        Course* Search(const string& courseNumber);
        void InOrder();
        void Clear();
        bool IsEmpty() const {
            return root == nullptr;
        }
};

//Prototypes
bool ValidatePrerequisites (const vector<Course>& courses, const set<string>& courseNumbersSeen);
bool LoadCourses(const string& fileName, BinarySearchTree& bst);
void PrintCourse (BinarySearchTree& bst, string searchCourseNumber);
void PrintAllCourses (BinarySearchTree& bst);

//Removes leading and trailing whitespace
string Trim (const string& s) {
    //If string is empty
    if (s.empty()) {
        return "";
    }

    //Finds first non whitespace character
    int front = 0;
    while (front < s.size() && isspace(static_cast<unsigned char> (s[front]))) {
        front ++;
    }

    //If the string was all whitespace
    if (front == s.size()) {
        return "";
    }

    //Finds last non whitespace character
    int back = s.size() - 1;
    while (back > front && isspace(static_cast<unsigned char> (s[back]))) {
        back--;
    }

    //Returns trimmed substring
    return s.substr(front, back - front + 1);
}

//Splits the lines from the file into tokens by a comma
vector<string> Split (string line) {
    vector<string> tokens;
    string substring;
    int begin = 0;

    //Reads through the string 
    for (int i = 0; i < line.size(); i++) {
        if (line[i] == ',') {
            //Gets the token
            substring = line.substr(begin, i - begin);
            //Adds token to vector
            tokens.push_back(substring);
            begin = i+1;
        }
    }

    //Adds final token after the last comma to vector
    substring = line.substr(begin);
    tokens.push_back(substring);

    return tokens;
}

//Reads course data from a CSV file, validates the data, and inserts the courses into a Binary Search Tree. Returns true if loading succeeds, otherwise false.
bool LoadCourses(const string& fileName, BinarySearchTree& bst) {
    bst.Clear();

    ifstream file(fileName);

    //Makes sure file opened
    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    vector<Course> parsedCourses;
    set<string> courseNumbersSeen;
    int lineNumber = 0;

    //Reads line from the CSV 
    string line;
    while (getline(file, line)) {
        lineNumber++;
        line = Trim(line);
        if (line.empty()) {
            cout << "Error: Empty line at line " << lineNumber << "." << endl;
            file.close();
            bst.Clear();
            return false;
        }
        vector<string> tokens;

        tokens = Split(line);

        //Trims tokens
        for (int i = 0; i < tokens.size(); i++) {
            tokens[i] = Trim(tokens[i]);
        }

        //Makes sure there are two parameters on each line
        if (tokens.size() < 2) {
            cout << "Error: Line " << lineNumber << " has fewer than 2 fields." << endl;
            file.close();
            bst.Clear();
            return false;
        }

        string courseNum = tokens[0];
        string title = tokens[1];

        //Makes sure there is a title and course number
        if (courseNum.empty() || title.empty()) {
            cout << "Error: Missing course number or title at line " << lineNumber << "." << endl;
            file.close();
            bst.Clear();
            return false;
        }

        //Prevents duplicate course numbers
        if (courseNumbersSeen.count(courseNum)) {
            cout << "Error: Duplicate course number " << courseNum << " at line " << lineNumber << "." << endl;
            file.close();
            bst.Clear();
            return false;
        }

        //Creates a course object for this line
        Course course;
        course.courseNumber = courseNum;
        course.courseTitle = title;

        //Adds prerequisites to prereq
        for (int j = 2; j < tokens.size(); j++) {
            if (!tokens[j].empty()) {
                course.prerequisites.push_back(tokens[j]);
            }
        }

        //Stage course for validation
        parsedCourses.push_back(course);
        courseNumbersSeen.insert(courseNum);
    }

    file.close();

    //Makes sure every prerequisite exists as a course in the file
    if (ValidatePrerequisites(parsedCourses, courseNumbersSeen) == false) {
        bst.Clear();
        return false;
    }

    //inserts into BST after validation passes
    for (int k = 0; k < parsedCourses.size(); k++) {
        bst.Insert(parsedCourses[k]);
    }

    return true;   
}

//Returns true if all prerequisites are valid
bool ValidatePrerequisites (const vector<Course>& courses, const set<string>& courseNumbersSeen) {
    for (int i = 0; i < courses.size(); i++) {
        for (int k = 0; k < courses[i].prerequisites.size(); k++) {
            string prereq = courses[i].prerequisites[k];

            if (!courseNumbersSeen.count(prereq)) {
                cout << "Error: Course " << courses[i].courseNumber << " has invalid prerequisite " << prereq << "." << endl;
                return false;
            }
        }
    }
    return true;
}

//Prints a specific course
void PrintCourse (BinarySearchTree& bst, string searchCourseNumber) {
    searchCourseNumber = Trim(searchCourseNumber);

    Course* course = bst.Search(searchCourseNumber);

    if (course == nullptr) {
        cout << "Course not found: " << searchCourseNumber << endl;
        return;
    }

    cout << course -> courseNumber << ", " << course -> courseTitle << endl;

    if (course -> prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else{
        cout << "Prerequisites:";
        for (int i = 0; i < course -> prerequisites.size(); i++) {
            cout << " " << course -> prerequisites[i];
        }
        cout << endl;
    }
}

//Prints courses alphabetically
void InOrder(TreeNode* node) {
    if (node == nullptr) {
        return;
    }

    InOrder(node -> left);
    cout << node -> course.courseNumber << ", " << node -> course.courseTitle << endl;
    InOrder(node -> right);
}

void BinarySearchTree::InOrder() {
    ::InOrder(root);
}

//Inserts a Course into the bst in alphanumeric order
void BinarySearchTree::Insert(const Course& course) {
    if (root == nullptr) {
        root = new TreeNode(course);
        return;
    }

    TreeNode* current = root;
    while (true) {
        if (course.courseNumber < current -> course.courseNumber) {
            if (current -> left == nullptr) {
                current -> left = new TreeNode(course);
                return;
            }
            current = current -> left;
        }
        else if (course.courseNumber > current -> course.courseNumber) {
            if (current -> right == nullptr) {
                current -> right = new TreeNode(course);
                return;
            }
            current = current -> right;
        }
        else {
            //Duplicates shouldnt happen
            return;
        }
    }
}

//Searches the bst for a course by course number
Course* BinarySearchTree::Search(const string& courseNumber) {
    TreeNode* current = root;
    while (current != nullptr) {
        if (courseNumber == current -> course.courseNumber) {
            return &(current -> course);
        }
        else if (courseNumber < current -> course.courseNumber) {
            current = current -> left;
        }
        else {
            current = current -> right;
        }
    }
    return nullptr;
}

//Deletes all nodes in the bst
static void ClearNode(TreeNode*& node) {
    if (node == nullptr) return;
    ClearNode(node -> left);
    ClearNode(node -> right);
    delete node;
    node = nullptr;
}

void BinarySearchTree::Clear() {
    ClearNode(root);
}

void PrintAllCourses (BinarySearchTree& bst) {
    if (bst.IsEmpty()) {
        cout << "No courses available." << endl;
        return;
    }

    bst.InOrder();
}

int main() {
    bool dataLoaded = false;
    string fileName;
    BinarySearchTree bst;
    string searchCourseNumber;

    while (true) {
        cout << "" << endl;
        cout << "Welcome to the course planner." << endl;
        cout << "1. Load Data Structure" << endl;
        cout << "2. Print Course List" << endl;
        cout << "3. Print Specific Course" << endl;
        cout << "9. Exit" << endl;
        cout << "" << endl;
        cout << "What would you like to do?";
        int choice = 0;
        cin >> choice;

        switch(choice) {
            case 1: {
                cout << "Enter file name:";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, fileName);
                fileName = Trim(fileName);

                cout << "Attempting to open: [" << fileName << "]" << endl;

                bool loadSuccess = LoadCourses(fileName, bst);

                if (loadSuccess) {
                    dataLoaded = true;
                    cout << "Data loaded successfully." << endl;
                }
                else {
                    dataLoaded = false;
                    cout << "Data loading failed." << endl;
                }
                
                break;
            }

            case 2: {
                if (dataLoaded == false) {
                    cout << "Error: Please load the data first." << endl;
                }
                else {
                    cout << "Here is a sample schedule: " << endl;
                    PrintAllCourses(bst);
                }

                break;
            }

            case 3: {
                if (dataLoaded == false) {
                    cout << "Error: Please load the data first." << endl;
                }
                else {
                    cout << "What course would you like to know about?";
                    cin >> searchCourseNumber;
                    searchCourseNumber = Trim(searchCourseNumber);
                    PrintCourse(bst, searchCourseNumber);
                }

                break;
            }

            case 9: {
                cout << "Thank you for using the course planner!" << endl;
                return 0;
            }

            default: {
                cout << choice << " is not a valid option." << endl;
                break;
            }
        }
    }
}