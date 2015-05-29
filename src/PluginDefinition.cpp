//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <iostream>
#include <windows.h>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <fstream>
#include <conio.h>
#include <string>
#include <vector>
#include <stdlib.h>
using namespace std;

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];
int rtnFlg = 0;

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
     //keyboard shortcuts for some commands
    ShortcutKey *Compile_key = new ShortcutKey;
    Compile_key->_isAlt      = false;
    Compile_key->_isCtrl     = true;
    Compile_key->_isShift    = true;
    Compile_key->_key        = 0x76; // F7 Key
    
    ShortcutKey *Run_key = new ShortcutKey;
    Run_key->_isAlt      = false;
    Run_key->_isCtrl     = true;
    Run_key->_isShift    = true;
    Run_key->_key        = 0x74; // F5 Key
    
    ShortcutKey *Java_key = new ShortcutKey;
    Java_key->_isAlt      = false;
    Java_key->_isCtrl     = true;
    Java_key->_isShift    = true;
    Java_key->_key        = 0x4A; // J Key

     //customizing plugin commands
	setCommand(0, TEXT("Compile"), compile, Compile_key, false);
     setCommand(1, TEXT("Compile and Run"), compileAndRun, Run_key, false);
	setCommand(2, TEXT("Tabs Check"), tabChecker, NULL, false);
	setCommand(3, TEXT("Compare Two Files"), compareTwoFiles, NULL, false);
	setCommand(4, TEXT("Connect to Java Documentation"),connectToJavaDocs, Java_key, false);
	setCommand(5, TEXT("Connect to StackOverflow"),connectToStackOverflow, NULL, false);
	setCommand(6, TEXT("Connect to GitBash"), connectToGitBash, NULL, false);
	setCommand(7, TEXT("Beginner's Guide"), openBeginnersGuide, NULL, false);
	setCommand(8, TEXT("---"), NULL, NULL, false);
     setCommand(9, TEXT("Edit config file"), openConfigFile, NULL, false);
	setCommand(10, TEXT("About"),about, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- DEFINITION OF FUNCTIONS --//
//----------------------------------------------//


void about()
{
    ::MessageBox(NULL, TEXT("PESMU Plugin\nVersion: 1.0\nContributors: Justin, Daniel, Matt, Kashif, Nikhil"), TEXT("Notepad++"), MB_OK);
}


string getPath()
{
    wchar_t *fullPath = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    wstring fP = fullPath;
    return string(fP.begin(),fP.end());
}

string getFileName()
{
	wchar_t *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    return string(ws.begin(),ws.end());
}


string getNPPDirectory()
{
	wchar_t *directory = new wchar_t[MAX_PATH];
	SendMessageA(nppData._nppHandle, NPPM_GETNPPDIRECTORY, 0, (LPARAM)directory);
	wstring ws = directory;
    return string(ws.begin(),ws.end());
}



void compile()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	
	//get the path of the document
	string fileName, path;
	path = getPath();
	//get the name of the document
	fileName = getFileName();

	//making a log file
	string log = "log_" + fileName + ".txt" ;
	string logPath = path + "//"+ log;

	//combine the strings together to make one command
	string compileAndRedirCommand = "/k cd /d" + path + " & javac " + fileName + " 2> " + log;
	string compileCommand = "/k cd /d" + path + " & javac " + fileName;

	//Open Command Prompt and pipe program to JDK and redirect output to log file
	ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileAndRedirCommand.c_str(), NULL, SW_HIDE);
	Sleep(2000);

	//check log file, if empty, compiling success, else fail and show errors
	ifstream file(logPath);
	file.seekg(0, file.end);
	int fileLength = file.tellg();
	if (fileLength == 0) 
    {
        ::MessageBox(NULL, TEXT("Compiling Successful!"), TEXT("PESMU Compiler Message"), MB_OK);
		rtnFlg = 0;
    }
    //otherwise show message
	else if (fileLength > 0)
    {
		::MessageBox(NULL, TEXT("Compiling Not Successful!"), TEXT("PESMU Compiler Message"), MB_OK);
        ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileCommand.c_str(), NULL, SW_SHOW);
		rtnFlg = 1;
    }
	else
	{
		::MessageBox(NULL,TEXT("Could not compile. Either JDK is not installed, or the path provided is incorrect."), TEXT("PESMU Compiler Message"), MB_OK);
		rtnFlg = 0;
	}
	file.close();
	remove(logPath.c_str());
}

void compileAndRun()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	//Use Compile
	compile();
	//get parameters for run
	string fileName,path;
	path = getPath();
	fileName = getFileName();
	//make the string to run the file
	int length = fileName.length();
	length = length - 5;
	fileName.erase(fileName.begin()+length,fileName.end());
	if (rtnFlg != 1)
	{
		string runCommand = "/k cd /d" + path + " & java " + fileName;
		ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", runCommand.c_str(), NULL, SW_SHOW);
	}

}

