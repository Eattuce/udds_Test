/**********************************************************
*****************发布端程序publisher.cpp********************
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <iomanip> 

/* IDL_TypeSupport.h中包含所有依赖的头文件 */
#include "IDL_TypeSupport.h"

/* 删除所有实体 */
static int publisher_shutdown(DomainParticipant *participant)
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

/* 发布者函数 */
extern "C" int publisher_main(int domainId, int sample_count)
{
	DomainParticipant *participant = NULL;
	Publisher *publisher = NULL;
	Topic *topic = NULL;
	DataWriter *writer = NULL;
	UserDataTypeDataWriter * UserDataType_writer = NULL;
	UserDataType *instance = NULL;
	ReturnCode_t retcode;
	InstanceHandle_t instance_handle = HANDLE_NIL;
	const char *type_name = NULL;
	int count = 0;



	Topic *demo_topic = NULL;
	DataWriter *demo_writer = NULL;
	demoDataWriter * demoType_writer = NULL;
	demo *demo_instance = NULL;
	ReturnCode_t democode;
	InstanceHandle_t demo_instance_handle = HANDLE_NIL;
	const char *demo_name = NULL;

	/* 1. 创建一个participant，可以在此处定制participant的QoS */
	/* 建议1：在程序启动后优先创建participant，进行资源初始化*/
	/* 建议2：相同的domainId只创建一次participant，重复创建会占用大量资源 */ 
	participant = DomainParticipantFactory::get_instance()->create_participant(
		domainId, PARTICIPANT_QOS_DEFAULT/* participant默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (participant == NULL) {
		fprintf(stderr, "create_participant error\n");
		publisher_shutdown(participant);
		return -1;
	}

	/* 2. 创建一个publisher，可以在创建publisher的同时定制其QoS  */
	/* 建议1：在程序启动后优先创建publisher */
	/* 建议2：一个participant下创建一个publisher即可，无需重复创建 */
	publisher = participant->create_publisher(
		PUBLISHER_QOS_DEFAULT /* 默认QoS */, 
		NULL /* listener */, STATUS_MASK_NONE);
	if (publisher == NULL) {
		fprintf(stderr, "create_publisher error\n");
		publisher_shutdown(participant);
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
		publisher_shutdown(participant);
		return -1;
	}




	demo_name = demoTypeSupport::get_type_name();
	democode = demoTypeSupport::register_type(
		participant, demo_name);
	if (democode != RETCODE_OK) {
		fprintf(stderr, "register_type error %d\n", democode);
		publisher_shutdown(participant);
		return -1;
	}






	/* 4. 创建主题，并定制主题的QoS  */
	/* 建议1：在程序启动后优先创建Topic */
	/* 建议2：一种主题名创建一次即可，无需重复创建 */
	topic = participant->create_topic(
		"Example UserDataType"/* 主题名 */,
		type_name /* 类型名 */, TOPIC_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (topic == NULL) {
		fprintf(stderr, "create_topic error\n");
		publisher_shutdown(participant);
		return -1;
	}

	demo_topic = participant->create_topic(
		"Example demo"/* 主题名 */,
		demo_name /* 类型名 */, TOPIC_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (demo_topic == NULL) {
		fprintf(stderr, "create_topic error\n");
		publisher_shutdown(participant);
		return -1;
	}

	/* 5. 创建datawriter，并定制datawriter的QoS  */
	/* 建议1：在程序启动后优先创建datawriter */
	/* 建议2：创建一次即可，无需重复创建 */
	/* 建议3：在程序退出时再进行释放 */
	/* 建议4：避免打算发送数据时创建datawriter，发送数据后删除，该做法消耗资源，影响性能 */
	writer = publisher->create_datawriter(
		topic , DATAWRITER_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (writer == NULL) {
		fprintf(stderr, "create_datawriter error\n");
		publisher_shutdown(participant);
		return -1;
	}
	UserDataType_writer = UserDataTypeDataWriter::narrow(writer);
	if (UserDataType_writer == NULL) {
		fprintf(stderr, "DataWriter narrow error\n");
		publisher_shutdown(participant);
		return -1;
	}



	demo_writer = publisher->create_datawriter(
		demo_topic , DATAWRITER_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (demo_writer == NULL) {
		fprintf(stderr, "create_datawriter error\n");
		publisher_shutdown(participant);
		return -1;
	}
	demoType_writer = demoDataWriter::narrow(demo_writer);
	if (demoType_writer == NULL) {
		fprintf(stderr, "DataWriter narrow error2\n");
		publisher_shutdown(participant);
		return -1;
	}




	/* 6. 创建一个数据样本 */
	/* 建议：该数据为new出来的，使用后用户需要调用delete_data进行释放内存*/
	instance = UserDataTypeTypeSupport::create_data();
	if (instance == NULL) {
		fprintf(stderr, "UserDataTypeTypeSupport::create_data error\n");
		publisher_shutdown(participant);
		return -1;
	}




	demo_instance = demoTypeSupport::create_data();
	if (demo_instance == NULL) {
		fprintf(stderr, "demoTypeSupport::create_data error\n");
		publisher_shutdown(participant);
		return -1;
	}



	std::vector<uint8_t> data_buffer(255, 0);
    auto start_time = std::chrono::system_clock::now();
    uint64_t msgcount = 0;

	/* 7. 主循环 ，发送数据 */
	for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {

        std::generate(data_buffer.begin(), data_buffer.end(), []() {
            return static_cast<uint8_t>(std::rand() % 256);
            });
        memcpy(instance->value, data_buffer.data(), 255);



		retcode = UserDataType_writer->write(*instance, instance_handle);
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "write error %d\n", retcode);
		}
		// else
			// fprintf(stderr, "%d : write  successfully . . \n", count);


        msgcount++;

        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - start_time).count() / 1000.0;
        if (elapsed_time >= 1.0) {
            double throughput = msgcount * 255 * 8 / elapsed_time;
            std::cout << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " Mbps" << std::endl;

			// write the value to demo->value
			demo_instance->value = throughput;
			democode = demoType_writer->write(*demo_instance, demo_instance_handle);
			if (democode != RETCODE_OK) {
				fprintf(stderr, "write error %d\n", retcode);
			}


            // reset the counter and timer
            msgcount = 0;
            start_time = std::chrono::system_clock::now();
        }

        // sleep for 1 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(1));


	}







	/* 8. 删除数据样本 */
	retcode = UserDataTypeTypeSupport::delete_data(instance);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "UserDataTypeTypeSupport::delete_data error %d\n", retcode);
	}



	democode = demoTypeSupport::delete_data(demo_instance);
	if (democode != RETCODE_OK) {
		fprintf(stderr, "demoTypeSupport::delete_data error %d\n", democode);
	}



	/* 9. 删除所有实例 */
	return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
	int domain_id = 0;
	int sample_count = 0; /* 无限循环 */

	if (argc >= 2) {
		domain_id = atoi(argv[1]);  /* 发送至域domain_id */
	}
	if (argc >= 3) {
		sample_count = atoi(argv[2]); /* 发送sample_count次 */
	}

	return publisher_main(domain_id, sample_count);
}
