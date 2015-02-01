#include <windows.h>
#include <string>

using std::string;

#include "Debug.h"
#include "Application.h"

/////////////////////////////////////////////////
int WINAPI WinMain (HINSTANCE hInst, 
					HINSTANCE hPrev, LPSTR szArgs, int nShow)
{

	const string appName("Firebird");
	cApplication app;
	if(!app.Init(appName, hInst))
		return 1;

	return app.Run();
}