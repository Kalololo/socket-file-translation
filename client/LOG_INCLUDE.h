//
//  LOG_INCLUDE.h
//  client
//
//  Created by 李灏然 on 16/11/27.
//  Copyright © 2016年 李灏然. All rights reserved.
//

#ifndef LOG_INCLUDE_h
#define LOG_INCLUDE_h

#define LOG_LINE_MAX 1024   //日志单行最大字符长度

#include <fstream>

using std::string;
using std::fstream;

class CLog
{
    public:
    enum LOG_LEVEL     // 日志级别
    {
        LL_ERROR = 1,
        LL_WARN = 2,
        LL_INFORMATION = 3
    };
    
    public:
    CLog(void):m_openSuccess(false), m_LogLevel(LL_ERROR), m_showLogFlag(true),
    m_maxLogFileSize(10 * 1024 *1024)
    {
    }
    CLog(const char *filePath, LOG_LEVEL level = LL_ERROR);
    CLog(const wchar_t *filePath, LOG_LEVEL level = LL_ERROR);
    virtual ~CLog(void)
    {
        if (m_openSuccess)
        {
            CloseLogFile();
        }
    }
    
    // 打开日志文件
    void OpenLogFile(const char *pFilePath, LOG_LEVEL level = LL_ERROR);
    void OpenLogFile(const wchar_t *pFilePath, LOG_LEVEL level = LL_ERROR);
    // 写日志操作
    void WriteLog(LOG_LEVEL level, const char *pLogText, ...);
    void WriteLog(string logText, LOG_LEVEL level = LL_ERROR);
    void WriteLog(const char *pLogText, LOG_LEVEL level = LL_ERROR);
    void WriteLogEx(LOG_LEVEL level, const char *pLogText, ...);
    // 得到日志文件大小
    size_t GetLogFileSize();
    // 清空日志文件内容
    void ClearLogFile();
    void CloseLogFile();
    
    void SetLogLevel(LOG_LEVEL level = LL_ERROR)
    {
        m_LogLevel = level;
    }
    LOG_LEVEL GetLogLevel() const
    {
        return m_LogLevel;
    }
    
    void SetShowFlag(bool flag = true)
    {
        m_showLogFlag = flag;
    }
    bool GetShowFlag() const
    {
        return m_showLogFlag;
    }
    
    void SetMaxLogFileSize(size_t size)
    {
        m_maxLogFileSize = size;
    }
    size_t GetMaxLogFileSize() const
    {
        return m_maxLogFileSize;
    }
    
    string& GetLogFilePath()
    {
        return m_logFilePath;
    }
    
    // 获得打开文件是否成功的标识
    bool GetOpenStatus() const
    {
        return m_openSuccess;
    }
    
    protected:
    string W2A(const wchar_t *pWcsStr);  // wchar_t转化为string
    string ConvertToRealLogText(const char *pLogText, LOG_LEVEL level = LL_ERROR);
    const std::string &StringFormat(std::string &srcString, const char *pFormatString, ...);
    
    protected:
    fstream m_fileOut;
    bool m_openSuccess;  // 日志文件打开状态 true表示已经成功打开 否则没有打开
    string m_logFilePath; // 日志文件完整路径 string类型
    
    protected:
    LOG_LEVEL m_LogLevel;  // 日志记录级别 默认为LL_ERROR
    bool m_showLogFlag;    // 显示日志标志  默认为true
    size_t m_maxLogFileSize; // 日志文件最大大小 超过该大小将被删除 默认为10MB
};

#endif /* LOG_INCLUDE_h */
