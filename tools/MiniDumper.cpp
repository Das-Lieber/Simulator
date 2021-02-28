#include <Windows.h>
#include "MiniDumper.h"
#include <QtDebug>

LPCWSTR MiniDumper::m_szAppName;

LPWSTR MiniDumper::m_szAppVersion;

LPWSTR MiniDumper::m_szAppBuildNumber;

WCHAR MiniDumper::m_szMessageText[MAX_WARNING_MESSAGE_PATH];

LPWSTR MiniDumper::m_szDumpFilePath;

#define DEFAULT_ENGLISH_MESSAGE_TEXT L"%s experienced an unknown error and had to exit. \nHowever, some error information has been saved in %s. \nPlease, email this file to <hassan_deldar@yahoo.com> if you would like to help us debug the problem."

#define MAX_DUMP_FILE_NUMBER 9999


//static int DUMP_TYPE_MINI = MiniDumpWithUnloadedModules;

//static int DUMP_TYPE_MIDD = MiniDumpWithUnloadedModules | MiniDumpWithIndirectlyReferencedMemory;

static int DUMP_TYPE_FULL = MiniDumpNormal | MiniDumpWithFullMemory | MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithProcessThreadData;

//static int DUMP_TYPE_FULL = MiniDumpNormal;
//默认值是只有 MiniDumpNormal 文件比较小
//如果把所有的选项都加上， 文件很大， 本机调试也没关系


MiniDumper::MiniDumper()
{
    // if this assert fires then you have two instances of MiniDumper
    // which is not allowed
    Q_ASSERT( m_szAppName==NULL );

    m_szAppName = wcsdup(L"Simulator");
    m_szAppVersion = wcsdup( L"CrashDump");
    m_szAppBuildNumber = wcsdup( L"0000");

    wcscpy(m_szMessageText,DEFAULT_ENGLISH_MESSAGE_TEXT);


    m_szDumpFilePath = NULL;
    ::SetUnhandledExceptionFilter( TopLevelFilter );
}

MiniDumper::~MiniDumper()
{
}

void MiniDumper::SetVersion(LPCWSTR szVersion)
{
    if(szVersion)
    {
        free(m_szAppVersion);
        m_szAppVersion = wcsdup(szVersion);
    }
}

void MiniDumper::SetBuildNumber(LPCWSTR szBuildNumber)
{
    if(szBuildNumber)
    {
        free(m_szAppBuildNumber);
        m_szAppBuildNumber = wcsdup(szBuildNumber);
    }
}

void MiniDumper::SetDumpFilePath(LPCWSTR szFilePath)
{
    free(m_szDumpFilePath);
    m_szDumpFilePath = NULL;
    if(szFilePath != NULL)
    {
        m_szDumpFilePath = wcsdup(szFilePath);
    }
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{

    LONG retval = EXCEPTION_CONTINUE_SEARCH;
    HWND hParent = NULL; // find a better value for your app

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    WCHAR szDbgHelpPath[_MAX_PATH];

    if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
    {
        WCHAR *pSlash = wcsrchr( szDbgHelpPath, L'\\');
        if (pSlash)
        {
            wcscpy( pSlash+1, L"DBGHELP.DLL" );
            hDll = ::LoadLibrary( szDbgHelpPath );

        }

        if (hDll==NULL)
        {
            // load any version we can
            hDll = ::LoadLibrary( L"DBGHELP.DLL");
        }

        LPCWSTR szResult = NULL;

        if (hDll)
        {
            MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
            if (pDump)
            {
                WCHAR szDumpPath[_MAX_PATH];
                WCHAR szDumpRootPath[_MAX_PATH];
                WCHAR szScratch[_MAX_PATH];

                // work out a good place for the dump file

                if(m_szDumpFilePath == NULL)
                {
                    if (GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH))
                    {
                        WCHAR *pSlash = wcsrchr(szDbgHelpPath, L'\\');
                        if (pSlash)
                        {
                            wcscpy(pSlash + 1, L"");
                            wcscpy(szDumpPath, szDbgHelpPath);
                        }
                    }
                    else if (!GetTempPath( _MAX_PATH, szDumpPath ))
                        wcscpy( szDumpPath, L"c:\\temp\\" );
                }
                else
                {
                    wcscpy( szDumpPath, m_szDumpFilePath );
                }
                wcscpy( szDumpRootPath, szDumpPath);

                //PrintDebug(L"[MiniDumper] Mini Dump file:[%s]",szDumpPath);

                // ask the user if they want to save a dump file
                //if (::MessageBox( NULL, _T("Something bad happened in your program, would you like to save a diagnostic file?"), m_szAppName, MB_YESNO )==IDYES)
                {
                    HANDLE hFile = INVALID_HANDLE_VALUE;
                    int i = 1;
                    WCHAR szFileNumber[_MAX_PATH];
                    while(hFile == INVALID_HANDLE_VALUE)
                    {
                        swprintf(szFileNumber, sizeof(szFileNumber), L"_%04d",i);
                        wcscpy( szDumpPath, szDumpRootPath);
                        wcscat( szDumpPath, m_szAppName );
                        wcscat( szDumpPath, L"_" );
                        wcscat( szDumpPath, m_szAppVersion);
                        wcscat( szDumpPath, L"_" );
                        wcscat( szDumpPath, m_szAppBuildNumber);
                        wcscat( szDumpPath, szFileNumber);
                        wcscat( szDumpPath, L".dmp" );
                        hFile = CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW,
                                            FILE_ATTRIBUTE_NORMAL, NULL );
                        i++;
                        if(i > MAX_DUMP_FILE_NUMBER)
                        {
                            hFile = CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                                                FILE_ATTRIBUTE_NORMAL, NULL );
                            break;
                        }
                    }
                    // create the file

                    if (hFile!=INVALID_HANDLE_VALUE)
                    {
                        _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                        ExInfo.ThreadId = GetCurrentThreadId();
                        ExInfo.ExceptionPointers = pExceptionInfo;
                        ExInfo.ClientPointers = NULL;

                        // write the dump
                        BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)DUMP_TYPE_FULL, &ExInfo, NULL, NULL );
                        if (bOK)
                        {
                            swprintf( szScratch, sizeof(szScratch), L"Saved dump file to '%s'", szDumpPath );
                            szResult = szScratch;
                            retval = EXCEPTION_EXECUTE_HANDLER;
                        }
                        else
                        {
                            swprintf( szScratch, sizeof(szScratch),L"Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
                            szResult = szScratch;
                        }
                        CloseHandle(hFile);

                        WCHAR csOutMessage[MAX_WARNING_MESSAGE_PATH];
                        swprintf(csOutMessage, sizeof(csOutMessage), m_szMessageText, m_szAppName, szDumpPath);

                        //PrintError(_T("%s"), csOutMessage);
                        //MessageBoxW( NULL, csOutMessage, m_szAppName , MB_OK );
                        qDebug() << "Dump Crash file ...";
                    }
                    else
                    {
                        swprintf( szScratch, sizeof(szScratch),L"Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
                        szResult = szScratch;
                    }
                }

            }
            else
            {
                szResult = L"DBGHELP.DLL too old";
            }
        }
        else
        {
            szResult = L"DBGHELP.DLL not found";
        }

        if (szResult)
        {
            //PrintDebug(_T("[MiniDumper] Mini Dump result:[%s]"),szResult);
        }

        return retval;
    }
}
