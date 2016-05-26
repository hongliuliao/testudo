/*
 * data_storage.cc
 *
 *  Created on: Feb 8, 2015
 *      Author: liao
 */
#include <sstream>
#include "pthread.h"

#include "simple_log.h"
#include "data_storage.h"

int DataStorage::init(DataStorageConfig storage_config) {
    pthread_rwlock_init(&rwlock, NULL);
    int ret = format_data.init(storage_config.data_config);
    return ret;
}

int DataStorage::put(std::string &key, std::string &value) {
    pthread_rwlock_wrlock(&rwlock);
    int ret = this->format_data.update(key, value);
    pthread_rwlock_unlock(&rwlock);
    return ret;
}

int DataStorage::get(std::string &key, std::string &value) {
    pthread_rwlock_rdlock(&rwlock);
    int ret = format_data.get(key, value);
    pthread_rwlock_unlock(&rwlock);
    return ret;
}

int DataStorage::del(std::string &key) {
    pthread_rwlock_wrlock(&rwlock);
    int ret = format_data.del(key);
    pthread_rwlock_unlock(&rwlock);
    return ret;
}
