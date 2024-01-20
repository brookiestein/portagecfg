#ifndef PORTAGECFG_HPP
#define PORTAGECFG_HPP

#include <string>
#include <vector>

enum CONFIG {
    MASK, LICENSE, KEYWORD,
    UNMASK, USEFLAG
};

enum ERROR_CODES {
    SUCCESS = 0,
    NOT_ENOUGH_ARGS,
    NOT_ENOUGH_PERMISSIONS,
    PACKAGE_NOT_PROVIDED
};

void usage(const std::string& name);
bool writeConfig(const std::string& package,
            const std::vector<std::string>& values,
            CONFIG cfg,
            const std::vector<std::string>& comments,
            std::string filename = "");
void version(const std::string& name);

#endif // PORTAGECFG_HPP
