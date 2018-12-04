#include <fstream>
#include "kene-sys.h"
using namespace NS_KENE;

#define USAGE \
"Usage: kene data-path config-file input-file output-file\n" \
"  or   kene data-path config-file (for interactive test)\n"

void kene_main(const char *path, const char *fn_cfg,
	const char *fn_in, const char *fn_out);
void kene_test(const char *path, const char *fn_cfg);

int main(int argc, char **argv) {
	if (argc != 3 && argc != 5)
		return cerr << USAGE, 1;

	string err;
	try {
		if (argc == 3)
			kene_test(argv[1], argv[2]);
		else
			kene_main(argv[1], argv[2], argv[3], argv[4]);
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
void kene_main(const char *path, const char *fn_cfg,
	const char *fn_in, const char *fn_out) {
	SCfg cfg; cfg.Set(path, fn_cfg);
	CDat dat; dat.Load(&cfg);
	CSys sys; sys.Create();
	sys.m_bTest = false;

	string line;
	SKene *kene_list; int n_kene;
	SInput in;
	int lno = 0;

	string p(path); SetPath(p);
	ifstream is((p+fn_in).c_str());
	if (!is) Throw(fn_in << ": open error");
	ofstream os((p + fn_out).c_str());
	if (!os) Throw(fn_out << ": open error");

	os << "[\n";

	while (getline(is, line)) {
		lno++;
		ParseInput((char*)line.c_str(), lno, in);
		n_kene = sys.Extract(&dat, in.caption.c_str(), kene_list);
		if (n_kene <= 0) continue;
		if (lno > 1) os << ",\n";
		OutEntryJson(os, in, kene_list, n_kene);
	}

	os << "\n\n]\n";
}
//============================================================================
void kene_test(const char *path, const char *fn_cfg) {
	SCfg cfg; cfg.Set(path, fn_cfg);
	CDat dat; dat.Load(&cfg);
	CSys sys; sys.Create();
	sys.m_bTest = true;

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
