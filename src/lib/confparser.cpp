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
#include <cstdio>
#include <config.h>
#include <cctype>
#include <cstdlib>

#include "confparser.h"

using namespace satip;

ConfIndex::ConfIndex(){
  _nextIndex=NULL;
  _Name=NULL;
}

ConfIndex::~ConfIndex(){
  delete[] _Name;
  if(_nextIndex)
    delete _nextIndex;
}

ConfSection::ConfSection(){
  _nextSection=NULL;
  _ChildSection=NULL;
  _SectionName=NULL;
  _Idxstart=NULL;
  _Idxend=NULL;
  _Attributes=new Attributes;
}

ConfSection::~ConfSection(){
  if(_SectionName)
    delete[] _SectionName;
  if(_nextSection)
    delete _nextSection;
  delete _Attributes;
}

void ConfSection::addChildSection(ConfSection* children){
  _ChildSection=children;
}


ConfSection* ConfSection::ChildSection(){
  return _ChildSection;
}

const char* ConfSection::getSectionName(){
  return _SectionName;
}

const char* ConfSection::getAttribute(const char* name){
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(attr && attr->_Type==STRING)
    return (const char*)attr->_Value;
  return NULL;
}

int ConfSection::getIntAttribute(const char* name){
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(attr && attr->_Type==INTGER)
    return *(int*)attr->_Value;
  return 0;
}

double ConfSection::getDoubleAttribute(const char* name){
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(attr && attr->_Type==DOUBLE)
    return *(double*)attr->_Value;
  return 0;
}

void ConfSection::setAttribute(const char* name, const char* value){
  if(!name || !value)
    return;
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(!attr)
    attr=_Attributes->_addAttribute(name);
  else
    attr->_delValue();
  attr->_Type=STRING;
  size_t valsize=strlen(value)+1;
  attr->_Value=new char[valsize];
  memcpy(attr->_Value,value,valsize);
}

void ConfSection::setAttribute(const char* name, int value){
  if(!name || !value)
    return;
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(!attr)
    attr=_Attributes->_addAttribute(name);
  else
    attr->_delValue();
  attr->_Type=STRING;
  attr->_Value=new int;
  attr->_Value=&value;
}

void ConfSection::setAttribute(const char* name, double value){
  if(!name || !value)
    return;
  Attributes::Attribute *attr=_Attributes->_findAttribute(name);
  if(!attr)
    attr=_Attributes->_addAttribute(name);
  attr->_Type=STRING;
  attr->_Value=new double;
  attr->_Value=&value;
}

bool ConfSection::havechilds(){
  if(!_ChildSection)
    return false;
  return true;
}

ConfSection* ConfSection::nextSection(){
  return _nextSection;
}

Attributes::Attributes(){
  _firstAttribute=NULL;
  _lastAttribute=NULL;
}

Attributes::~Attributes(){
  if(_firstAttribute)
    delete _firstAttribute;
}

Attributes::Attribute* Attributes::_addAttribute(const char* key){
  if(!key)
    return NULL;
  if(!_firstAttribute){
    _firstAttribute = new Attribute;
    _lastAttribute = _firstAttribute;
  }else{
    _lastAttribute->_nextAttribute=new Attribute;
    _lastAttribute=_lastAttribute->_nextAttribute;
  }
  size_t keylen=strlen(key)+1;
  _lastAttribute->_Key=new char[keylen];
  memcpy(_lastAttribute->_Key,key,keylen);
  _lastAttribute->_nextAttribute=NULL;
  return _lastAttribute;
}

Attributes::Attribute* Attributes::_findAttribute(const char* key){
  if(!key)
    return NULL;
  for(Attributes::Attribute *curattr=_firstAttribute; curattr; curattr=curattr->_nextAttribute){
    if(strcmp(curattr->_Key,key)==0 && curattr->_Type==INTGER){
      return curattr;
    }
  }
  return NULL;
}


Attributes::Attribute::Attribute(){
  _Key=NULL;
  _Type=STRING;
  _Value=NULL;
}

Attributes::Attribute::~Attribute(){
  _delValue();
  if(_Key)
    delete[] _Key;
  if(_nextAttribute)
    delete _nextAttribute;
}

