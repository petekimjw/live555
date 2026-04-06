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
// Implementation


#include "BasicUsageEnvironment.hh"
#include "HandlerSet.hh"
#include <stdio.h>
#if defined(_QNX4)
#include <sys/select.h>
#include <unix.h>
#endif

////////// BasicTaskScheduler //////////

BasicTaskScheduler* BasicTaskScheduler::createNew(unsigned maxSchedulerGranularity) {
   return new BasicTaskScheduler(maxSchedulerGranularity);
}

BasicTaskScheduler::BasicTaskScheduler(unsigned maxSchedulerGranularity)
   : fMaxSchedulerGranularity(maxSchedulerGranularity), fMaxNumSockets(0)
#if defined(__WIN32__) || defined(_WIN32)
   , fDummySocketNum(-1)
#endif
{
   FD_ZERO(&fReadSet);
   FD_ZERO(&fWriteSet);
   FD_ZERO(&fExceptionSet);

   if (maxSchedulerGranularity > 0) schedulerTickTask(); // ensures that we handle events frequently
}

BasicTaskScheduler::~BasicTaskScheduler() {
#if defined(__WIN32__) || defined(_WIN32)
   if (fDummySocketNum >= 0) closeSocket(fDummySocketNum);
#endif
}

void BasicTaskScheduler::schedulerTickTask(void* clientData) {
   ((BasicTaskScheduler*)clientData)->schedulerTickTask();
}

void BasicTaskScheduler::schedulerTickTask() {
   scheduleDelayedTask(fMaxSchedulerGranularity, schedulerTickTask, this);
}

#ifndef MILLION
#define MILLION 1000000
#endif

