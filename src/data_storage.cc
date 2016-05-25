/*
 * data_storage.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */
#include <sstream>
#include "simple_log.h"
#include "data_storage.h"

int DataStorage::init(DataStorageConfig storage_config) {
    int ret = format_data.init(storage_config.data_config);
    return ret;
}

int DataStorage::put(std::string &key, std::string &value) {
    return this->format_data.update(key, value);
}

int DataStorage::get(std::string &key, std::string &value) {
    return format_data.get(key, value);
}

int DataStorage::del(std::string &key) {
    return format_data.del(key);
}
