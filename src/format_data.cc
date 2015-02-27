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
static uint32_t dict_hash_function_seed = 5381;

FormatLine::FormatLine(int key_size, int value_size) {
    _key_size = key_size;
    _value_size = value_size;
    status = 0;
    next_index = -1;
}

int FormatLine::deserialize(char *buffer, int size) {
    char tmp_key[_key_size];
    bzero(tmp_key, _key_size);
    char tmp_value[_value_size];
    bzero(tmp_value, _value_size);
    char tmp_status[1];
    bzero(tmp_status, 1);
    char tmp_next_index[MAX_LINE_DIGIT];
    bzero(tmp_next_index, MAX_LINE_DIGIT);

    if (sscanf(buffer, get_format().c_str(), tmp_key, tmp_value, tmp_status, tmp_next_index) != 4) {
        LOG_ERROR("deserialize FAIL which input :%s", buffer);
        return -1;
    }

    key.assign(tmp_key);
    value.assign(tmp_value);
    status = atoi(tmp_status);
    next_index = atoi(tmp_next_index);
    return 0;
}

int FormatLine::serialize(char *result, int size) {
    std::string line_format = get_format();
    char line[size + 1];
    bzero(line, size + 1);

    std::stringstream status_ss;
    status_ss << status;
    std::stringstream next_index_ss;
    next_index_ss << next_index;
    snprintf(line, size + 1, line_format.c_str(), key.c_str(), value.c_str(), status_ss.str().c_str(), next_index_ss.str().c_str());
    memcpy(result, line, size);
    return 0;
}

// key \t value \t status \t next_index \n
std::string FormatLine::get_format() {
    std::stringstream line_format;
    line_format << "%" << _key_size << "s\t%" << _value_size << "s\t%1s\t%" << MAX_LINE_DIGIT << "s\n";
    return line_format.str();
};

int FormatLine::get_line_size() {
    return _key_size + SPLIT_SIZE + _value_size + SPLIT_SIZE + sizeof(char) + SPLIT_SIZE + MAX_LINE_DIGIT + ENDL_SIZE;
}