//이벤트 루프의 한 틱을 수행
//select()로 소켓 I/O 준비 상태를 기다리고,
//준비된 소켓의 해당 핸들러를 최대 1개 호출한 뒤,
//트리거형 이벤트와 지연(타이머) 이벤트를 처리
void BasicTaskScheduler::SingleStep(unsigned maxDelayTime) 
{
   fd_set readSet = fReadSet; // 이 select() 호출에 대한 복사본을 만듭니다.
   fd_set writeSet = fWriteSet; // 동일
   fd_set exceptionSet = fExceptionSet; // 동일

   #pragma region tv_timeToDelay 계산

   DelayInterval const& timeToDelay = fDelayQueue.timeToNextAlarm();
   struct timeval tv_timeToDelay;
   tv_timeToDelay.tv_sec = timeToDelay.seconds();
   tv_timeToDelay.tv_usec = timeToDelay.useconds();
   // "tv_sec" 값이 너무 크면 select()가 실패합니다.
   // 100만 초(11.5일)보다 크게 설정하지 마세요.
   const long MAX_TV_SEC = MILLION;
   if (tv_timeToDelay.tv_sec > MAX_TV_SEC) {
      tv_timeToDelay.tv_sec = MAX_TV_SEC;
   }
   // 또한 "maxDelayTime" 매개변수를 확인하세요(0보다 큰 경우):
   if (maxDelayTime > 0 &&
      (tv_timeToDelay.tv_sec > (long)maxDelayTime / MILLION ||
         (tv_timeToDelay.tv_sec == (long)maxDelayTime / MILLION &&
            tv_timeToDelay.tv_usec > (long)maxDelayTime % MILLION))) {
      tv_timeToDelay.tv_sec = maxDelayTime / MILLION;
      tv_timeToDelay.tv_usec = maxDelayTime % MILLION;
   }

   #pragma endregion

   //Windows Socket API(WSA, WinSock 2)
   //현재 Socket들 중에서 읽기, 쓰기, 예외 상태가 발생한 Socket들을 감지하여 반환
   int selectResult = select(fMaxNumSockets, &readSet, &writeSet, &exceptionSet, &tv_timeToDelay);

   if (selectResult < 0) 
   {
      #pragma region select 에러처리

      #if defined(__WIN32__) || defined(_WIN32)

      int err = WSAGetLastError();
      // 알 수 없는 이유로 Windows의 select() 함수가 "readSet"에 항목이 설정되지 않은 상태에서 호출되면 WSAEINVAL과 함께 실패하는 경우가 있습니다.
      // 이 경우 무시하세요.
      if (err == WSAEINVAL && readSet.fd_count == 0) {
         err = EINTR;
         // 이런 일이 다시 발생하지 않도록 하려면 더미 소켓을 만듭니다.
         if (fDummySocketNum >= 0) closeSocket(fDummySocketNum);
         fDummySocketNum = socket(AF_INET, SOCK_DGRAM, 0);
         FD_SET((unsigned)fDummySocketNum, &fReadSet);
      }
      if (err != EINTR) 
      {
      #else
      if (errno != EINTR && errno != EAGAIN) {
      #endif
         // 예기치 않은 오류 - 치명적인 오류로 처리합니다.
         #if !defined(_WIN32_WCE)
         perror("BasicTaskScheduler::SingleStep(): select() fails");
         // 이 오류는 종종 "잘못된 파일 설명자" 오류로 인해 발생합니다. 
         // 이는 "select()"에서 잘못된 소켓 번호(즉, 이미 닫힌 소켓 번호)가 사용되어 발생하기 때문입니다. 
         // 디버깅을 돕기 위해 "select()"에서 사용 중이던 소켓을 출력합니다.
         fprintf(stderr, "socket numbers used in the select() call:");

         for (int i = 0; i < 10000; ++i) {
            if (FD_ISSET(i, &fReadSet) || FD_ISSET(i, &fWriteSet) || FD_ISSET(i, &fExceptionSet)) {
               fprintf(stderr, " %d(", i);
               if (FD_ISSET(i, &fReadSet)) fprintf(stderr, "r");
               if (FD_ISSET(i, &fWriteSet)) fprintf(stderr, "w");
               if (FD_ISSET(i, &fExceptionSet)) fprintf(stderr, "e");
               fprintf(stderr, ")");
            }
         }
         fprintf(stderr, "\n");
         #endif
         internalError();
      }
      #pragma endregion
   }

   #pragma region 소켓핸들러 호출(fHandlers)

   // 읽을 수 있는 소켓 하나에 대한 핸들러 함수를 호출합니다.
   HandlerIterator iter(*fHandlers);
   HandlerDescriptor* handler;
   // 핸들러를 통해 앞으로 진행되도록 하려면 마지막으로 처리한 소켓 번호를 지나서 시작하세요.
   if (fLastHandledSocketNum >= 0)
   {
      while ((handler = iter.next()) != NULL) {
         if (handler->socketNum == fLastHandledSocketNum) break;
      }
      if (handler == NULL) {
         fLastHandledSocketNum = -1;
         iter.reset(); // start from the beginning instead
      }
   }
   while ((handler = iter.next()) != NULL)
   {
      int sock = handler->socketNum; // alias
      int resultConditionSet = 0;
      if (FD_ISSET(sock, &readSet) && FD_ISSET(sock, &fReadSet)/*sanity check*/) resultConditionSet |= SOCKET_READABLE;
      if (FD_ISSET(sock, &writeSet) && FD_ISSET(sock, &fWriteSet)/*sanity check*/) resultConditionSet |= SOCKET_WRITABLE;
      if (FD_ISSET(sock, &exceptionSet) && FD_ISSET(sock, &fExceptionSet)/*sanity check*/) resultConditionSet |= SOCKET_EXCEPTION;
      if ((resultConditionSet & handler->conditionSet) != 0 && handler->handlerProc != NULL) {
         fLastHandledSocketNum = sock;
         // 참고: 핸들러가 "doEventLoop()"를 재진입 방식으로 호출하는 경우를 대비해 
         // 핸들러를 호출하기 전에 "fLastHandledSocketNum"을 설정합니다.
         (*handler->handlerProc)(handler->clientData, resultConditionSet);
         break;
      }
   }
   if (handler == NULL && fLastHandledSocketNum >= 0)
   {
      // 핸들러를 호출하지 않았지만, 모든 핸들러를 확인하지 못했습니다.
      // 따라서 처음부터 다시 시도해 보세요.
      iter.reset();
      while ((handler = iter.next()) != NULL) {
         int sock = handler->socketNum; // alias
         int resultConditionSet = 0;
         if (FD_ISSET(sock, &readSet) && FD_ISSET(sock, &fReadSet)/*sanity check*/) resultConditionSet |= SOCKET_READABLE;
         if (FD_ISSET(sock, &writeSet) && FD_ISSET(sock, &fWriteSet)/*sanity check*/) resultConditionSet |= SOCKET_WRITABLE;
         if (FD_ISSET(sock, &exceptionSet) && FD_ISSET(sock, &fExceptionSet)/*sanity check*/) resultConditionSet |= SOCKET_EXCEPTION;
         if ((resultConditionSet & handler->conditionSet) != 0 && handler->handlerProc != NULL) {
            fLastHandledSocketNum = sock;
            // 참고: 핸들러가 "doEventLoop()"를 재진입 방식으로 호출하는 경우를 대비해 
            // 핸들러를 호출하기 전에 "fLastHandledSocketNum"을 설정합니다.
            (*handler->handlerProc)(handler->clientData, resultConditionSet);
            break;
         }
      }
      if (handler == NULL) fLastHandledSocketNum = -1;//because we didn't call a handler
   }

   #pragma endregion


   #pragma region 이벤트트리거 호출(fTriggeredEventHandlers)

   // 새로 트리거된 이벤트도 처리합니다. (소켓 핸들러를 호출한 *후*에 이 작업을 수행합니다.
   // 트리거된 이벤트 핸들러가 읽을 수 있는 소켓 세트를 수정하는 경우를 대비해서입니다.)
   if (fEventTriggersAreBeingUsed)
   {
      // 처리가 필요한 이벤트 트리거를 찾습니다(가능한 모든 트리거를 통해 계속 진행되도록 합니다).
      unsigned i = fLastUsedTriggerNum;
      EventTriggerId mask = fLastUsedTriggerMask;

      do {
         i = (i + 1) % MAX_NUM_EVENT_TRIGGERS;
         mask >>= 1;
         if (mask == 0) mask = EVENT_TRIGGER_ID_HIGH_BIT;

#ifndef NO_STD_LIB
         if (fTriggersAwaitingHandling[i].test()) {
            fTriggersAwaitingHandling[i].clear();
#else
         if (fTriggersAwaitingHandling[i]) {
            fTriggersAwaitingHandling[i] = False;
#endif
            if (fTriggeredEventHandlers[i] != NULL) {
               (*fTriggeredEventHandlers[i])(fTriggeredEventClientDatas[i]);
            }

            fLastUsedTriggerMask = mask;
            fLastUsedTriggerNum = i;
            break;
         }
      } while (i != fLastUsedTriggerNum);
   }

   #pragma endregion

   // 또한 예정된 대로 지연된 이벤트도 처리합니다.
   fDelayQueue.handleAlarm();
}

void BasicTaskScheduler::setBackgroundHandling(int socketNum, int conditionSet, BackgroundHandlerProc * handlerProc, void* clientData) {
   if (socketNum < 0) return;
#if !defined(__WIN32__) && !defined(_WIN32) && defined(FD_SETSIZE)
   if (socketNum >= (int)(FD_SETSIZE)) return;
#endif
   FD_CLR((unsigned)socketNum, &fReadSet);
   FD_CLR((unsigned)socketNum, &fWriteSet);
   FD_CLR((unsigned)socketNum, &fExceptionSet);
   if (conditionSet == 0) {
      fHandlers->clearHandler(socketNum);
      if (socketNum + 1 == fMaxNumSockets) {
         --fMaxNumSockets;
      }
   }
   else {
      fHandlers->assignHandler(socketNum, conditionSet, handlerProc, clientData);
      if (socketNum + 1 > fMaxNumSockets) {
         fMaxNumSockets = socketNum + 1;
      }
      if (conditionSet & SOCKET_READABLE) FD_SET((unsigned)socketNum, &fReadSet);
      if (conditionSet & SOCKET_WRITABLE) FD_SET((unsigned)socketNum, &fWriteSet);
      if (conditionSet & SOCKET_EXCEPTION) FD_SET((unsigned)socketNum, &fExceptionSet);
   }
}

void BasicTaskScheduler::moveSocketHandling(int oldSocketNum, int newSocketNum) {
   if (oldSocketNum < 0 || newSocketNum < 0) return; // sanity check
#if !defined(__WIN32__) && !defined(_WIN32) && defined(FD_SETSIZE)
   if (oldSocketNum >= (int)(FD_SETSIZE) || newSocketNum >= (int)(FD_SETSIZE)) return; // sanity check
#endif
   if (FD_ISSET(oldSocketNum, &fReadSet)) { FD_CLR((unsigned)oldSocketNum, &fReadSet); FD_SET((unsigned)newSocketNum, &fReadSet); }
   if (FD_ISSET(oldSocketNum, &fWriteSet)) { FD_CLR((unsigned)oldSocketNum, &fWriteSet); FD_SET((unsigned)newSocketNum, &fWriteSet); }
   if (FD_ISSET(oldSocketNum, &fExceptionSet)) { FD_CLR((unsigned)oldSocketNum, &fExceptionSet); FD_SET((unsigned)newSocketNum, &fExceptionSet); }
   fHandlers->moveHandler(oldSocketNum, newSocketNum);

   if (oldSocketNum + 1 == fMaxNumSockets) {
      --fMaxNumSockets;
   }
   if (newSocketNum + 1 > fMaxNumSockets) {
      fMaxNumSockets = newSocketNum + 1;
   }
}