void Attributes::Attribute::_delValue(){
  if(_Value){
    switch(_Type){
      case STRING:
        delete[] (char*)_Value;
        break;
      case INTGER:
        delete[] (int*)_Value;
        break;
      case DOUBLE:
        delete[] (double*)_Value;
        break;
    }
  }
}

ConfParser::ConfParser(){
  _Buffer=NULL;
  _BufferSize=0;
  _firstSection=NULL;
  _lastSection=NULL;
  _rootSection=NULL;
  _lastIndex=NULL;
  _firstIndex=NULL;
}

ConfParser::~ConfParser(){
  if(_firstIndex)
    delete _firstIndex;
  if(_firstSection)
    delete _firstSection;
  if(_rootSection)
    delete _rootSection;
  if(_Buffer)
    delete[] _Buffer;
}

ConfSection* ConfParser::firstSection(){
  return _firstSection;
}

ConfSection* ConfParser::rootSection(){
  return _rootSection;
}

ConfSection* ConfParser::addSection(const char* sectionname){
  if(!_firstSection){
    _firstSection=new ConfSection;
    _lastSection=_firstSection;
  }else{
    _lastSection->_nextSection=new ConfSection;
    _lastSection=_lastSection->_nextSection;
  }
  size_t namelen=strlen(sectionname)+1;
  _lastSection->_SectionName=new char[namelen];
  memcpy(_lastSection->_SectionName,sectionname,namelen);
  return _lastSection;
}

int ConfParser::Loadfile(const char* path){
  if(!path)
    return CONFIGFILE_NOTFOUND;
  if(_Buffer){
    delete[] _Buffer;
    _Buffer=NULL;
  }
  _BufferSize=0;
  FILE *file=fopen(path,"r");
  if(ferror(file)!=0)
    return CONFIGFILE_NOTFOUND;
  fseek(file, 0L, SEEK_END);
  _BufferSize=ftell(file);
  fseek(file, 0L, SEEK_SET);
  _Buffer=new unsigned char[_BufferSize];
  if(_BufferSize<=0)
    return CONFIGFILE_CORRUPTED;
  char buf;
  size_t curlen=0;
  size_t start=NPOS,end=NPOS;
  for(size_t readlen=fread(&buf,sizeof(char),1,file); (readlen==1 && curlen<_BufferSize); 
      readlen=fread(&buf,sizeof(char),1,file)){
    if(buf==' '){
      continue;
    }
    _Buffer[curlen]=buf;
    switch(buf){
      case '[':
        start=curlen+1;
        break;
      case ']':
        end=curlen;
        break;
    }
    if(start!=NPOS && end != NPOS){
      _ParseSections(start,end);
      end=NPOS;
      start=NPOS;
    }

    curlen++;
  }
  fclose(file);
  _BufferSize=curlen;
  memmove(_Buffer,_Buffer,_BufferSize);
#ifdef DEBUG
  fprintf(DEBUGOUT,"RootSection\n");
#endif
  if(_rootSection)
    delete _rootSection;
  _rootSection=new ConfSection;
  _rootSection->_Idxstart=_firstIndex;
  _rootSection->_Idxend=_lastIndex;
  _BuildTree(_rootSection);
  _ParseAttributes();
  return CONFIGFILE_LOADED;
}

void ConfParser::_ParseSections(size_t start,size_t end){
  if(!_firstIndex){
    _firstIndex=new ConfIndex;
    _lastIndex=_firstIndex;
  }else{
    _lastIndex->_nextIndex=new ConfIndex;
    _lastIndex=_lastIndex->_nextIndex;
  }
  _lastIndex->_nextIndex=NULL;
  size_t bufsize=(end-start);
  _lastIndex->_Name= new char[bufsize+1];
  _lastIndex->_Startpos=start;
  _lastIndex->_Endpos=end;
  memcpy(_lastIndex->_Name,&_Buffer[start],bufsize);
  _lastIndex->_Name[bufsize]='\0';
}

void ConfParser::_BuildTree(ConfSection* section){
  _BuildTree(section,section->_Idxstart,0);
}

