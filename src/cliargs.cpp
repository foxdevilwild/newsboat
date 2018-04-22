#include "cliargs.h"

#include <iostream>

#include <getopt.h>
#include <unistd.h>

#include "globals.h"
#include "utils.h"

namespace newsboat {

void CLIArgs::parse(
		int argc,
		char* argv[])
{
	static const char getopt_str[] = "i:erhqu:c:C:d:l:vVoxXI:E:";
	static const struct option longopts[] = {
		{"cache-file"      , required_argument, 0, 'c'},
		{"config-file"     , required_argument, 0, 'C'},
		{"execute"         , required_argument, 0, 'x'},
		{"export-to-file"  , required_argument, 0, 'E'},
		{"export-to-opml"  , no_argument      , 0, 'e'},
		{"help"            , no_argument      , 0, 'h'},
		{"import-from-file", required_argument, 0, 'I'},
		{"import-from-opml", required_argument, 0, 'i'},
		{"log-file"        , required_argument, 0, 'd'},
		{"log-level"       , required_argument, 0, 'l'},
		{"quiet"           , no_argument      , 0, 'q'},
		{"refresh-on-start", no_argument      , 0, 'r'},
		{"url-file"        , required_argument, 0, 'u'},
		{"vacuum"          , no_argument      , 0, 'X'},
		{"version"         , no_argument      , 0, 'v'},
		{0                 , 0                , 0,  0 }
	};

	/* Now that silencing's set up, let's rewind to the beginning of argv and
	 * process the options */
	optind = 1;

	int c;
	while ((c = ::getopt_long(argc, argv, getopt_str, longopts, nullptr)) != -1) {
		switch (c) {
		case ':': /* fall-through */
		case '?': /* missing option */
			should_print_usage = true;
			should_exit_with_failure = true;
			return;
		case 'i':
			if (do_export) {
				should_print_usage = true;
				should_exit_with_failure = true;
				return;
			}
			do_import = true;
			importfile = optarg;
			break;
		case 'r':
			refresh_on_start = true;
			break;
		case 'e':
			// disable logging of newsboat's startup progress to stdout, because the
			// OPML export will be printed to stdout
			silent = true;
			if (do_import) {
				should_print_usage = true;
				should_exit_with_failure = true;
				return;
			}
			do_export = true;
			break;
		case 'h':
			should_print_usage = true;
			should_exit_with_success = true;
			return;
		case 'u':
			url_file = optarg;
			urlfile_given_on_cmdline = true;
			using_nonstandard_configs = true;
			break;
		case 'c':
			cache_file = optarg;
			lock_file = std::string(cache_file) + LOCK_SUFFIX;
			cachefile_given_on_cmdline = true;
			using_nonstandard_configs = true;
			break;
		case 'C':
			config_file = optarg;
			configfile_given_on_cmdline = true;
			using_nonstandard_configs = true;
			break;
		case 'X':
			do_vacuum = true;
			break;
		case 'v':
		case 'V':
			show_version++;
			break;
		case 'x':
			// disable logging of newsboat's startup progress to stdout, because the
			// command execution result will be printed to stdout
			silent = true;
			execute_cmds = true;
			break;
		case 'q':
			silent = true;
			break;
		case 'd':
			logger::getInstance().set_logfile(optarg);
			break;
		case 'l': {
			level l = static_cast<level>(atoi(optarg));
			if (l > level::NONE && l <= level::DEBUG) {
				logger::getInstance().set_loglevel(l);
			} else {
				std::cerr << strprintf::fmt(_("%s: %d: invalid loglevel value"), argv[0], l) << std::endl;
				should_exit_with_failure = true;
				return;
			}
		}
		break;
		case 'I':
			if (do_read_export) {
				should_print_usage = true;
				should_exit_with_failure = true;
				return;
			}
			do_read_import = true;
			readinfofile = optarg;
			break;
		case 'E':
			if (do_read_import) {
				should_print_usage = true;
				should_exit_with_failure = true;
				return;
			}
			do_read_export = true;
			readinfofile = optarg;
			break;
		default:
			std::cout << strprintf::fmt(_("%s: unknown option - %c"), argv[0], static_cast<char>(c)) << std::endl;
			should_print_usage = true;
			should_exit_with_failure = true;
			return;
		}
	};
}

} // newsboat
