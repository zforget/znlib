/**
* 要求所有的测试代码写到一个文件里
*/
#ifndef __LOG4Z_H__
#define __LOG4Z_H__

#include <stdio.h>
#include <string>
#include "zmutex.hpp"


#define L4Z_LOG_GUID "{8B91C3F3-96AA-4551-80C3-8BCBD45A724E}"
enum ZLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2,logDEBUG3, logDEBUG4};

#ifndef ZLOG_MAX_LEVEL
#define ZLOG_MAX_LEVEL logDEBUG4
#endif

std::string NowTime();
std::string ZLogLevelToString(ZLogLevel level);
ZLogLevel ZLogLevelFromString(const std::string& level);

    
class Log4z
{
public:
	inline Log4z(const char *path = NULL, ZLogLevel level = logDEBUG4)
		:mutex_(L4Z_LOG_GUID, true)
        ,f_(stderr)
        ,level_(level)
        {
            if(path != NULL){
                f_ = fopen(path, "a");
                if(f_ == NULL)
                    f_ = stderr;
            }
        }

	inline void setLogPath(const char *path)
        {
            ZScopedLock lock(&mutex_);
            if(f_ != stderr && f_ != NULL){
                fclose(f_);
                f_ = stderr;
            }
        
            if(path != NULL){
                f_ = fopen(path, "a");
                if(f_ == NULL)
                    f_ = stderr;
            }
        }
    inline void setLogLevel(ZLogLevel level)
        {
            ZScopedLock lock(&mutex_);
            level_ = level > logDEBUG4 ? level_ : level;
        }
    inline void setLogLevel(const char *level)
        {
            setLogLevel(ZLogLevelFromString(level));
        }

	inline ~Log4z()
        {
        }

	inline void WriteLog(char *format, ...)
        {
            if(level_ > ZLOG_MAX_LEVEL)
                return;
            
            va_list args;
            va_start(args, format);
            if(f_){
                fprintf(f_, "- %s %s: ", NowTime().c_str(),
                        ZLogLevelToString(level_).c_str());
                for(int i = level_ - logDEBUG; i > 0; i--)
                    fprintf(f_, "\t");
                vfprintf(f_, format, args);
                fprintf(f_, "\n");
                fflush(f_);
            } 
            va_end(args);
        }
    Log4z& operator<<(const std::string& msg)
        {
            return operator<<(msg.c_str());
        }
    Log4z& operator<<(const char *msg)
        {
            if(level_ > ZLOG_MAX_LEVEL || msg == NULL)
                return *this;
            
            if(f_){
                fprintf(f_, "- %s %s: ", NowTime().c_str(),
                        ZLogLevelToString(level_).c_str());
                for(int i = level_ - logDEBUG; i > 0; i--)
                    fprintf(f_, "\t");
                fprintf(f_, msg);
                fprintf(f_, "\n");
                fflush(f_);
            }
            return *this;
        }
    
private:
    FILE *f_;
	ZMutex mutex_;
    ZLogLevel level_;
};

inline std::string ZLogLevelToString(ZLogLevel level)
{
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

inline ZLogLevel ZLogLevelFromString(const std::string& level)
{
    if (level == "DEBUG4")
        return logDEBUG4;
    if (level == "DEBUG3")
        return logDEBUG3;
    if (level == "DEBUG2")
        return logDEBUG2;
    if (level == "DEBUG1")
        return logDEBUG1;
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    // default
    return logINFO;
}

inline std::string NowTime()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if(GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss",buffer, MAX_LEN) == 0)
        return "Error in NowTime()";
    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000); 
    return result;
}


#endif // #ifndef __LOG4Z_H__
