/*
 * data_index.h
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */

#ifndef DATA_INDEX_H_
#define DATA_INDEX_H_

#include <map>
#include <string>
#include <fstream>
#include "format_data.h"

struct DataIndexConfig {
    std::string dir;
    std::string file_name;
    int key_limit_size;
};

class DataIndex {
private:
    std::map<std::string, std::string> *mem_index;
    FormatData format_index;
    DataIndexConfig config;

    int load_index(DataIndexConfig &_config);

public:
    DataIndex();

    ~DataIndex();

    int init(DataIndexConfig &config);

    int insert(std::string &key, std::string &line_index);

    int get(std::string &key, std::string &line_index);

    int del(std::string &key);
};


#endif /* DATA_INDEX_H_ */
