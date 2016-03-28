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

#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <cstdio>

#include "channel.h"

using namespace satip;

Channel::Channel(){
  _nextChannel=NULL;
}

Channel::~Channel(){
  delete _nextChannel;
}

const char *Channel::getName(){
  return _ChannelData.Name;
}

const char *Channel::getSatellite(){
  return _ChannelData.Satellite;
}

int Channel::getTunerType(){
  return _ChannelData.TunerType;
}

int Channel::getDiseqC(){
  return _ChannelData.DiseqC;
}

int Channel::getLNBSelection(){
  return _ChannelData.LNBSelection;
}

int Channel::getLNB(){
  return _ChannelData.LNB;
}

int Channel::getFrequency(){
  return _ChannelData.Frequency;
}

Channel *Channel::nextChannel(){
  return _nextChannel;
}

ChannelList::ChannelList(){
  _firstChannel=NULL;
  _lastChannel=NULL;
  _Elements=0;
}

ChannelList::~ChannelList(){
  if(_firstChannel)
    delete _firstChannel;
}

void ChannelList::CSVImport(const char *filepath){
  if(!filepath)
    return;
   FILE *csvfile = fopen(filepath,"r");
   if(ferror(csvfile)!=0)
     return;
   fseek(csvfile,0,SEEK_END);
   size_t fsize = ftell(csvfile);
   fseek(csvfile, 0 ,SEEK_SET);
   char *cstring = new char[fsize+1];
   char buf[255];
   size_t csvpos=0;
   for(size_t readlen=fread(&buf,sizeof(char),255,csvfile); csvpos!=fsize; readlen=fread(&buf,sizeof(char),255,csvfile)){
     if(readlen==0){
       break;
     }
     memmove(&cstring[csvpos],buf,readlen);
     csvpos+=readlen;
   }
   cstring[fsize]='\0';
   fclose(csvfile);
   char *line=NULL, *linebuf=NULL; 
   for(line=strtok_r(cstring,"\n",&linebuf); line; line=strtok_r(NULL,"\n",&linebuf)){
     char *buf=NULL;
     char *ptr = strtok_r(line, "\t",&buf);
     if(ptr==NULL || strcmp(ptr,"Root")==0)
       continue;
     Channel *newchannel = addChannel();
     memcpy(newchannel->_ChannelData.Satellite,ptr,strlen(ptr));
     ptr = strtok_r(NULL, "\t",&buf);
     memcpy(newchannel->_ChannelData.Category,ptr,strlen(ptr));
     ptr = strtok_r(NULL, "\t",&buf);
     memcpy(newchannel->_ChannelData.Name,ptr,strlen(ptr));
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.TunerType=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.DiseqC=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.LNBSelection=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.LNB=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Frequency=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Polarity=ptr[0];
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Symbolrate=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.FEC=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.APID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.VPID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.PMTPID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.PCRPID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.AC3=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.TelePID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.SID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.StreamID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Encrypted=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.NetworkID=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.SatModulation=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.SatModulationSystem=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Group=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       memcpy(newchannel->_ChannelData.Language,ptr,strlen(ptr));
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.AudioChannel=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.EPGFlag=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.Volume=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.OrbitalPos=atoi(ptr);
     ptr = strtok_r(NULL, "\t",&buf);
     if(ptr)
       newchannel->_ChannelData.DiSEqCValue=atoi(ptr);
  }
  delete[] cstring;
}

void ChannelList::LoadFile(const char* filepath){
  FILE *cfile = fopen(filepath,"r");
  if(!cfile)
    return;
  fseek(cfile,0L,SEEK_END);
  long int fsize = ftell(cfile);
  fseek(cfile,0L,SEEK_SET);
  size_t fval=0;
  do{
    if(fsize==ftell(cfile))
      break;
    Channel *curchannel=addChannel();
    fval=fread(&curchannel->_ChannelData,sizeof(curchannel->_ChannelData),1,cfile);
  }while(fval!=0);
  fclose(cfile);
}

void ChannelList::Savefile(const char* filepath){
  if(!_firstChannel)
    return;
  FILE *cfile = fopen(filepath, "w");
  for(Channel *curchannel=_firstChannel; curchannel; curchannel=curchannel->nextChannel()){
    fwrite(&curchannel->_ChannelData,sizeof(curchannel->_ChannelData),1,cfile);
  }
  fclose(cfile);
}


int ChannelList::getElements(){
  return _Elements;
}

Channel *ChannelList::addChannel(){
  if(!_firstChannel){
    _firstChannel= new Channel;
    _lastChannel=_firstChannel;
  }else{
    _lastChannel->_nextChannel = new Channel;
    _lastChannel=_lastChannel->_nextChannel;
  }
  _Elements++;
  return _lastChannel;
}

Channel *ChannelList::delChannel(Channel *delchannel){
  _Elements--;
  Channel *prev=NULL;
  for(Channel *delchan=_firstChannel; delchan; delchan=delchan->nextChannel()){
    if(delchan==delchannel){
      if(prev)
        prev->_nextChannel=delchan->_nextChannel;
      else
        _firstChannel=delchan->_nextChannel;
      delchan->_nextChannel=NULL;
      delete delchan;
    }
    prev=delchan;
  }
  return prev;
}

Channel *ChannelList::firstChannel(){
  return _firstChannel;
}

Channel *ChannelList::lastChannel(){
  return _lastChannel;
}