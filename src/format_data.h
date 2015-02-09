/*
 * format_data.h
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */

#ifndef FORMAT_DATA_H_
#define FORMAT_DATA_H_

#include <string>
#include <map>
#include <fstream>

struct FormatDataConfig {
    std::string dir;
    std::string file_name;
    int key_limit_size;
    int value_limit_size;
};

class FormatData {
private:
    FormatDataConfig config;
    std::fstream fs;
    size_t write_index;

public:
    ~FormatData();

    int init(FormatDataConfig &_config);

    int insert(std::string &key, std::string &value);

    int update(std::string &key, size_t line_index, std::string &value);

    int get(size_t &line_index, std::string &value);

    int del(std::string &key);

    int load_data(std::map<std::string, std::string> &map);

    size_t get_line_size();

    std::string get_line_format();

    size_t get_write_index();

    size_t incr_write_index();
};

#endif /* FORMAT_DATA_H_ */
