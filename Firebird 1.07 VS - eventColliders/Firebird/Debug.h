// Debug.h - External declaration of global 
// output stream object

//checks for debug mode to determine what to allow


//#endif

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef NDEBUG
#define MY_DEBUG 1
#define MY_PROFILE 1
#endif

#ifdef NDEBUG
#ifdef _DEBUG
#define MY_PROFILE 1
#endif
#endif

const static bool DEBUG_MODE = false;

#include <fstream>
#include <string>
using std::ofstream;
using std::endl;
using std::string;

extern ofstream debug;

void FileNotFoundMsg(const string&);
void ErrorMsg(const string&);

#endif // __DEBUG_H__