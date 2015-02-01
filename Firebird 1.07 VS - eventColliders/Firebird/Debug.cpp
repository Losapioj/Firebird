// Debug.cpp - External definition of global 
// output stream object 

#include <string>
using std::string;
#include "Debug.h"

extern ofstream debug("debug.txt");

void FileNotFoundMsg(const string& filename)
{
	debug << "File not found: " << filename 
		<< endl;
}

void ErrorMsg(const string& msg)
{
	debug << "ERROR: " << msg << endl;
}