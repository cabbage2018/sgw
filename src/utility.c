/*!**********************************************************************
*
* utility.c
*
*
************************************************************************/

#include <process.h>  
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#pragma warning(disable:4996)
#include <io.h> //_finddata_t, _findfirst(), _findnext(), _findclose()
#include"defs.h"
extern const char*time_series_monitor_directory;

unsigned int workThreadId;
HANDLE workThreadAliveSemaphore;
HANDLE workThread;

// 'abc' => 'ABC'
char* str2up(char* str)	/// ASCII ONLY
{
  int len = strlen(str);
  char* buf = malloc(len);
  memset(buf, len, '\0');
  for (int i = 0; i < strlen(str); i++) {
    if ((str[i] >= 'a') && (str[i] <= 'z')) {
      buf[i] = str[i] + 'A' - 'a';
    }
    else {
      buf[i] = str[i];
    }
  }
  return buf;
}

// 'ABC' => 'abc'
char* str2low(char* str)/// ASCII ONLY
{
  int len = strlen(str);
  char* buf = malloc(len);
  memset(buf, len, '\0');
  for (int i = 0; i < strlen(str); i++) {
    if ((str[i] >= 'A') && (str[i] <= 'Z')) {
      buf[i] = str[i] + 'a' - 'A';
    }
    else {
      buf[i] = str[i];
    }
  }
  return buf;
}

// crt_find.c
// This program uses the 32-bit _find functions to print
// a list of all files (and their attributes) with a .C extension
// in the current directory.
int scanFolder(const LPCWSTR lpwstr_dest_dir, const LPCWSTR lpwstr_src_dir) {
  wchar_t dest[255] = { '\0' };
  wchar_t source_dir[255] = { '\0' };

  swprintf_s(dest, 255, L"%s%s", lpwstr_dest_dir, L"\\");
  swprintf_s(source_dir, 255, L"%s%s", lpwstr_src_dir, L"\\");

  FILE *fp = fopen("C:\\Temp\\test2.txt", "w");
  if (NULL != fp) {
    fwrite(dest, strlen(dest), 1, fp);
    fwrite(source_dir, strlen(source_dir), 1, fp);
    fclose(fp);
  }
  // need check that dest does not equal src
  if (_access(dest, 0) == -1 || _access(source_dir, 0) == -1) {
    return -1;
  }

  struct /*_finddata64i32_t*/__finddata64_t c_file;
  intptr_t hFile;

  // Find first .json file in input "directory"
  if ((hFile = _findfirst(source_dir, &c_file)) == -1L) {
    //printf("No *.c files in current directory!\n");
  } else {
    int index = 0;
    do {
      char buffer[30];
      ctime_s(buffer, _countof(buffer), &c_file.time_write);
      //printf(" %-12s %.24s  %9ld\n", c_file.name, buffer, c_file.size);

      // feed the file name 
      char source_file[260] = { '\0' };
      strcpy_s(source_file, _countof(source_file), source_dir);
      strcat_s(source_file, _countof(source_file), c_file.name);

      // process
      //json_timeseries_file_process2(source_dir, c_file.name);

      // calculate destinated
      char destinated_file[260] = { '\0' };
      strcpy_s(destinated_file, _countof(destinated_file), dest);
      strcat_s(destinated_file, _countof(destinated_file), c_file.name);

      // remove file; override destinated if the file with same name exists
      rename(source_file, destinated_file);
    } while (_findnext(hFile, &c_file) == 0 && index++ < 30);

    _findclose(hFile);
  }

  return 0;
}

int startWorkThread() {
	return 1;
}
void threading(void* input_string) {
  printf("TIMER ROUTINE <%s>", input_string);
  workThreadAliveSemaphore = CreateSemaphore(NULL, 1, 1, (LPCWSTR)"CreateSyncThreadSemaphore");
  int signaling = startWorkThread();
  Sleep(1);
  if (0 == signaling) {
    // Sync to threaded
    WaitForSingleObject(workThread, INFINITE);
    CloseHandle(workThread);
    CloseHandle(workThreadAliveSemaphore);
  }
}

/*************************************************************************
*
************************************************************************/
void deleteChar(char str[], char target) {
  int i, j;
  for (i = j = 0; str[i] != '\0'; i++) {
    if (str[i] != target) {
      str[j++] = str[i];
    }
  }
  str[j] = '\0';
}

