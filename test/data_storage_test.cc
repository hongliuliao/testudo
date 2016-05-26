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
#include "format_data.h"
#include "simple_log.h"

int test_basic() {
    int hash_size = 100000;

    DataStorageConfig config;
    config.data_config.dir = "/tmp/";
    config.data_config.file_name = "test_basic.data";
    config.data_config.key_limit_size = 15;
    config.data_config.value_limit_size = 100;
    config.data_config.hash_size = hash_size;
    config.data_config.expire_seconds = 1;

    DataStorage storage;
    int ret = storage.init(config);
    if (ret != 0) {
        LOG_ERROR("STORAGE INIT ERROR WHCH ret:%d", ret);
        return ret;
    }
    std::string key = "111";
    std::string value = "tom1";
    ret = storage.put(key, value);
    if (ret != 0) {
        return ret;
    }

    std::string key2 = "222";
    std::string value2 = "jetty3";
    ret = storage.put(key2, value2);

    std::string value3;
    ret = storage.get(key2, value3);
    if (ret != 0) {
        LOG_INFO("NOT FOUND FOR key2:%s", key2.c_str());
        return ret;
    } else {
        LOG_INFO("FOUND FOR key2:%s, value:%s", key2.c_str(), value3.c_str());
    }
    return 0;
}

int test_perf() {
    int hash_size = 100000;

    DataStorageConfig config;
    config.data_config.dir = "/tmp/";
    config.data_config.file_name = "test_perf.data";
    config.data_config.key_limit_size = 15;
    config.data_config.value_limit_size = 100;
    config.data_config.hash_size = hash_size;
    config.data_config.expire_seconds = 10;

    DataStorage storage;
    int ret = storage.init(config);
    if (ret != 0) {
        LOG_ERROR("STORAGE INIT ERROR WHCH ret:%d", ret);
        return ret;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    size_t put_size = 100000;
    for (size_t i = 0; i < put_size; i++) {
        std::stringstream temp;
        temp << "k_" << i;
        std::string key4 = temp.str();
        std::string value4 = "jetty6";
        ret = storage.put(key4, value4);
        if (ret != 0) {
            LOG_ERROR("CAN NOT PUT FOR KEY:%s, ret:%d", key4.c_str(), ret);
            continue;
        }
        //storage.del(key4);
        ret = storage.get(key4, value4);
        if (ret != 0) {
            LOG_ERROR("CAN NOT GET FOR KEY:%s, ret:%d", key4.c_str(), ret);
        }
    }

    gettimeofday(&end, NULL);
    int cost_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    LOG_INFO("####### TESTUDO hash_size:%d, put_size:%d, cost_time:%d ms #######", hash_size, put_size, cost_time);
    return 0;
}

int test_expire() {
    int hash_size = 100000;

    DataStorageConfig config;
    config.data_config.dir = "/tmp/";
    config.data_config.file_name = "test_expire.data";
    config.data_config.key_limit_size = 15;
    config.data_config.value_limit_size = 100;
    config.data_config.hash_size = hash_size;
    config.data_config.expire_seconds = 1;

    DataStorage storage;
    int ret = storage.init(config);
    if (ret != 0) {
        LOG_ERROR("STORAGE INIT ERROR WHCH ret:%d", ret);
        return ret;
    }
    std::string key = "aaa";
    std::string value = "bbb";
    storage.put(key, value);
    int wait_time = 2;
    LOG_INFO("start wait for data expired , wait time:%d", wait_time);
    sleep(wait_time);
    ret = storage.get(key, value); 
    LOG_INFO("get ret:%d, key:%s", ret, key.c_str());
    return 0;
}


int main() {
    //test_basic(); 
    test_perf();
    //test_expire();
    return 0;
}
