/*
 * data_storage_test.cc
 *
 *  Created on: Feb 9, 2015
 *      Author: liao
 */
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include "data_storage.h"
#include "simple_log.h"

int main() {
    DataStorageConfig config;
    config.data_config.dir = "/tmp/";
    config.data_config.file_name = "testdudo_test.data";
    config.data_config.key_limit_size = 10;
    config.data_config.value_limit_size = 100;

    config.index_config.dir = "/tmp/";
    config.index_config.file_name = "testdudo_test.index";
    config.index_config.key_limit_size = 10;

    DataStorage storage;
    int ret = storage.init(config);
    if (ret != 0) {
        LOG_ERROR("STORAGE INIT ERROR WHCH ret:%d", ret);
        return ret;
    }
//
//    std::string key = "111";
//    std::string value = "tom1";
//    ret = storage.put(key, value);
//    if (ret != 0) {
//        return ret;
//    }
//
//    std::string key2 = "222";
//    std::string value2 = "jetty2";
//    ret = storage.put(key2, value2);

//    std::string value3;
//    ret = storage.get(key2, value3);
//    if (ret != 0) {
//        LOG_INFO("NOT FOUND FOR key2:%s", key2.c_str());
//        return ret;
//    } else {
//        LOG_INFO("FOUND FOR key2:%s, value:%s", key2.c_str(), value3.c_str());
//    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (size_t i = 0; i < 100000; i++) {
        std::stringstream temp;
        temp << i;
        std::string key4 = temp.str();
        std::string value4 = "jetty6";
        ret = storage.put(key4, value4);
//        ret = storage.get(key4, value4);
    }

    gettimeofday(&end, NULL);
    int cost_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    std::cout << "cost_time:" << cost_time << "ms" << std::endl;

    return ret;
}
