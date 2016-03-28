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


/*File states for save/loadfunctions*/
#define CONFIGFILE_LOADED     0
#define CONFIGFILE_SAVED      0
#define CONFIGFILE_NOTFOUND   1
#define CONFIGFILE_PERMISIONS 2
#define CONFIGFILE_CORRUPTED  3

/*Value types for attributes*/
#define INTGER 0
#define STRING 1
#define DOUBLE 2

#ifndef CONFPARSER_H
#define CONFPARSER_H

namespace satip {
  class ConfIndex {
  private:
      size_t     _Startpos;
      size_t     _Endpos;
      char      *_Name;
      ConfIndex *_nextIndex;
      ConfIndex();
      ~ConfIndex();
      friend class ConfParser;
  };

  class Attributes {
  private:
    Attributes();
    ~Attributes();
    struct Attribute{
      Attribute();
      ~Attribute();
      void         _delValue();
      char        *_Key;
      short        _Type;
      void        *_Value;
      Attribute   *_nextAttribute;
    };
    Attribute    *_addAttribute(const char* key);
    Attribute    *_findAttribute(const char* key);
    Attribute    *_firstAttribute;
    Attribute    *_lastAttribute;
    friend class ConfSection;
  };
   
  class ConfSection {
  public:
    
    bool         havechilds();
    void         addChildSection(ConfSection *children);
    ConfSection *ChildSection();
    
    ConfSection *nextSection();
    
    const char  *getSectionName();
    
    const char  *getAttribute(const char *name);
    int          getIntAttribute(const char *name);
    double       getDoubleAttribute(const char *name);
    
    void         setAttribute(const char *name,const char *value);
    void         setAttribute(const char *name,int value);
    void         setAttribute(const char *name,double value);
    
    void         delAttribute(const char *name);
  private:
    ConfSection();
    ~ConfSection();
    Attributes  *_Attributes;
    ConfIndex   *_Idxstart;
    ConfIndex   *_Idxend;
    char        *_SectionName;
    ConfSection *_nextSection;
    ConfSection *_ChildSection;
    friend class ConfParser;
  };
  
  class ConfParser{
  public:
    ConfParser();
    ~ConfParser();
    int Loadfile(const char *path);
    int Savefile(const char *path);
    
    ConfSection   *firstSection();
    ConfSection   *rootSection();
    ConfSection   *addSection(const char *sectionname);
  private:
    void           _ParseSections(size_t start,size_t end);
    void           _BuildTree(ConfSection *section);
    void           _BuildTree(ConfSection *section,ConfIndex *start,size_t lvl);
    ConfSection   *_SaveTree(int fd,int lvl,ConfSection *section);
    
    void           _ParseAttributes();
    
    ConfIndex     *_firstIndex;
    ConfIndex     *_lastIndex;
    ConfSection   *_firstSection;
    ConfSection   *_lastSection;
    ConfSection   *_rootSection;
    unsigned char *_Buffer;
    size_t         _BufferSize;
  };
}

#endif