BOOL instr(const char c, const char*str) {
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] == c) {
      return TRUE;
    }
  }
  return FALSE;
}

int try_read_lines(char*file_path, int __maxlen, char **lines) {
  int n = 0;
  char*temp = malloc(65535);
  memset(temp, '\0', 65535);
  FILE*fp = fopen(file_path, "r");
  if ((fp) != NULL) {
    char c = EOF;
    while ((c = getc(fp)) != EOF) {
      if (n >= __maxlen) {
        break;
      }
      temp[n++] = c;
    }
    temp[n++] = '\0';
    fclose(fp);
    *lines = temp;
  }
  return n;
}

int readText(char*file_path, int maxlen, char**outputs, char*forbidden) {
  int n = try_read_lines(file_path, maxlen, outputs);
  if (n > 0 && n < maxlen) {
    int i;
    int j;
    for (i = j = 0; (*outputs)[i] != '\0'; i++) {
      char c = (*outputs)[i];
      // Delete some chars like \r\n
      if (/*c != '\n' && c != '\r' &&c != '\t' forbidden != NULL && strlen(forbidden) > 0 &&*/  FALSE == instr(c, forbidden)) {
        (*outputs)[j++] = (*outputs)[i];
      }
    }
    (*outputs)[j] = '\0';
    return 0;
  }
  return -1;
}

int attributeFile(const char* path, char filename[])
{
  int result = -1;
  struct _finddata_t data;
  long hnd = _findfirst(path, &data);// �����ļ������������ʽchRE��ƥ���һ���ļ�
  if (hnd < 0) {
    return -1; //perror(path);
  }

  int  nRet = (hnd <0) ? -1 : 1;
  if (nRet >= 0) {
    if (data.attrib == _A_SUBDIR) {
      result = -1;
    } else {
      strcpy(filename, data.name);
      result = 0;
    }
  }
  _findclose(hnd);
  return result;
}

int try_write_text_file(const char*file_path, const char*string, const char*format) {
  if ((_access(file_path, 0)) != -1) {
    char saving_path[260] = { '\0' };
    // Switch to working directory
    //_getcwd(saving_path, MAXPATH);
    strcpy_s(saving_path, _countof(saving_path), "C:/Temp2");
    strcat_s(saving_path, _countof(saving_path), "/");

    char*time_stamp;
    time_t ticks;
    ticks = time(NULL);
    //try_generate_time_string_as_file_name(&ticks, &time_stamp);

    strcat(saving_path, time_stamp);
    strcat(saving_path, "-");

    char filename[260] = { '\0' };
    //attribute_file_name(file_path, filename);
    strcat(saving_path, filename);

    free(time_stamp);

    int result = rename(file_path, saving_path);

    printf("MOVED a duplicate file result: %d", result);
  }

  FILE *fp = fopen(file_path, format);
  if (NULL != fp) {
    fwrite(string, strlen(string), 1, fp);
    fclose(fp);
    return 0;
  }
  else {
    return -1;
  }
}

// crt_find.c
// This program uses the 32-bit _find functions to print
// a list of all files (and their attributes) with a .C extension
// in the current directory.
int scan_folder(const char *source_dir/*, const char*destinated_dir*/) {
  // claim
  if (_access(source_dir, 0) == -1 /*|| _access(destinated_dir, 0) ==-s1*/) {
    return -1;
  }

  int result = -1;
  char search_wildchar[255] = { '\0' };
  result = sprintf_s(search_wildchar, _countof(search_wildchar), "%s%s", source_dir, "*.json");
  printf("scan_folder <%s> <%d>", search_wildchar, result);

  struct __finddata64_t c_file;
  intptr_t hFile;

  // Find first .json file in input "directory"
  if ((hFile = _findfirst(search_wildchar, &c_file)) == -1L) {
    //printf("No *.c files in current directory!\n");
  }
  else
  {
    int index = 0;
    do {
      char buffer[30];
      ctime_s(buffer, _countof(buffer), &c_file.time_write);
      printf(" %-12s %.24s  %9ld", c_file.name, buffer, c_file.size);
      //printf(" %-12s %.24s  %9ld\n", c_file.name, buffer, c_file.size);

      // feed the file name 
      char source_file[255] = { '\0' };
      int len = _countof(source_file);
      strcpy_s(source_file, _countof(source_file), source_dir);
      strcat_s(source_file, _countof(source_file), c_file.name);

      // process
	  int result = 0;//process_json_timeseries_file(source_dir, c_file.name);

      char destinated_file[255] = { '\0' };
      //strcpy_s(destinated_file, _countof(destinated_file), time_series_archive_directory);
      strcat_s(destinated_file, _countof(destinated_file), c_file.name);

      // remove file; override destinated if the file with same name exists
      result = rename(source_file, destinated_file);

      printf("INFO: Move %s to %s: Result:%d\n", source_file, destinated_file, result);
    } while (_findnext(hFile, &c_file) == 0 && index++ < 10/**/);
    // bear in mind each upload consumes 6 seconds around
    _findclose(hFile);
  }

  return result;
}

