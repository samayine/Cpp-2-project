#include <iostream>
#include <fstream>
#include <iomanip>
#include <Windows.h>

using namespace std;

const char* filename{ "datafile.dat" };// our main file 
const char* tempFilename{ "datafile.dat.temp" };// temporary file
const char* firedFilename{ "firedfile.dat" };// to store employees that have been fired



struct TIME
{
	int day;
	int month;
	int year;
};


struct Employee
{
	char fullname[30]; 
	char position[30];
	int ID;
	int salary{};
	TIME dateOfEmployment{};
};


void displayInfo(Employee& employee)
{
	// just display information of the employee
	cout << "============================================================================\n";
	cout << "\t\t||" << setw(20) << "Full name: " << employee.fullname << '\n';
	cout << "\t\t||" << setw(20) << "ID: " << employee.ID << '\n';
	cout << "\t\t||" << setw(20) << "Position: " << employee.position << '\n';
	cout << "\t\t||" << setw(20) << "Salary: " << employee.salary << '\n';
	cout << "\t\t||" << setw(20) << "date of employment: " << " year: "
		<< employee.dateOfEmployment.year << " month: " << employee.dateOfEmployment.month
		<< " date: " << employee.dateOfEmployment.day << '\n';
}


void displayAllEmployees(int fired=0)
{
	// go through the whole file and list all the employees
	char input{};
	Employee employee;
	ifstream infile;
	cout << "********************************************************\n";
	if (fired) {
		infile.open(firedFilename, ios::in | ios::binary);
		cout << "\tLIST OF EMPLOYEES FIRED.\n";
	}
	else {
		infile.open(filename, ios::in | ios::binary);
		cout << "\tLIST OF EMPLOYEES CURRENTLY WITH THE COMPANY.\n";
	}

	cout << "********************************************************\n";
	infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	cout <<"|"<< setw(5) << "ID" << " |" << setw(20) << "Fullname" << " |" << setw(10) << "Position"
		<< " |" << setw(10)	<< "Salary" << " |" << '\n';
	cout << "_________________________________________________________\n";
	while (infile)
	{
		cout <<"|"<< setw(5) << employee.ID<<" |" << setw(20) << employee.fullname<<" |"
			<< setw(10) << employee.position<<" |" << setw(10) << employee.salary <<" |"<< '\n';
		infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	}
	cin >> input;
	infile.close();
}


int searchForEmployee(Employee& employee)
{
	// this function searches for employee
	// it returns the employee data by reference
	// and its position in the file as integer
	int position{ -1 };
	bool employeefound{ false };

	char userInput[30];
	cout << "Enter fullname or ID to search for employee.\n====>";
	cin.getline(userInput, 30, '\n');

	ifstream infile(filename, ios::in | ios::binary);
	infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	while (infile)
	{
		// compare userinput to employee name and id
		// but since employee.ID is an integer we have to convert it to char[]
		char ID[10];
		sprintf_s(ID, "%d", employee.ID);
		if ((strcmp(employee.fullname, userInput) == 0) || (strcmp(ID, userInput) == 0))
		{
			// if you found the employee get it's position in file
			employeefound = true;
			position = static_cast<int>(infile.tellg());// 
			break;
		}
		infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	}

	// if you didn't find the employee display this message
	if (!employeefound) {
		cout << "\033[31m";
		cout << "Employee record not found. Enter any key to go back.\n";
		cout << "\033[37m";

		cin >> userInput;
	}

	infile.close();

	return position;
}


