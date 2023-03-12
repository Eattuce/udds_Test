/**********************************************************
*****************订阅端程序subscriber.cpp*******************
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>

/* IDL_TypeSupport.h中包含所有依赖的头文件 */
#include "IDL_TypeSupport.h"

int count = 0;
bool cansend = false;

DomainParticipant *participant = NULL;
Subscriber *subscriber = NULL;
Topic *topic = NULL;
DataReader *reader = NULL;
ReturnCode_t retcode;
const char *type_name = NULL;
int status = 0;
InstanceHandle_t instance_handle = HANDLE_NIL;
Publisher *publisher = NULL;
UserDataTypeDataReader *UserDataType_reader = NULL;
DataWriter *writer = NULL;
UserDataTypeDataWriter * UserDataType_writer = NULL;
UserDataType *instance = NULL;


class UserDataTypeListener : public DataReaderListener {
public:
	virtual void on_data_available(DataReader* reader);
};

/* 重写继承过来的方法on_data_available()，在其中进行数据监听读取操作 */
void UserDataTypeListener::on_data_available(DataReader* reader)
{
	UserDataTypeDataReader *UserDataType_reader = NULL;
	UserDataTypeSeq data_seq;
	SampleInfoSeq info_seq;
	ReturnCode_t retcode;
	int i;

	/* 利用reader，创建一个读取UserDataType类型的UserDataType_reader*/
	UserDataType_reader = UserDataTypeDataReader::narrow(reader);
	if (UserDataType_reader == NULL) {
		fprintf(stderr, "DataReader narrow error\n");
		return;
	}

	/* 获取数据，存放至data_seq，data_seq是一个队列 */
	retcode = UserDataType_reader->read(
		data_seq, info_seq, 10, 0, 0, 0);

	if (retcode == RETCODE_NO_DATA) {
		return;
	}
	else if (retcode != RETCODE_OK) {
		fprintf(stderr, "take error %d\n", retcode);
		return;
	}

	for (int i = 0; i < data_seq.length(); ++i) {
		auto sample = data_seq[i];
		// if (/* sample.value == count && sample.id == 'a' && */ count < 1000) {
			std::cout << "B: Received message " << count << " from A." << std::endl;
			instance->value = count;
			instance->id = 'b';

			// sleep(1);
			retcode = UserDataType_writer->write(*instance, instance_handle);
			if (retcode != RETCODE_OK) {
				fprintf(stderr, "write error %d\n", retcode);
			}

			std::cout << "B: Sent reply message " << count << " to A." << std::endl;

			count++;
			cansend = true;

			// break;
		// }
	}
}

UserDataTypeListener *reader_listener = new UserDataTypeListener();


/* 删除所有实体 */
static int subscriber_shutdown(
	DomainParticipant *participant)
{
	ReturnCode_t retcode;
	int status = 0;

	if (participant != NULL) {
		retcode = participant->delete_contained_entities();
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_contained_entities error %d\n", retcode);
			status = -1;
		}

		retcode = DomainParticipantFactory::get_instance()->delete_participant(participant);
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_participant error %d\n", retcode);
			status = -1;
		}
	}
	return status;
}

