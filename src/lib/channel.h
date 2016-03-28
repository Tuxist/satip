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

#ifndef CHANNEL_H
#define CHANNEL_H

#define MAXLEN 512

namespace satip {
class Channel {
public:
  const char *getName();
  const char *getSatellite();
  int         getTunerType();
  int         getDiseqC();
  int         getLNBSelection();
  int         getLNB();
  int         getFrequency();
  Channel    *nextChannel();
private:
  Channel();
  ~Channel();
  Channel *_nextChannel;
  struct {
    char     Category[MAXLEN];
    char     Name[MAXLEN];
    char     Satellite[MAXLEN];
    int      TunerType;
    int      DiseqC;
    int      LNBSelection;
    int      LNB;
    int      Frequency;
    char     Polarity;
    int      Symbolrate;
    int      FEC;
    int      APID;
    int      VPID;
    int      PMTPID;
    int      PCRPID;
    int      AC3;
    int      TelePID;
    int      SID;
    int      StreamID;
    int      Encrypted;
    int      NetworkID;
    int      SatModulation;
    int      SatModulationSystem;
    int      Group;
    char     Language[MAXLEN];
    int      AudioChannel;
    int      EPGFlag;
    int      Volume;
    int      OrbitalPos;
    int      DiSEqCValue;
  } _ChannelData;
  friend class ChannelList;
};

class ChannelList {
public:
  ChannelList();
  ~ChannelList();
  void        LoadFile(const char *filepath);
  void        Savefile(const char *filepath);
  void        CSVImport(const char *filepath);
  Channel    *firstChannel();
  Channel    *lastChannel();
  Channel    *addChannel();
  Channel    *delChannel(Channel *delchannel);
  int         getElements();
private:
  Channel    *_firstChannel;
  Channel    *_lastChannel;
  size_t      _Elements;
};
}
#endif