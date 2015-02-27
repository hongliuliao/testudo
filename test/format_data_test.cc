/*
 * format_data_test.cc
 *
 *  Created on: Feb 26, 2015
 *      Author: liao
 */
#include <map>
#include <sstream>
#include "simple_log.h"
#include "format_data.h"


int main() {
    FormatLine fline(15, 100);
    std::map<uint32_t, bool> key_map;
    for (size_t i = 0; i < 10000; i++) {
        std::stringstream temp;
        temp << "k_" << i;
        fline.key = temp.str();
        uint32_t hash = get_key_hash(fline.key) % 100000;
        if (key_map.find(hash) == key_map.end()) {
            key_map[hash] = true;
        }
    }
    LOG_INFO("GET KEY MAP SIZE:%u", key_map.size());
    return 0;
}
