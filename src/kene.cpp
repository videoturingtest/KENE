#include "kene-sys.h"
using namespace NS_KENE;

#define USAGE \
"Usage: kene path config-file [-lex|-token|-match]\n" \
"       -lex : show lexical analysis result\n" \
"       -token : show token list\n" \
"       -match : show match result\n"

void kene_main(const char *path, const char *fn_cfg, const SKeneOption *pOpt);

int main(int argc, char **argv) {
	SKeneOption opt = { false, false, false, &cout };

	if (argc < 3) { cerr << USAGE; return 1; }
	for (int i = 3; i < argc; i++)
		if (!strcmp(argv[i], "-lex")) opt.show_lex = true;
		else if (!strcmp(argv[i], "-token")) opt.show_token = true;
		else if (!strcmp(argv[i], "-match")) opt.show_match = true;
		else { cerr << USAGE; return 1; }

	string err;
	try {
		kene_main(argv[1], argv[2], &opt);
	}
	catch (const char *s) { err = s; goto ERROR; }
	catch (const string &s) { err = s; goto ERROR; }
	catch (const exception &e) { err = e.what(); goto ERROR; }
	catch (...) { err = "unknown error"; goto ERROR; }
	return 0;

ERROR:
	cerr << "[ERROR] " << err << endl;
	return 1;
}
//============================================================================
void kene_main(const char *path, const char *fn_cfg, const SKeneOption *pOpt) {
	SCfg cfg; cfg.Set(path, fn_cfg);
	CDat dat; dat.Load(&cfg);
	CSys sys; sys.Create();
	sys.SetOption(pOpt);

	string sent_str;
	SKene *kene_list; int n_kene;

	cout << "Enter '##RU' to reload user data!\n";

	while (cout << ">> ", getline(cin, sent_str)) {
		if (sent_str == "##RU") {	// reload user data
			dat.ReloadUser(&cfg);
			cout << "-- reloaded user data\n";
			continue;
		}

		cout << ";;; " << sent_str << '\n';
		if ((n_kene = sys.Extract(&dat, sent_str.c_str(), kene_list)) > 0) {
			cout << "*** KENE result: " << n_kene << '\n';
			OutKene(cout, kene_list, n_kene);
		}
	}
}

