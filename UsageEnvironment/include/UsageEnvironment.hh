/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2025 Live Networks, Inc.  All rights reserved.
// Usage Environment
// C++ header

#ifndef _USAGE_ENVIRONMENT_HH
#define _USAGE_ENVIRONMENT_HH

#ifndef _USAGEENVIRONMENT_VERSION_HH
#include "UsageEnvironment_version.hh"
#endif

#ifndef _NETCOMMON_H
#include "NetCommon.h"
#endif

#ifndef _BOOLEAN_HH
#include "Boolean.hh"
#endif

#ifndef _STRDUP_HH
// "strDup()" is used often, so include this here, so everyone gets it:
#include "strDup.hh"
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef NO_STD_LIB
#ifndef _LIBCPP_ATOMIC
#include <atomic>
#endif
#endif

#ifdef __BORLANDC__
#define _setmode setmode
#define _O_BINARY O_BINARY
#endif

class TaskScheduler; // forward

// An abstract base class, subclassed for each use of the library

class LIVEMEDIA_API UsageEnvironment {
public:
   Boolean reclaim();
   // returns True iff we were actually able to delete our object

// task scheduler:
   TaskScheduler& taskScheduler() const { return fScheduler; }

   // result message handling:
   typedef char const* MsgString;
   virtual MsgString getResultMsg() const = 0;

   virtual void setResultMsg(MsgString msg) = 0;
   virtual void setResultMsg(MsgString msg1, MsgString msg2) = 0;
   virtual void setResultMsg(MsgString msg1, MsgString msg2, MsgString msg3) = 0;
   virtual void setResultErrMsg(MsgString msg, int err = 0) = 0;
   // like setResultMsg(), except that an 'errno' message is appended.  (If "err == 0", the "getErrno()" code is used instead.)

   virtual void appendToResultMsg(MsgString msg) = 0;

   virtual void reportBackgroundError() = 0;
   // used to report a (previously set) error message within
   // a background event

   virtual void internalError(); // used to 'handle' a 'should not occur'-type error condition within the library.

   // 'errno'
   virtual int getErrno() const = 0;

   // 'console' output:
   virtual UsageEnvironment& operator<<(char const* str) = 0;
   virtual UsageEnvironment& operator<<(int i) = 0;
   virtual UsageEnvironment& operator<<(unsigned u) = 0;
   virtual UsageEnvironment& operator<<(double d) = 0;
   virtual UsageEnvironment& operator<<(void* p) = 0;

   // a pointer to additional, optional, client-specific state
   void* liveMediaPriv;
   void* groupsockPriv;

protected:
   UsageEnvironment(TaskScheduler& scheduler); // abstract base class
   virtual ~UsageEnvironment(); // we are deleted only by reclaim()

private:
   TaskScheduler& fScheduler;
};


typedef void TaskFunc(void* clientData);
typedef void* TaskToken;
typedef u_int32_t EventTriggerId;

#ifndef NO_STD_LIB
typedef std::atomic_char EventLoopWatchVariable;
#else
typedef char volatile EventLoopWatchVariable;
#endif

//핵심 이벤트 루프 기반 클래스로, 서버나 클라이언트의 비동기 작업(소켓 I/O, 타이머, 사용자 이벤트 등)을 예약하고 실행
//다음과 같은 세 가지 종류의 “작업(Task)”을 관리
//1. 소켓 이벤트(Socket Event) :	네트워크 소켓에서 읽기/쓰기/예외가 가능한 상태가 되었을 때 호출할 핸들러를 등록
//2. 지연 이벤트(Delayed Event) : 일정 시간이 지난 후 실행할 함수(타이머 콜백)를 예약
//3. 트리거 이벤트(Triggered Event)	: 즉시(또는 다른 스레드에서) 실행 요청할 수 있는 이벤트 핸들러
class LIVEMEDIA_API TaskScheduler 
{
public:
   virtual ~TaskScheduler();

