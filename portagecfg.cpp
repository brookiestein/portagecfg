#include "portagecfg.hpp"
#include "version.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

void usage(const std::string& name)
{
    std::cout << name << ": usage:\n" << std::endl;
    std::cout << "-c | --comment" << "\t\t\t\t" << "Optional comment to be added at the beginning of the configuration.\n";
    std::cout << "-f | --filename" << "\t\t\t\t" << "Optional filename to write the configuration to. If not specified, package name will be used instead.\n";
    std::cout << "-h | --help" << "\t\t\t\t" << "Shows this help.\n";
    std::cout << "-l | --license <license>" << "\t\t" << "Write given license to portage's licenses folder.\n";
    std::cout << "-m | --mask" << "\t\t\t\t" << "Mask given package.\n";
    std::cout << "-k | --keyword <keyword>" << "\t\t" << "Write given keyword to portage's keywords folder.\n";
    std::cout << "-p | --package <category/package_name>" << '\t' << "The package in question. This option is mandatory!\n";
    std::cout << "-u | --useflag <use flag>" << "\t\t" << "Write given use flag to portage's use flags folder.\n";
    std::cout << "-U | --unmask" << "\t\t\t\t" << "Unmask given package.\n";
    std::cout << "-v | --version" << "\t\t\t\t" << "Shows this program's version." << std::endl;
}

bool writeConfig(const std::string& package,
        const std::vector<std::string>& values,
        CONFIG where,
        const std::vector<std::string>& comments,
        std::string filename)
{
    if (values.empty())
        return true;

    const std::string PORTAGE_FOLDER { "/etc/portage/" };
    const std::string MASK_FOLDER { PORTAGE_FOLDER + "package.mask/" };
    const std::string LICENSES_FOLDER { PORTAGE_FOLDER + "package.license/" };
    const std::string KEYWORDS_FOLDER { PORTAGE_FOLDER + "package.accept_keywords/" };
    const std::string UNMASK_FOLDER { PORTAGE_FOLDER + "package.unmask/" };
    const std::string USES_FOLDER { PORTAGE_FOLDER + "package.use/" };

    std::string path;

    switch (where) {
    case CONFIG::MASK:
        path = MASK_FOLDER;
        break;
    case CONFIG::LICENSE:
        path = LICENSES_FOLDER;
        break;
    case CONFIG::KEYWORD:
        path = KEYWORDS_FOLDER;
        break;
    case CONFIG::UNMASK:
        path = UNMASK_FOLDER;
        break;
    case CONFIG::USEFLAG:
        path = USES_FOLDER;
        break;
    }

    if (not std::filesystem::exists(path)) {
        std::clog << "Directory: '" << path << "' doesn't exist. Creating it.\n";
        if (not std::filesystem::create_directory(path)) {
            std::cerr << "Error creating directory: '" << path << "'. Aborting.\n";
            return false;
        }
    }

    if (filename.empty())
        filename = package.substr(package.rfind('/') + 1, package.size());
    auto fullpath = path + filename;
    std::ofstream file(fullpath, std::ios::out | std::ios::app);

    std::string comment { "# This file was written by portagecfg." };
    for (const auto& c : comments)
        comment += "\n# " + c;

    std::string contents { package };

    // If we want to mask or unmask a package, just write the package name to the corresponding directory.
    if (where != CONFIG::MASK and where != CONFIG::UNMASK) {
        for (const auto& value : values)
            contents += " " + value;
    }

    std::clog << "Writing: '" << contents << "' to: " << path + filename << std::endl;

    file << comment << std::endl;
    file << contents << std::endl;
    file.flush();

    return true;
}

void version(const std::string& name)
{
    std::cout << name << " version " << VERSION << std::endl;
}