int request() {
  /*
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    */
    system("pause");
    return 0;
}

/*
[
	{
		"TimeStamp":"2018-12-19T23:00:00.000Z",
		"StartTime":"2018-12-19T23:00:27.000Z",
		"EndTime":"2018-12-20T15:35:10.000Z",
		"ChangeTimes":27,"Output":18,
		"AlarmTimes":0,
		"VarComment":"_u0059_u005a_u004d_u0020_u0023_u0031"
	},
	{
		"TimeStamp":"2018-12-19T23:00:01.000Z",
		"StartTime":"2018-12-20T15:47:29.000Z",
		"EndTime":"2018-12-20T15:46:12.000Z",
		"ChangeTimes":79,"Output":39,
		"AlarmTimes":3,
		"VarComment":"_u0046_u0055_u002d_u0032_u0032_u0030"
	}
]*/


/*
    PACKAGE_ERROR_CODE 
    
    [
        {
            "prtc": "MODBUSTCP",
            "addr": "MODBUSTCP 127.0.0.1 502 127:30046:4",

            "etyp": "DOUBLE",
            "valu": "386.984",
        },
        {
            "prtc": "OPCUA",
            "addr": "OPCUA 192.168.101.202 3836 ns=127;i=30046",

            "etyp": "INT",
            "valu": "3868",
        },
        {
            "prtc": "MODBUSRTU",
            "addr": "MODBUSRTU COM9 BAUD152000:8:0:1 127:30046:4",

            "etyp": "DOUBLE",
            "valu": "221.21",
        },
        {
            "prtc": "IEC62541",
            "addr": "IEC62541 ns=127;i=30046",

            "etyp": "INT",
            "valu": "3868",
        },
        {
            "prtc": "IEC61580",
            "addr": "IEC61580 transformerMain/xtop/sup/dio10076",

            "etyp": "BOOLEAN",
            "valu": "true",
        },
        {
            "prtc": "IEC61158",
            "addr": "IEC61158 acb/trip/status/grounded",

            "etyp": "BOOLEAN",
            "valu": "false",
        },
        {
            "prtc": "SNAP7",
            "addr": "SNAP7 192.168.100.245 83 DBW:DB1684",

            "etyp": "LONG",
            "valu": "0987654322",
        },
        {
            "prtc": "OPCUA",
            "addr": "OPCUA 192.168.101.202 3836 ns=127;s=256",

            "etyp": "STRING",
            "valu": "SN:90887-54334-1987, 20210930T19:53:59.931Z",
        }
    ]*/
//"./assets"
int showSubfolder(char* dir) {
  char absolute_root[255] = { '\0' };
  _getcwd(absolute_root, 255 - 1);
  printf("_getcwd: <%s>", absolute_root);

  if (_access(dir, 0) == -1) {
    printf("dir: <%s>", dir);
    _mkdir(dir);
  }
}


void synchronize_thread_for_folder(void *param) {
	// process json time series in specific
	scan_folder("c:\\temp");
	// leave a sensor time series for next round uploading
	//generate_random_json_sensor_ts(time_series_monitor_directory);
}


int workerThread() {
	char*arglist = "C:/Temp/";
	workThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(_stdcall *)(void *))synchronize_thread_for_folder, arglist, 0, &workThreadId);
	if (NULL == workThread) {
		printf("ERROR: Create work thread failed!\n");
		return -1;
	}
	else {
		printf("INFO: Create work thread succeed!\n");
		return 0;
	}
}
