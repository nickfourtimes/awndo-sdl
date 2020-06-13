#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <string>

using namespace std;


class Log {
private:
	ofstream fout;

	Log();	// singleton

protected:
public:
	static Log* Instance();
	~Log();

	/** Open and set up the log file for writing */
	int InitLog(const char*);

	/** Log a given message to the log file */
	void LogMsg(const char*);

	/** Flush everything to file and close the log */
	void CloseLog();
};

#endif /*LOGGER_H_*/
