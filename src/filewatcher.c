//https://developersarea.wordpress.com/2014/09/26/win32-file-watcher-api-to-monitor-directory-changes/
#include <Winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include "cJSON.h"
#include "defs.h"
#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#define MAX_DIRS 25
#define MAX_FILES 255
#define MAX_BUFFER 4096
extern WINBASEAPI BOOL WINAPI ReadDirectoryChangesW(
  HANDLE hDirectory,
  LPVOID lpBuffer, DWORD nBufferLength,
  BOOL bWatchSubtree, DWORD dwNotifyFilter,
  LPDWORD lpBytesReturned,
  LPOVERLAPPED lpOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

// all purpose structure to contain directory information and provide
// the input buffer that is filled with file change data
typedef struct _DIRECTORY_INFO {
  HANDLE hDir;
  TCHAR lpszDirName[MAX_PATH];
  CHAR lpBuffer[MAX_BUFFER];
  DWORD dwBufLength;
  OVERLAPPED Overlapped;
}DIRECTORY_INFO, *PDIRECTORY_INFO, *LPDIRECTORY_INFO;

DIRECTORY_INFO DirInfo[MAX_DIRS];   // Buffer for all of the directories
TCHAR FileList[MAX_FILES*MAX_PATH]; // Buffer for all of the files
DWORD numDirs;

//Method to start watching a directory. Call it on a separate thread so it wont block the main thread.  
void WatchDirectory2(LPCWSTR path) {
  char buf[MAX_BUFFER];
  DWORD nRet;
  BOOL result = TRUE;
  char filename[MAX_PATH];
  DirInfo[0].hDir = CreateFile(path/*_T("C:/Temp")*/, GENERIC_READ | FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
    NULL);

  if (DirInfo[0].hDir == INVALID_HANDLE_VALUE)
  {
    printf(" DirInfo[0].hDir == INVALID_HANDLE_VALUE <%ls>", path);
    return; //cannot open folder
  }

  lstrcpy(DirInfo[0].lpszDirName, path);
  OVERLAPPED PollingOverlap;

  FILE_NOTIFY_INFORMATION* pNotify;
  int offset;
  PollingOverlap.OffsetHigh = 0;
  PollingOverlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  while (result)
  {
    result = ReadDirectoryChangesW(
      DirInfo[0].hDir,// handle to the directory to be watched
      &buf,// pointer to the buffer to receive the read results
      sizeof(buf),// length of lpBuffer
      TRUE,// flag for monitoring directory or directory tree
      FILE_NOTIFY_CHANGE_FILE_NAME |
      FILE_NOTIFY_CHANGE_DIR_NAME |
      FILE_NOTIFY_CHANGE_SIZE,
      //FILE_NOTIFY_CHANGE_LAST_WRITE |
      //FILE_NOTIFY_CHANGE_LAST_ACCESS |
      //FILE_NOTIFY_CHANGE_CREATION,
      &nRet,// number of bytes returned
      &PollingOverlap,// pointer to structure needed for overlapped I/O
      NULL);
    WaitForSingleObject(PollingOverlap.hEvent, INFINITE);
    offset = 0;
    do {
      pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buf + offset);
      strcpy(filename, "");
      int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
      filename[pNotify->FileNameLength / 2] = '\0';
      switch (pNotify->Action) {

        case FILE_ACTION_ADDED: {
          wchar_t* pwszUnicode;

          //返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
          int iSize = MultiByteToWideChar(CP_ACP, 0, filename, -1, NULL, 0); //iSize =wcslen(pwsUnicode)+1=6
          pwszUnicode = (wchar_t *)malloc(iSize * sizeof(wchar_t)); //不需要 pwszUnicode = (wchar_t *)malloc((iSize+1)*sizeof(wchar_t))
          MultiByteToWideChar(CP_ACP, 0, filename, -1, pwszUnicode, iSize);
          wchar_t filenamewUnicode[255];

          //MCL_WARN("\nThe file is added to the directory: [%s] \n", filename);
          char oldName[255] = { '\0' };
          char newName[255] = { '\0' };

          //Send the file name to a processing thread
          //json_timeseries_file_process(path, pwszUnicode /*filename*/);

          // Move to another
          sprintf_s(oldName, 255, "%s%ws", path, pwszUnicode);
          sprintf_s(newName, 255, "%ws%ws", L"C:/Temp2/", pwszUnicode);
          //rename(oldName, newName);
        }
          break;
        case FILE_ACTION_REMOVED:
          //MCL_WARN("\nThe file is removed from the directory: [%s] \n", filename);
          break;
        case FILE_ACTION_MODIFIED:
          //MCL_WARN("\nThe file is modified. This can be a change in the time stamp or attributes: [%s]\n", filename);
          break;
        case FILE_ACTION_RENAMED_OLD_NAME:
          //MCL_WARN("\nThe file was renamed and this is the old name: [%s]\n", filename);
          break;
        case FILE_ACTION_RENAMED_NEW_NAME:
          //MCL_WARN("\nThe file was renamed and this is the new name: [%s]\n", filename);
          break;
        default:
          //MCL_WARN("\nDefault error.\n");
          break;
      }
      offset += pNotify->NextEntryOffset;
    } while (pNotify->NextEntryOffset); //(offset != 0);
  }

  CloseHandle(DirInfo[0].hDir);
}

/*
Visual Studio Makefile 方式编译
2010年02月11日 14:52:00 cnwarden 阅读数：15565 标签： makefile linux windows 工具 ide include  更多
个人分类： C++
在Linux下，使用makefile是一种常见的编译方式，感觉这是跟IDE的缺乏有关，另外就是IDE比较多，支持不完善。而在windows下常使用VS进行编译，方法是比linux简单了，但是导致程序员往往不关系编译的过程，过度依赖于IDE工具。下面简单介绍一下windows下的Makefile编译方式。
首先注意这么几个工具:
cl.exe ：C/C++ 编译器，通常在VC 环境中的“编译”选项执行的程序，会生成一堆OBJ 文件
link.exe ：连接器，编译后需要连接程序把各个OBJ 文件连接成最终的可执行程序
nmake.exe ：make 工具，就是它将要执行我们的Makefile 文件
例如在 VS2005下可以拷贝一下：C:/Program Files/Microsoft Visual Studio 8/Common7/Tools下的vsvars32.bat设置编译的环境。当然也可以在设置完之后再添加自己的include,lib...
然后就是makefile的编写，具体写法和linux类似，注意命令之前使用tab，linux也是一样。当然了，编译选项肯定不一样，如果刚开始不熟悉，可以打开vs，建立一个工程，看看设置中的编译选项，最好是在vs2003以上，vs6.0看不到这么全。
最后使用nmake进行编译运行。
*/