#include <cstring>
#include <errno.h>
#include <iostream>

#include "cache.h"
#include "config.h"
#include "controller.h"
#include "rss.h"
#include "rsspp.h"
#include "view.h"

using namespace newsboat;

int main(int argc, char* argv[])
{
	utils::initialize_ssl_implementation();

	setlocale(LC_CTYPE, "");
	setlocale(LC_MESSAGES, "");

	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	rsspp::parser::global_init();

	controller c;
	newsboat::view v(&c);
	c.set_view(&v);

	int ret = c.run(argc, argv);

	rsspp::parser::global_cleanup();

	return ret;
}
