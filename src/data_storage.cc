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
    int ret = data_index.init(storage_config.index_config);
    if (ret != 0) {
        return ret;
    }
    ret = format_data.init(storage_config.data_config);
    return ret;
}

int DataStorage::insert(std::string &key, std::string &value) {
    // insert
    int ret = format_data.insert(key, value);
    if (ret != 0) {
        return ret;
    }
    size_t line_index = format_data.get_write_index();
    std::stringstream line_index_ss;
    line_index_ss << line_index;
    std::string line_index_str = line_index_ss.str();

    ret = data_index.insert(key, line_index_str);
    format_data.incr_write_index();
    return ret;
}

int DataStorage::update(std::string &key, std::string &line_index, std::string &value) {
    return format_data.update(key, (size_t) atol(line_index.c_str()), value);
}

int DataStorage::put(std::string &key, std::string &value) {
    std::string line_index_str;
    int ret = data_index.get(key, line_index_str);
    if (ret == 0) { // found
        LOG_DEBUG("DataStorage FOUND FOR KEY:%s will update", key.c_str());
        return update(key, line_index_str, value);
    }
    return insert(key, value);
}

int DataStorage::get(std::string &key, std::string &value) {
    std::string line_index_str;

    int ret = data_index.get(key, line_index_str);
    if (ret != 0) {
        return ret;
    }

    LOG_DEBUG("GET LINE INDEX :%s for key:%s", line_index_str.c_str(), key.c_str());
    size_t line_index = (size_t) atol(line_index_str.c_str());
    return format_data.get(line_index, value);
}

int DataStorage::del(std::string &key) {
    return 0;
}
