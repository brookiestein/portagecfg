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

	const char* const short_options { "hl:k:p:u:v" };
	const struct option long_options[] = {
		{ "help",	no_argument,		nullptr,	'h' },
		{ "license",	required_argument,	nullptr,	'l' },
		{ "keyword",	required_argument,	nullptr,	'k' },
		{ "package",	required_argument,	nullptr,	'p' },
		{ "useflag",	required_argument,	nullptr,	'u' },
		{ "version",	no_argument,		nullptr,	'v' },
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
			usage(name);
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

	writeConfig(package, licenses, CONFIG::LICENSE);
	writeConfig(package, keywords, CONFIG::KEYWORD);
	writeConfig(package, useflags, CONFIG::USEFLAG);
}

