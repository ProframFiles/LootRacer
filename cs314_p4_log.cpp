#include "cs314_p4_log.hpp"
#include <stdexcept>
namespace p4{

Log* p4::Log::sInstance = NULL;

Log::~Log(void)
{
	if(mFileHandle){
		fclose(mFileHandle);
	}
}

Log::Log( void )
	:mLogLevelThreshold(LOG_LEVEL_DEBUG)
	,mConsoleLevelThreshold(LOG_LEVEL_DEBUG)
	,mCurrentLogLevel(LOG_LEVEL_OFF)
	,mFileHandle(NULL)
{
	
	mFileHandle = fopen("p4_log.txt","w");
	if (!mFileHandle) 
	{
		// die, with prejudice
		throw std::runtime_error("could not open log file p4_log.txt for append");
	}
	mLogBuffer.reserve(4096);
	mStream.str(mLogBuffer);
}

const char* Log::getLabel( eLogLevel level)
{
	static const char* labels[LOG_LEVEL_NUMLEVELS] = 
		{"OFF:      "
		,"CRITICAL: "
		,"ERROR:    "
		,"WARNING:  "
		,"INFO:     "
		,"DEBUG:    "
		,"TMI:      "
	};
	return labels[level];
}

}