void fireEmployee(int position)
{
	// to fire employee copy everything in "datafile.dat" to "datafile.dat.temp", 
	// except the data of the employee to be fired
	// then delete "datafile.dat" and rename "datafile.dat.temp" to "datafile.dat"
 
	char input{}; // this is just to handle some user inputs
	Employee employee;

	// open our main file, our temporary file and the fired employee file
	ifstream infile(filename, ios::in | ios::binary);
	ofstream outfiletemp(tempFilename, ios::app | ios::binary);
	ofstream firedFile(firedFilename, ios::app | ios::binary);

	// copy everything to the temporary file except the fired employee data 
	infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	while (infile)
	{
		if (infile.tellg() == position)
		{
			firedFile.write(reinterpret_cast<char*>(&employee), sizeof(employee));
		}
		else
		{
			outfiletemp.write(reinterpret_cast<char*>(&employee), sizeof(employee));
		}

		infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
	}
	// close all our files
	infile.close();
	outfiletemp.close();
	firedFile.close();

	// replace the old file with the temporary file.
	remove(filename);
	if (rename(tempFilename, filename) == 0)
		cout << "\033[32m";
		cout << "Employee deleted successfully. Enter any key to finish.\n";
		cout << "\033[37m";
	cin >> input;
}


void changeSalary(Employee& employee, int pos)
{
	// we have the employee structure and its position in the file
    // edit the employee structure (change its salary)
	// and put it back in the file at "pos"
	char input{};
	int newSalary{};

	cout << setw(20) << "current salary: " << employee.salary << '\n';
	cout << setw(20) << "Enter New salary: ";
	cin >> newSalary;
	employee.salary = newSalary;

	pos -= sizeof(employee);// adjust the position

	ofstream outfile(filename, ios::in | ios::out | ios::binary);
	outfile.seekp(pos, ios::beg);
	outfile.write(reinterpret_cast<const char*>(&employee), sizeof(employee));
	outfile.close();

	cout << "\033[32m";
	cout << "Salary Changed successfully. Enter any key to finish.\n";
	cout << "\033[37m";
	cin >> input;
}


void changePosition(Employee employee, int pos)
{
	// we have the employee structure and its position in the file
	// edit the employee structure (change the employee position)
	// and put it back in the file at "pos"
	char input{};

	cout << setw(20) << "current Position: " << employee.position;
	cout << setw(20) << "\nEnter New Position: ";
	cin >> employee.position;

	pos -= sizeof(employee);// adjust the position

	ofstream outfile(filename, ios::in | ios::out | ios::binary);
	outfile.seekp(pos, ios::beg);
	outfile.write(reinterpret_cast<const char*>(&employee), sizeof(employee));
	outfile.close();

	cout << "\033[32m";
	cout << "Position Changed successfully. Enter any key to finish.\n";
	cout << "\033[37m";
	cin >> input;
}


void editEmployee()
{
	char input{};
	Employee employee{};
	int position(searchForEmployee(employee));
	if (position != -1)
	{
		displayInfo(employee);
		cout << "\nEnter (1) to fire employee\n";
		cout << "Enter (2) to change position\n";
		cout << "Enter (3) to change salary\n";
		cout << "Enter any other key to finish.\n";
		cin >> input;
		switch (input)
		{
		case '1':
			fireEmployee(position);
			break;
		case '2':
			changePosition(employee, position);
			break;
		case '3':
			changeSalary(employee, position);
			break;
		default:
			break;
		}
	}
}


void displayEmployee()
{
	// go through the file and display employee information
	Employee employee{};
	int position{ searchForEmployee(employee) };

	if (position != -1)
	{
		cout << "============================================================================\n";
		cout << "\t\t||"<< setw(20) << "Full name: " << employee.fullname << '\n';
		cout << "\t\t||" << setw(20) << "ID: " << employee.ID << '\n';
		cout << "\t\t||" << setw(20) << "Position: " << employee.position << '\n';
		cout << "\t\t||" << setw(20) << "Salary: " << employee.salary << '\n';
		cout << "\t\t||" << setw(20) << "date of employment: " << " year: "
			<< employee.dateOfEmployment.year << " month: " << employee.dateOfEmployment.month
			<< " date: " << employee.dateOfEmployment.day << '\n';

		cout << "Enter any key to finish.\n";
		char input{};
		cin >> input;
	}

}


