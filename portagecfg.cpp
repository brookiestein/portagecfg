#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <getopt.h>
#include <unistd.h>

enum CONFIG {
	LICENSE, KEYWORD, USEFLAG
};

enum ERROR_CODES {
	SUCCESS = 0,
	NOT_ENOUGH_ARGS,
	NOT_ENOUGH_PERMISSIONS,
	PACKAGE_NOT_PROVIDED
};

void usage(const std::string& name);
bool writeConfig(const std::string& package, const std::vector<std::string>& value, CONFIG cfg);

int main(int argc, char* argv[])
{
	if (argc == 1) {
		usage(argv[0]);
		return ERROR_CODES::NOT_ENOUGH_ARGS;
	}

	if (geteuid() != 0) {
		std::cerr << "No enough rights, you must run me as root.\n";
		std::clog << "What I'm going to do is to write in portage's folder,\n"
			"and that directory belongs to root, so I need rights to write in them." << std::endl;
		return ERROR_CODES::NOT_ENOUGH_PERMISSIONS;
	}

	const char* const short_options { "l:k:p:u:" };
	const struct option long_options[] = {
		{ "help",	no_argument,		nullptr,	'h' },
		{ "license",	required_argument,	nullptr,	'l' },
		{ "keyword",	required_argument,	nullptr,	'k' },
		{ "package",	required_argument,	nullptr,	'p' },
		{ "useflag",	required_argument,	nullptr,	'u' },
		{ nullptr }
	};

	std::vector<std::string> licenses {};
	std::vector<std::string> keywords {};
	std::string package {};
	std::vector<std::string> useflags {};

	int opt {};
	while ((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			return ERROR_CODES::SUCCESS;
		case 'l':
			licenses.push_back(std::string(optarg));
			break;
		case 'k':
			keywords.push_back(std::string(optarg));
			break;
		case 'p':
			package = std::string(optarg);
			break;
		case 'u':
			useflags.push_back(std::string(optarg));
			break;
		default:
			std::cerr << "Unknown option." << std::endl;
		}
	}

	writeConfig(package, licenses, CONFIG::LICENSE);
	writeConfig(package, keywords, CONFIG::KEYWORD);
	writeConfig(package, useflags, CONFIG::USEFLAG);

}

void usage(const std::string& name)
{
	std::cout << name << ": usage:\n" << std::endl;
	std::cout << "-l | --license <license>" << "\t\t" << "Write given license to portage's licenses folder.\n";
	std::cout << "-k | --keyword <keyword>" << "\t\t" << "Write given keyword to portage's keywords folder.\n";
	std::cout << "-p | --package <category/package_name>" << '\t' << "The package in question. This option is mandatory!\n";
	std::cout << "-u | --useflag <use flag>" << "\t\t" << "Write given use flag to portage's use flags folder.\n";
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
