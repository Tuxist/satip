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

#include <cstdio>
#include <cstring>

#ifdef WIN32
  #include <io.h> 
#else
  #include <unistd.h>
  #include <sys/errno.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "socket.h"

using namespace satip;

Socket::Socket(const char* addr, int port,int type){
  _Port=port;
  _Data=NULL;
  _Addr=NULL;
  _Sequence=1;
  _firstSocketdata=NULL;
  _lastSocketdata=NULL;
  if(!addr){
    return;
  }
  _Addr=new char[strlen(addr)+1];
  mempcpy(_Addr,addr,strlen(addr)+1);
  _SockAddr = new sockaddr_in;
  int adrfm=AF_INET;
  if(strstr(_Addr,":")){
    adrfm=AF_INET6;
  }else if(strstr(_Addr,"/")){
    adrfm=AF_UNIX;
  }
  _SockAddr->sin_family=adrfm;
  switch(type){
    case TCP:
      _Socket=socket(adrfm, SOCK_STREAM,IPPROTO_TCP);
      break;
    case UDP:
      _Socket=socket(adrfm, SOCK_DGRAM,IPPROTO_UDP);
      break;
  }
  if(_Socket<0){
    perror("Can't bind Port %d: %s !\n");
    return;
  }
  if(inet_aton(_Addr,&_SockAddr->sin_addr) == 0){
    perror("Can't set Address %s: %s !\n");
    return;
  }
  _SockAddr->sin_port=htons(_Port);
  if(connect(_Socket,(struct sockaddr*)_SockAddr,sizeof(*_SockAddr))<0){
    perror("Can't connect to Socket %d: %s !\n");
    return;
  }
}

Socket::~Socket(){
  close(_Socket);
  if(_Data)
    delete[] _Data;
  if(_Addr)
    delete[] _Addr;
  if(_SockAddr)
    delete _SockAddr;
  if(_firstSocketdata)
    delete _firstSocketdata;
}

int Socket::getSocket(){
  return _Socket;
}

int Socket::sendData(const unsigned char* data, size_t datalen){
  int sendstate=0;
  sendstate=send(_Socket,data,datalen,0);
  _Sequence++;
  return sendstate;
}

int Socket::sendData(const char* data){
  return sendData((const unsigned char*)data,strlen(data));
}

int Socket::reciveData(size_t maxdata){
  size_t recived=0;
  clearData();
  if(_firstSocketdata)
    delete _firstSocketdata;
  _firstSocketdata=NULL;
  _lastSocketdata=NULL;
  ssize_t currecv=0;
  do{
    char buffer[PACKAGESIZE];
    currecv=recv(_Socket,buffer,PACKAGESIZE,0);
    if(maxdata!=0 && (size_t)currecv < maxdata)
      break;
    recived+=currecv;
    _AppendData(buffer,currecv);
  }while(currecv<0);
  return recived;
}

void Socket::clearData(){
  if(_Data)
    delete[] _Data;
  _Data=NULL;
}


void Socket::_AppendData(const char* data, size_t datasize){
  if(!_firstSocketdata){
    _firstSocketdata=new Socket::SocketData;
    _lastSocketdata=_firstSocketdata;
  }else{
    _lastSocketdata->_nextSocketData=new Socket::SocketData;
    _lastSocketdata=_lastSocketdata->_nextSocketData;
  }
  _lastSocketdata->_DataSize=datasize;
  memcpy(_lastSocketdata->_Data,data,_lastSocketdata->_DataSize);
  _lastSocketdata->_nextSocketData=NULL;
}

const unsigned char* Socket::getRawData(){
  size_t sdat=getDataSize();
  if(sdat==0)
    return NULL;
  if(_Data)
    delete[] _Data;
  _Data = new char[sdat];
  size_t curpos=0;
  for(Socket::SocketData *curdata=_firstSocketdata; curdata; curdata=curdata->_nextSocketData){
    memcpy(_Data+curpos,curdata->_Data,curdata->_DataSize);
    curpos+=curdata->_DataSize;
  }
  return (unsigned char*)_Data;
}


const char* Socket::getData(){
  size_t sdat=getDataSize();
  if(sdat==0)
    return NULL;
  if(_Data)
    delete[] _Data;
  _Data=NULL;
  _Data = new char[sdat+1];
  size_t curpos=0;
  for(Socket::SocketData *curdata=_firstSocketdata; curdata; curdata=curdata->_nextSocketData){
    memcpy(_Data+curpos,curdata->_Data,curdata->_DataSize);
    curpos+=curdata->_DataSize;
  }
  _Data[sdat]='\0';
  return _Data;
}

size_t Socket::getDataSize(){
  size_t ds=0;
  for(Socket::SocketData *curdata=_firstSocketdata; curdata; curdata=curdata->_nextSocketData){
    ds+=curdata->_DataSize;
  }
  return ds;
}

const char* Socket::getAddr(){
  return _Addr;
}

int Socket::getPort(){
  return _Port;
}

size_t Socket::getSequence(){
  return _Sequence;
}

Socket::SocketData::SocketData(){
  _DataSize=0;
  _nextSocketData=NULL;
}

Socket::SocketData::~SocketData(){
  if(_nextSocketData)
    delete  _nextSocketData;
}
