/*
 * data_storage_test.cc
 *
 *  Created on: Feb 9, 2015
 *      Author: liao
 */
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
        return ret;
    }

    std::string key = "111";
    std::string value = "tom1";
    ret = storage.put(key, value);
    if (ret != 0) {
        return ret;
    }

    std::string key2 = "222";
    std::string value2 = "jetty2";
    ret = storage.put(key2, value2);

    std::string value3;
    ret = storage.get(key2, value3);
    if (ret != 0) {
        LOG_INFO("NOT FOUND FOR key2:%s", key2.c_str());
        return ret;
    } else {
        LOG_INFO("FOUND FOR key2:%s, value:%s", key2.c_str(), value3.c_str());
    }

    std::string key4 = "444";
    std::string value4 = "jetty5";
    ret = storage.put(key4, value4);

    std::string key5 = "555";
    std::string value5 = "jetty4";
    ret = storage.put(key5, value5);

    return ret;
}