std::vector<wstring> getAllOpenFiles()
{
     vector<wstring> vps;
	vector<TCHAR*> wps;
	int n = SendMessage(nppData._nppHandle,NPPM_GETNBOPENFILES,0,ALL_OPEN_FILES);
	if( n > 0 && n <128 )
	{
		wps.resize(n);
		for( int i = 0;i < n;i++ )
		{
			wstring ws;
			ws.resize(512);
			vps.push_back(ws);
			wps[i] = (TCHAR*)vps[i].c_str();
		}
		SendMessage(nppData._nppHandle,NPPM_GETOPENFILENAMES,(LPARAM)&wps[0],n);
	}
     return vps;
}

void compareTwoFiles()
{
	vector<wstring> vps = getAllOpenFiles();

     wstring path1 = vps[0].c_str();
	wstring path2 = vps[1].c_str();
	string fileOne = string(path1.begin(),path1.end());
	string fileTwo = string(path2.begin(),path2.end());
	string compareCommand = "/k fc /n /lb200 " + fileOne + " " + fileTwo;
	
	ShellExecuteA(NULL,"open", "C:/WINDOWS/system32/cmd.exe",compareCommand.c_str(),NULL,SW_SHOW);
}


void connectToJavaDocs()
{
     string javaDocsLink = "http://docs.oracle.com/javase/tutorial/";
	ShellExecuteA(NULL,"open",javaDocsLink.c_str(),NULL,NULL,SW_SHOW);
}

void connectToStackOverflow()
{
     string stackOverflowLink = "www.stackoverflow.com";
	ShellExecuteA(NULL,"open",stackOverflowLink.c_str(),NULL,NULL,SW_SHOW);
}

void connectToGitBash()
{
     string bashPath = getNPPDirectory()+ "/plugins/Config/PESMU/config.txt";
     bashPath = string(bashPath.begin()+14,bashPath.end());
     ifstream conFilepath(bashPath);
     if(conFilepath.fail())
	{
		::MessageBox(NULL, TEXT("The config.txt file is not found or corrupt. Please replace the document."), TEXT("PESMU Plugin"), MB_OK);
		return;
	}
	string path;
	while(conFilepath.is_open())
	{
		getline(conFilepath,path);
		conFilepath.close();
	}
	ShellExecuteA(NULL,"open",path.c_str(),NULL,NULL,SW_SHOW);
}

void openBeginnersGuide()
{
     string guidePath = getNPPDirectory() + "/plugins/doc/PESMU/BeginnersGuide.pdf";
	 ShellExecuteA(NULL,"open",guidePath.c_str(),NULL,NULL,SW_SHOW);
}

void openConfigFile()
{
     string directory = getNPPDirectory();
     directory += "/notepad++.exe";
     string configFile = getNPPDirectory() + "/plugins/Config/PESMU/config.txt";
     ShellExecuteA(NULL,"open",directory.c_str(),configFile.c_str(),NULL,SW_SHOW);

     //::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_OPEN);
}

void tabChecker()
{
	//save current document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	//get the path of the document
	string fileName, path;
	path = getPath();
	//get the name of the document
	fileName = getFileName();
	ifstream inFile(path + "//" + fileName);
	string reportPath = getNPPDirectory() + "/plugins/doc/PESMU/report.txt";
	ofstream outFile(reportPath);
	outFile.clear();
	//number of tabs
	int numTabs = 0; 
	string line;
	vector<string> textLine;
	/*vector<int> lineLengths;
	int i = 0;*/
	while(!inFile.eof())
	{ 
		getline(inFile,line);
		textLine.push_back(line);
		/*lineLengths.push_back(line.length());
		if(line.length() >= 72)
			{
				outFile<<"Error: Line "<<i+1<<" in the input contains "
				<<lineLengths[i]
				<<" characters."<<endl;
				outFile<<"This exceeds the maximum permitted length of "
					"72 characters."<<endl;
			}
		i++;*/
	}
	inFile.close();
	bool tabsExist = false;
	for (int numLine = 0; numLine < textLine.size(); ++numLine)
	{
			numTabs = 0;
			string t = textLine[numLine];
			t.substr(0,t.length());

			for(int i = 0; i < textLine[numLine].length(); i++)
			{
				if(t[i] == '\t')
				{
					numTabs++;
				}
			}
			if(numTabs > 0)
			{
				tabsExist = true;
				outFile<<"Line "<<numLine+1<<" in the input contains "
					<<numTabs<<" tabs"<<endl;
			}	
	}
	outFile.close();
	if(!tabsExist)
	{
		::MessageBox(NULL, TEXT("The file contains no tabs!"), TEXT("PESMU Plugin"), MB_OK);
	}
	else
	{
		ShellExecuteA(NULL,"open",reportPath.c_str(),NULL,NULL,SW_SHOW);
	}
}