int generateID()
{
	// this function generates id by looking at the last employee in file
	// and adding 1 to the id of the last employee
	// if the file is empty then ID=1
	Employee employee;
	int ID{ 1 };
	ifstream infile(filename, ios::in | ios::binary);
	while (infile) {
		infile.read(reinterpret_cast<char*>(&employee), sizeof(employee));
		ID = employee.ID + 1;
	}
	infile.close();
	return ID;
}


void addEmployee()
{
	// take user input and create the employee structure
	char input{};
	Employee employee;
	cout << "To add new Employee.\n";
	cout << setw(30) << "Enter Full name: ";
	cin.get(employee.fullname, 30, '\n');
	cin.ignore(10000, '\n');
	cout << setw(30) << "Enter Position: ";
	cin.get(employee.position, 30, '\n');
	cin.ignore(10000, '\n');
	cout << setw(30) << "Enter Salary: ";
	cin >> employee.salary;
	cout << setw(30) << "Enter date of employement: \n" << setw(25) << "year: ";
	cin >> employee.dateOfEmployment.year;
	cout << setw(25) << "month: ";
	cin >> employee.dateOfEmployment.month;
	cout << setw(25) << "date: ";
	cin >> employee.dateOfEmployment.day;
	employee.ID = generateID();

	// open our files and store the employee
	ofstream ofile(filename, ios::app | ios::binary);
	ofile.write(reinterpret_cast<const char*>(&employee), sizeof(employee));
	ofile.close();

	cout << "\033[32m"; // green
	cout << "Employee is added. Employee ID: " << employee.ID << '\n';
	cout << "\033[37m";
	cout << "Enter (1) to add another employee. any other key to finish.\n";
	cin >> input;
	cin.ignore(100000, '\n');
	if (input == '1') addEmployee();
	
	return;
}


void mainmenu()
{
	cout << "\033[39m"; // bold cyan color
	while (true) {
		// this function displays the main menu
		// Enter 1 to add employee..etc
		// and calls other functions based on the user entry
		cout << "=========================================================\n";
		cout << "||\tA COMPANY EMPLOYMENT SYSTEM.\t\t\t||\n";
		cout << "=========================================================\n";
		cout << "||\tEnter (1) to add new employees.\t\t\t||\n";
		cout << "||\tEnter (2) to view employee record.\t\t||\n";
		cout << "||\tEnter (3) to manage existing employees.\t\t||\n";
		cout << "||\tEnter (4) to list all employees in the company.\t||\n";
		cout << "||\tEnter (5) to list fired employees.\t\t||\n";
		cout << "||\tEnter any other key to exit.\t\t\t||\n";
		char input{};
		cin >> input;
		cin.ignore(10000, '\n');
		switch (input) {
		case '1':
			addEmployee();// adds new employee
			break;
		case '2':
			displayEmployee();
			break;
		case '3':
			editEmployee();// edits existing employee
			break;
		case '4':
			displayAllEmployees();// display every employee in data, ??find someway to sort this by position??
			break;
		case '5':
			displayAllEmployees(1);
			break;
		default:
			system("cls");
			cout << "\033[31m"; // red
			cout << "Exiting Thanks for using our program.\n";
			cout << "\033[37m"; // white4
			return;
		}
		system("cls");
	}
}


int main()
{
	mainmenu();
}

/* misc. ideas
* add an intern
* add levels so it's easier to sort
* look up employee info
* date of employment
* make sure to close all files after use
* add some kind of DATE delay
* have an explanatory ppt for defense
* use colors for the whole thing
* when displaying info make it look like a real ID
* when displaying all employees, make it look like a table
* display errors in red color
*/

/*
	NAME							ID
Tariku Temesgen					UGR/7565/15
Samuel Ayine					UGR/6563/13
Tewobsta Seyoum					UGR/3422/15
Tsion Teklay					UGR/1801/15
Ruhama Muluneh					UGR/3645/15
*/