# Install script for directory: D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Debug" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/Debug/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Release" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/Release/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/MinSizeRel" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/MinSizeRel/liveMedia.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/RelWithDebInfo" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/RelWithDebInfo/liveMedia.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/liveMedia" TYPE FILE FILES
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AC3AudioStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ADTSAudioStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AMRAudioSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AVIFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AudioInputDevice.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/AudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Base64.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BasicUDPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BasicUDPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/BitVector.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamMemoryBufferSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ByteStreamMultiFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DVVideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DeviceSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/DigestAuthentication.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedFilter.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/FramedSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/GSMAudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/GenericMediaServer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H261VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H263plusVideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264VideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H264or5VideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/H265VideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HLSSegmenter.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HMAC_hash.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/HMAC_SHA1.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/InputFile.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEG2000VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEG2000VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/JPEGVideoSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Locale.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADU.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADURTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADURTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADUTranscoder.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3ADUinterleaving.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3AudioFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3FileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MP3Transcoder.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2AudioStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2Demux.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2DemuxedElementaryStream.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2DemuxedServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2FileServerDemux.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG1or2VideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2IndexFromTransportStream.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamAccumulator.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamDemux.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFromESSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamFromPESSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamIndexFile.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamMultiplexor.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportStreamTrickModeFilter.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG2TransportUDPServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4ESVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4ESVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4GenericRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4GenericRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4LATMAudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4LATMAudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoStreamDiscreteFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEG4VideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MPEGVideoStreamFramer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MatroskaFile.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MatroskaFileServerDemux.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/Media.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MediaTranscodingTable.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MIKEY.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MultiFramedRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/MultiFramedRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFile.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFileServerDemux.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OggFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OnDemandServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/OutputFile.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/PassiveServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ProxyServerMediaSession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QCELPAudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QuickTimeFileSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/QuickTimeGenericRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoFrameParameters.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RawVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTCP.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPInterface.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPClient.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPCommon.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPRegisterSender.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/RTSPServer.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SIPClient.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SRTPCryptographicContext.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ServerMediaSession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SimpleRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/SimpleRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/StreamReplicator.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/T140TextRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TextRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TheoraVideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TheoraVideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/TLSState.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP8VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP8VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP9VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VP9VideoRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VideoRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VorbisAudioRTPSink.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/VorbisAudioRTPSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/WAVAudioFileServerMediaSubsession.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/WAVAudioFileSource.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/liveMedia.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/liveMedia_version.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/ourMD5.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/liveMedia/include/uLawAudioFilter.hh"
    )
endif()

