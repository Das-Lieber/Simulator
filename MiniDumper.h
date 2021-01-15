#ifndef MINIDUMP_H
#define MINIDUMP_H
#include <Windows.h>
#include <DbgHelp.h>
// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                         CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                         );
#define MAX_WARNING_MESSAGE_PATH 1024
class MiniDumper
{
private:
    static LPCWSTR m_szAppName;
    static LPWSTR m_szAppVersion;
    static LPWSTR m_szAppBuildNumber;
 
    static WCHAR m_szMessageText[MAX_WARNING_MESSAGE_PATH];
 
    static LPWSTR m_szDumpFilePath;
 
    static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
 
public:
    MiniDumper( );
    ~MiniDumper();
    static void SetVersion(LPCWSTR szVersion);
    static void SetBuildNumber(LPCWSTR szBuildNumber);
    static void SetDumpFilePath(LPCWSTR szFilePath);
    static int SetWarningMessage(LPCWSTR szMessageText)
    {
        if(szMessageText)
        {
            int iLen = wcslen(szMessageText);
            if(iLen < MAX_WARNING_MESSAGE_PATH - MAX_PATH)
            {
                wcscpy(m_szMessageText,szMessageText);
                return 0;
            }
        }
        return 1;
    }
};
 
 
#endif