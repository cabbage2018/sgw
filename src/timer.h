#include"defs.h"
#pragma comment(lib,"Winmm.lib")	//For timeSetEvent
#if (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
#define strcasecmp _stricmp
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

/*
[https://www.cnblogs.com/shikamaru/p/7656532.html]
1、timeSetEvent最长时间间隔不能超过1000秒，即1000000毫秒，超过返回失败，可用CreateTimerQueryTimer或SetTimer（回调的方式）代替
2、timeSetEvent会生成一个独立的Timer回调线程，属多线程
3、timeSetEvent可创建高精度定时器，精确到1ms，SetTimer无法精确到1ms
4、timeKillEvent关掉定时器的函数，一定要一一对应，每次timeSetEvent返回的定时器的ID是不一样的，调用一次timeSetEvent就会产生一次Id，调用了几次timeSetEvent，就需要调用几次timeKillEvent ，而且必须是相对应的ID,否则可能出现程序崩溃！
*/
typedef void(*PROCESSOR1)(char *input_line_string); //*类型
static PROCESSOR1 timerHandler = NULL;
int timerID = -1;
void try_register(PROCESSOR1 handler) {
  timerHandler = handler;
}
//定时器的回调函数
void WINAPI timerTask(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
  //关闭定时器：周期Renew
  timeKillEvent(timerID);
  int delay = (int)dwUser;
  char buffer[32] = { '\0' };
  int processed = snprintf(buffer, 32, "<%d>", delay);

  // 执行定时器任务
  if (NULL != timerHandler) {
    timerHandler(buffer);
  }

  timerID = timeSetEvent(delay, 1, (LPTIMECALLBACK)timerTask, dwUser, /*TIME_PERIODIC*/TIME_ONESHOT);
}

int startTimer(int milliseconds) {
  DWORD_PTR dwUser = NULL;
  dwUser = (DWORD)milliseconds;
  // 设置定时器
  int timerID = timeSetEvent(milliseconds, 1, (LPTIMECALLBACK)timerTask, dwUser, /*TIME_PERIODIC*/TIME_ONESHOT);
  if (timerID != NULL) {
    fprintf(stdout, "Start timerID = %d", timerID);
  }
  return 0;
}

//关闭定时器：用户退出
void stopTimer() {
  if (timerID > 0) {
    fprintf(stdout, "kill the timer event, timerID = %d", timerID);
    timeKillEvent(timerID);
  }
  timerID = 0;
}