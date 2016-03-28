/*******************************************************************************
Copyright (c) 2014, Jan Koester jan.koester@gmx.net
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/


#include <stdint.h>

#ifndef RTP_H
#define RTP_H

namespace std {
  class mutex;
}

namespace satip {
  class Socket;
  
  class RTPHeader {
  public:
    int8_t         getVersion();
    int8_t         getPadding();
    bool           isExtension();
    size_t         getCC();
    int            getCSRC();
    int            getMarker();
    int            getPayload();
    size_t         getSequence();
    int            getTimestap();
    int            getSSRC();
    unsigned char *getExtensionData();
    size_t        *getExtensionSize();
  private:
    RTPHeader();
    ~RTPHeader();
    int8_t         _Version;
    int8_t         _Padding;
    bool           _Extension;
    size_t         _CC;
    int            _CSRC;
    int            _Marker;
    int            _Payload;
    size_t         _Sequence;
    int            _Timestamp;
    int            _SSRC;
    unsigned char *_ExtensionData;
    size_t        *_ExtensionSize;
    friend class   RTP;
  };
  
  class RTPFrame{
  public:
    RTPHeader     *getRTPHeader();
    unsigned char *getData();
    size_t         getDataSize();
    RTPFrame      *prevRTPFrame();
    RTPFrame      *nextRTPFrame();
  private:
    RTPFrame();
    ~RTPFrame();
    RTPHeader     *_RTPHeader;
    unsigned char *_Data;
    size_t         _DataSize;
    std::mutex     _FrameLock;
    RTPFrame      *_prevRTPFrame;
    RTPFrame      *_nextRTPFrame;
    friend class  RTP;
  };
  
  class RTP {
  public:
    RTP(Socket *socket);
    ~RTP();
    RTPFrame *getfirstRTPFrame();
    RTPFrame *getlastRTPFrame();
    RTPFrame *deleteFrame(RTPFrame *delframe);
  private:
    void       _readStream();
    RTPFrame  *_firstRTPFrame;
    RTPFrame  *_lastRTPFrame;
    Socket    *_Socket;
  };
}
#endif