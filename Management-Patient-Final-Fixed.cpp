#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <thread>
#include <chrono>
#include <cctype>

using namespace std;

const int MAX_PATIENTS = 100;

// Definition of struct to store patient data
struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string blood;
    string phone;
    string cnic;
    string address;
    string diagnosis;
};

// Using a two-dimensional array to meet the "Multidimensional Array" requirement
// Second dimension is only 1, so we still store MAX_PATIENTS patients
Patient patients[MAX_PATIENTS][1];
int patientCount = 0;

// Function prototypes
void loadFromFile();
void saveToFile();
void addPatient();
void diagnosePatient();
void showAllPatients(int sortChoice, bool ascending);
void showPatientData();
void deletePatient();
void updatePatient();
void handleDataPatientMenu();
void handleModifyPatientDataMenu();
void handleMainMenu();
void clear();
void continueLoad();
int findPatientIndexByID(int id, int idx);
void swapPatients(int idx1, int idx2);
void sortPatientsByID(int n, bool ascending);
void sortPatientsByName(int n, bool ascending);
bool isValidName(const string& name);
bool isValidAge(const string& ageStr, int& age);
bool isValidGender(const string& gender);
bool isValidBloodType(const string& blood);
string promptValidName();
int promptValidAge();
string promptValidGender();
string promptValidBloodType();
int promptValidInt(const string& prompt);

