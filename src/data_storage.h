/*
 * data_storage.h
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */

#ifndef DATA_STORAGE_H_
#define DATA_STORAGE_H_

#include "format_data.h"
#include "data_index.h"

struct DataStorageConfig {
    DataIndexConfig index_config;
    FormatDataConfig data_config;
};

class DataStorage {
private:
    DataIndex data_index;
    FormatData format_data;

    int insert(std::string &key, std::string &value);
    int update(std::string &key, std::string &line_index, std::string &value);
public:
    int init(DataStorageConfig storage_config);

    int put(std::string &key, std::string &value);

    int get(std::string &key, std::string &value);

    int del(std::string &key);
};


#endif /* DATA_STORAGE_H_ */
