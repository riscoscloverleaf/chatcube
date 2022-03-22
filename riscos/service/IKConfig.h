//
// Created by lenz on 9/1/20.
//

#ifndef ROCHAT_IKCONFIG_H
#define ROCHAT_IKCONFIG_H

#include <inipp/inipp.h>

class IKConfig {
public:
    static void start(const std::string& file_name);
    static std::string& get_value(const std::string& section, const std::string& key, std::string& default_val);
    static std::string get_value(const std::string& section, const std::string& key);
    static int get_value(const std::string& section, const std::string& key, int default_val);
    static void set_value(const std::string& section, const std::string& key, std::string& val);
    static void set_value(const std::string& section, const std::string& key, char* val);
    static void set_value(const std::string& section, const std::string& key, int val);
    static void stop();
private:
    std::string config_file_name;
    inipp::Ini<char> ini;
    static IKConfig* instance;
    void save();
};


#endif //ROCHAT_IKCONFIG_H
