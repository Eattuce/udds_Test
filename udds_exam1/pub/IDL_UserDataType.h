// Don't modify this file as it will be overwritten.
//
#include "CDR/CDR.h"
#include "ReturnCode.h"
#include "BasicTypes.h"

#ifndef IDL_UserDataType_hh
#define IDL_UserDataType_hh

#ifndef UserDataType_defined
#define UserDataType_defined

struct UserDataType {
  UserDataType()
	{
		id = 0;
		value = new char[255];
		value[0]= '\0';
	}

  UserDataType(const UserDataType  &IDL_s);

  ~UserDataType(){
		delete value;
		value = NULL;
	}

  	int StructSize()
	{
		int strSize = 0;
		strSize += sizeof(UserDataType);
		strSize += strlen(value);
		return strSize;
	}
  UserDataType& operator= (const UserDataType &IDL_s);

  void Marshal(CDR *cdr) const;
  void UnMarshal(CDR *cdr);

  short id;
char* value;
  
};

typedef sequence<UserDataType> UserDataTypeSeq;

#endif

#ifndef demo_defined
#define demo_defined

struct demo {
  demo()
	{
		data = new char[255];
		data[0]= '\0';
		mdval = new char[255];
		mdval[0]= '\0';
	}

  demo(const demo  &IDL_s);

  ~demo(){
		delete data;
		data = NULL;
		delete mdval;
		mdval = NULL;
	}

  	int StructSize()
	{
		int strSize = 0;
		strSize += sizeof(demo);
		strSize += strlen(data);
		strSize += strlen(mdval);
		return strSize;
	}
  demo& operator= (const demo &IDL_s);

  void Marshal(CDR *cdr) const;
  void UnMarshal(CDR *cdr);

  char* data;
char* mdval;
  
};

typedef sequence<demo> demoSeq;

#endif




#endif /*IDL_UserDataType_hh */
