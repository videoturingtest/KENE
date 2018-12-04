#ifndef KENE_SYS_H
#define KENE_SYS_H
#include "kene-tool.h"
#include "kema-api.h"
#include "kept-api.h"
#include "kpat-api.h"

namespace NS_KENE {

struct SInput {		// CSV format input
	string period_num, start_time, end_time, speaker, caption;
};

struct SKene {		// KENE result
	int n;				// argument count -- > 0
	char **args;		// argument list -- args[0] : predicate
};
//############################################################################
enum {
	TAG_CC = 1,	TAG_CD,		TAG_DT,		TAG_EX,		TAG_IN,
	TAG_JJ,		TAG_JJR,	TAG_JJS,	TAG_LS,		TAG_MD,
	TAG_NN,		TAG_NNS,	TAG_PDT,	TAG_POS,	TAG_PRP,
	TAG_PRPS,	TAG_RB,		TAG_RBR,	TAG_RBS,	TAG_RP,
	TAG_SYM,	TAG_TO,		TAG_UH,		TAG_VB,		TAG_VBD,
	TAG_VBG,	TAG_VBN,	TAG_VBZ,	TAG_WDT,	TAG_WP,
	TAG_WPS,	TAG_WRB,
	NO_TAGS = TAG_WRB
};

extern CChar *TAG_TBL[1 + NO_TAGS];

inline CChar *TagName(unsigned id) {
	return (id <= NO_TAGS) ? TAG_TBL[id] : TAG_TBL[0];
}
//############################################################################
inline void OutKene(ostream &os, const SKene *p) {
	os << " <" << p->args[0] << "> ( ";
	if (p->n > 1) os << '<' << p->args[1] << '>';
	for (int i = 2; i < p->n; ++i)
		os << ", <" << p->args[i] << '>';
	os << " )\n";
}
inline void OutKene(ostream &os, const SKene *p, int n) {
	for (const SKene *up = p + n; p < up; ++p)
		OutKene(os, p);
}
//############################################################################
inline void ParseInput(char *s, int lno, SInput &in) {
	char *p;
	if (!(p = strchr(s, ','))) Throw(lno << ": bad input format");
	*p = '\0'; in.period_num = s; s = p + 1;
	if (!(p = strchr(s, ','))) Throw(lno << ": bad input format");
	*p = '\0'; in.start_time = s; s = p + 1;
	if (!(p = strchr(s, ','))) Throw(lno << ": bad input format");
	*p = '\0'; in.end_time = s; s = p + 1;
	if (!(p = strchr(s, ','))) Throw(lno << ": bad input format");
	*p = '\0'; in.speaker = s;
	in.caption = p + 1;
}

inline void OutStrJson(ostream &os, const char *s) {
	for (os << '"'; *s; ++s) {
		if (*s == '"') os << '\\';
		os << *s;
	}
	os << '"';
}
inline void OutKeneJson(ostream &os, const SKene *p) {
	os <<    "   {";
	os <<  "\n    \"pred\": "; OutStrJson(os, p->args[0]);
	os << ",\n    \"args\": [ ";
	if (p->n > 1) OutStrJson(os, p->args[1]);
	for (int i = 2; i < p->n; ++i)
		os << ", ", OutStrJson(os, p->args[i]);
	os << " ]\n   }";
}
inline void OutKeneJson(ostream &os, const SKene *p, int n) {
	if (n <= 0) return;
	OutKeneJson(os, p);
	for (const SKene *up = p + n; ++p < up; )
		os << ",\n", OutKeneJson(os, p);
	os << '\n';
}
inline void OutEntryJson(ostream &os, const SInput &in, const SKene *p, int n) {
	os <<  "\n {";
	os <<  "\n  \"period_num\": "; OutStrJson(os, in.period_num.c_str());
	os << ",\n  \"start_time\": "; OutStrJson(os, in.start_time.c_str());
	os << ",\n  \"end_time\": "; OutStrJson(os, in.end_time.c_str());
	os << ",\n  \"speaker\": "; OutStrJson(os, in.speaker.c_str());
	os << ",\n  \"caption\": "; OutStrJson(os, in.caption.c_str());
	os << ",\n  \"results\": ";
	if (n > 0) {
		os << "[\n";
		OutKeneJson(os, p, n);
		os << "  ]\n";
	} else
		os << "[ ]\n";
	os << " }";
}
//############################################################################
struct SCfg {	// KENE configuration manager
	string
		path,
		tag_def,
		kema_dat, kema_dic,
		kept_dat, kept_rule,
		kpat_freq, kpat_ptn_s1, kpat_ptn_s2, kpat_ptn_s3, kpat_ptn_g;
	void Set(CChar *path, CChar *fn);
};
//############################################################################
class CDat {		// KENE data manager
public:
	CDat() {
		m_hdKemaSys = 0, m_hdKema = 0;
		m_hdKeptSys = 0, m_hdKept = 0;
		m_hdFreq = 0, m_hdToken = 0;
		m_hdPtnS1 = 0, m_hdPtnS2 = 0, m_hdPtnS3 = 0, m_hdPtnG = 0;
	}
	~CDat() { Unload(); }

