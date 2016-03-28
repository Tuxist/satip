/*******************************************************************************
Copyright 2013  Jan Koester jan.koester@gmx.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either 
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "satip.h"
#include <cstdio>
#include <cstring>
#include "../channel.h"
#include "../confparser.h"

satip::SATIPDevice::SATIPDevice(){
  _ID=-1;
  _Addr=NULL;
  _Port=0;
  _Url=NULL;
}

satip::SATIPDevice::~SATIPDevice(){
  if(_Addr)
    delete[] _Addr;
  if(_Url)
    delete[] _Url;
}

void satip::SATIPDevice::setAddr(const char *addr){
  if(_Addr){
    delete _Addr;
    _Addr=NULL;
  }
  if(addr){
    _Addr=new char[strlen(addr)+1];
    memcpy(_Addr,addr,strlen(addr)+1);
  }
}

const char* satip::SATIPDevice::getAddr(){
  return _Addr;
}

int satip::SATIPDevice::getID(){
  return _ID;
}

void satip::SATIPDevice::setPort(int port){
  _Port=port;
}

int satip::SATIPDevice::getPort(){
  return _Port;
}

const char* satip::SATIPDevice::getUrl(Channel *channel){
  channel->getDiseqC();
  channel->getFrequency();
  channel->getLNB();
  channel->getLNBSelection();
  channel->getSatellite();
  channel->getTunerType();
  return _Url;
}

satip::SATIPDevice* satip::SATIPDevice::nextDeviceSATIP(){
  return _nextDeviceSATIP;
}

satip::SATIPDevices::SATIPDevices(){
  _LastID=-1;
  _firstSATIPDevice=NULL;
  _lastSATIPDevice=NULL;
  _Buffer=NULL;
  _BufferDevStart=NULL;
  _BufferDevEnd=NULL;
  _BufferSize=0;
  _ConfParser = new ConfParser;
}

satip::SATIPDevices::~SATIPDevices(){
  if(_firstSATIPDevice)
    delete _firstSATIPDevice;
  if(_Buffer)
    delete[] _Buffer;
  delete _ConfParser;
}

void satip::SATIPDevices::LoadFile(const char* filepath){
  _ConfParser->Loadfile(filepath);
}

void satip::SATIPDevices::Savefile(const char* filepath){
  _ConfParser->Savefile(filepath);
}

satip::SATIPDevice* satip::SATIPDevices::addSATIPDevice(){
  if(!_firstSATIPDevice){
    _firstSATIPDevice=new SATIPDevice;
    _lastSATIPDevice=_firstSATIPDevice;
  }else{
    _lastSATIPDevice=_lastSATIPDevice->_nextDeviceSATIP= new SATIPDevice;
  }
  if(_lastSATIPDevice->getID()==-1){
    _LastID++;
    _lastSATIPDevice->_ID=_LastID;
  }
  return _lastSATIPDevice;
}

satip::SATIPDevice* satip::SATIPDevices::delSATIPDevice(SATIPDevice* deldevive){
  SATIPDevice *prev=NULL;
  for(SATIPDevice *deldev=_firstSATIPDevice; deldev; deldev=deldev->nextDeviceSATIP()){
    if(deldev==deldevive){
      if(prev)
        prev->_nextDeviceSATIP=deldev->_nextDeviceSATIP;
      else
        _firstSATIPDevice=deldev->_nextDeviceSATIP;
      deldev->_nextDeviceSATIP=NULL;
      delete deldev;
    }
    prev=deldev;
  }
  return prev;
}

satip::SATIPDevice* satip::SATIPDevices::firstSATIPDevice(){
  return _firstSATIPDevice;
}