void clear() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void continueLoad() {
    cout << "Press any key to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Recursive function to find patient index by ID
// Returns the index of the patient in the array (0..patientCount-1), or -1 if not found
int findPatientIndexByID(int id, int idx) {
    if (idx >= patientCount) return -1;
    if (patients[idx][0].id == id) return idx;
    return findPatientIndexByID(id, idx + 1);
}

// Function to swap two Patient objects
void swapPatients(int idx1, int idx2) {
    Patient temp = patients[idx1][0];
    patients[idx1][0] = patients[idx2][0];
    patients[idx2][0] = temp;
}

#include <algorithm> // for std::swap

// Recursive sort function (recursive bubble sort) based on patient ID ascending
void sortPatientsByID(int n, bool ascending) {
    if (n <= 1) return;
    for (int i = 0; i < n - 1; ++i) {
        bool condition = ascending ? (patients[i][0].id > patients[i + 1][0].id) : (patients[i][0].id < patients[i + 1][0].id);
        if (condition) {
            swapPatients(i, i + 1);
        }
    }
    sortPatientsByID(n - 1, ascending);
}

// Recursive sort function (recursive bubble sort) based on patient name ascending
void sortPatientsByName(int n, bool ascending) {
    if (n <= 1) return;
    for (int i = 0; i < n - 1; ++i) {
        bool condition = ascending ? (patients[i][0].name > patients[i + 1][0].name) : (patients[i][0].name < patients[i + 1][0].name);
        if (condition) {
            swapPatients(i, i + 1);
        }
    }
    sortPatientsByName(n - 1, ascending);
}

void loadFromFile() {
    clear();
    cout << "Loading patient data";
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    cout << "\n";
    clear();

    ifstream inFile("patients.txt");
    if (!inFile.is_open()) {
        // If the file does not exist or failed to open, just return
        return;
    }

    string line;
    patientCount = 0;

    while (getline(inFile, line) && patientCount < MAX_PATIENTS) {
        // Storage format: id|name|age|gender|blood|phone|cnic|address|diagnosis
        size_t pos = 0;
        size_t nextPos;
        string fields[9];

        for (int i = 0; i < 8; ++i) {
            nextPos = line.find('|', pos);
            if (nextPos == string::npos) {
                fields[i] = line.substr(pos);
                pos = line.length();
            } else {
                fields[i] = line.substr(pos, nextPos - pos);
                pos = nextPos + 1;
            }
        }
        // The remainder after the 8th delimiter is diagnosis
        if (pos < line.length()) {
            fields[8] = line.substr(pos);
        } else {
            fields[8] = "";
        }

        // Assign to Patient struct
        patients[patientCount][0].id = stoi(fields[0]);
        patients[patientCount][0].name = fields[1];
        patients[patientCount][0].age = stoi(fields[2]);
        patients[patientCount][0].gender = fields[3];
        patients[patientCount][0].blood = fields[4];
        patients[patientCount][0].phone = fields[5];
        patients[patientCount][0].cnic = fields[6];
        patients[patientCount][0].address = fields[7];
        patients[patientCount][0].diagnosis = fields[8];

        patientCount++;
    }

    inFile.close();
}

// Function to save patient data to "patients.txt" file before the program exits
void saveToFile() {
    clear();
    cout << "Saving patient data";
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    cout << "\n";

    ofstream outFile("patients.txt", ios::out);
    if (!outFile.is_open()) {
        clear();
        cout << "Failed to save data to file.\n";
        return;
    }

    for (int i = 0; i < patientCount; ++i) {
        Patient &p = patients[i][0];
        outFile
            << p.id << "|"
            << p.name << "|"
            << p.age << "|"
            << p.gender << "|"
            << p.blood << "|"
            << p.phone << "|"
            << p.cnic << "|"
            << p.address << "|"
            << p.diagnosis
            << "\n";
    }

    outFile.close();
}

// Helper functions for input validation
bool isValidName(const string& name) {
    for (char c : name) {
        if (isdigit(c)) return false;
    }
    return !name.empty();
}

bool isValidAge(const string& ageStr, int& age) {
    if (ageStr.empty()) return false;
    for (char c : ageStr) {
        if (!isdigit(c)) return false;
    }
    age = stoi(ageStr);
    return age >= 0;
}

bool isValidGender(const string& gender) {
    return gender == "Male" || gender == "Female" || gender == "?";
}

bool isValidBloodType(const string& blood) {
    size_t len = blood.length();
    return (len == 1 || len == 2 || len == 3);
}

string promptValidName() {
    string name;
    do {
        cout << "Enter name: ";
        getline(cin, name);
        if (!isValidName(name)) {
            clear();
            cout << "Invalid name. Please try again.\n";
        }
    } while (!isValidName(name));
    return name;
}

int promptValidAge() {
    string ageStr;
    int age;
    do {
        cout << "Enter age: ";
        getline(cin, ageStr);
        if (!isValidAge(ageStr, age)) {
            cout << "Invalid age. Please try again.\n";
        }
    } while (!isValidAge(ageStr, age));
    return age;
}

string promptValidGender() {
    string gender;
    do {
        cout << "Enter gender (Male, Female, ?): ";
        getline(cin, gender);
        if (!isValidGender(gender)) {
            cout << "Invalid gender. Please try again.\n";
        }
    } while (!isValidGender(gender));
    return gender;
}

string promptValidBloodType() {
    string blood;
    do {
        cout << "Enter blood type: ";
        getline(cin, blood);
        if (!isValidBloodType(blood)) {
            cout << "Invalid blood type. Please try again.\n";
        }
    } while (!isValidBloodType(blood));
    return blood;
}


// Helper function to safely read an integer with validation and looping
int promptValidInt(const string& prompt) {
    int value;
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        bool valid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }
        if (valid && !input.empty()) {
            try {
                value = stoi(input);
                break;
            } catch (...) {
                valid = false;
            }
        }
        if (!valid) {

            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
    return value;
}

// Modified addPatient function with looping input validation for ID and other fields
void addPatient() {
    if (patientCount >= MAX_PATIENTS) {
        clear();
        cout << "Patient capacity is full.\n";
        return;
    }

    clear();

    Patient newP;
    // Loop for valid patient ID input
    while (true) {

        newP.id = promptValidInt("Enter patient ID: ");
        if (findPatientIndexByID(newP.id, 0) != -1) {
            cout << "ID is already registered. Please enter a different ID.\n";
        } else {
            break;
        }
    }

    newP.name = promptValidName();
    newP.age = promptValidAge();
    newP.gender = promptValidGender();
    newP.blood = promptValidBloodType();

    cout << "Enter phone number: ";
    getline(cin, newP.phone);
    cout << "Enter CNIC: ";
    getline(cin, newP.cnic);
    cout << "Enter address: ";
    getline(cin, newP.address);

    newP.diagnosis = ""; // diagnosis is empty when adding patient

    patients[patientCount][0] = newP;
    patientCount++;

    saveToFile();
    clear();
    cout << "Patient successfully added.\n";
}

// Function to add/change patient diagnosis by ID
void diagnosePatient() {
    if (patientCount == 0) {
        clear();
        cout << "No patient data available.\n";
        return;
    }

    clear();

    int id;
    // Loop for valid patient ID input
    while (true) {
        cout << "Enter patient ID to diagnose: ";
        string input;
        getline(cin, input);
        bool valid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }
        if (!valid || input.empty()) {
            cout << "Invalid input. Please enter a valid numeric ID.\n";
            continue;
        }
        id = stoi(input);
        break;
    }

    int idx = findPatientIndexByID(id, 0);
    if (idx == -1) {
        clear();
        cout << "Patient with that ID not found.\n";
        return;
    }

    if (!patients[idx][0].diagnosis.empty()) {
        clear();
        cout << "Error: Patient already has a diagnosis. Please update the diagnosis through the Update Patient feature.\n";
        return;
    }

    cout << "Enter diagnosis for patient (ID " << id << "): ";
    string diag;
    getline(cin, diag);
    patients[idx][0].diagnosis = diag;

    saveToFile();
    clear();
    cout << "Diagnosis saved successfully.\n";
}

