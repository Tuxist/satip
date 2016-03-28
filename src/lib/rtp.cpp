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


#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include "rtp.h"
#include "socket.h"

using namespace satip;

RTPHeader::RTPHeader(){
  _ExtensionData=NULL;
}

RTPHeader::~RTPHeader(){
  if(_ExtensionData)
    delete _ExtensionData;
}

int8_t RTPHeader::getVersion(){
  return _Version;
}

int8_t RTPHeader::getPadding(){
  return _Padding;
}

bool RTPHeader::isExtension(){
  return _Extension;
}

size_t RTPHeader::getCC(){
  return _CC;
}

int RTPHeader::getCSRC() {
  return _CSRC;
}

int RTPHeader::getMarker(){
  return _Marker;
}

int RTPHeader::getPayload(){
  return _Payload;
}

size_t RTPHeader::getSequence(){
  return _Sequence;
}

int RTPHeader::getTimestap(){
  return _Timestamp;
}

int RTPHeader::getSSRC(){
  return _SSRC;
}

unsigned char *RTPHeader::getExtensionData(){
  return _ExtensionData;
}

size_t *RTPHeader::getExtensionSize(){
  return _ExtensionSize;
}

RTPHeader *RTPFrame::getRTPHeader(){
  return _RTPHeader;
}

unsigned char *RTPFrame::getData(){
  return _Data;
}

size_t RTPFrame::getDataSize(){
  return _DataSize;
}

RTPFrame *RTPFrame::prevRTPFrame(){
  return _prevRTPFrame;
}

RTPFrame *RTPFrame::nextRTPFrame(){
  return _nextRTPFrame;
}

RTPFrame::RTPFrame(){
  _FrameLock.lock();
  _RTPHeader=NULL;
  _prevRTPFrame=NULL;
  _nextRTPFrame=NULL;
  _Data=NULL;
}

RTPFrame::~RTPFrame(){
  if(_Data)
    delete _Data;
}

RTP::RTP(Socket *socket){
  if(!socket)
    throw "Invalid Socket";
  _Socket=socket;
  std::thread t1(&RTP::_readStream,this);
  t1.detach();
}

RTP::~RTP(){
  if(_firstRTPFrame)
    delete _firstRTPFrame;
}

void RTP::_readStream(){
  for(;;){
    if(_Socket->reciveData(512)<0){
    if(!_firstRTPFrame){
      _firstRTPFrame=new RTPFrame;
      _lastRTPFrame=_firstRTPFrame;
    }else{
      _lastRTPFrame->_nextRTPFrame=new RTPFrame;
      _lastRTPFrame->_nextRTPFrame->_prevRTPFrame=_lastRTPFrame;
      _lastRTPFrame=_lastRTPFrame->_nextRTPFrame;
    }
    const unsigned char *data=_Socket->getRawData();
    size_t datasize=_Socket->getDataSize();
    _lastRTPFrame->_FrameLock.unlock();
    }else{
      usleep(1000);
    }
  }
}

RTPFrame *RTP::getfirstRTPFrame(){
  return _firstRTPFrame;
}

RTPFrame *RTP::getlastRTPFrame(){
  return _lastRTPFrame;
}

RTPFrame *RTP::deleteFrame(RTPFrame *delframe){
  
}