// MurmurHash2 from redis (dict.c)
uint32_t FormatLine::get_key_hash() {
    int len = key.size();
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key.c_str();

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

bool FormatLine::is_delete() {
    return status == IS_DELETE_NODE;
}

bool FormatLine::key_equal(std::string &input) {
//    LOG_DEBUG("GET KEY which input:%s, actual:%s", input.c_str(), key.c_str());
    return input == key;
}

int FormatLine::write_to(std::fstream &fs, uint32_t line_index) {
    size_t line_size = this->get_line_size();
    char line[line_size];
    bzero(line, line_size);
    this->serialize(line, line_size);

    size_t offset = line_index * line_size;
    fs.seekp(offset);
    fs.write(line, line_size);
    if (!fs || !fs.good()) {
        LOG_ERROR("fs not valid after write which offset:%u", offset);
        return -1;
    }
    return 0;
}

FormatData::~FormatData() {
    if (fs) {
        fs.close();
    }
    if (ext_fs) {
        ext_fs.close();
    }
}


int FormatData::init(FormatDataConfig &_config) {
    config = _config;
    std::string file_path = config.dir + config.file_name;
    fs.open(file_path.c_str(), std::fstream::out | std::fstream::in);
    FormatLine fline(config.key_limit_size, config.value_limit_size);
    if (!fs) {
        LOG_INFO("FILE NOT FOUND WE WILL create it:%s", file_path.c_str());
        size_t file_size = config.hash_size * fline.get_line_size();
        std::ofstream create_fs(file_path.c_str());
        create_fs.seekp(file_size);
        create_fs.write("\n", 1);
        create_fs.close();

        fs.open(file_path.c_str(), std::fstream::out | std::fstream::in);
        if (!fs) {
            LOG_ERROR("open file error for path:%s", file_path.c_str());
            return -1;
        }

    }

    std::string ext_file_path = config.dir + config.file_name + ".ext";
    ext_fs.open(ext_file_path.c_str(), std::fstream::out | std::fstream::in);
    if (!ext_fs) {
        LOG_INFO("FILE NOT FOUND WE WILL create it:%s", ext_file_path.c_str());
        std::ofstream create_fs(ext_file_path.c_str(), std::fstream::app);
        create_fs.close();

        ext_fs.open(ext_file_path.c_str(), std::fstream::out | std::fstream::in);
        if (!ext_fs) {
            LOG_ERROR("ext_fs open fail in init! which path:%s", ext_file_path.c_str());
            return -1;
        }
    }

    this->config.key_limit_size = config.key_limit_size;
    this->config.value_limit_size = config.value_limit_size;

    struct stat tmp_stat;
    if(stat(ext_file_path.c_str(), &tmp_stat) != 0){
        LOG_WARN("Oops  stats ext file failed [path:%s][errno:%s]", ext_file_path.c_str(), strerror(errno));
        return -1;
    }
    this->ext_write_index = tmp_stat.st_size / fline.get_line_size();
    return 0;
}

int FormatData::update(std::string &key, std::string &value, bool is_delete) {
    if (key.size() > config.key_limit_size) {
        LOG_ERROR("key SIZE REACH LIMIT which size:%d, limit:%d", key.size(), config.key_limit_size);
        return -1;
    }
    if (value.size() > config.value_limit_size) {
        LOG_ERROR("value SIZE REACH LIMIT which size:%d, limit:%d", value.size(), config.value_limit_size);
        return -1;
    }

    FormatLine bucket_node(config.key_limit_size, config.value_limit_size);
    bucket_node.key = key;
    bucket_node.value = value;

    uint32_t hash = bucket_node.get_key_hash();
    uint32_t line_index = hash % config.hash_size;

    int ret = this->get_by_index(line_index, bucket_node);
    if (ret == GET_RET_OF_FAIL) {
        return ret;
    }

    if (ret == GET_RET_OF_NOFOUND) {
        return bucket_node.write_to(fs, line_index);
    }

    if (bucket_node.key_equal(key)) { // update
        if (!is_delete) {
            bucket_node.value = value;
        } else {
            bucket_node.status = FormatLine::IS_DELETE_NODE;
        }
        return bucket_node.write_to(fs, line_index);
    }

    int32_t current_index = -1;
    bool update_bucket = true;
    FormatLine ext_fnode(config.key_limit_size, config.value_limit_size);

    // FIND INSERT OR UPDATE POSTION
    int32_t next_index = bucket_node.next_index;
    while (next_index != -1) {
        int ret = get_next_ext_nodex(next_index, ext_fnode);
        if (ret != 0) {
            return ret;
        }
        if (ext_fnode.key_equal(key)) {
            if (!is_delete) {
                ext_fnode.value = value;
            } else {
                ext_fnode.status = FormatLine::IS_DELETE_NODE;
            }
            return ext_fnode.write_to(ext_fs, next_index);
        }
        update_bucket = false;
        current_index = next_index;
        next_index = ext_fnode.next_index;
    }

    // write for data file
    // 1. insert new node
    FormatLine new_ext_fnode(config.key_limit_size, config.value_limit_size);
    new_ext_fnode.key = key;
    new_ext_fnode.value = value;
    ret = new_ext_fnode.write_to(ext_fs, ext_write_index);
    if (ret != 0) {
        return ret;
    }

    // 2. update preposition node
    if (update_bucket) { // 2.1 update hash bucket node
        bucket_node.next_index = ext_write_index;
        ret = bucket_node.write_to(fs, line_index);
    } else {             // 2.2 update hash data node
        ext_fnode.next_index = ext_write_index;
        ret = ext_fnode.write_to(ext_fs, current_index);
    }
    ext_write_index++;

    return ret;
}

int FormatData::get_next_ext_nodex(int32_t next_index, FormatLine &ext_fnode) {
    int line_size = ext_fnode.get_line_size();
    size_t ext_offset = next_index * line_size;
    char ext_node[line_size];
    bzero(ext_node, line_size);
    ext_fs.seekg(ext_offset);
    ext_fs.read(ext_node, line_size);
    if (!ext_fs) {
        LOG_ERROR("EXT FS not valid after read! which next_index:%d", next_index);
        return -1;
    }
    return ext_fnode.deserialize(ext_node, line_size);
}

int FormatData::get(std::string &key, std::string &value) {
    FormatLine format_line(config.key_limit_size, config.value_limit_size);
    format_line.key = key;
    int line_size = format_line.get_line_size();
    uint32_t hash = format_line.get_key_hash();
    uint32_t line_index = hash % config.hash_size;

    LOG_DEBUG("GET FOR KEY:%s, hash:%u, line_index:%u", key.c_str(), hash, line_index);
    int ret = this->get_by_index(line_index, format_line);
    if (ret != 0) {
        return ret;
    }

    if (format_line.key_equal(key)) {
        value = format_line.value;
        return 0;
    }
    int32_t next_index = format_line.next_index;
    FormatLine ext_fnode(config.key_limit_size, config.value_limit_size);
    while(next_index != -1) {
        int ret = get_next_ext_nodex(next_index, ext_fnode);
        if (ret != 0) {
            return ret;
        }
        if (ext_fnode.key_equal(key)) {
            value = ext_fnode.value;
            return 0;
        }
        next_index = ext_fnode.next_index;

    }

    return GET_RET_OF_NOFOUND;
}


int FormatData::get_by_index(uint32_t &line_index, FormatLine &fline) {
    size_t line_size = fline.get_line_size();
    size_t offset = line_index * line_size;
    if (!fs) {
        LOG_ERROR("FS not valid!");
        return -1;
    }
    fs.seekg(offset, fs.beg);
    if (fs.eof()) {
        LOG_DEBUG("FS REACH EOF ON OFFSET:%u", offset);
        return GET_RET_OF_NOFOUND;
    }
    if (!fs || !fs.good()) {
        LOG_ERROR("FS not valid! after seekg offset:%u", offset);
        return -1;
    }
    char line[line_size];
    bzero(line, line_size);
    fs.read(line, line_size);
    if (!fs) {
        LOG_ERROR("FS not valid! after read offset:%u, line_size:%u", offset, line_size);
        return -1;
    }
    fs.seekg(0, fs.end);
    char empty_key[fline._key_size];
    bzero(empty_key, fline._key_size);
    if (memcmp(line, empty_key, fline._key_size) == 0) {
        return GET_RET_OF_NOFOUND;
    }

    LOG_DEBUG("READ LINE : %s FOR LINE INDE:%u, offset:%u", line, line_index, offset);

    int ret = fline.deserialize(line, line_size);
    if (ret != 0) {
        LOG_ERROR("FLINE deserialize fail for line:%s", line);
        return ret;
    }

    if (fline.is_delete()) {
        LOG_DEBUG("fline is delete");
        return GET_RET_OF_DELETED;
    }

    return 0;
}
