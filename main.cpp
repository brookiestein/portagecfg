#include <iostream>
#include <string>
#include <vector>

#include <getopt.h>
#include <unistd.h>

#include "portagecfg.hpp"

int main(int argc, char* argv[])
{
    auto name { std::string(argv[0]) };
    if (name.find('/') != std::string::npos)
        name = name.substr(name.rfind('/') + 1, name.size());

    if (argc == 1) {
        usage(name);
        return ERROR_CODES::NOT_ENOUGH_ARGS;
    }

    const char* const short_options { "c:df:hml:k:p:Uu:v" };
    const struct option long_options[] = {
        { "comment",            required_argument,  nullptr,    'c' },
        { "default-comment",    required_argument,  nullptr,    'd' },
        { "filename",           required_argument,  nullptr,    'f' },
        { "help",               no_argument,        nullptr,	'h' },
        { "mask",               no_argument,        nullptr,    'm' },
        { "license",            required_argument,  nullptr,	'l' },
        { "keyword",            required_argument,  nullptr,	'k' },
        { "package",            required_argument,  nullptr,	'p' },
        { "unmask",             no_argument,        nullptr,    'U' },
        { "useflag",            required_argument,  nullptr,	'u' },
        { "version",            no_argument,        nullptr,	'v' },
        { nullptr }
    };

    std::vector<std::string> comments;
    std::string filename {};
    bool mask {false};
    bool default_comment {false};
    std::vector<std::string> licenses {};
    std::vector<std::string> keywords {};
    std::string package {};
    bool unmask {false};
    std::vector<std::string> useflags {};

    int opt {};
    while ((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (opt) {
        case 'c':
            comments.push_back(std::string(optarg));
            break;
        case 'd':
            default_comment = true;
            break;
        case 'f':
            filename = std::string(optarg);
            break;
        case 'h':
            usage(name);
            return ERROR_CODES::SUCCESS;
        case 'm':
            mask = true;
            if (unmask) {
                std::cerr << "Unmask option was given together to mask, deactivating unmask. Working with mask.\n";
                unmask = false;
            }
            break;
        case 'l':
            licenses.push_back(std::string(optarg));
            break;
        case 'k':
            keywords.push_back(std::string(optarg));
            break;
        case 'p':
            package = std::string(optarg);
            break;
        case 'U':
            unmask = true;
            if (mask) {
                std::cerr << "Mask option was given together to unmask, deactivating mask. Working with unmask.\n";
                mask = false;
            }
            break;
        case 'u':
            useflags.push_back(std::string(optarg));
            break;
        case 'v':
            version(name);
            return ERROR_CODES::SUCCESS;
        default:
            std::cerr << "Unknown option." << std::endl;
        }
    }

    if (geteuid() != 0) {
        std::cerr << "No enough rights, you must run me as root.\n";
        std::clog << "What I'm going to do is to write in portage's folder,\n"
            "and that directory belongs to root, so I need rights to write in them." << std::endl;
        return ERROR_CODES::NOT_ENOUGH_PERMISSIONS;
    }

    if (package.empty()) {
        std::cerr << "Package wasn't provided." << std::endl;
        return ERROR_CODES::PACKAGE_NOT_PROVIDED;
    }

    if (mask)
        writeConfig(package, { "" }, CONFIG::MASK, comments, filename, default_comment);
    else if (unmask)
        writeConfig(package, { "" }, CONFIG::UNMASK, comments, filename, default_comment);

    writeConfig(package, licenses, CONFIG::LICENSE, comments, filename, default_comment);
    writeConfig(package, keywords, CONFIG::KEYWORD, comments, filename, default_comment);
    writeConfig(package, useflags, CONFIG::USEFLAG, comments, filename, default_comment);
}
