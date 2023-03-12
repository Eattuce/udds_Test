// Don't modify this file as it will be overwritten.
//
#include "IDL_UserDataType.h"

UserDataType::UserDataType(const UserDataType &IDL_s){
  id = IDL_s.id;
  value = IDL_s.value;
}

UserDataType& UserDataType::operator= (const UserDataType &IDL_s){
  if (this == &IDL_s) return *this;
  id = IDL_s.id;
  value = IDL_s.value;
  return *this;
}

void UserDataType::Marshal(CDR *cdr) const {
  cdr->PutShort(id);
  cdr->PutString(value);
}

void UserDataType::UnMarshal(CDR *cdr){
  cdr->GetShort(id);
  {
    char *IDL_str;
    cdr->GetString(IDL_str);
    if(value != NULL )
    {
        delete value;
        value = NULL;
    }
    value = IDL_str;
  }
}

demo::demo(const demo &IDL_s){
  data = IDL_s.data;
  mdval = IDL_s.mdval;
}

demo& demo::operator= (const demo &IDL_s){
  if (this == &IDL_s) return *this;
  data = IDL_s.data;
  mdval = IDL_s.mdval;
  return *this;
}

void demo::Marshal(CDR *cdr) const {
  cdr->PutString(data);
  cdr->PutString(mdval);
}

void demo::UnMarshal(CDR *cdr){
  {
    char *IDL_str;
    cdr->GetString(IDL_str);
    if(data != NULL )
    {
        delete data;
        data = NULL;
    }
    data = IDL_str;
  }
  {
    char *IDL_str;
    cdr->GetString(IDL_str);
    if(mdval != NULL )
    {
        delete mdval;
        mdval = NULL;
    }
    mdval = IDL_str;
  }
}

