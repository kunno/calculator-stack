#ifndef DRIVER_H
#define DRIVER_H

#include <string.h>
#include <iostream>
#include <cstdlib>
using namespace std;

class UCSDStudent {
/******************************************************************************
% Routine Name : UCSDStudent
% File :         Driver.h
% 
% Description :  This class contains all the necessary functions creating the
%                data needed for a ucsd student that is passed to the operation
%                to be performed on the student. 
%
% Parameters descriptions :
% 
% name               description
% ------------------ ---------------------------------------------------------
% none               N/A
******************************************************************************/

        friend ostream & operator << (ostream &, const UCSDStudent &);
        char name[8];  // name of student 
        long studentnum; // value of interest

public:
        // constructor for member function 
        UCSDStudent( char * nm, long num = 0 ) : studentnum( num ){
            strcpy( name, nm );
        }
        // copy constructor memeber function
        UCSDStudent( void ) : studentnum (0){
            strcpy( name, "" );
        }
        // constructor for member function 
        UCSDStudent( const UCSDStudent & ucsdstudent ) 
                    : studentnum( ucsdstudent.studentnum ){
                strcpy( name, ucsdstudent.name );
        }
        // operator for name
        operator const char * ( void ) const{
            return name;
        }
        // operator for equals to comparison
        long operator == ( const UCSDStudent & xxx ) const{
            return ! strcmp( name, xxx.name );
        }
        // operator for less than comparison
        long operator < ( const UCSDStudent &xxx ) const{
            return( strcmp (name, xxx.name) < 0 ) ? 1 : 0;
        }
};

#endif