void ConfParser::_BuildTree(ConfSection *section,ConfIndex *start,size_t lvl){
  if(!section && !start){
#ifdef DEBUG
  fprintf(DEBUGOUT,"What a fuck creating tree failed missing start point or section\n");
#endif
    throw section;
  }
  ConfSection *newsec=NULL;
  for(ConfIndex *curidx=start; curidx; curidx=curidx->_nextIndex){
    if(!newsec && curidx->_Name[0]!='/'){
      newsec=addSection(curidx->_Name);
      newsec->_Idxstart=curidx;
#ifdef DEBUG
      for(size_t i=0; i<lvl; i++){
        fprintf(DEBUGOUT,"| ");
      }
      fprintf(DEBUGOUT,"|__(%zu) %s %zu\n",lvl ,curidx->_Name,curidx->_Startpos);
#endif
    }else if(newsec && strcmp(curidx->_Name+1,newsec->_SectionName)==0){
      newsec->_Idxend=curidx;
      newsec=NULL;
      continue;
    }else if(section->getSectionName() && strcmp(curidx->_Name+1,section->getSectionName())==0){
      return;
    }else if(newsec){
      section->addChildSection(newsec);
      _BuildTree(newsec,curidx->_nextIndex,lvl+1);
    }
  }
}

void ConfParser::_ParseAttributes(){
  size_t startpos=0,delimter=0,endpos=0;
  for(size_t i=0; i<_BufferSize; i++){
    if(_Buffer[i]=='\n'){
      endpos=i;
    }
    if(endpos!=0){
      for(size_t d=startpos; d<endpos ;d++){
        if(_Buffer[d]=='=')
          delimter=d;
      }
      if(delimter==0)
        startpos=endpos+1;
    }
    if(delimter!=0 && endpos!=0){
      for(ConfSection *cursec=_firstSection; cursec; cursec=cursec->nextSection()){
        if(startpos>cursec->_Idxstart->_Endpos && endpos<cursec->_Idxend->_Startpos){
          ConfSection *subsec=cursec->ChildSection();
          if(subsec && (startpos<=subsec->_Idxstart->_Endpos && endpos>=subsec->_Idxend->_Startpos)){
            cursec=subsec;
            continue;
          }else{
            size_t ksize=(delimter-startpos);
            size_t vsize=(endpos-delimter);
            if(ksize==0)
              break;
            char *key = new char[ksize+1];
            char *value = new char[vsize+1];
            memcpy(key,_Buffer+startpos,ksize);
            memcpy(value,_Buffer+delimter+1,vsize);
            key[ksize]='\0';
            value[vsize]='\0';
            int alpha=0,digit=0,dot=0;
            for(size_t i=0; i<strlen(value); i++){
              if(value[i]=='.')
                dot++;
              else if(isdigit(value[i]))
                digit++;
              else
                alpha++;
            }
            if(alpha==0 && dot==0){
              cursec->setAttribute(key,atoi(value));
            }else if(alpha==0 && dot==1){
              cursec->setAttribute(key,atof(value));
            }else{
              cursec->setAttribute(key,value);
            }
            fprintf(stdout,"Section: %s Key: %s Value: %s\n",cursec->getSectionName(),key,value);
            delete[] key;
            delete[] value;
            break;
          }
        }
      }
      startpos=endpos+1;
      delimter=0;
      endpos=0;
    }
  }
}


ConfSection *ConfParser::_SaveTree(int fd,int lvl,ConfSection *section){
  for(ConfSection *cursec=section; cursec; cursec=cursec->nextSection()){
    if(cursec->havechilds()){
       lvl++;
       _SaveTree(fd,lvl,cursec->ChildSection());
    }
//     fwrite()
  }
  return NULL;
}

int ConfParser::Savefile(const char* path){
//   FILE *cfile = fopen(path, O_RDONLY);
//   if(cfile){
//     perror("SatIP: Can't write Devices config file!\n");
//     return CONFIGFILE_NOTFOUND;
//   }
//   _SaveTree(cfile,0,_firstSection);
//   return CONFIGFILE_SAVED;
}



