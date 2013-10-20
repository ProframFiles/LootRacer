#pragma once
#include <string>
#include <sstream>
#include "akj_typedefs.hpp"
#include <cassert>

namespace p4{

#define P4_ASSERT(cond_) {if(!(cond_)){p4::Log::Error("(%s:%s) Failed assertion \"%s\"",__FILE__,__LINE__, #cond_ ); assert(false); } }
#define P4_WARN_IF(cond_) {if(!(cond_)){p4::Log::Warn("(%s:%s) Warning: \"%s\"",__FILE__,__LINE__, #cond_ ); } } 
#define P4_TERMINATE(reason) {p4::Log::Error("(%s:%s) Execution terminated: %s",__FILE__,__LINE__, reason ); } 

class Log
{
public:
	enum eLogLevel
	{
		LOG_LEVEL_OFF = 0,
		LOG_LEVEL_CRITICAL = 1,
		LOG_LEVEL_ERROR = 2,
		LOG_LEVEL_WARN = 3,
		LOG_LEVEL_INFO = 4,
		LOG_LEVEL_DEBUG = 5,
		LOG_LEVEL_TMI = 6,
		LOG_LEVEL_NUMLEVELS
	};

	~Log(void);

	template< typename... Args>
	static void TMI( Args... args){
		GetInstance()->NewMessage(LOG_LEVEL_TMI).Write(args...);
	}

	template< typename... Args>
	static void Debug( Args... args){
		GetInstance()->NewMessage(LOG_LEVEL_DEBUG).Write(args...);
	}

	template< typename... Args>
	static void Critical(Args... args){
		GetInstance()->NewMessage(LOG_LEVEL_CRITICAL).Write(args...);
	}

	template<typename... Args>
	static void Warn( Args... args){
		GetInstance()->NewMessage(LOG_LEVEL_WARN).Write(args...);
	}

	template< typename... Args>
	static void Info( Args... args){
		GetInstance()->NewMessage(LOG_LEVEL_INFO).Write(args...);
	}

	template<typename... Args>
	static void Error(Args... args) {
		GetInstance()->NewMessage(LOG_LEVEL_ERROR).Write(args...);
	}

	Log& SetLogLevel(eLogLevel level){
		mLogLevelThreshold = level;
		return *this;
	}

	Log& SetConsoleLogLevel(eLogLevel level){
		mConsoleLevelThreshold = level;
		return *this;
	}
	
	// adapted from http://www.stroustrup.com/C++11FAQ.html#variadic-templates
	template<typename T, typename... Args>
	void Write(const char* format_string, T first_arg, Args... other_args) {
		if(mCurrentLogLevel <= mLogLevelThreshold){
			while (format_string && *format_string) {
				if (*format_string=='%' && *++format_string!='%') {	
					mStream.str(std::string());
					mStream << first_arg;
					mLogBuffer.append(mStream.str());
					Write(++format_string, other_args...);
					return;
				}
				mLogBuffer.push_back(*(format_string++));
			}
			Write("\n\t ... Error in format specifier: too many arguments");
		}
	}
	template<typename T>
	void Write(T arg){
		if(mCurrentLogLevel <= mLogLevelThreshold){
			mStream.str(std::string());
			mStream << arg << std::endl;
			mLogBuffer.append(mStream.str());
			fputs(mLogBuffer.c_str(), mFileHandle);
			if(mCurrentLogLevel <= mConsoleLevelThreshold){
				printf("%s",mLogBuffer.c_str());
			}
			fflush(mFileHandle);
		}
	}

	void Write(const char* arg){
		if(mCurrentLogLevel <= mLogLevelThreshold){
			mLogBuffer.append(arg);
			mLogBuffer.push_back('\n');
			fputs(mLogBuffer.c_str(), mFileHandle);
			if(mCurrentLogLevel <= mConsoleLevelThreshold){
				printf("%s",mLogBuffer.c_str());
			}
			fflush(mFileHandle);
		}
	}

	const char* getLabel(eLogLevel);

	Log& NewMessage(eLogLevel level){
		mCurrentLogLevel = level;
		if(mCurrentLogLevel <= mLogLevelThreshold){
			mLogBuffer.clear();
			mLogBuffer.append(getLabel(mCurrentLogLevel));
		}
		return *this;
	}
	
	static Log* GetInstance(){
		if(sInstance == NULL){
			sInstance = new Log();
		}
		return sInstance;
	}
private:
	
	Log(void);
	FILE* mFileHandle;
	std::ostringstream mStream;
	std::string mLogBuffer;
	eLogLevel mLogLevelThreshold;
	eLogLevel mConsoleLevelThreshold;
	eLogLevel mCurrentLogLevel;
	static Log* sInstance;

};
}

