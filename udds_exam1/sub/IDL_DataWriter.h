#ifndef _IDL_DATAWRITER_H_INCLUDED_
#define _IDL_DATAWRITER_H_INCLUDED_
#include "ReturnCode.h"
#include "BasicTypes.h"
#include "DDS/DomainParticipantFactory.h"
#include "DDS/Topic.h"
#include "DDS/Publisher.h"
#include "DDS/DataWriter.h"
#include "IDL_UserDataType.h"
class DataWriterImpl;

class UserDataTypeDataWriter : public DataWriter
{
public:
	UserDataTypeDataWriter(DataWriterImpl* pDataWriterImpl);

	~UserDataTypeDataWriter();

	static UserDataTypeDataWriter* narrow(DataWriter* pDataWriter);

	ReturnCode_t write(UserDataType& data,const InstanceHandle_t & handle);

};

class demoDataWriter : public DataWriter
{
public:
	demoDataWriter(DataWriterImpl* pDataWriterImpl);

	~demoDataWriter();

	static demoDataWriter* narrow(DataWriter* pDataWriter);

	ReturnCode_t write(demo& data,const InstanceHandle_t & handle);

};

#endif
