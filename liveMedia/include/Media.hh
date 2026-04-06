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
// "liveMedia"
// Copyright (c) 1996-2025 Live Networks, Inc.  All rights reserved.
// Medium
// C++ header

#ifndef _MEDIA_HH
#define _MEDIA_HH

#ifndef _LIVEMEDIA_VERSION_HH
#include "liveMedia_version.hh"
#endif

#ifndef _HASH_TABLE_HH
#include "HashTable.hh"
#endif

#ifndef _USAGE_ENVIRONMENT_HH
#include "UsageEnvironment.hh"
#endif

// Lots of files end up needing the following, so just #include them here:
#ifndef _NET_COMMON_H
#include "NetCommon.h"
#endif
#include <stdio.h>

// The following makes the Borland compiler happy:
#ifdef __BORLANDC__
#define _strnicmp strnicmp
#define fabsf(x) fabs(x)
#endif

#define mediumNameMaxLen 30

//liveMedia 계층의 루트(base) 클래스. 대부분의 미디어 관련 클래스(MediaSource, MediaSink, MediaSession, RTCPInstance, RTSPClient/Server 등)가 Medium을 상속
//1. 환경 보관: UsageEnvironment& envir()
//2. 이름 보관: char const* name() const — 등록된 미디엄 이름을 반환합니다(fMediumName). 
//3. 생성자/소멸자 둘 다 protected → 직접 생성/삭제가 아닌 서브클래스의 createNew() 및 **Medium::close()**로 수명 관리
//4. 이름 기반 조회: lookupByName. 환경 내 등록 테이블에서 이름으로 객체 검색
//5. 타입 질의(가벼운 RTTI): isSource(), isMediaSession(), isRTCPInstance() 등
//6. 스케줄링 훅: TaskToken& nextTask() — 이벤트 루프 스케줄링에서 사용
class LIVEMEDIA_API Medium 
{
public:
  static Boolean lookupByName(UsageEnvironment& env,
			      char const* mediumName,
			      Medium*& resultMedium);
  static void close(UsageEnvironment& env, char const* mediumName);
  static void close(Medium* medium); // alternative close() method using ptrs
      // (has no effect if medium == NULL)

  UsageEnvironment& envir() const {return fEnviron;}

  char const* name() const {return fMediumName;}

  // Test for specific types of media:
  virtual Boolean isSource() const;
  virtual Boolean isSink() const;
  virtual Boolean isRTCPInstance() const;
  virtual Boolean isRTSPClient() const;
  virtual Boolean isRTSPServer() const;
  virtual Boolean isMediaSession() const;
  virtual Boolean isServerMediaSession() const;

protected:
  friend class MediaLookupTable;
  Medium(UsageEnvironment& env); // abstract base class
  virtual ~Medium(); // instances are deleted using close() only

  TaskToken& nextTask() {
	return fNextTask;
  }

private:
  UsageEnvironment& fEnviron;
  char fMediumName[mediumNameMaxLen];
  TaskToken fNextTask;
};


// 문자열 이름으로 매체를 조회하는 데이터 구조입니다.
// ("Medium"을 구현하는 데만 사용되지만, 개발자가 생성한 "Medium" 객체 전체를 반복하는 데 사용할 수 있도록 여기에 표시합니다.)
class LIVEMEDIA_API MediaLookupTable 
{
public:
  static MediaLookupTable* ourMedia(UsageEnvironment& env);
  HashTable const& getTable() { return *fTable; }

protected:
  MediaLookupTable(UsageEnvironment& env);
  virtual ~MediaLookupTable();

private:
  friend class Medium;

  Medium* lookup(char const* name) const;
  // Returns NULL if none already exists

  void addNew(Medium* medium, char* mediumName);
  void remove(char const* name);

  void generateNewName(char* mediumName, unsigned maxLen);

private:
  UsageEnvironment& fEnv;
  HashTable* fTable;
  unsigned fNameGenerator;
};


// The structure pointed to by the "liveMediaPriv" UsageEnvironment field:
class _Tables {
public:
  static _Tables* getOurTables(UsageEnvironment& env, Boolean createIfNotPresent = True);
      // returns a pointer to a "_Tables" structure (creating it if necessary)
  void reclaimIfPossible();
      // used to delete ourselves when we're no longer used

  MediaLookupTable* mediaTable;
  void* socketTable;

protected:
  _Tables(UsageEnvironment& env);
  virtual ~_Tables();

private:
  UsageEnvironment& fEnv;
};

#endif
