/*
 * data_storage.h
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */

#ifndef DATA_STORAGE_H_
#define DATA_STORAGE_H_

#include "format_data.h"

struct DataStorageConfig {
    FormatDataConfig data_config;
};

class DataStorage {
private:
    FormatData format_data;
public:
    int init(DataStorageConfig storage_config);

    int put(std::string &key, std::string &value);

    /**
     * GET_RET_OF_FAIL == -1
     * GET_RET_OF_NOFOUND = -2;
     * GET_RET_OF_DELETED = -3;
     */
    int get(std::string &key, std::string &value);

    int del(std::string &key);
};


#endif /* DATA_STORAGE_H_ */
