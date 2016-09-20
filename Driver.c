/******************************************************************************
  File Name:    Driver.c
  Description:  This file is part of a multiple files to create a binary tree
                that implements a symbol tree to allow variable assignment to
                the calculator. This one in particular is the driver the one
                that runs the program for the c++ files. 
 *****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <getopt.h>
#include "Driver.h"
#include "SymTab.h"

using namespace std;

#ifdef NULL
#undef NULL
#define NULL 0
#endif

istream * is = &cin;
ostream * os = &cout;

ostream & operator << (ostream & stream, const UCSDStudent & stu) {
        return stream << "name:  " << stu.name
                << " with studentnum:  " << stu.studentnum;
}

int main (int argc, char * const * argv) {
        char buffer[BUFSIZ];
        char command;
        long number;
        char option;
        
        SymTab<UCSDStudent>::Set_Debug_Off ();

        while ((option = getopt (argc, argv, "x")) != EOF) {

        switch (option) {
                case 'x': SymTab<UCSDStudent>::Set_Debug_On ();
                        break;
                }       
        }
        
        SymTab<UCSDStudent> ST( "Driver.datafile" );
        ST.Write (*os << "Initial Symbol Table:\n" );

        while (cin) {
                // if pointer is null
                if( *is == NULL ){
                    // is input from keyboard
                    if( is == &cin )
                        break;
                    // if input is from file
                    else{
                        // delete pointers
                        delete is;
                        delete os;
                        is = &cin; //reassign pointers
                        os = &cout;
                    }
                }
                command = NULL;         // reset command each time in loop
                *os << "Please enter a command ((f)ile, (i)nsert, "
                        << "(l)ookup, (r)emove, (w)rite):  ";
                *is >> command;

                switch (command) {
                // case f to read from file
                case 'f': {
                        *os << "Please enter file name for commands: ";
                        *is >> buffer;
                        // if reading from file
                        if( is != &cin )
                            delete is;
                        // reassign pointer to read from file
                        is = new ifstream( buffer );
                        // send output to garbage file
                        os = new ofstream( "/dev/null" ); 
                        break;
                        }

                case 'i': {
                        *os << "Please enter UCSD student name to insert:  ";
                        *is >> buffer;  // formatted input

                        *os << "Please enter UCSD student number:  ";
                        *is >> number;

                        UCSDStudent stu (buffer, number);

                        // create student and place in symbol table
                        ST.Insert (stu);
                        break;
                }
                case 'l': { 
                        unsigned long found;    // whether found or not

                        *os << "Please enter UCSD student name to lookup:  ";
                        *is >> buffer;  // formatted input

                        UCSDStudent stu (buffer, 0);
                        found = ST.Lookup (stu);
                        
                        if (found)
                                cout << "Student found!!!\n" << stu << "\n";
                        else
                                cout << "student " << buffer << " not there!\n";
                        break;
                        }
                case 'r': { 
                        unsigned long removed;

                        *os << "Please enter UCSD student name to remove:  ";
                        *is >> buffer;  // formatted input

                        UCSDStudent stu (buffer, 0);
                        removed = ST.Remove(stu);

                        if (removed)
                                cout << "Student removed!!!\n" << stu << "\n";
                        else
                                cout << "student " << buffer << " not there!\n";
                        break;
                }
                case 'w':
                        ST.Write (*os << "The Symbol Table contains:\n");       
                }
        }

        ST.Write (*os << "\nFinal Symbol Table:\n");
        // writing out cost and operations of executions
        // if operation is not 0
        if (ST.GetOperation() != 0) {
            *os << "\nCost of operations:    ";
            *os << ST.GetCost();
            *os << " tree accesses";

            *os << "\nNumber of operations:  ";
            *os << ST.GetOperation();

            *os << "\nAverage cost:          ";
            *os << (((float)(ST.GetCost()))/(ST.GetOperation()));
            *os << " tree accesses/operation\n";
        }

        else
            *os << "\nNo cost information available.\n";

}
