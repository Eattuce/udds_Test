#ifndef _IDL_TYPESUPPORT_H_INCLUDED_ 
#define _IDL_TYPESUPPORT_H_INCLUDED_

#include <string>
using std::string;
#include "IDL_DataWriter.h"
#include "IDL_DataReader.h"
#include "DDS/TypeSupport.h"
#include "md5.h"

class UserDataTypeTypeSupport : public TypeSupport {
public:
	static UserDataType* create_data()
	{
		return new UserDataType;
	}

	static ReturnCode_t delete_data(UserDataType* pUserDataType)
	{
		delete pUserDataType;
		pUserDataType = NULL;
		return ReturnCode_t();
	}

	static void print_data(UserDataType* pUserDataType,int layer = 0)
	{
		char *c = new char[layer];
		int i = 0;
		for (; i < layer; i++)
		{
			c[i] = '\t';
		}
		c[i] = '\0';
		std::cout<<c<<"UserDataType:"<<std::endl;
		std::cout<<c<<"  id = "<< pUserDataType->id <<std::endl;
		std::cout<<c<<"  value = "<< pUserDataType->value <<std::endl;
	}

	static const char* get_type_name()
	{
		return "UserDataType";
	}

	static int register_type(DomainParticipant* participant, string type_name)
	{
		if (type_name.empty())
			type_name = get_type_name();

		return participant->register_type(type_name);
	}
};

class demoTypeSupport : public TypeSupport {
public:
	static demo* create_data()
	{
		return new demo;
	}

	static ReturnCode_t delete_data(demo* pdemo)
	{
		delete pdemo;
		pdemo = NULL;
		return ReturnCode_t();
	}

	static void print_data(demo* pdemo,int layer = 0)
	{
		char *c = new char[layer];
		int i = 0;
		for (; i < layer; i++)
		{
			c[i] = '\t';
		}
		c[i] = '\0';
		std::cout<<c<<"demo:"<<std::endl;
		std::cout<<c<<"  data = "<< pdemo->data <<std::endl;
		std::cout<<c<<"  mdval = "<< pdemo->mdval <<std::endl;

		char *value_p = new char[255];
		string str = pdemo->data;
		const char *val_pointer = GetDataMD5(str).data();
		strcpy(value_p, val_pointer);

		if (strcmp(pdemo->mdval, value_p) == 0){
			std::cout << "Correct!" << std::endl;
		}else{
			std::cout << "Wrong!" << std::endl;
		}
		std::cout << "Sub MD5:" << pdemo->mdval << "\tCal MD5:" << value_p << std::endl;
	}

	static const char* get_type_name()
	{
		return "demo";
	}

	static int register_type(DomainParticipant* participant, string type_name)
	{
		if (type_name.empty())
			type_name = get_type_name();

		return participant->register_type(type_name);
	}
};

#endif
