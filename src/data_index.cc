/*
 * data_index.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */
#include "sstream"
#include "data_index.h"
#include "simple_log.h"

DataIndex::DataIndex() {
    mem_index = new std::map<std::string, std::string>();
}

DataIndex::~DataIndex() {
    if (mem_index != NULL) {
        delete mem_index;
        mem_index = NULL;
    }
}

int DataIndex::load_index(DataIndexConfig &config) {
    return this->format_index.load_data(*mem_index);
}

int DataIndex::init(DataIndexConfig &_config) {
    config = _config;
    FormatDataConfig data_config;
    data_config.dir = config.dir;
    data_config.file_name = config.file_name;
    data_config.key_limit_size = config.key_limit_size;
    data_config.value_limit_size = 10;

    int ret = format_index.init(data_config);
    this->format_index.load_data(*mem_index);

    return ret;
}

int DataIndex::insert(std::string &key, std::string &line_index) {
    (*mem_index)[key] = line_index;
    return format_index.insert(key, line_index);
}

int DataIndex::get(std::string &key, std::string &line_index) {
    char origin_key[this->config.key_limit_size + 1];
    std::stringstream key_format;
    key_format << "%" << config.key_limit_size << "s";
    snprintf(origin_key, config.key_limit_size + 1, key_format.str().c_str(), key.c_str());
    std::map<std::string, std::string>::iterator i = mem_index->find(origin_key);
    if (i == mem_index->end()) {
        LOG_DEBUG("CAN NOT FOUND FOR origin KEY:%s, input key:%s", origin_key, key.c_str());
        return -1;
    }
    line_index = i->second;
    return 0;
}

int DataIndex::del(std::string &key) {
    return mem_index->erase(key) == 1 ? 0 : -1;
}
