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


#include <cstdint>
#include <cstdio>
#include <cstring>

#ifdef WIN32

#else
  #include <unistd.h>
#endif

#include "channel.h"
#include "rtsp.h"
#include "socket.h"

using namespace satip;

RTSPOptions::RTSPOptions(Socket *socket){
  _Descripe=false;
  _Setup=false;
  _Play=false;
  _Teardown=false;
  if(!socket){
    return;
  }
  char sendbuf[SENDBUFFER]="\0";
  snprintf(sendbuf,SENDBUFFER,"OPTIONS rtsp://%s:%d/ RTSP/1.0\r\nCSeq: %zu\r\n\r\n",socket->getAddr(),socket->getPort(),socket->getSequence());
  socket->sendData((const unsigned char*)sendbuf,strlen(sendbuf));
  socket->reciveData();
  size_t socksize=socket->getDataSize();
  if(socksize==0)
    return;
  char *recvbuf=new char[socksize+1];
  memcpy(recvbuf,socket->getData(),socksize+1);
  socket->clearData();
#ifdef DEBUG
  fprintf(stdout,"Recv:\n%s\n",recvbuf);
#endif
  if(!strstr(recvbuf,"RTSP/1.0 200 OK\r\n"))
    return;
  char *tokbuf=NULL,*tokptr=NULL;
  for(tokbuf=strtok_r(recvbuf,"\n",&tokptr); tokbuf; tokbuf=strtok_r(NULL,"\n",&tokptr)){
    if(strstr(tokbuf,"Public:") && strstr(tokbuf,"OPTIONS")){
      if(strstr(tokbuf,"PLAY"))
        _Play=true;
      if(strstr(tokbuf,"SETUP"))
        _Setup=true;
      if(strstr(tokbuf,"TEARDOWN"))
        _Teardown=true;
      if(strstr(tokbuf,"DESCRIPE"))
        _Descripe=true;
    }
  }
  delete[] recvbuf;
}

RTSPOptions::~RTSPOptions(){

}

bool RTSPOptions::isDescripeable(){
  return _Descripe;
}

bool RTSPOptions::isPlayable(){
  return _Play;
}

bool RTSPOptions::isSetupable(){
  return _Setup;
}

bool RTSPOptions::isTeardownable(){
  return _Teardown;
}

RTSPTransport::RTSPTransport(){
  _Type=RTP_AVR;
  _Destination=NULL;
  _UniCast=true;
  _startPort=52042;
  _endPort=52043;
  _Transport=NULL;
}

RTSPTransport::~RTSPTransport(){
  if(_Destination)
    delete[] _Destination;
  if(_Transport)
    delete[] _Transport;
}

void RTSPTransport::setType(int type){
  _Type=type;
}

int RTSPTransport::getType(){
  return _Type;
}

void RTSPTransport::setDestination(const char *destination){
  if(_Destination)
    delete[] _Destination;
  if(destination){
    _Destination=new char[strlen(destination)+1];
    memcpy(_Destination,destination,strlen(destination)+1);
  }
}

const char *RTSPTransport::getDestination(){
  return _Destination;
}

void RTSPTransport::setUniCast(bool uni){
  _UniCast=uni;
}

bool RTSPTransport::getUniCast(){
  return _UniCast;
}

void RTSPTransport::setstartPort(int port){
  _startPort=port;
}

int RTSPTransport::getstartPort(){
  return _startPort;
}

void RTSPTransport::setendPort(int port){
  _endPort=port;
}

int RTSPTransport::getendPort(){
  return _endPort;
}

const char *RTSPTransport::_getTransport(){
  if(_Transport)
    delete _Transport;
  _Transport=new char[255];
  switch(_Type){
    case RTP_AVR:{
      if(_UniCast)
        snprintf(_Transport,255,"Transport: RTP/AVP;unicast;client_port=%d-%d\r\n",_startPort,_endPort);
      break;
    }
  }
  return _Transport;
}