   // 다음 스케줄링 지점에 도달했을 때 (지연 후) 작업이 발생하도록 스케줄링합니다.
   // ("microseconds" <= 0이면 지연되지 않습니다.)
   // 후속 호출에서 사용할 수 있는 토큰을 반환합니다.
   // unscheduleDelayedTask() 또는 rescheduleDelayedTask()
   // (단, 작업이 아직 발생하지 않은 경우에만 해당).
   virtual TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,
      void* clientData) = 0;

   // ("prevTask" == NULL인 경우 효과가 없습니다.)
   // 이후 "prevTask"를 NULL로 설정합니다.
   // 참고: 예약된 작업이 이미 발생한 경우에는 이 메서드를 호출해서는 안 됩니다.
   virtual void unscheduleDelayedTask(TaskToken& prevTask) = 0;

   // "unscheduleDelayedTask()"와 "scheduleDelayedTask()"를 결합합니다.
   // ("task"를 새 작업 토큰으로 설정합니다.)
   // 참고: 예약된 작업이 이미 발생한 경우에는 이 메서드를 호출해서는 안 됩니다.
   virtual void rescheduleDelayedTask(TaskToken& task,
      int64_t microseconds, TaskFunc* proc,
      void* clientData);

   // 백그라운드에서 소켓 작업을 처리하는 경우(이벤트 루프에서):
   typedef void BackgroundHandlerProc(void* clientData, int mask);

   // "mask"에 설정할 수 있는 비트입니다. (이것들은 Tcl 기반 하위 클래스를 쉽게 만들기 위해
   // Tcl과 동일하게 의도적으로 정의되었습니다.)
#define SOCKET_READABLE    (1<<1)
#define SOCKET_WRITABLE    (1<<2)
#define SOCKET_EXCEPTION   (1<<3)

   virtual void setBackgroundHandling(int socketNum, int conditionSet, BackgroundHandlerProc* handlerProc, void* clientData) = 0;
   void disableBackgroundHandling(int socketNum) { setBackgroundHandling(socketNum, 0, NULL, NULL); }
   // "oldSocketNum"에 대한 모든 소켓 처리를 변경하여 대신 "newSocketNum"에서 처리되도록 합니다.
   virtual void moveSocketHandling(int oldSocketNum, int newSocketNum) = 0;

   // 이벤트 루프 내에서 추가 실행이 발생하도록 합니다.
   // 지연된 작업, 백그라운드 I/O 처리 및 기타 이벤트는 단일 제어 스레드로 순차적으로 처리됩니다.
   // ("watchVariable"이 NULL이 아니면 *watchVariable != 0일 때 이 루틴에서 복귀합니다.)
   virtual void doEventLoop(EventLoopWatchVariable* watchVariable = NULL) = 0;

   // 이벤트에 대한 '트리거'를 생성합니다. 이벤트가 발생하면 "eventHandlerProc"를 사용하여 이벤트 루프에서 처리됩니다.
   // (트리거를 생성할 수 없는 경우(예: 트리거 개수에 대한 구현 제한으로 인해) 0을 반환합니다.)
   virtual EventTriggerId createEventTrigger(TaskFunc* eventHandlerProc) = 0;
   virtual void deleteEventTrigger(EventTriggerId eventTriggerId) = 0;

   // 지정된 이벤트에 대한 (이전에 등록된) 핸들러 함수가 (이벤트 루프에서) 처리되도록 합니다.
   // 핸들러 함수는 "clientData"를 매개변수로 사용하여 호출됩니다.
   // 참고: 이 함수는 (다른 라이브러리 함수와 달리) 외부 스레드에서 호출하여 외부 이벤트를 알릴 수 있습니다.
   // (실제로 이 함수는 LIVE555가 아닌 스레드에서 호출할 수 있는 *유일한* LIVE55 함수입니다.)
   // (하지만 "triggerEvent()"는 다른 스레드에서 동일한 '이벤트 트리거 ID'로 호출해서는 안 됩니다.
   // 또한 "triggerEvent()"가 하나의 '이벤트 트리거 ID'로 호출되면
   // 해당 이벤트가 처리될 때까지 동일한 '이벤트 트리거 ID'로 다시 호출해서는 안 됩니다.
   virtual void triggerEvent(EventTriggerId eventTriggerId, void* clientData = NULL) = 0;

   // 다음 두 함수는 더 이상 사용되지 않으며, 이전 버전과의 호환성을 위해서만 제공됩니다.
   void turnOnBackgroundReadHandling(int socketNum, BackgroundHandlerProc* handlerProc, void* clientData) {
      setBackgroundHandling(socketNum, SOCKET_READABLE, handlerProc, clientData);
   }
   void turnOffBackgroundReadHandling(int socketNum) { disableBackgroundHandling(socketNum); }

   virtual void internalError(); // 라이브러리 내에서 '발생해서는 안 되는' 유형의 오류 조건을 '처리'하는 데 사용됩니다.

protected:
   TaskScheduler(); // abstract base class
};

#endif
