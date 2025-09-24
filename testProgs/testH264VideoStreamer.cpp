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

void play(); // ����

int main(int argc, char** argv)
{
   // ���� ��� ȯ���� �����մϴ�.
   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   env = BasicUsageEnvironment::createNew(*scheduler);

   // RTP �� RTCP�� 'groupsocks'�� �����մϴ�.
   struct sockaddr_storage destinationAddress;
   destinationAddress.ss_family = AF_INET;
   ((struct sockaddr_in&)destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env);
   // ����: �̴� ��Ƽĳ��Ʈ �ּ��Դϴ�. ����ĳ��Ʈ�� ����Ͽ� ��Ʈ�����Ϸ���
   // "testOnDemandRTSPServer"�� ����ؾ� �մϴ�.
   // �� �׽�Ʈ ���α׷��� �ƴ� "testOnDemandRTSPServer" �׽�Ʈ ���α׷��� �𵨷� ����ؾ� �մϴ�.

   const unsigned short rtpPortNum = 18888;
   const unsigned short rtcpPortNum = rtpPortNum+1;
   const unsigned char ttl = 255;

   const Port rtpPort(rtpPortNum);
   const Port rtcpPort(rtcpPortNum);

   Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
   rtpGroupsock.multicastSendOnly(); // SSM �ҽ��Դϴ�.
   Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
   rtcpGroupsock.multicastSendOnly(); // SSM �ҽ��Դϴ�.

   // RTP 'groupsock'���� 'H264 ���� RTP' ��ũ�� �����մϴ�.
   OutPacketBuffer::maxSize = 100000;
   videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

   // �� RTP ��ũ�� ���� 'RTCP �ν��Ͻ�'�� �����ϰ� �����մϴ�.
   const unsigned estimatedSessionBandwidth = 500; // kbps ����; RTCP ����� ������ ���
   const unsigned maxCNAMElen = 100;
   unsigned char CNAME[maxCNAMElen+1];
   gethostname((char*)CNAME, maxCNAMElen);
   CNAME[maxCNAMElen] = '\0'; // ������ ����Ͽ�
   RTCPInstance* rtcp
      = RTCPInstance::createNew(*env, &rtcpGroupsock,
         estimatedSessionBandwidth, CNAME,
         videoSink, NULL /* �����Դϴ�. */,
         True /* SSM �ҽ��Դϴ�. */);
   // ����: �̷��� �ϸ� RTCP�� �ڵ����� ����˴ϴ�.

   RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
   if (rtspServer == NULL) {
      *env << "RTSP ������ �������� ���߽��ϴ�. " << env->getResultMsg() << "\n";
      exit(1);
   }
   ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, "testStream", inputFileName,
         "������ \"testH264VideoStreamer\"�� ���� ��Ʈ���ֵǾ����ϴ�.",
         True /*SSM*/);
   sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
   rtspServer->addServerMediaSession(sms);
   announceURL(rtspServer, sms);

   // ��Ʈ���� ����:
   *env << "��Ʈ���� ����...\n";
   play();

   env->taskScheduler().doEventLoop(); // ��ȯ���� ����

   return 0; // �����Ϸ� ��� ������
}

void afterPlaying(void* /*clientData*/) {
   *env << "...���� �б� �Ϸ�\n";
   videoSink->stopPlaying();
   Medium::close(videoSource);
   // �� �ҽ��� �о���� �Է� ���ϵ� �����ϴ�.

   // �ٽ� ����� �����մϴ�.
   play();
}

void play() {
   // �Է� ������ '����Ʈ ��Ʈ�� ���� �ҽ�'�� ���ϴ�.
   ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(*env, inputFileName);
   if (fileSource == NULL)
   {
      *env << "���� \"��(��) �� �� �����ϴ�." << inputFileName
         << "\"��(��) ����Ʈ ��Ʈ�� ���� �ҽ���\n";
      exit(1);
   }

   FramedSource* videoES = fileSource;

   // ���� �⺻ ��Ʈ���� ���� �����̸Ӹ� �����մϴ�.
   videoSource = H264VideoStreamFramer::createNew(*env, videoES);

   // ���������� ����� �����մϴ�.
  *env << "Beginning to read from file...\n";
  videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}
