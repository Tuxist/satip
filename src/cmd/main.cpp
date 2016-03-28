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

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef WIN32
  #include <io.h>
  #include <windows.h> 
#else
  #include <unistd.h>
  #include <sys/errno.h>
#endif

#include <sys/stat.h>

#include "channel.h"
#include "socket.h"
#include "rtsp.h"
#include "device/satip.h"

#define DEFCONFIGPATH ".satip"
#define CHANNELDB     "channels.dat"
#define SATIPCONFIG   "satdevices.conf"

class SatIP {
public:
  SatIP(){
    _ChannelList = new satip::ChannelList;
    _Currsocket = NULL;
    snprintf(_WorkDir,240,"%s/%s",getenv("HOME"),DEFCONFIGPATH);
    if(mkdir(_WorkDir,0777)!=0)
      fprintf(stdout,"Can't create default config path: %s \n",strerror(errno));
    snprintf(_ChannelPath,255,"%s/%s",_WorkDir,CHANNELDB);
    _ChannelList->LoadFile(_ChannelPath);
  }

  ~SatIP(){
    if(_Currsocket)
      delete _Currsocket;
    if(_ChannelList)
      delete _ChannelList;
  }
  
  void showChannels(){
    for(satip::Channel *curchannel=_ChannelList->firstChannel(); curchannel; curchannel=curchannel->nextChannel()){
      if(curchannel->getSatellite() && curchannel->getName())
        fprintf(stdout,"Satellite : %s channelname: %s \n",curchannel->getSatellite(),curchannel->getName());
    }
  }

  void importChannels(const char *path){
    _ChannelList->CSVImport(path);
    _ChannelList->Savefile(_ChannelPath);
  }

  void streamTest(const char*addr,int port){
    fprintf(stdout,"Connect to: %s on Port: %d\n",addr,port);
    if(_Currsocket)
      delete _Currsocket;
    _Currsocket=new satip::Socket(addr,port);
    _RTSP = new satip::RTSP(_Currsocket);
    satip::RTSPTransport rtsptrans;
    satip::RTSPSession *cursess=_RTSP->setupRTSPSession(&rtsptrans,"?src=1&freq=11362&pol=h&sr=22000&msys=dvbs2&ro=0.35&mtype=8psk&plts=on&fec=23&pids=0");
    for(;;/*int i=0; i!=5; i++*/){
      cursess->Play();
      printf("play\n");
      sleep(1);
    }
    delete _RTSP; 
  }
  
  void showDevicesConfig(){
    satip::SATIPDevices devs;
    snprintf(_DevicesConfig,255,"%s/%s",_WorkDir,SATIPCONFIG);
    devs.LoadFile(_DevicesConfig);
  }
  
private:
  char _WorkDir[240];
  char _ChannelPath[255];
  char _DevicesConfig[255];
  satip::ChannelList *_ChannelList;
  satip::Socket      *_Currsocket;
  satip::RTSP        *_RTSP;
};

int main(){
  SatIP satip;
  int sel=0;
  fprintf(stdout,"Select Option:\n  1) Show Channels\n  2) Import CSV file\n  3) RTSP Stream\n  4) Show SatIP devices\n  9) Exit\n");
  scanf("%2d", &sel);
  switch(sel){
    case 1:{
      satip.showChannels();
      break;
    }
    case 2:{
      fprintf(stdout,"Enter Path: \n");
      char input[1024];
      scanf ("%1024s", input);
      satip.importChannels(input);
      break;
    }
    case 3:{
      fprintf(stdout,"Enter: Addr Port\n");
      char addr[255];
      int port =554;
      scanf ("%255s %d", addr,&port);
      satip.streamTest(addr,port);
      break;
    }
    case 4:{
      satip.showDevicesConfig();
      break;
    }
    case 9:{
      return 0;
    }
  }
}