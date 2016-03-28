/*******************************************************************************
Copyright 2012  Jan Koester jan.koester@gmx.net

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

#ifndef SATIP_H
#define SATIP_H

namespace satip {
  class ConfParser;
  class Channel;
  
  class SATIPDevice{
  public:
    SATIPDevice *nextDeviceSATIP();
    int          getID();
    void         setAddr(const char *addr);
    const char  *getAddr();
    void         setPort(int port);
    int          getPort();
    const char  *getUrl(Channel *channel);
  private:
    SATIPDevice();
    ~SATIPDevice();
    int            _ID;
    char          *_Addr;
    int            _Port;
    char          *_Url;
    SATIPDevice   *_nextDeviceSATIP;
    friend class   SATIPDevices;
  };
  
  class SATIPDevices {
  public:
    SATIPDevices();
    ~SATIPDevices();
    void         LoadFile(const char *filepath);
    void         Savefile(const char *filepath);
    SATIPDevice *firstSATIPDevice();
    SATIPDevice *addSATIPDevice();
    SATIPDevice *delSATIPDevice(SATIPDevice* deldevive);
  private:
    char        *_Buffer;
    char        *_BufferDevStart;
    char        *_BufferDevEnd;
    unsigned int _BufferSize;
    int          _LastID;
    SATIPDevice *_firstSATIPDevice;
    SATIPDevice *_lastSATIPDevice;
    ConfParser  *_ConfParser;
  };
}
#endif
