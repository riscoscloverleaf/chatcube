//
// Created by lenz on 9/1/20.
//

#include <fstream>
#include "IKConfig.h"
#include "../utils.h"

IKConfig* IKConfig::instance = nullptr;

void IKConfig::start(const std::string &file_name) {
    if (!instance) {
        instance = new IKConfig();
    }
    instance->config_file_name = file_name;
    if (is_file_exist(file_name)) {
        std::ifstream is(file_name);
        instance->ini.parse(is);
        is.close();
    }
}

void IKConfig::save() {
    std::ofstream os(instance->config_file_name);
    instance->ini.generate(os);
    os.close();
}

void IKConfig::stop() {
    if (instance) {
        delete instance;
    }
}

std::string& IKConfig::get_value(const std::string &section, const std::string &key, std::string& default_val) {
    auto &sections = instance->ini.sections;
    auto sfound = sections.find(section);
    if (sfound != sections.end()) {
        auto keyfound = sfound->second.find(key);
        if (keyfound != sfound->second.end()) {
            return keyfound->second;
        }
    }
    return default_val;
}

std::string IKConfig::get_value(const std::string &section, const std::string &key) {
    auto &sections = instance->ini.sections;
    auto sfound = sections.find(section);
    if (sfound != sections.end()) {
        auto keyfound = sfound->second.find(key);
        if (keyfound != sfound->second.end()) {
            return keyfound->second;
        }
    }
    return std::string();
}

int IKConfig::get_value(const std::string &section, const std::string &key, int default_val) {
    auto &sections = instance->ini.sections;
    auto sfound = sections.find(section);
    if (sfound != sections.end()) {
        auto keyfound = sfound->second.find(key);
        if (keyfound != sfound->second.end()) {
            std::basic_string<char> strval = keyfound->second;
            int val = default_val;
            if (inipp::extract(strval, val)) {
                return val;
            }
        }
    }
    return default_val;
}

void IKConfig::set_value(const std::string &section, const std::string &key, std::string &val) {
    instance->ini.sections[section][key] = val;
    instance->save();
}

void IKConfig::set_value(const std::string &section, const std::string &key,  char *val) {
    instance->ini.sections[section][key] = std::string(val);
    instance->save();
}

void IKConfig::set_value(const std::string &section, const std::string &key, int val) {
    instance->ini.sections[section][key] = to_string(val);
    instance->save();
}