/* 订阅者函数 */
extern "C" int subscriber_main(int domainId, int sample_count)
{

	/* 1. 创建一个participant，可以在此处定制participant的QoS */
	/* 建议1：在程序启动后优先创建participant，进行资源初始化*/
	/* 建议2：相同的domainId只创建一次participant，重复创建会占用大量资源 */ 
	participant = DomainParticipantFactory::get_instance()->create_participant(
		domainId, PARTICIPANT_QOS_DEFAULT/* participant默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (participant == NULL) {
		fprintf(stderr, "create_participant error\n");
		subscriber_shutdown(participant);
		return -1;
	}

	/* 2. 创建一个subscriber，可以在创建subscriber的同时定制其QoS  */
	/* 建议1：在程序启动后优先创建subscriber*/
	/* 建议2：一个participant下创建一个subscriber即可，无需重复创建 */
	subscriber = participant->create_subscriber(
		SUBSCRIBER_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (subscriber == NULL) {
		fprintf(stderr, "create_subscriber error\n");
		subscriber_shutdown(participant);
		return -1;
	}

	publisher = participant->create_publisher(
		PUBLISHER_QOS_DEFAULT /* 默认QoS */, 
		NULL /* listener */, STATUS_MASK_NONE);
	if (publisher == NULL) {
		fprintf(stderr, "create_publisher error\n");
		subscriber_shutdown(participant);
		return -1;
	}


	/* 3. 在创建主题之前注册数据类型 */
	/* 建议1：在程序启动后优先进行注册 */
	/* 建议2：一个数据类型注册一次即可 */
	type_name = UserDataTypeTypeSupport::get_type_name();
	retcode = UserDataTypeTypeSupport::register_type(
		participant, type_name);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "register_type error %d\n", retcode);
		subscriber_shutdown(participant);
		return -1;
	}

	/* 4. 创建主题，并定制主题的QoS  */
	/* 建议1：在程序启动后优先创建Topic */
	/* 建议2：一种主题名创建一次即可，无需重复创建 */
	topic = participant->create_topic(
		"Example UserDataType"/* 主题名，应与发布者主题名一致 */,
		type_name, TOPIC_QOS_DEFAULT/* 默认QoS */, 
		NULL /* listener */, STATUS_MASK_NONE);
	if (topic == NULL) {
		fprintf(stderr, "create_topic error\n");
		subscriber_shutdown(participant);
		return -1;
	}


	/* 6. 创建datareader，并定制datareader的QoS */
	/* 建议1：在程序启动后优先创建datareader*/
	/* 建议2：创建一次即可，无需重复创建 */
	/* 建议3：在程序退出时再进行释放 */
	/* 建议4：避免打算接收数据时创建datareader，接收数据后删除，该做法消耗资源，影响性能 */
	reader = subscriber->create_datareader(
		topic, DATAREADER_QOS_DEFAULT/* 默认QoS */,
		reader_listener/* listener */, STATUS_MASK_ALL);
	if (reader == NULL) {
		fprintf(stderr, "create_datareader error\n");
		subscriber_shutdown(participant);
		delete reader_listener;
		return -1;
	}

	UserDataType_reader = UserDataTypeDataReader::narrow(reader);
	if (UserDataType_reader == NULL) {
		fprintf(stderr, "DataReader narrow error\n");
		return -1;
	}


	// Writer

	writer = publisher->create_datawriter(
		topic , DATAWRITER_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (writer == NULL) {
		fprintf(stderr, "create_datawriter error\n");
		subscriber_shutdown(participant);
		return -1;
	}


	UserDataType_writer = UserDataTypeDataWriter::narrow(writer);
	if (UserDataType_writer == NULL) {
		fprintf(stderr, "DataWriter narrow error\n");
		subscriber_shutdown(participant);
		return -1;
	}

	instance = UserDataTypeTypeSupport::create_data();
	if (instance == NULL) {
		fprintf(stderr, "UserDataTypeTypeSupport::create_data error\n");
		subscriber_shutdown(participant);
		return -1;
	}

	/* 7. 主循环 ，监听器会默认调用on_data_available()监听数据 */
	// for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
	// 	//保持进程一直运行
	// }

	// UserDataTypeSeq data_seq;
	// SampleInfoSeq info_seq;
	instance->value = count;
	instance->id = 'b';
	retcode = UserDataType_writer->write(*instance, instance_handle);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "write error %d\n", retcode);
	}

	while (1) {
		if (count > 1000){
			break;
		}
		// if (cansend){
		// 	retcode = UserDataType_writer->write(*instance, instance_handle);
		// 	cansend = false;
		// 	if (retcode != RETCODE_OK) {
		// 		fprintf(stderr, "write error %d\n", retcode);
		// 	}
		// }

		// // wait for the message from A
		// do{
		// 	retcode = UserDataType_reader->take(data_seq, info_seq, 10, 0, 0, 0);
		// } while (retcode == RETCODE_OK);

		// bool received_message = false;
		// for (int i = 0; i < data_seq.length(); ++i) {
		// 	auto sample = data_seq[i];
		// 	if (sample.value == count && sample.id == 'a') {
		// 		received_message = true;
		// 		std::cout << "B: Received message " << count << " from A." << std::endl;

		// 		// create a reply message and write it to the data writer
		// 		instance->value = count;
		// 		instance->id = 'b';
		// 		retcode = UserDataType_writer->write(*instance, instance_handle);
		// 		if (retcode != RETCODE_OK) {
		// 			fprintf(stderr, "write error %d\n", retcode);
		// 		}

		// 		std::cout << "B: Sent reply message " << count << " to A." << std::endl;
		// 	}
		// }

		// if (!received_message) {
		// 	std::cerr << "B: Did not receive message " << count << " from A." << std::endl;
		// }

		// }
		// sleep(1);
	}


	/* 8. 删除所有实体和监听器 */
	ReturnCode_t retcode = UserDataTypeTypeSupport::delete_data(instance);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "UserDataTypeTypeSupport::delete_data error %d\n", retcode);
	}
	delete reader_listener;

	status = subscriber_shutdown(participant);

	return status;
}

int main(int argc, char *argv[])
{
	int domain_id = 0;
	int sample_count = 0; /* 无限循环 */

	if (argc >= 2) {
		domain_id = atoi(argv[1]);/* 发送至域domain_id */
	}
	if (argc >= 3) {
		sample_count = atoi(argv[2]);/* 发送sample_count次 */
	}
	return subscriber_main(domain_id, sample_count);
}

