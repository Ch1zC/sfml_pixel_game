#pragma once

#include "public_info.h"
#include <string>
#include <map>

class Quest_Manager {
public:
    // ¾çÇé½×¶Î
    void set_stage(std::string quest_name, int stage) { 

        Utils::print_log("FFFF - set stage: " + quest_name + " -> " + std::to_string(stage));
        quest_stages[quest_name] = stage; 
     }
    int  get_stage(std::string quest_name) {
        
        Utils::print_log("FFFF - get stage: " + quest_name);
        return quest_stages.find(quest_name) == quest_stages.end() ? 0 : quest_stages[quest_name];
    }

    // ÎïÆ·×´Ì¬
    void set_flag(std::string flag_name, bool value) { quest_flags[flag_name] = value; }
    bool get_flag(std::string flag_name)             { return quest_flags[flag_name] ; }

private:
    std::map<std::string, int> quest_stages;
    std::map<std::string, bool> quest_flags;
};