// Function to display summary of all patients (after sorting by ID)
void showAllPatients(int sortChoice, bool ascending) {
    if (patientCount == 0) {
        clear();
        cout << "No patient data available.\n";
        return;
    }

    clear();

    if (sortChoice == 1) {
        sortPatientsByID(patientCount, ascending);
    } else if (sortChoice == 2) {
        sortPatientsByName(patientCount, ascending);
    } else {
        cout << "Invalid sort choice. Defaulting to sort by ID ascending.\n";
        sortPatientsByID(patientCount, true);
    }

    cout << "List of All Patients:\n";
    cout << "====================\n";
    for (int i = 0; i < patientCount; ++i) {
        Patient &p = patients[i][0];
        cout << "ID: " << p.id << "\tName: " << p.name << "\n";
    }
    cout << "====================\n";

    continueLoad();
}

// Function to display complete data of one patient
void showPatientData() {
    if (patientCount == 0) {
        clear();
        cout << "No patient data available.\n";
        continueLoad();
        return;
    }

    clear();

    int id;
    cout << "Enter patient ID to view complete data: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int idx = findPatientIndexByID(id, 0);
    if (idx == -1) {
        clear();
        cout << "Patient with that ID not found.\n";
        continueLoad();
        return;
    }

    clear();

    Patient &p = patients[idx][0];
    cout << "Complete Patient Data (ID " << p.id << "):\n";
    cout << "------------------------------------\n";
    cout << "Name      : " << p.name << "\n";
    cout << "Age       : " << p.age << "\n";
    cout << "Gender    : " << p.gender << "\n";
    cout << "Blood Type: " << p.blood << "\n";
    cout << "Phone     : " << p.phone << "\n";
    cout << "CNIC      : " << p.cnic << "\n";
    cout << "Address   : " << p.address << "\n";
    cout << "Diagnosis : " << (p.diagnosis.empty() ? "-" : p.diagnosis) << "\n";
    cout << "------------------------------------\n";

    continueLoad();
}

// Function to delete patient data by ID
void deletePatient() {
    if (patientCount == 0) {
        clear();
        cout << "No patient data available.\n";
        return;
    }

    clear();

    int id;
    cout << "Enter patient ID to delete: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int idx = findPatientIndexByID(id, 0);
    if (idx == -1) {
        clear();
        cout << "Patient with that ID not found.\n";
        return;
    }

    // Shift data behind the deleted index
    for (int i = idx; i < patientCount - 1; ++i) {
        patients[i][0] = patients[i + 1][0];
    }
    patientCount--;
    saveToFile();
    clear();
    cout << "Patient data successfully deleted.\n";
}

// Function to update patient data by ID
void updatePatient() {
    if (patientCount == 0) {
        clear();
        cout << "No patient data available.\n";
        return;
    }

    clear();

    int id;
    cout << "Enter patient ID to update data: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int idx = findPatientIndexByID(id, 0);
    if (idx == -1) {
        clear();
        cout << "Patient with that ID not found.\n";
        return;
    }

    clear();

    Patient &p = patients[idx][0];
    cout << "Old patient data (ID " << p.id << "):\n";
    cout << "Name      : " << p.name << "\n";
    cout << "Age       : " << p.age << "\n";
    cout << "Gender    : " << p.gender << "\n";
    cout << "Blood Type: " << p.blood << "\n";
    cout << "Phone     : " << p.phone << "\n";
    cout << "CNIC      : " << p.cnic << "\n";
    cout << "Address   : " << p.address << "\n";
    cout << "Diagnosis : " << (p.diagnosis.empty() ? "-" : p.diagnosis) << "\n";

    cout << "\nEnter new data (leave blank if no change):\n";

    cout << "New name: ";
    string input;
    getline(cin, input);
    if (!input.empty()) p.name = input;

    cout << "New age: ";
    getline(cin, input);
    if (!input.empty()) p.age = stoi(input);

    cout << "New gender: ";
    getline(cin, input);
    if (!input.empty()) p.gender = input;

    cout << "New blood type: ";
    getline(cin, input);
    if (!input.empty()) p.blood = input;

    cout << "New phone number: ";
    getline(cin, input);
    if (!input.empty()) p.phone = input;

    cout << "New CNIC: ";
    getline(cin, input);
    if (!input.empty()) p.cnic = input;

    cout << "New address: ";
    getline(cin, input);
    if (!input.empty()) p.address = input;

    cout << "New diagnosis: ";
    getline(cin, input);
    if (!input.empty()) p.diagnosis = input;

    saveToFile();
    clear();
    cout << "Patient data successfully updated.\n";
}