	void Load(SCfg *p) { LoadSys(p); LoadUser(p); }
	void ReloadUser(SCfg *p) { UnloadUser(); LoadUser(p); }
	void Unload() {
		char msg[1024];
		if (KEMA_unload_data(msg, m_hdKemaSys)) Throw(msg); m_hdKemaSys = 0;
		if (KEPT_unload_data(msg, m_hdKeptSys)) Throw(msg); m_hdKeptSys = 0;
		if (KPAT_unload_data(msg, m_hdFreq)) Throw(msg); m_hdFreq = 0;
		if (KPAT_unload_data(msg, m_hdToken))Throw(msg); m_hdToken = 0;
		UnloadUser();
	}
	void *HdKema() const { return m_hdKema; }
	void *HdKept() const { return m_hdKept; }
	void *HdFreq() const { return m_hdFreq; }
	void *HdToken() const { return m_hdToken; }
	void *HdPtnS1() const { return m_hdPtnS1; }
	void *HdPtnS2() const { return m_hdPtnS2; }
	void *HdPtnS3() const { return m_hdPtnS3; }
	void *HdPtnG() const { return m_hdPtnG; }
private:
	void *m_hdKemaSys, *m_hdKema;
	void *m_hdKeptSys, *m_hdKept;
	void *m_hdFreq, *m_hdToken;
	void *m_hdPtnS1, *m_hdPtnS2, *m_hdPtnS3, *m_hdPtnG;

	void LoadSys(SCfg *p);
	void LoadUser(SCfg *p);
	void UnloadUser() {
		char msg[1024];
		if (KEMA_unload_data(msg, m_hdKema)) Throw(msg); m_hdKema = 0;
		if (KEPT_unload_data(msg, m_hdKept)) Throw(msg); m_hdKept = 0;
		if (KPAT_unload_data(msg, m_hdPtnS1)) Throw(msg); m_hdPtnS1 = 0;
		if (KPAT_unload_data(msg, m_hdPtnS2)) Throw(msg); m_hdPtnS2 = 0;
		if (KPAT_unload_data(msg, m_hdPtnS3)) Throw(msg); m_hdPtnS3 = 0;
		if (KPAT_unload_data(msg, m_hdPtnG)) Throw(msg); m_hdPtnG = 0;
	}
};
//############################################################################
class CSys {			// KENE system
public:
	enum {
		K_ = 1024, M_ = 1024 * K_,
		MEM_INIT = 2 * M_, MEM_INCR = 1 * M_,
		POOL_INIT = 100, POOL_INCR = 100,
	};

	CSys(): m_Mem(MEM_INIT, MEM_INCR), m_Pool(POOL_INIT, POOL_INCR) {
		m_bTest = false;
		m_hKema = 0; m_hKept = 0;
		m_hToken = 0, m_hPtnS1 = 0, m_hPtnS2 = 0, m_hPtnS3 = 0, m_hPtnG = 0;
	}
	~CSys() { Destroy(); }

	void Create();
	void Destroy() {
		char msg[1024];
		if (KEMA_destroy(msg, m_hKema)) Throw(msg); m_hKema = 0;
		if (KEPT_destroy(msg, m_hKept)) Throw(msg); m_hKept = 0;
		if (KPAT_destroy(msg, m_hToken)) Throw(msg); m_hToken = 0;
		if (KPAT_destroy(msg, m_hPtnS1)) Throw(msg); m_hPtnS1 = 0;
		if (KPAT_destroy(msg, m_hPtnS2)) Throw(msg); m_hPtnS2 = 0;
		if (KPAT_destroy(msg, m_hPtnS3)) Throw(msg); m_hPtnS3 = 0;
		if (KPAT_destroy(msg, m_hPtnG)) Throw(msg); m_hPtnG = 0;
	}

	bool m_bTest;

	// return # of SKene entries
	int Extract(const CDat *pd, const char *sent_str, SKene *&kene_list);
private:
	CMemX m_Mem;			// temp. memory object
	CPool<SKene> m_Pool;	// result pool object
	void *m_hKema;
	KEMA_option m_optKema;
	void *m_hKept;
	KEPT_option m_optKept;
	void *m_hToken, *m_hPtnS1, *m_hPtnS2, *m_hPtnS3, *m_hPtnG;
	KPAT_tokener_option m_optToken;
	KPAT_matcher_option m_optPtnS, m_optPtnG;
	KEMA_sent *m_pSent;
	KPAT_token *m_Tokens; int m_NoToken;

	static void ChangeTag(KPAT_token *p, int n) {
		KPAT_token *up = p + n; do {
			switch (p->tag) {
			case TAG_VBZ: p->tag = TAG_VB; break;
			case TAG_NNS: p->tag = TAG_NN; break;
			case TAG_PRP: p->tag = TAG_NN; break;
			case TAG_JJR: p->tag = TAG_JJ; break;
			case TAG_JJS: p->tag = TAG_JJ; break;
			case TAG_RBR: p->tag = TAG_RB; break;
			case TAG_RBS: p->tag = TAG_RB; break;
			}
		} while (++p < up);
	}
	void SetSurfForm();

	void AdjustResultG(KPAT_result_general *p, int n) {
		KPAT_result_general *up = p + n;
		KPAT_result_record *pr, *upr;
		KPAT_result_field *pf, *upf;
		do {
			pr = p->p_record, upr = pr + p->n_record; do {
				pf = pr->p_field, upf = pf + pr->n_field;
				do { if (*pf->str == ' ') pf->str++; } while (++pf < upf);
			} while (++pr < upr);
		} while (++p < up);
	}

	void MakeResult(const KPAT_result_general *p_result_g, int n_result_g);
	static void MkRStr(string &sb, string &sf, const char *s);
	static void MkRStr(char *s) {
		for (; *s; ++s)
			if (*s == '_') *s = ' ';
	}

	static bool IsNegForm(const char *s) {
		return StrEq(s, "not") || StrEq(s, "never");
	}
	bool IsNegated(const KPAT_result_field *pf) const;

	static void OutKema(ostream &os, const KEMA_sent *p);

	static void OutToken(ostream &os, const KPAT_token *p, int n);
	void OutSpecial(ostream &os, const KPAT_result_special *p, int n) const;
	static void OutGeneral(ostream &os, const KPAT_result_general *p, int n);
};
}
#endif
