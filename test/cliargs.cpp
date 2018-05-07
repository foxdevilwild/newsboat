#include "3rd-party/catch.hpp"

#include <iostream>
#include <cstring>

#include <cliargs.h>

using namespace newsboat;

/// Helper class to create argc and argv arguments for CLIArgs
///
/// When testing CLIArgs, resource management turned out to be a problem:
/// CLIArgs requires char** pointing to arguments, but a pointer like it can't
/// be easily obtained from any of standard containers. To overcome that,
/// I wrote Opts, which simply copies elements of initializer_list into
/// separate unique_ptr<char>, and presents useful accessors argc() and argv()
/// whose results can be passed right into CLIArgs. Problem solved!
class Opts
{
	/// Individual elements of argv.
	std::vector<std::unique_ptr<char[]>> m_opts;
	/// This is argv as main() knows it.
	std::unique_ptr<char*[]> m_data;
	/// This is argc as main() knows it.
	std::size_t m_argc;

	public:
		/// Turns \a opts into argc and argv.
		Opts(std::initializer_list<std::string> opts)
			: m_argc(opts.size())
		{
			m_opts.reserve(m_argc);

			for (const std::string& option : opts) {
				std::cerr << "Copying `" << option << "'\n";
				// Copy string into separate char[], managed by unique_ptr.
				auto ptr = std::unique_ptr<char[]>(new char[option.size() + 1]);
				std::cerr << (void*)ptr.get() << '\n';
				std::copy(option.cbegin(), option.cend(), ptr.get());
				std::cerr << (void*)ptr.get() << '\n';
				ptr.get()[option.size()] = '\0';

				// Hold onto the smart pointer to keep the entry in argv alive.
				m_opts.emplace_back(std::move(ptr));
			}

			// Copy out intermediate argv vector into its final storage.
			m_data = std::unique_ptr<char*[]>(new char*[m_argc + 1]);
			int i = 0;
			for (const auto& ptr : m_opts) {
				m_data.get()[i++] = ptr.get();
			}
			m_data.get()[i] = nullptr;
		}

		std::size_t argc() const
		{
			return m_argc;
		}

		char** argv() const
		{
			return m_data.get();
		}
};

TEST_CASE("Asks to print usage info and exit with failure if unknown option is "
		"provided", "[cliargs1]")
{
	SECTION("Example No.1") {
		const Opts opts {"newsboat", "--some-unknown-option"};
		CLIArgs args(opts.argc(), opts.argv());

		REQUIRE(args.should_print_usage);
		REQUIRE(args.should_exit_with_failure);
		REQUIRE_FALSE(args.should_exit_with_success);
	}

	SECTION("Example No.2") {
		const Opts opts {"newsboat", "-s"};
		CLIArgs args(opts.argc(), opts.argv());

		REQUIRE(args.should_print_usage);
		REQUIRE(args.should_exit_with_failure);
		REQUIRE_FALSE(args.should_exit_with_success);
	}

	SECTION("Example No.3") {
		const Opts opts {"newsboat", "-m ix"};
		CLIArgs args(opts.argc(), opts.argv());
		//char* opts[] {::strdup("newsboat"), ::strdup("-mix"), nullptr};
		//CLIArgs args(2, opts);

		REQUIRE(args.should_print_usage);
		REQUIRE(args.should_exit_with_failure);
		REQUIRE_FALSE(args.should_exit_with_success);
	}

	SECTION("Example No.4") {
		const Opts opts {"newsboat", "-wtf"};
		CLIArgs args(opts.argc(), opts.argv());
		//char* opts[] {::strdup("newsboat"), ::strdup("-wtf"), nullptr};
		//CLIArgs args(2, opts);

		REQUIRE(args.should_print_usage);
		REQUIRE(args.should_exit_with_failure);
		REQUIRE_FALSE(args.should_exit_with_success);
	}
}

TEST_CASE("Sets `do_import` and `importfile` if -i/--import-from-opml is "
		"provided", "[cliargs]")
{
	SECTION("-i") {
		const std::string filename("blogroll.opml");
		const Opts opts {"newsboat", "-i", filename};
		CLIArgs args(opts.argc(), opts.argv());

		REQUIRE(args.do_import);
		REQUIRE(args.importfile == filename);
	}

	SECTION("--import-from-opml") {
		const std::string filename("blogroll.opml");
		const Opts opts {"newsboat", "--import-from-opml=" + filename};
		CLIArgs args(opts.argc(), opts.argv());

		REQUIRE(args.do_import);
		REQUIRE(args.importfile == filename);
	}
}

TEST_CASE("Asks to print usage and exit with failure if both "
		"-i/--import-from-opml and -e/--export-to-opml are provided",
		"[cliargs]")
{
}

TEST_CASE("Sets `refresh_on_start` if -r/--refresh-on-start is provided",
		"[cliargs]")
{
}

TEST_CASE("Requests silent mode if -e/--export-to-opml is provided",
		"[cliargs]")
{
}

TEST_CASE("Sets `do_export` if -e/--export-to-opml is provided", "[cliargs]")
{
}

TEST_CASE("Asks to print usage and exit with success if -h/--help is provided",
		"[cliargs]")
{
}

TEST_CASE("Sets `url_file`, `urlfile_given_on_cmdline`, and "
		"`using_nonstandard_configs` if -u/--url-file is provided", "[cliargs]")
{
}

TEST_CASE("Sets `cache_file`, `lock_file`, `cachefile_given_on_cmdline`, and "
		"`using_nonstandard_configs` if -c/--cache-file is provided", "[cliargs]")
{
}

TEST_CASE("Sets `config_file`, `configfile_given_on_cmdline`, and "
		"`using_nonstandard_configs` if -C/--config-file is provided",
		"[cliargs]")
{
}

TEST_CASE("Sets `do_vacuum` if -X/--vacuum is provided", "[cliargs]")
{
}

TEST_CASE("Increases `version` with each -v/-V/--version provided",
		"[cliargs]")
{
}

TEST_CASE("Requests silent mode if -x/--execute is provided", "[cliargs]")
{
}

TEST_CASE("Sets `execute_cmds` if -x/--execute is provided", "[cliargs]")
{
}

TEST_CASE("Requests silent mode if -q/--quiet is provided", "[cliargs]")
{
}

TEST_CASE("Sets `do_read_import` and `readinfofile` if -I/--import-from-file "
		"is provided", "[cliargs]")
{
}

TEST_CASE("Sets `do_read_export` and `readinfofile` if -E/--export-to-file "
		"is provided", "[cliargs]")
{
}

TEST_CASE("Asks to print usage info and exit with failure if both "
		"-I/--import-from-file and -E/--export-to-file are provided", "[cliargs]")
{
}

// TODO: specify behaviour if no options are provided
