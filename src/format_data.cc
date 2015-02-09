/*
 * format_data.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */
#include <cstring>
#include <cerrno>
#include <sstream>
#include <fstream>
#include "sys/stat.h"
#include "simple_log.h"
#include "format_data.h"

const static int SPLIT_SIZE = 1;
const static int ENDL_SIZE = 1;

FormatData::~FormatData() {
    if (fs) {
        fs.close();
    }
}

int FormatData::insert(std::string &key, std::string &value) {
    if (value.size() > config.value_limit_size) {
        LOG_ERROR("value SIZE REACH LIMIT which size:%d, limit:%d", value.size(), config.value_limit_size);
        return -1;
    }
    size_t line_size = get_line_size();
    char line[line_size];
    bzero(line, line_size);

    std::string line_format = get_line_format();
    snprintf(line, line_size, line_format.c_str(), key.c_str(), value.c_str());

    fs.seekp(0, fs.end);
    fs.write(line, line_size);

    return 0;
}

int FormatData::update(std::string &key, size_t line_index, std::string &value) {
    if (value.size() > config.value_limit_size) {
        LOG_ERROR("value SIZE REACH LIMIT which size:%d, limit:%d", value.size(), config.value_limit_size);
        return -1;
    }
    size_t line_size = get_line_size();
    char line[line_size];
    bzero(line, line_size);

    std::string line_format = get_line_format();
    snprintf(line, line_size, line_format.c_str(), key.c_str(), value.c_str());

    size_t offset = line_index * line_size;
    fs.seekp(offset, fs.beg);
    LOG_DEBUG("SEEKP FOR OFFSET : %u", offset);
    fs.write(line, line_size);

    return 0;
}

int FormatData::init(FormatDataConfig &_config) {
    config = _config;
    std::string file_path = config.dir + config.file_name;
    fs.open(file_path.c_str(), std::fstream::out | std::fstream::in);
    if (!fs) {
        LOG_INFO("FILE NOT FOUND WE WILL create it:%s", file_path.c_str());
        std::fstream create_fs(file_path.c_str(), std::fstream::app);
        create_fs.close();

        fs.open(file_path.c_str(), std::fstream::out | std::fstream::in);
        if (!fs) {
            LOG_ERROR("open file error for path:%s", file_path.c_str());
            return -1;
        }
    }
    this->config.key_limit_size = config.key_limit_size;
    this->config.value_limit_size = config.value_limit_size;
    struct stat tmp_stat;
    if(stat(file_path.c_str(), &tmp_stat) != 0){
        LOG_WARN("Oops  stats file failed [path:%s][errno:%s]", file_path.c_str(), strerror(errno));
        return -1;
    }
    this->write_index = tmp_stat.st_size / this->get_line_size();
    return 0;
}

int FormatData::load_data(std::map<std::string, std::string> &map) {
    std::string file_path = config.dir + config.file_name;
    std::fstream data_fs(file_path.c_str(), std::fstream::in);
    if (!data_fs) {
        LOG_INFO("load_data NOT FOUND FILE: %s", file_path.c_str());
        return -1;
    }
    while(data_fs.good()) {
        size_t line_size = this->get_line_size();
        char line[line_size];
        data_fs.read(line, line_size);
        if (!data_fs) {
            break;
        }

        std::string line_format = get_line_format();

        char key[config.key_limit_size];
        char value[config.value_limit_size];
        sscanf(line, line_format.c_str(), key, value);
        char fixed_key[config.key_limit_size];
        char fixed_value[config.value_limit_size + 1];
        std::stringstream key_format;
        key_format << "%" << config.key_limit_size << "s";
        snprintf(fixed_key, config.key_limit_size + 1, key_format.str().c_str(), key);

        LOG_DEBUG("LOAD DATA KEY: %s, value:%s", fixed_key, value);
        map[fixed_key] = value;
    }
    data_fs.close();
    return 0;
}

int FormatData::get(size_t &line_index, std::string &value) {
    size_t line_size = this->get_line_size();
    size_t offset = line_index * line_size;
    if (!fs) {
        LOG_ERROR("FS not valid!");
        return -1;
    }
    fs.seekg(offset, fs.beg);
    if (!fs || !fs.good()) {
        LOG_ERROR("FS not valid! after seekg offset:%u", offset);
        return -1;
    }
    char line[line_size];
    fs.read(line, line_size);
    if (!fs) {
        LOG_ERROR("FS not valid! after read offset:%u, line_size:%u", offset, line_size);
        return -1;
    }
    fs.seekg(0, fs.end);

    LOG_DEBUG("READ LINE : %s FOR LINE INDE:%u, offset:%u", line, line_index, offset);
    char value_part[config.value_limit_size];
    char *start_point = line + config.key_limit_size + SPLIT_SIZE;
    memcpy(value_part, start_point, config.value_limit_size);
    value.assign(value_part, config.value_limit_size);
    return 0;
}

size_t FormatData::get_line_size() {
    return config.key_limit_size + SPLIT_SIZE + config.value_limit_size + ENDL_SIZE + 1;
}

std::string FormatData::get_line_format() {
    std::stringstream line_format;
    line_format << "%" << config.key_limit_size << "s\t%" << config.value_limit_size << "s\n";
    return line_format.str();
}

size_t FormatData::get_write_index() {
    return write_index;
}

size_t FormatData::incr_write_index() {
    write_index++;
    return write_index;
}