void RTSPTransport::_parseTransport(const char *trans){
  if(!trans)
    return;
  char *transbuff=new char[strlen(trans)+1];
  memcpy(transbuff,trans,strlen(trans)+1);
  char *transptr=NULL;
  setUniCast(false);
   for(char *subtrans=strtok_r(transbuff,";",&transptr); subtrans; subtrans=strtok_r(NULL,";",&transptr)){
     if(strstr(subtrans,"Transport")){
       if(strstr(subtrans,"RTP/AVP")){
         setType(RTP_AVR);
       }
     }else if(strstr(trans,"unicast")){
       setUniCast(true);
     }else if(strstr(trans,"client_port")){
       int start=0,end=0;
       sscanf(trans,"client_port=%d-%d",&start,&end);
       setstartPort(start);
       setendPort(end);
     }else if(strstr(trans,"Destination")){
       setDestination(trans+12);
     }
   }
   delete[] transbuff;
}

RTSPSession::RTSPSession(Socket *socket,RTSPOptions *rtspoptions,RTSPTransport *transport,
                         const char* rtspurl){
  _Socket=socket;
  _Session=NULL;
  _StreamID=0;
  if(!rtspoptions->isSetupable() || rtspurl==NULL)
    return;
  char sendbuf[SENDBUFFER];
  snprintf(sendbuf,SENDBUFFER,"SETUP rtsp://%s:%d/%s RTSP/1.0\r\nCSeq: %zu\r\n%s\r\n",
           _Socket->getAddr(),_Socket->getPort(),rtspurl,_Socket->getSequence(),transport->_getTransport());
  _Socket->sendData((unsigned char*)sendbuf,strlen(sendbuf));
  _Socket->reciveData();
  const char *recvbuf=_Socket->getData();
  if(!strstr(recvbuf,"RTSP/1.0 200 OK\r\n"))
    return;
  transport->_parseTransport(recvbuf);
  char *transbuff=new char[strlen(recvbuf)+1];
  memcpy(transbuff,recvbuf,strlen(recvbuf)+1);
  char *transptr=NULL;
  for(char *subtrans=strtok_r(transbuff,";",&transptr); subtrans; subtrans=strtok_r(NULL,";",&transptr)){
    char *sesval=strstr(subtrans,"Session:");
    if(sesval){
      sesval=sesval+8;
      _Session=new char[strlen(sesval)+1];
      memcpy(_Session,sesval,strlen(sesval)+1);
    }else if(char *sesval=strstr(subtrans,"com.ses.streamID:")){
      sscanf(sesval,"com.ses.streamID:%d",&_StreamID);
    }
  }
  delete[] transbuff;
  _Socket->clearData();
}

RTSPSession::~RTSPSession(){
  Teardown();
  if(_Session)
    delete[] _Session;
}

const char* RTSPSession::getStreamURL(){
  return NULL;
}

void RTSPSession::Play(){
  char sendbuf[SENDBUFFER];
  snprintf(sendbuf,SENDBUFFER,"PLAY rtsp://%s:%d/stream=%d RTSP/1.0\r\nCSeq: %zu\r\nSession: %s\r\n\r\n",
           _Socket->getAddr(),_Socket->getPort(),_StreamID,_Socket->getSequence(),_Session);
  _Socket->sendData(sendbuf);
}

void RTSPSession::Teardown(){
  char sendbuf[SENDBUFFER];
  snprintf(sendbuf,SENDBUFFER,"TEARDOWN rtsp://%s:%d/stream=%d RTSP/1.0\r\nCSeq: %zu\r\nSession: %s\r\n\r\n",
           _Socket->getAddr(),_Socket->getPort(),_StreamID,_Socket->getSequence(),_Session);
  _Socket->sendData(sendbuf);
  while(_Socket->reciveData()){
    if(strstr(_Socket->getData(),"RTSP/1.0 200 OK\r\n"))
      return;
  }
}

RTSP::RTSP(Socket *socket){
  _Socket=socket;
  _RTSPOptions=new RTSPOptions(_Socket);
  _RTSPSession=NULL;
}

RTSP::~RTSP(){
  closeRTSPSession();
  delete _RTSPOptions;
}

RTSPOptions* RTSP::getRTSPOptions(){
  return _RTSPOptions;
}

RTSPSession* RTSP::setupRTSPSession(RTSPTransport *transport,const char* rtspurl){
  closeRTSPSession();
  _RTSPSession=new RTSPSession(_Socket,_RTSPOptions,transport,rtspurl);
  return _RTSPSession;
}

void RTSP::closeRTSPSession(){
  if(_RTSPSession)
    delete _RTSPSession;
}
