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


#define PACKAGESIZE 255

#define TCP 0
#define UDP 1

#ifndef SOCKET_H
#define SOCKET_H

extern "C" {
  struct sockaddr_in;
}

namespace satip{ 
  class Socket {
  public:
    Socket(const char *addr=NULL,int port=554,int type=TCP);
    ~Socket();
    int                   getSocket();
    int                   sendData(const char* data);
    int                   sendData(const unsigned char *data,size_t datalen);
    int                   reciveData(size_t maxdata=0);
    const char           *getData();
    const unsigned char  *getRawData();
    size_t                getDataSize();
    const char           *getAddr();
    int                   getPort();
    size_t                getSequence();
    void                  clearData();
  private:
    class SocketData{
      SocketData();
      ~SocketData();
    private:
      char          _Data[PACKAGESIZE];
      size_t        _DataSize;
      SocketData   *_nextSocketData;
      friend class Socket;
    };
    SocketData  *_firstSocketdata;
    SocketData  *_lastSocketdata;
    sockaddr_in *_SockAddr;
    void         _AppendData(const char *data,size_t datasize);
    char        *_Addr;
    int          _Port;
    int          _Socket;
    char        *_Data;
    size_t       _Sequence;
  };
}

#endif