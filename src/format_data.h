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

class FormatDataConfig {
public:
    FormatDataConfig();
    std::string dir;
    std::string file_name;
    int key_limit_size;
    int value_limit_size;
    int hash_size;
    time_t expire_seconds;
};

uint32_t get_key_hash(std::string &key);

class FormatLine {
public:
    std::string _key;
    int _key_size;
    std::string value;
    int _value_size;
    int status;
    int32_t next_index;
    time_t expired_time;

    const static int SPLIT_SIZE = 1;
    const static int MAX_LINE_DIGIT = 9;
    const static int MAX_TIME_LEN = 19;
    const static int ENDL_SIZE = 1;
    const static int IS_DELETE_NODE = 1;
    const static int IS_EMPTY_NODE = 2;

    FormatLine(int key_size, int value_size);

    int deserialize(char *input, int size);

    int serialize(char *output, int size);

    int write_to(std::fstream &fs, uint32_t line_index);

    std::string get_format();

    int get_line_size();

    bool is_deleted();

    bool is_empty();

    bool key_equal(std::string &input);
};

class FormatData {
private:
    FormatDataConfig config;
    std::fstream fs;

    std::fstream ext_fs;
    int ext_write_index;

    int get_main_index(uint32_t &line_index, FormatLine &fline);

    int get_next_ext_nodex(int32_t next_index, FormatLine &ext_fnode);
public:
    const static int RET_OF_FAIL = -1;
    const static int RET_OF_NOFOUND = -2;
    const static int RET_OF_DELETED = -3;
    const static int RET_OF_EXPIRED = -4;

    ~FormatData();

    int init(FormatDataConfig &_config);

    int update(std::string &key, std::string &value);

    int get(std::string &key, std::string &value);
    
    int del(std::string &key);

};

#endif /* FORMAT_DATA_H_ */
