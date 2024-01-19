#include "portagecfg.hpp"
#include "version.hpp"

#include <iostream>
#include <fstream>

void usage(const std::string& name)
{
	std::cout << name << ": usage:\n" << std::endl;
	std::cout << "-h | --help" << "\t\t\t\t" << "Shows this help.\n";
	std::cout << "-l | --license <license>" << "\t\t" << "Write given license to portage's licenses folder.\n";
	std::cout << "-k | --keyword <keyword>" << "\t\t" << "Write given keyword to portage's keywords folder.\n";
	std::cout << "-p | --package <category/package_name>" << '\t' << "The package in question. This option is mandatory!\n";
	std::cout << "-u | --useflag <use flag>" << "\t\t" << "Write given use flag to portage's use flags folder.\n";
	std::cout << "-v | --version" << "\t\t\t\t" << "Shows this program's version." << std::endl;
}

bool writeConfig(const std::string& package,
		const std::vector<std::string>& values,
		CONFIG where)
{
	if (values.empty())
		return true;

	const std::string PORTAGE_FOLDER { "/etc/portage/" };
	const std::string LICENSES_FOLDER { PORTAGE_FOLDER + "package.license/" };
	const std::string KEYWORDS_FOLDER { PORTAGE_FOLDER + "package.accept_keywords/" };
	const std::string USES_FOLDER { PORTAGE_FOLDER + "package.use/" };

	std::string path;

	switch (where) {
	case CONFIG::LICENSE:
		path = LICENSES_FOLDER;
		break;
	case CONFIG::KEYWORD:
		path = KEYWORDS_FOLDER;
		break;
	case CONFIG::USEFLAG:
		path = USES_FOLDER;
		break;
	}

	auto filename = package.substr(package.rfind('/') + 1, package.size());
	std::ofstream file(path + filename, std::ios::out | std::ios::app);

	const std::string comment { "# This file was written by portagecfg." };
	std::string contents { package };
	for (const auto& value : values)
		contents += " " + value;
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
