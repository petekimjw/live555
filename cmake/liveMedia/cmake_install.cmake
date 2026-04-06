# Install script for directory: E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/live555")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Debug" TYPE STATIC_LIBRARY FILES "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/Debug/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Release" TYPE STATIC_LIBRARY FILES "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/Release/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/MinSizeRel" TYPE STATIC_LIBRARY FILES "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/MinSizeRel/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/RelWithDebInfo" TYPE STATIC_LIBRARY FILES "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/RelWithDebInfo/liveMedia.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/liveMedia" TYPE FILE FILES
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AVIFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AudioInputDevice.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Base64.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BasicUDPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BasicUDPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BitVector.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamMemoryBufferSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamMultiFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DeviceSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DigestAuthentication.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedFilter.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/GSMAudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/GenericMediaServer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H261VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HLSSegmenter.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HMAC_hash.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HMAC_SHA1.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/InputFile.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEG2000VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEG2000VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Locale.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADU.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADURTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADURTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADUTranscoder.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADUinterleaving.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3AudioFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3FileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3Transcoder.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2Demux.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2DemuxedElementaryStream.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2DemuxedServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2FileServerDemux.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2IndexFromTransportStream.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamAccumulator.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamDemux.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFromESSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFromPESSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamIndexFile.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamMultiplexor.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamTrickModeFilter.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportUDPServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4ESVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4ESVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4GenericRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4GenericRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4LATMAudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4LATMAudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoStreamDiscreteFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEGVideoStreamFramer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MatroskaFile.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MatroskaFileServerDemux.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Media.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaTranscodingTable.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MIKEY.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MultiFramedRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MultiFramedRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFile.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFileServerDemux.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OnDemandServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OutputFile.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/PassiveServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ProxyServerMediaSession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QCELPAudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QuickTimeFileSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QuickTimeGenericRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoFrameParameters.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTCP.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPInterface.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPClient.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPCommon.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPRegisterSender.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPServer.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SIPClient.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SRTPCryptographicContext.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ServerMediaSession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SimpleRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SimpleRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/StreamReplicator.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/T140TextRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TextRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TheoraVideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TheoraVideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TLSState.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP8VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP8VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP9VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP9VideoRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VideoRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VorbisAudioRTPSink.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VorbisAudioRTPSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/WAVAudioFileServerMediaSubsession.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/WAVAudioFileSource.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/liveMedia.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/liveMedia_version.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ourMD5.hh"
    "E:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/uLawAudioFilter.hh"
    )
endif()

