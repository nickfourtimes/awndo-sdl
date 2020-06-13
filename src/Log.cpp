#include <iostream>

#include "common.h"
#include "Log.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Log::Log() {

}


Log::~Log() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Log* Log::Instance() {
	static Log mInstance;
	return &mInstance;
}


int Log::InitLog(const char* filename) {
	fout.open(filename);
	if (!fout.good()) {
		return RETURN_ERROR;
	}

	this->LogMsg("LOG: Log file initialised!\n");

	return RETURN_SUCCESS;
}


void Log::LogMsg(const char* msg) {
	fout << msg;
	fout.flush();
}


void Log::CloseLog() {
	LogMsg("LOG: Log file closing.\n");
	fout.close();
}
