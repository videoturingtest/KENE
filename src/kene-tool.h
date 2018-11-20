#ifndef KENE_TOOL_H
#define KENE_TOOL_H
#include "kt-lib.h"
using namespace NS_KtLib;

namespace NS_KENE {

inline void SetPath(string &path) {
	if (!path.empty() &&
		path[path.size()-1] != '/' && path[path.size()-1] != '\\')
		path += '/';
}

int GetLine(istream &is, Uchar *line, Uint size, bool &b_over);
int CvLineCmt(Uchar *s, Uchar **forms, int max, Uchar dc);
//============================================================================
struct SLine {
	enum { LINE_SIZE = 1024, FORMS_SIZE = 256, CMT_CH = ';' };
	CChar *fn;
	ifstream is;
	Uint lno;
	Uchar line[LINE_SIZE], *forms[FORMS_SIZE];
	int len;

	SLine(CChar *fn) : fn(fn), is(fn), lno(0) {
		if (!is) Throw(fn << ": open error");
	}
	// return # of forms (> 0), -1 if EOF 
	int GetEnt(Uchar *p_line_org = 0) {
		int n; bool b_over;
	NEXT:
		if ((len = GetLine(is, line, LINE_SIZE, b_over)) < 0) return -1;
		lno++;
		if (b_over) Throw(fn << ':' << lno << " too long line");
		if (p_line_org) StrCpy(p_line_org, line);
		n = CvLineCmt(line, forms, FORMS_SIZE, CMT_CH);
		if (n < 0) Throw(fn << ':' << lno << " too many forms");
		if (!n) goto NEXT;
		return n;
	}

	// return # of value values (>= 0) (except key), -1 if EOF
	int GetEntCfg(char **&vals) {
		int n = GetEnt(); if (n < 0) return -1;
		Uchar *s = forms[0], *p = StrChr(s, (Uchar)'=');
		if (p) {
			if (p == s) goto BAD;
			*p++ = '\0';
			if (*p) { vals = (char**)forms; vals[0] = (char*)p; return n; }
			else { vals = (char**)forms + 1; return n - 1; }
		}
		if (n == 1) goto BAD;
		p = forms[1];
		if (*p != '=') goto BAD;
		if (*++p) { vals = (char**)forms + 1; vals[0] = (char*)p; return n - 1; }
		else { vals = (char**)forms + 2; return n - 2; }
	BAD:
		Throw(fn << ':' << lno << " bad format");
	}
};

}
#endif
