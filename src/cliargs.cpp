#include "cliargs.h"

#include <iostream>

#include <getopt.h>
#include <unistd.h>

#include "globals.h"
#include "utils.h"

namespace newsboat {

void CLIArgs::parse(
		const std::string& _cache,
		const std::string& _config,
		const std::string& _lock,
		const std::string& _url,
		int argc,
		char* argv[])
{
	cache_file = _cache;
	config_file = _config;
	lock_file = _lock;
	url_file = _url;

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
			print_usage(argv[0]);
			should_exit_with_failure = true;
			return;
		case 'i':
			if (do_export) {
				print_usage(argv[0]);
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
				print_usage(argv[0]);
				should_exit_with_failure = true;
				return;
			}
			do_export = true;
			break;
		case 'h':
			print_usage(argv[0]);
			should_exit_with_success = true;
			return;
		case 'u':
			url_file = optarg;
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
				print_usage(argv[0]);
				should_exit_with_failure = true;
				return;
			}
			do_read_import = true;
			readinfofile = optarg;
			break;
		case 'E':
			if (do_read_import) {
				print_usage(argv[0]);
				should_exit_with_failure = true;
				return;
			}
			do_read_export = true;
			readinfofile = optarg;
			break;
		default:
			std::cout << strprintf::fmt(_("%s: unknown option - %c"), argv[0], static_cast<char>(c)) << std::endl;
			print_usage(argv[0]);
			should_exit_with_failure = true;
			return;
		}
	};
}

void CLIArgs::print_usage(char * argv0) {
	auto msg =
	    strprintf::fmt(_("%s %s\nusage: %s [-i <file>|-e] [-u <urlfile>] "
	    "[-c <cachefile>] [-x <command> ...] [-h]\n"),
	    PROGRAM_NAME,
	    PROGRAM_VERSION,
	    argv0);
	std::cout << msg;

	struct arg {
		const char name;
		const std::string longname;
		const std::string params;
		const std::string desc;
	};

	static const std::vector<arg> args = {
		{ 'e', "export-to-opml"  , ""                , _s("export OPML feed to stdout") }                                                 ,
		{ 'r', "refresh-on-start", ""                , _s("refresh feeds on start") }                                                     ,
		{ 'i', "import-from-opml", _s("<file>")      , _s("import OPML file") }                                                           ,
		{ 'u', "url-file"        , _s("<urlfile>")   , _s("read RSS feed URLs from <urlfile>") }                                          ,
		{ 'c', "cache-file"      , _s("<cachefile>") , _s("use <cachefile> as cache file") }                                              ,
		{ 'C', "config-file"     , _s("<configfile>"), _s("read configuration from <configfile>") }                                       ,
		{ 'X', "vacuum"          , ""                , _s("compact the cache") }                                                  ,
		{ 'x', "execute"         , _s("<command>..."), _s("execute list of commands") }                                                   ,
		{ 'q', "quiet"           , ""                , _s("quiet startup") }                                                              ,
		{ 'v', "version"         , ""                , _s("get version information") }                                                    ,
		{ 'l', "log-level"       , _s("<loglevel>")  , _s("write a log with a certain loglevel (valid values: 1 to 6)") }                 ,
		{ 'd', "log-file"        , _s("<logfile>")   , _s("use <logfile> as output log file") }                                           ,
		{ 'E', "export-to-file"  , _s("<file>")      , _s("export list of read articles to <file>") }                                     ,
		{ 'I', "import-from-file", _s("<file>")      , _s("import list of read articles from <file>") }                                   ,
		{ 'h', "help"            , ""                , _s("this help") }
	};

	for (const auto & a : args) {
		std::string longcolumn("-");
		longcolumn += a.name;
		longcolumn += ", --" + a.longname;
		longcolumn += a.params.size() > 0 ? "=" + a.params : "";
		std::cout << "\t" << longcolumn;
		for (unsigned int j = 0; j < utils::gentabs(longcolumn); j++) {
			std::cout << "\t";
		}
		std::cout << a.desc << std::endl;
	}
}

} // newsboat