void handleDataPatientMenu() {
    int dataChoice;
    do {
        cout << "\n========== DATA PATIENT MENU ==========\n";
        cout << "1. Diagnose Patient\n";
        cout << "2. Display All Patients\n";
        cout << "3. Show Patient Data\n";
        cout << "4. Count Patients by Diagnosis\n";
        cout << "5. Search Patients by Blood Type\n";
        cout << "6. Back to Main Menu\n";
        cout << "Your choice (1-6): ";
        cin >> dataChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (dataChoice) {
            case 1:
                diagnosePatient();
                break;
            case 2:
                {
                    int sortChoice;
                    int orderChoice;
                    clear();
                    cout << "Display All Patients - Sort by:\n";
                    cout << "1. ID\n";
                    cout << "2. Name\n";
                    cout << "Enter choice (1-2): ";
                    cin >> sortChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    clear();
                    cout << "Order:\n";
                    cout << "1. Ascending\n";
                    cout << "2. Descending\n";
                    cout << "Enter choice (1-2): ";
                    cin >> orderChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    bool ascending = (orderChoice == 1);

                    switch (sortChoice) {
                        case 1:
                        case 2:
                            showAllPatients(sortChoice, ascending);
                            break;
                        default:
                            cout << "Invalid choice. Defaulting to sort by ID ascending.\n";
                            showAllPatients(1, true);
                            break;
                    }
                    clear();
                }
                break;
            case 3:
                showPatientData();
                break;
            case 4:
                {
                    if (patientCount == 0) {
                        clear();
                        cout << "No patient data available.\n";
                        break;
                    }
                    clear();
                    cout << "Enter diagnosis to count patients: ";
                    string diagToCount;
                    getline(cin, diagToCount);
                    int count = 0;
                    for (int i = 0; i < patientCount; ++i) {
                        if (patients[i][0].diagnosis == diagToCount) {
                            count++;
                        }
                    }
                    clear();
                    cout << "Number of patients with diagnosis \"" << diagToCount << "\": " << count << "\n";
                    continueLoad();
                }
                clear();
                break;
            case 5:
                {
                    if (patientCount == 0) {
                        clear();
                        cout << "No patient data available.\n";
                        break;
                    }
                    clear();
                    cout << "Enter blood type to search patients: ";
                    string bloodTypeToSearch;
                    getline(cin, bloodTypeToSearch);
                    bool found = false;
                    cout << "Patients with blood type \"" << bloodTypeToSearch << "\":\n";
                    cout << "------------------------------------\n";
                    for (int i = 0; i < patientCount; ++i) {
                        if (patients[i][0].blood == bloodTypeToSearch) {
                            Patient &p = patients[i][0];
                            cout << "ID: " << p.id << ", Name: " << p.name << ", Age: " << p.age << "\n";
                            found = true;
                        }
                    }
                    if (!found) {
                        cout << "No patients found with blood type \"" << bloodTypeToSearch << "\".\n";
                    }
                    cout << "------------------------------------\n";
                    continueLoad();
                }
                clear();
                break;
            case 6:
                // Back to main menu
                clear();
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (dataChoice != 6);
}

void handleModifyPatientDataMenu() {
    int subChoice;
    do {
        cout << "\nModify Patient Data Menu:\n";
        cout << "1. Update Patient\n";
        cout << "2. Delete Patient\n";
        cout << "3. Back to Main Menu\n";
        cout << "Your choice (1-3): ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (subChoice) {
            case 1:
                updatePatient();
                break;
            case 2:
                deletePatient();
                break;
            case 3:
                // Back to main menu
                clear();
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    } while (subChoice != 3);
}

void handleMainMenu() {
    int choice;
    do {
        cout << "\n========== PATIENT MANAGEMENT MENU ==========\n";
        cout << "1. Add New Patient\n";
        cout << "2. Data Patient\n";
        cout << "3. Modify Patient Data\n";
        cout << "4. Save & Exit\n";
        cout << "Your choice (1-4): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                addPatient();
                break;
            case 2:
                clear();
                handleDataPatientMenu();
                break;
            case 3:
                clear();
                handleModifyPatientDataMenu();
                break;
            case 4:
                saveToFile();
                clear();
                cout << "Patient data has been saved. Program End\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 4);
}

int main() {
    // Load patient data from file when program starts
    loadFromFile();
    handleMainMenu();
    return 0;
}
