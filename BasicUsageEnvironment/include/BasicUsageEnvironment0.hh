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
// Basic Usage Environment: for a simple, non-scripted, console application
// C++ header

#ifndef _BASIC_USAGE_ENVIRONMENT0_HH
#define _BASIC_USAGE_ENVIRONMENT0_HH

#ifndef _BASICUSAGEENVIRONMENT_VERSION_HH
#include "BasicUsageEnvironment_version.hh"
#endif

#ifndef _USAGE_ENVIRONMENT_HH
#include "UsageEnvironment.hh"
#endif

#ifndef _DELAY_QUEUE_HH
#include "DelayQueue.hh"
#endif

#define RESULT_MSG_BUFFER_MAX 1000

// 추상 기본 클래스로, 하위 클래스화에 유용합니다.
// (예: "operator<<" 구현을 재정의하는 경우)
class LIVEMEDIA_API BasicUsageEnvironment0 : public UsageEnvironment {
public:
   // redefined virtual functions:
   virtual MsgString getResultMsg() const;

   virtual void setResultMsg(MsgString msg);
   virtual void setResultMsg(MsgString msg1,
      MsgString msg2);
   virtual void setResultMsg(MsgString msg1,
      MsgString msg2,
      MsgString msg3);
   virtual void setResultErrMsg(MsgString msg, int err = 0);

   virtual void appendToResultMsg(MsgString msg);

   virtual void reportBackgroundError();

protected:
   BasicUsageEnvironment0(TaskScheduler& taskScheduler);
   virtual ~BasicUsageEnvironment0();

private:
   void reset();

   char fResultMsgBuffer[RESULT_MSG_BUFFER_MAX];
   unsigned fCurBufferSize;
   unsigned fBufferMaxSize;
};

class HandlerSet; // forward

// 참고: MAX_NUM_EVENT_TRIGGERS를 다시 정의할 수 있지만
// "EventTriggerId"의 비트 수보다 작아야 합니다.
#ifndef MAX_NUM_EVENT_TRIGGERS
#define MAX_NUM_EVENT_TRIGGERS 32
#endif
#define EVENT_TRIGGER_ID_HIGH_BIT (1 << (MAX_NUM_EVENT_TRIGGERS-1))

// 추상 기본 클래스로, 하위 클래스화에 유용합니다.
// (예: 소켓 이벤트 처리 구현을 재정의하는 경우)
class LIVEMEDIA_API BasicTaskScheduler0 : public TaskScheduler {
public:
   virtual ~BasicTaskScheduler0();

   // "maxDelayTime"은 마이크로초 단위입니다. 하위 클래스에서 "select()"가 폴링도 수행하려는 경우 지연 시간을 제한할 수 있습니다.
   // 0(기본값)은 최대값이 없음을 의미합니다. 지연 큐를 확인하세요.
   virtual void SingleStep(unsigned maxDelayTime = 0) = 0;

public:
   // Redefined virtual functions:
   virtual TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,
      void* clientData);
   virtual void unscheduleDelayedTask(TaskToken& prevTask);

   virtual void doEventLoop(EventLoopWatchVariable* watchVariable);

   virtual EventTriggerId createEventTrigger(TaskFunc* eventHandlerProc);
   virtual void deleteEventTrigger(EventTriggerId eventTriggerId);
   virtual void triggerEvent(EventTriggerId eventTriggerId, void* clientData = NULL);

protected:
   BasicTaskScheduler0();

protected:
   intptr_t fTokenCounter;
   //지연 이벤트 핸들러 목록
   DelayQueue fDelayQueue;

   //소켓 이벤트 핸들러 목록
   HandlerSet* fHandlers;
   int fLastHandledSocketNum;

   // 이벤트 트리거를 구현하려면:
#ifndef NO_STD_LIB
   std::atomic_flag fTriggersAwaitingHandling[MAX_NUM_EVENT_TRIGGERS];
#else
   Boolean volatile fTriggersAwaitingHandling[MAX_NUM_EVENT_TRIGGERS];
#endif
   u_int32_t fLastUsedTriggerMask; // implemented as a 32-bit bitmap

   //트리거 이벤트 핸들러 목록
   TaskFunc* fTriggeredEventHandlers[MAX_NUM_EVENT_TRIGGERS];
   //트리거 이벤트 핸들러 파라미터 목록
   void* fTriggeredEventClientDatas[MAX_NUM_EVENT_TRIGGERS];

   unsigned fLastUsedTriggerNum; // in the range [0,MAX_NUM_EVENT_TRIGGERS)
   Boolean fEventTriggersAreBeingUsed;
};

#endif
