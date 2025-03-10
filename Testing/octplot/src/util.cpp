#include <assert.h>
#include <string>

#include "globals.h"
#include "figure.h"
#include "root.h"
#include "octplotapp.h"
#include "op_com.h"

bool _set(ocpl::Handle hnd, char* prop,ocpl::type_id type,char* buff, int len)
{
  double h = hnd;
  char* bf = new char[len];
  memcpy(bf,buff,len);
  ocpl::command com(ocpl::set,3);
  com.argin(0,ocpl::real,1,1,reinterpret_cast<char*>(&h),false);
  com.argin(1,ocpl::str,strlen(prop)+1,1,prop,false);
  com.argin(2,type,len,1,bf,true);
  ::GetObjectD(hnd)->Set(com);
  if(com.nargout() && com.argout(0)->id==ocpl::err) return false;
  else return true;
}

bool ::Set(ocpl::Handle hnd, char* prop, char* str)
{
  return _set(hnd,prop,ocpl::str,str,strlen(str)+1);
}

bool ::Set(ocpl::Handle hnd, char* prop, double* dbl,int len)
{
  return _set(hnd,prop,ocpl::real,reinterpret_cast<char*>(dbl),len*sizeof(double));
}

Object* ::GetObjectD(const ocpl::Handle hnd)
{ 
  Object *obj = OctPlotApp::Get()->GetObjectD(hnd);
  assert(obj);
};

Property* ::GetObjProp(const ocpl::Handle hnd, char const* Name) 
{ 
  return (::GetObjectD(hnd))->GetProperty(Name);
};

std::string tolower(std::string in)
{
  for(unsigned int i=0;i<in.size();i++)
    if (in[i]>='A' && in[i]<='Z') in[i] += -'A' + 'a';

  return in;
}
