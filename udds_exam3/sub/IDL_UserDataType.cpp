// Don't modify this file as it will be overwritten.
//
#include "IDL_UserDataType.h"

UserDataType::UserDataType(const UserDataType &IDL_s){
  value = IDL_s.value;
  id = IDL_s.id;
}

UserDataType& UserDataType::operator= (const UserDataType &IDL_s){
  if (this == &IDL_s) return *this;
  value = IDL_s.value;
  id = IDL_s.id;
  return *this;
}

void UserDataType::Marshal(CDR *cdr) const {
  cdr->PutShort(value);
  cdr->PutChar(id);
}

void UserDataType::UnMarshal(CDR *cdr){
  cdr->GetShort(value);
  cdr->GetChar(id);
}

