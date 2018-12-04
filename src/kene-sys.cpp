#include "kene-sys.h"

namespace NS_KENE {

CChar *TAG_TBL[1 + NO_TAGS] = {
	"??",
	"CC",	"CD",	"DT",	"EX",	"IN",
	"JJ",	"JJR",	"JJS",	"LS",	"MD",
	"NN",	"NNS",	"PDT",	"POS",	"PRP",
	"PRPS",	"RB",	"RBR",	"RBS",	"RP",
	"SYM",	"TO",	"UH",	"VB",	"VBD",
	"VBG",	"VBN",	"VBZ",	"WDT",	"WP",
	"WPS",	"WRB",
};
//############################################################################
#define Err(ol, m) Throw((ol).fn<<':'<<(ol).lno<<' '<<(m))
#define Bad(ol) Err((ol), "bad format")
#define NoCfg(fn, m) Throw(fn << ": no " << #m)

void SCfg::Set(CChar *data_path, CChar *fn) {
	path = data_path; SetPath(path);
	SLine oL((path+fn).c_str());
	char *&key = (char*&)oL.forms[0], **vals; int n;
	while ((n = oL.GetEntCfg(vals)) >= 0) {
		if (n != 1) Bad(oL);
		else if (StrUpEq(key, "TAG_DEF"))		tag_def = *vals;
		else if (StrUpEq(key, "KEMA_DAT"))		kema_dat = *vals;
		else if (StrUpEq(key, "KEMA_DIC"))		kema_dic = *vals;
		else if (StrUpEq(key, "KEPT_DAT"))		kept_dat = *vals;
		else if (StrUpEq(key, "KEPT_RULE"))		kept_rule = *vals;
		else if (StrUpEq(key, "KPAT_FREQ"))		kpat_freq = *vals;
		else if (StrUpEq(key, "KPAT_PTN_S_1"))	kpat_ptn_s1 = *vals;
		else if (StrUpEq(key, "KPAT_PTN_S_2"))	kpat_ptn_s2 = *vals;
		else if (StrUpEq(key, "KPAT_PTN_S_3"))	kpat_ptn_s3 = *vals;
		else if (StrUpEq(key, "KPAT_PTN_G"))	kpat_ptn_g = *vals;
		else Bad(oL);
	}
	if (tag_def.empty())		NoCfg(fn, TAG_DEF);
	if (kema_dat.empty())		NoCfg(fn, KEMA_DAT);
	if (kema_dic.empty())		NoCfg(fn, KEMA_DIC);
	if (kept_dat.empty())		NoCfg(fn, KEPT_DAT);
	if (kept_rule.empty())		NoCfg(fn, KEPT_RULE);
	if (kpat_freq.empty())		NoCfg(fn, KPAT_FREQ);
	if (kpat_ptn_g.empty())		NoCfg(fn, KPAT_PTN_G);
}
//############################################################################
#define CSTR(s) ((char*)s.c_str())
#define	LT	KPAT_LT_KONAN_E
#define	CS	KPAT_CS_UTF8

void CDat::LoadSys(SCfg *p) {
	char msg[1024];

	if (KEMA_load_data_sys(msg, &m_hdKemaSys, CSTR(p->path),
		CSTR(p->kema_dat)))
		Throw(msg);
	if (KEPT_load_data_sys(msg, &m_hdKeptSys, CSTR(p->path),
		CSTR(p->kept_dat)))
		Throw(msg);
	if (KPAT_load_frequency_data(msg, &m_hdFreq, CSTR(p->path),
		CSTR(p->kpat_freq), 0))
		Throw(msg);
	if (KPAT_load_token_data(msg, &m_hdToken, LT, CS, 0, CSTR(p->path),
		CSTR(p->tag_def), 0, 0, 0, 0))
		Throw(msg);
}

void CDat::LoadUser(SCfg *p) {
	char msg[1024];

	if (KEMA_load_data(msg, &m_hdKema, m_hdKemaSys, (char*)"NN", CSTR(p->path),
		0, CSTR(p->kema_dic), 0, 0, 0))
		Throw(msg);
	if (KEPT_load_data(msg, &m_hdKept, m_hdKeptSys, CSTR(p->path),
		CSTR(p->kept_rule)))
		Throw(msg);
	if (!p->kpat_ptn_s1.empty() &&
		KPAT_load_kpat_data(msg, &m_hdPtnS1, LT, CS, true,
		0, m_hdFreq, m_hdToken,
		CSTR(p->path), CSTR(p->kpat_ptn_s1), 0, 0))
		Throw(msg);
	if (!p->kpat_ptn_s2.empty() &&
		KPAT_load_kpat_data(msg, &m_hdPtnS2, LT, CS, true,
		0, m_hdFreq, m_hdToken,
		CSTR(p->path), CSTR(p->kpat_ptn_s2), 0, 0))
		Throw(msg);
	if (!p->kpat_ptn_s3.empty() &&
		KPAT_load_kpat_data(msg, &m_hdPtnS3, LT, CS, true,
			0, m_hdFreq, m_hdToken,
			CSTR(p->path), CSTR(p->kpat_ptn_s3), 0, 0))
		Throw(msg);
	if (KPAT_load_kpat_data(msg, &m_hdPtnG, LT, CS, false,
		0, m_hdFreq, m_hdToken,
		CSTR(p->path), CSTR(p->kpat_ptn_g), 0, 0))
		Throw(msg);
}
//############################################################################
static void KpatSlotter(string *ps, const KPAT_token *p, char not_used) {
	*ps += ' '; *ps += p->str_surf;
}

void CSys::Create() {
	char msg[1024];

	m_optKema = KEMA_get_default_option();
	m_optKept = KEPT_get_default_option();
	m_optToken = KPAT_get_default_tokener_option();
	m_optPtnS = KPAT_get_default_matcher_option();
	m_optPtnG = KPAT_get_default_matcher_option();

	m_optKema.m_perform_profile = 0;

	m_optPtnS.perform_profile = 1;
	m_optPtnS.spacing_char_token = '_';
	m_optPtnS.spacing_char_slot = '_';

	m_optPtnG.user_slotter = (KPAT_PF_SLOTTER)KpatSlotter;
	m_optPtnG.apply_overlap_match = 0;
	m_optPtnG.perform_profile = 1;

	if (KEMA_create(msg, &m_hKema, &m_optKema)) Throw(msg);
	if (KEPT_create(msg, &m_hKept, &m_optKept)) Throw(msg);
	if (KPAT_create_tokener(msg, &m_hToken, &m_optToken)) Throw(msg);
	if (KPAT_create_matcher(msg, &m_hPtnS1, &m_optPtnS)) Throw(msg);
	if (KPAT_create_matcher(msg, &m_hPtnS2, &m_optPtnS)) Throw(msg);
	if (KPAT_create_matcher(msg, &m_hPtnS3, &m_optPtnS)) Throw(msg);
	if (KPAT_create_matcher(msg, &m_hPtnG, &m_optPtnG)) Throw(msg);
}
//============================================================================
int CSys::Extract(const CDat *pd, const char *sent_str, SKene *&kene_list) {
	char msg[1024];
	KEMA_sent sent;
	KEMA_word *pw;
	KPAT_token *pt;
	KPAT_result_special	*ps;
	KPAT_result_general	*pg;
	int n, nt, ns_all, done;
	
	m_Mem.Reset(); m_Pool.Reset();
MORE:
	ns_all = 0;
	if (KEMA_analyze_doc(msg, m_hKema, pd->HdKema(), &sent,
		(char*)sent_str, 0, &done)) Throw(msg);
	if (sent.m_n_word <= 0) goto DONE;
	m_pSent = &sent;
	if (m_bTest)
		cout << "; " << sent.m_str << '\n';

	// skip if question
	pw = sent.m_p_word + sent.m_n_word - 1;
	if (*pw->m_p_token[pw->m_n_token - 1].m_str == '?')
		goto DONE;

	if (KEPT_tag(msg, m_hKept, pd->HdKept(), &sent)) Throw(msg);
	if (m_bTest) {
		cout << "=== lexical analysis result: " << sent.m_n_word << '\n';
		OutKema(cout, &sent);
	}

	if (KPAT_set_token_list(msg, m_hToken, pd->HdPtnG(), &sent, &pt, &nt, 0))
		Throw(msg);
	if (nt <= 0) goto DONE;
	if (m_bTest) {
		cout << "=== original token list: " << nt << '\n';
		OutToken(cout, pt, nt);
	}

	m_Tokens = pt;
	ChangeTag(pt, nt);

	if (pd->HdPtnS1()) {
		if (KPAT_match_special(msg, m_hPtnS1, pd->HdPtnS1(), pt, nt, &ps, &n))
			Throw(msg);
		if (m_bTest && n > 0) {
			cout << "=== special match 1 result: " << n << '\n';
			OutSpecial(cout, ps, n);
		}
		if ((nt = KPAT_replace_token_list(n, pt, nt)) <= 0) goto DONE;
		ns_all += n;
	}
	if (pd->HdPtnS2()) {
		if (KPAT_match_special(msg, m_hPtnS2, pd->HdPtnS2(), pt, nt, &ps, &n))
			Throw(msg);
		if (m_bTest && n > 0) {
			cout << "=== special match 2 result: " << n << '\n';
			OutSpecial(cout, ps, n);
		}
		if ((nt = KPAT_replace_token_list(n, pt, nt)) <= 0) goto DONE;
		ns_all += n;
	}
	if (pd->HdPtnS3()) {
		if (KPAT_match_special(msg, m_hPtnS3, pd->HdPtnS3(), pt, nt, &ps, &n))
			Throw(msg);
		if (m_bTest && n > 0) {
			cout << "=== special match 3 result: " << n << '\n';
			OutSpecial(cout, ps, n);
		}
		if ((nt = KPAT_replace_token_list(n, pt, nt)) <= 0) goto DONE;
		ns_all += n;
	}

	m_NoToken = nt;
	SetSurfForm();

	if (m_bTest && ns_all > 0) {
		cout << "=== final token list: " << nt << '\n';
		OutToken(cout, pt, nt);
	}

	if (KPAT_match_general(msg, m_hPtnG, pd->HdPtnG(), pt, nt, &pg, &n))
		Throw(msg);
	if (n <= 0) goto DONE;

	AdjustResultG(pg, n);
	if (m_bTest) {
		cout << "=== general match result: " << n << '\n';
		OutGeneral(cout, pg, n);
	}

	MakeResult(pg, n);

DONE:
	if (done < (int)strlen(sent_str)) { sent_str += done; goto MORE; }

	kene_list = m_Pool.Pool();
	return (int)m_Pool.Size();
}
//============================================================================
void CSys::SetSurfForm() {
	const KEMA_word *pW = m_pSent->m_p_word, *pw, *qw;
	KPAT_token *pT = m_Tokens, *upT = pT + m_NoToken;
	const KEMA_token *pt, *qt;
	string s;
	do {
		if (pT->start_word_index == pT->end_word_index &&
			pT->start_lemma_index == pT->end_lemma_index)
			continue;

		pw = pW + pT->start_word_index, qw = pW + pT->end_word_index;
		pt = pw->m_p_token + pT->start_lemma_index;
		if (pw == qw) {
			qt = pw->m_p_token + pT->end_lemma_index;
			s = pt->m_str_surf;
			while (++pt <= qt) s += '_', s += pt->m_str_surf;
			goto DONE;
		}
		qt = pw->m_p_token + pw->m_n_token;
		s = pt->m_str_surf;
		while (++pt < qt) s += '_', s += pt->m_str_surf;
		while (++pw < qw) {
			pt = pw->m_p_token; qt = pt + pw->m_n_token;
			do { s += '_', s += pt->m_str_surf; } while (++pt < qt);
		}
		pt = pw->m_p_token; qt = pt + pT->end_lemma_index;
		do { s += '_', s += pt->m_str_surf; } while (++pt <= qt);
	DONE: pT->str_surf = m_Mem.Dup(s.c_str());
	} while (++pT < upT);
}
//============================================================================
void CSys::MakeResult(const KPAT_result_general *p_result_g, int n_result_g) {
	const KPAT_result_general *pr = p_result_g, *upr = pr + n_result_g;
	const KPAT_result_record *prr, *uprr;
	const KPAT_result_field *pf;
	SKene *p; int i; char *s;
	do {
		if (pr->n_record == 1 && pr->p_record->n_field == 1) {
			s = pr->p_record->p_field->str;
			if (*s == 'X' && s[1] == '\0') continue;
		}
			
		prr = pr->p_record, uprr = prr + pr->n_record; do {
			p = m_Pool.New();
			p->n = prr->n_field;
			p->args = (char**)m_Mem.New(p->n * sizeof(char*));
			pf = prr->p_field; i = 0; do {
				p->args[i] = m_Mem.Dup(pf[i].str);
				MkRStr(p->args[i]);
			} while (++i < p->n);
		} while (++prr < uprr);
	} while (++pr < upr);
}
//============================================================================
void CSys::MkRStr(string &sb, string &sf, const char *s) {
	const char *p; sb.clear(); sf.clear();
NEXT:
	if (!*s) return;
	if (*s == ' ') { ++s; goto NEXT; }
	if (*s == '|' && s[1] == '|') { p = s; goto SURF; }
	sb += '_';
	for (p = s + 1; *p && *p != ' '; ++p)
		if (*p == '|' && p[1] == '|') break;
	if (!*p) { sb += s; return; }
	sb.append(s, (int)(p - s));
	if (*p == ' ') { s = ++p; goto NEXT; }
SURF:
	s = p + 2;
	if (!*s) return;
	if (*s == ' ') { ++s; goto NEXT; }
	sf += '_';
	for (p = s + 1; *p && *p != ' '; ++p);
	if (!*p) { sf += s; return; }
	sf.append(s, (int)(p - s));
	s = ++p; goto NEXT;
}
//============================================================================
bool CSys::IsNegated(const KPAT_result_field *pf) const {
	const KEMA_word *pW = m_pSent->m_p_word, *pw, *qw;
	const KEMA_token *pt, *qt;
	const KPAT_token *pT = m_Tokens + pf->token_index, *upT = pT + pf->n_token;
	for (; pT < upT; ++pT) {
		pw = pW + pT->start_word_index, qw = pW + pT->end_word_index;
		pt = pw->m_p_token + pT->start_lemma_index;
		if (pw == qw) {
			qt = pw->m_p_token + pT->end_lemma_index; do {
				if (IsNegForm(pt->m_p_lemma->m_str)) return true;
			} while (++pt <= qt);
			continue;
		}
		qt = pw->m_p_token + pw->m_n_token; do {
			if (IsNegForm(pt->m_p_lemma->m_str)) return true;
		} while (++pt < qt);
		while (++pw < qw) {
			pt = pw->m_p_token; qt = pt + pw->m_n_token; do {
				if (IsNegForm(pt->m_p_lemma->m_str)) return true;
			} while (++pt < qt);
		}
		pt = pw->m_p_token; qt = pt + pT->end_lemma_index; do {
			if (IsNegForm(pt->m_p_lemma->m_str)) return true;
		} while (++pt <= qt);
	} while (++pT < upT);
	return false;
}
//============================================================================
void CSys::OutKema(ostream &os, const KEMA_sent *p) {
	const KEMA_word *pw = p->m_p_word;
	const KEMA_token *pt, *upt;
	for (int i = 0; i < p->m_n_word; i++) {
		os << " [" << i << "] " << pw[i].m_str;
		pt = pw[i].m_p_token, upt = pt + pw[i].m_n_token; do {
			os << ' ' << TagName(pt->m_p_lemma->m_tag) << ':' <<
				pt->m_p_lemma->m_str << "||" << pt->m_str_surf;
		} while (++pt < upt);
		os << '\n';
	}
}

void CSys::OutToken(ostream &os, const KPAT_token *p, int n) {
	for (int i = 0; i < n; i++)
		os << " [" << i << "] " << TagName(p[i].tag) << ':' <<
		p[i].str << "||" << p[i].str_surf << '\n';
}

void CSys::OutSpecial(ostream &os, const KPAT_result_special *p, int n) const {
	const KPAT_token *pt, *upt;
	for (const KPAT_result_special *up = p + n; p < up; ++p) {
		pt = m_Tokens + p->token_index, upt = pt + p->n_token; do {
			os << ' ' << TagName(pt->tag) << ':' << pt->str;
		} while (++pt < upt);
		os << " -> " << TagName(p->tag) << ':' << p->str << '\n';
		if (p->p_profile)
			os << '\t' << p->p_profile->user_pattern_id << ' ' <<
			p->p_profile->pattern_form << '\n';
	}
}

void CSys::OutGeneral(ostream &os, const KPAT_result_general *p, int n) {
	const KPAT_result_record *pr, *upr;
	const KPAT_result_field *pf, *upf;
	for (const KPAT_result_general *up = p + n; p < up; ++p) {
		pr = p->p_record, upr = pr + p->n_record; do {
			os << ' ';
			pf = pr->p_field, upf = pf + pr->n_field;
			do { os << '<' << pf->str << '>'; } while (++pf < upf);
		} while (++pr < upr);
		os << '\n';
		if (p->p_profile)
			os << '\t' << p->p_profile->user_pattern_id << ' ' <<
			p->p_profile->pattern_form << '\n';
	}
}

}
