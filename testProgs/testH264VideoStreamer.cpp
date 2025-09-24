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
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********/
// Copyright (c) 1996-2025, Live Networks, Inc.  All rights reserved
// A test program that reads a H.264 Elementary Stream video file
// and streams it using RTP
// main program
//
// NOTE: For this application to work, the H.264 Elementary Stream video file *must* contain SPS and PPS NAL units,
// ideally at or near the start of the file.  These SPS and PPS NAL units are used to specify 'configuration' information
// that is set in the output stream's SDP description (by the RTSP server that is built in to this application).
// Note also that - unlike some other "*Streamer" demo applications - the resulting stream can be received only using a
// RTSP client (such as "openRTSP")

#include <liveMedia.hh>

#include <BasicUsageEnvironment.hh>
#include "announceURL.hh"
#include <GroupsockHelper.hh>

UsageEnvironment* env;
char const* inputFileName = "test.264";
H264VideoStreamFramer* videoSource;
RTPSink* videoSink;

void play(); // 전달

int main(int argc, char** argv)
{
   // 먼저 사용 환경을 설정합니다.
   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   env = BasicUsageEnvironment::createNew(*scheduler);

   // RTP 및 RTCP용 'groupsocks'를 생성합니다.
   struct sockaddr_storage destinationAddress;
   destinationAddress.ss_family = AF_INET;
   ((struct sockaddr_in&)destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env);
   // 참고: 이는 멀티캐스트 주소입니다. 유니캐스트를 사용하여 스트리밍하려면
   // "testOnDemandRTSPServer"를 사용해야 합니다.
   // 이 테스트 프로그램이 아닌 "testOnDemandRTSPServer" 테스트 프로그램을 모델로 사용해야 합니다.

   const unsigned short rtpPortNum = 18888;
   const unsigned short rtcpPortNum = rtpPortNum+1;
   const unsigned char ttl = 255;

   const Port rtpPort(rtpPortNum);
   const Port rtcpPort(rtcpPortNum);

   Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
   rtpGroupsock.multicastSendOnly(); // SSM 소스입니다.
   Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
   rtcpGroupsock.multicastSendOnly(); // SSM 소스입니다.

   // RTP 'groupsock'에서 'H264 비디오 RTP' 싱크를 생성합니다.
   OutPacketBuffer::maxSize = 100000;
   videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

   // 이 RTP 싱크에 대한 'RTCP 인스턴스'를 생성하고 시작합니다.
   const unsigned estimatedSessionBandwidth = 500; // kbps 단위; RTCP 양방향 공유의 경우
   const unsigned maxCNAMElen = 100;
   unsigned char CNAME[maxCNAMElen+1];
   gethostname((char*)CNAME, maxCNAMElen);
   CNAME[maxCNAMElen] = '\0'; // 만약을 대비하여
   RTCPInstance* rtcp
      = RTCPInstance::createNew(*env, &rtcpGroupsock,
         estimatedSessionBandwidth, CNAME,
         videoSink, NULL /* 서버입니다. */,
         True /* SSM 소스입니다. */);
   // 참고: 이렇게 하면 RTCP가 자동으로 실행됩니다.

   RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
   if (rtspServer == NULL) {
      *env << "RTSP 서버를 생성하지 못했습니다. " << env->getResultMsg() << "\n";
      exit(1);
   }
   ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, "testStream", inputFileName,
         "세션이 \"testH264VideoStreamer\"에 의해 스트리밍되었습니다.",
         True /*SSM*/);
   sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
   rtspServer->addServerMediaSession(sms);
   announceURL(rtspServer, sms);

   // 스트리밍 시작:
   *env << "스트리밍 시작...\n";
   play();

   env->taskScheduler().doEventLoop(); // 반환하지 않음

   return 0; // 컴파일러 경고 방지용
}

void afterPlaying(void* /*clientData*/) {
   *env << "...파일 읽기 완료\n";
   videoSink->stopPlaying();
   Medium::close(videoSource);
   // 이 소스가 읽어들인 입력 파일도 닫힙니다.

   // 다시 재생을 시작합니다.
   play();
}

void play() {
   // 입력 파일을 '바이트 스트림 파일 소스'로 엽니다.
   ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(*env, inputFileName);
   if (fileSource == NULL)
   {
      *env << "파일 \"을(를) 열 수 없습니다." << inputFileName
         << "\"을(를) 바이트 스트림 파일 소스로\n";
      exit(1);
   }

   FramedSource* videoES = fileSource;

   // 비디오 기본 스트림에 대한 프레이머를 생성합니다.
   videoSource = H264VideoStreamFramer::createNew(*env, videoES);

   // 마지막으로 재생을 시작합니다.
  *env << "Beginning to read from file...\n";
  videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}
