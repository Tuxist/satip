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


#ifndef RTSP_H
#define RTSP_H

#define HEADERSIZE 8192
#define SENDBUFFER 8192

/*Transport types*/
#define RTP_AVR 1

namespace satip {
  class Channel;
  class Socket;

  class RTSPOptions{
  public:
    bool isDescripeable();
    bool isSetupable();
    bool isPlayable();
    bool isTeardownable();
  private:
    RTSPOptions(Socket *socket);
    ~RTSPOptions();
    bool _Descripe; 
    bool _Setup; 
    bool _Play; 
    bool _Teardown;
    friend class RTSP;
  };

  class RTSPTransport{
  public:
    RTSPTransport();
    ~RTSPTransport();
    void        setType(int type);
    int         getType();
    void        setDestination(const char *destination);
    const char *getDestination();
    void        setUniCast(bool uni);
    bool        getUniCast();
    void        setstartPort(int port);
    int         getstartPort();
    void        setendPort(int port);
    int         getendPort();
  private:
    int         _Type;
    char       *_Destination;
    bool        _UniCast;
    int         _startPort;
    int         _endPort;
    char       *_Transport;
    const char *_getTransport();
    void        _parseTransport(const char *trans);
    friend class RTSPSession;
  };
  
  class RTSPSession{
  public:
    const char  *getStreamURL();
    void         Play();
    void         Teardown();
  private:
    char          *_Session;
    int            _Timeout;
    int            _StreamID;
    int            _ClientPort;
    RTSPSession(Socket *socket,RTSPOptions *rtspoptions,RTSPTransport *transport,const char *rtspurl);
    ~RTSPSession();
    Socket        *_Socket;
    RTSPOptions   *_RTSPOptions;
    friend class RTSP;
  };

  class RTSP {
  public:
    RTSP(Socket *socket);
    ~RTSP();
    RTSPOptions      *getRTSPOptions();
    RTSPSession      *setupRTSPSession(RTSPTransport *transport,const char *rtspurl);
    void              closeRTSPSession();
  private:
    char             *_Url;
    const char       *_Host;
    int               _Port;
    Socket           *_Socket;
    RTSPOptions      *_RTSPOptions;
    RTSPSession      *_RTSPSession;
  };
};

#endif
 
