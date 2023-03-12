// Don't modify this file as it will be overwritten.
//
#include "IDL_UserDataType.h"

UserDataType::UserDataType(const UserDataType &IDL_s){
  value = IDL_s.value;
}

UserDataType& UserDataType::operator= (const UserDataType &IDL_s){
  if (this == &IDL_s) return *this;
  value = IDL_s.value;
  return *this;
}

void UserDataType::Marshal(CDR *cdr) const {
  cdr->PutString(value);
}

void UserDataType::UnMarshal(CDR *cdr){
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
  value = IDL_s.value;
}

demo& demo::operator= (const demo &IDL_s){
  if (this == &IDL_s) return *this;
  value = IDL_s.value;
  return *this;
}

void demo::Marshal(CDR *cdr) const {
  cdr->PutDouble(value);
}

void demo::UnMarshal(CDR *cdr){
  cdr->GetDouble(value);
}

