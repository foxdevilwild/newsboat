#ifndef NEWSBOAT_CLIARGS_H_
#define NEWSBOAT_CLIARGS_H_

#include <string>

namespace newsboat {

class CLIArgs {
	public:
		CLIArgs() = default;

		void parse(
				const std::string& cache_file,
				const std::string& config_file,
				const std::string& lock_file,
				const std::string& url_file,
				int argc,
				char* argv[]);

		std::string importfile;
		std::string readinfofile;
		std::string cache_file;
		std::string config_file;
		std::string lock_file;
		std::string url_file;
		unsigned int show_version = 0;
		bool cachefile_given_on_cmdline = false;
		bool do_export = false;
		bool do_import = false;
		bool do_read_export = false;
		bool do_read_import = false;
		bool do_vacuum = false;
		bool execute_cmds = false;
		bool silent = false;
		bool using_nonstandard_configs = false;
		bool should_exit_with_success = false;
		bool should_exit_with_failure = false;
		bool refresh_on_start = false;
		bool should_print_usage = false;
};

} // newsboat

#endif /* NEWSBOAT_CLIARGS_H_ */
