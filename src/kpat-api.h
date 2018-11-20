#ifndef KPAT_API_H
#define KPAT_API_H

#ifdef KPAT_MAKE_DLL
	#ifdef  __cplusplus
		#define KPAT_API extern "C" __declspec(dllexport)
	#else
		#define KPAT_API __declspec(dllexport)
	#endif
#else
	#ifdef  __cplusplus
		#define KPAT_API extern "C"
	#else
		#define KPAT_API
	#endif
#endif

/*
 * pattern class: general & special
 * match kinds
   - general match: match using general patterns
     . overlap match
	 . exclusive match
   - special match: exclusive match using special patterns
 * match result and token list acquired via KPAT APIs:
	data are kept only until next call/cycle
 * strings acquired via KPAT APIs: must not be changed outside
 * Feature definitions in feature data and in pattern data must be equal
   if they are used in the same group and have same charset.
   - can be tested if same using KPAT_check_feature_definition()
 * input argument string
   - must not be NULL pointer if not allowed explicitly
   - must not be empty except for 'path'
*/

// KPAT types #################################################################

typedef enum {	// KPAT Lexical Analyzer and Tag Set
	KPAT_LT_NONE,				// invalid
	KPAT_LT_KONAN_K_INTERNAL,	// KMA internal tag set
	KPAT_LT_KONAN_K_EXTERNAL,	// KMA external tag set
	KPAT_LT_KONAN_C,
	KPAT_LT_BASIS_C,
	KPAT_LT_KONAN_E,
	KPAT_LT_COUNT				// # of LTs
} KPAT_LT;

typedef enum {	// KPAT Character Set
	KPAT_CS_NONE,				// invalid
	KPAT_CS_UTF8,
	KPAT_CS_EUC_KR,
	KPAT_CS_GB,
	KPAT_CS_BIG5,
	KPAT_CS_COUNT				// # of CSs
} KPAT_CS;

typedef enum {	// KPAT Character Case Conversion
	KPAT_CC_NONE,				// invalid
	KPAT_CC_ASIS,				// do nothing
	KPAT_CC_UPPER,				// lowercase -> uppercase
	KPAT_CC_LOWER,				// uppercase -> lowercase
	KPAT_CC_COUNT				// # of CCs
} KPAT_CC;

//////////////////////////////////////////////////////////////////////////////
// ** pattern match profile
typedef enum {	// KPAT pattern key type
	KPAT_KT_NONE,		// not used
	KPAT_KT_LEMMA,		// lemma/word form
	KPAT_KT_SURFACE,	// surface form
	KPAT_KT_TAG			// tag
} KPAT_KT;

typedef struct {	// match profile
	int		n_cond;					// # of condition elements
	int		pattern_id;				// pattern ID
	int		key_token_index;		// key token index
	KPAT_KT	key_type;				// pattern key type
	char	*key;					// pattern key
	char	*user_pattern_id;		// user pattern ID
	char	*user_info;				// user info. string, NULL if no user info.
	char	*pattern_form;			// pattern form
	char	*file;					// pattern file name
} KPAT_profile;

typedef KPAT_profile KPAT_pattern;

//////////////////////////////////////////////////////////////////////////////
// ** general pattern match result
typedef struct {	// general match field
	int token_index;				// start token index of field match range
	int n_token;					// # of tokens in field match range
	char *str;						// field form, may be null string
} KPAT_result_field;					//  if option allow_empty_field == true
	// in case of no slot values in the field:
	//	token_index -> -1, n_token -> 0
	// even if there are slot values in the field,
	// in case of # of tokens in field match range is 0: n_token -> 0,
	//	token_index may be "token count" if match position -> end of token list
typedef struct {	// general match element
	int	n_field;					// # of fields, > 0
	KPAT_result_field *p_field;		// field list
} KPAT_result_record;
typedef struct {	// general match result
	int token_index;				// start token index of pattern match range
	int n_token;					// # of tokens in pattern match range, > 0
	int n_record;					// # of records, > 0
	int n_cond;						// # of pattern conditions
	KPAT_result_record *p_record;	// record list
	char *user_info;				// user info. string, NULL if no user info.
	KPAT_profile *p_profile;		// profile pointer, NULL if not profiled
} KPAT_result_general;

//////////////////////////////////////////////////////////////////////////////
// ** special pattern match result
typedef struct {	// special match result
	int token_index;				// start token index of pattern match range
	int n_token;					// # of tokens in pattern match range, > 0
	int n_cond;						// # of pattern conditions
	unsigned char tag;				// tag
	char *str;						// normalized lemma form
	char *str_org;					// original lemma form
	unsigned char *features;		// feature id list
		// NULL if no feature, ends with null char.
	char *user_info;				// user info. string, NULL if no user info.
	KPAT_profile *p_profile;		// profile pointer, NULL if not profiled
} KPAT_result_special;

//////////////////////////////////////////////////////////////////////////////
// ** KPAT token type
typedef struct {	// KPAT token
	int start_word_index;			// start word index
	int start_lemma_index;			// start lemma index
	int end_word_index;				// end word index
	int end_lemma_index;			// end lemma index
	int offset;						// start byte offset
	int length;						// original surface form length
	int len_surf;					// length of 'str_surf' member
	char *str;						// normalized lemma form
	char *str_org;					// origianl lemma form
	char *str_surf;					// surface form
	unsigned char *features;		// feature id list
		// NULL if no feature, ends with null char.
	unsigned char tag;				// tag
	char is_first_lemma;			// true if first lemma in a word
	char is_last_lemma;				// true if last lemma in a word
	char is_used;					// true if already used in match
		// only for exclusive/special match, set during pattern matching
	KPAT_result_special *p_result;	// pointer to special match result
		// whose match range starts from this token
		// only for special match, set during pattern matching
	void *p_user_data;				// reserved for user data
} KPAT_token;

//////////////////////////////////////////////////////////////////////////////
// ** user tokener(token setter) function pointer type
struct KPAT_tokener_option;
typedef int (*KPAT_PF_TOKENER)(
	KPAT_token *p_token,
	const void *p_lemma, int word_index, int lemma_index, int n_lemma,
		// current lemma structure, word index, lemma index, lemma count
	const void *hd_kpat, const struct KPAT_tokener_option *p_option, KPAT_CS cs,
		// data handle, tokener option, charset
	void *aux);
		// aux arg(passed through KPAT_set_token())
	// returns
	//	-: drop(do not make) token
	//	+: made token (by me)
	//		-> must set all KPAT_token members
	//			except for is_used, p_result, is_first_lemma, is_last_lemma
	//	0: do nothing except for setting p_user_data
	//		-> make or drop token according to the data given
//////////////////////////////////////////////////////////////////////////////
// ** user slotter(slot filler) function pointer type
typedef void (*KPAT_PF_SLOTTER)(void *p_string, const KPAT_token *p_token,
								char use_original_form);
	// p_string: pointer to C++ string object
	// p_token: pointer to corresponding token
	// use_original_form: 'use_original_form' in KPAT_matcher_option
	// fill the slot for the token by appending to *p_string
	//  during building the 'str' in KPAT_result_field or KPAT_result_special
//////////////////////////////////////////////////////////////////////////////
// ** KPAT tokener option
typedef struct KPAT_tokener_option {
	char apply_stop_data;		// default: true
	char apply_norm_data;		// default: true
	char character_case;		// default: KPAT_CC_ASIS, values -> KPAT_CC
	char full2half;				// fullwidth ascii -> halfwidth
								// default: 0(false, no conversion)
	char apply_user_func;		// default: true
	char reserved1, reserved2, reserved3;
	KPAT_PF_TOKENER user_func;	// default: NULL(do nothing)
} KPAT_tokener_option;

// ** KPAT pattern matcher option
typedef struct {
	KPAT_PF_SLOTTER user_slotter;// default: NULL(fill slot as lemma form)
		// not used if special match
	char apply_overlap_match;	// default: false, not used if special match
	char allow_empty_field;		// default: false, not used if special match
	char use_original_form;		// default: false
		// use original forms when building match results
		// default: use normalized forms
	char perform_profile;		// default: false
	char sort_result;			// default: false
		// sort match results according to start match locations
	char default_skip_distance;	// [0, 9], default: 3
	char spacing_char_token;	// default: 0(not spaced)
		// spacing char bet'n tokens in a slot for match result
	char spacing_char_slot;		// default: 0(not spaced)
		// spacing char bet'n slots in all slot for match result
} KPAT_matcher_option;

// KPAT API functions #########################################################
KPAT_API
int KPAT_load_frequency_data(char msg[], void **p_hd_frequency,
	char *path, char *frequency_data_file, unsigned long long offset);

KPAT_API
int KPAT_load_token_data_sys(char msg[], void **p_hd_token_sys,
	char *path, char *token_data_file, unsigned long long offset);

KPAT_API
int KPAT_load_token_data(char msg[], void **p_hd_token,
	KPAT_LT lt, KPAT_CS cs, void *hd_token_sys, char *path,
	char *user_tag_definition_file, char *user_feature_definition_file,
	char *user_stop_data_file, char *user_norm_data_file,
	char *user_feature_data_file);
// hd_token_sys or user files may be null

KPAT_API
int KPAT_load_match_data_sys(char msg[], void **p_hd_match_sys,
	char *path, char *match_data_file, unsigned long long offset);

KPAT_API
int KPAT_load_kpat_data(char msg[], void **p_hd_kpat,
	KPAT_LT lt, KPAT_CS cs, bool is_special,
	void *hd_match_sys, void *hd_frequency, void *hd_token, char *path,
	char *user_pattern_file, char *user_macro_file, char *user_rexp_file);
// hd_match_sys or user files may be null
// if user_pattern_file is null, hd_frequency may be null

KPAT_API
int KPAT_unload_data(char msg[], void *hd);

//////////////////////////////////////////////////////////////////////////////
// ** engine create & destroy
KPAT_API	// p_option: may be NULL -> use default option
int KPAT_create_tokener(char msg[], void **p_h_tokener,
	KPAT_tokener_option *p_option);

KPAT_API	// p_option: may be NULL -> use default option
int KPAT_create_matcher(char msg[], void **p_h_matcher,
	KPAT_matcher_option *p_option);

KPAT_API
int KPAT_destroy(char msg[], void *h);

//////////////////////////////////////////////////////////////////////////////
// ** main processing & option
KPAT_API
int KPAT_set_token_list(char msg[], void *h_tokener, void *hd_kpat,
	const void *p_sent, KPAT_token **pp_token, int *p_n_token, void *aux);

KPAT_API
int KPAT_replace_token_list(int n_result, KPAT_token *p_token, int n_token);
	// must be called immediately after special match
	// -- after calling KPAT_match_special()
	// n_result: # of results(patterns matched)
	// return: # of tokens after replace
	// user may need to adjust/reset p_user_data

KPAT_API	// if p_option == NULL, use default values
int KPAT_set_tokener_option(char msg[], void *h_tokener,
	KPAT_tokener_option *p_option);
KPAT_API
KPAT_tokener_option KPAT_get_default_tokener_option();
//----------------------------------------------------------------------------
KPAT_API
int KPAT_match_general(char msg[], void *h_matcher,
	void *hd_kpat, KPAT_token *p_token, int n_token,
	KPAT_result_general **pp_result, int *p_n_result);
KPAT_API
int KPAT_match_special(char msg[], void *h_matcher,
	void *hd_kpat, KPAT_token *p_token, int n_token,
	KPAT_result_special **pp_result, int *p_n_result);

KPAT_API	// if p_option == NULL, use default values
int KPAT_set_matcher_option(char msg[], void *h_matcher,
	KPAT_matcher_option *p_option);
KPAT_API
KPAT_matcher_option KPAT_get_default_matcher_option();

KPAT_API	// return 0 if correct pattern
int KPAT_check_pattern(char msg[], void *hd_kpat, char *path,
	char *pattern_file);
//----------------------------------------------------------------------------
KPAT_API
int KPAT_get_candidate_general(char msg[], void *h_matcher,
void *hd_kpat, KPAT_token *p_token, int n_token,
KPAT_pattern **pp_pattern, int *p_n_pattern);
KPAT_API
int KPAT_get_candidate_special(char msg[], void *h_matcher,
void *hd_kpat, KPAT_token *p_token, int n_token,
KPAT_pattern **pp_pattern, int *p_n_pattern);

//////////////////////////////////////////////////////////////////////////////
// ** general utility
// don't check if handle is valid except for KPAT_check_definition()

KPAT_API
char* KPAT_get_version(char para_data_location[]);

KPAT_API	// return "??" if bad tag
const char *KPAT_get_tag_name(void *hd_kpat, int tag);
KPAT_API	// return 0 if bad name
int KPAT_get_tag_id(void *hd_kpat, const char *tag_name);

KPAT_API	// return "??" if bad feature
const char *KPAT_get_feature_name(void *hd_kpat, int feature);
KPAT_API	// return 0 if bad name
int KPAT_get_feature_id(void *hd_kpat, const char *feature_name);

KPAT_API
int KPAT_is_stop(void *hd_kpat, int tag, const char *str);

KPAT_API	// return norm form or NULL
const char *KPAT_get_norm(void *hd_kpat, int tag, const char *str);

KPAT_API	// return feature id list or NULL
const char *KPAT_get_features(void *hd_kpat, int tag, const char *str);

//////////////////////////////////////////////////////////////////////////////
// ** output utility -- use C++ ostream
#ifdef  __cplusplus
#ifdef  _HPUX_CLASSIC_CPP_LIB
#   include <iostream.h>
namespace std {}
#else
#   include <iostream>
using std::ostream;
#endif

// pointer to token user data output function
typedef int (*KPAT_PF_OUTPUT_USER_DATA)
	(ostream &os, const KPAT_token *p_token, void *p_user_data, void *aux);
	// aux: passed through KPAT_output_token()/KPAT_output_token_list()
KPAT_API	// pf_output: may be NULL, 
int KPAT_output_token(char msg[], void *hd_kpat,
	ostream &os, const KPAT_token *p_token, void *aux,
	KPAT_PF_OUTPUT_USER_DATA pf_output_user_data);
KPAT_API	// pf_output: may be NULL
int KPAT_output_token_list(char msg[], void *hd_kpat,
	ostream &os, const KPAT_token *p_token, int n_token, void *aux,
	KPAT_PF_OUTPUT_USER_DATA pf_output_user_data);

KPAT_API	// need not data handle
void KPAT_output_result_general(ostream &os,
	const KPAT_result_general *p_result);
KPAT_API	// need not data handle
void KPAT_output_result_general_list(ostream &os,
	const KPAT_result_general *p_result, int n_result);
KPAT_API	// need data handle for tags & features
int KPAT_output_result_special(char msg[], void *hd_kpat, ostream &os,
	const KPAT_result_special *p_result);
KPAT_API	// need data handle for tags & features
int KPAT_output_result_special_list(char msg[], void *hd_kpat, ostream &os,
	const KPAT_result_special *p_result, int n_result);

KPAT_API
int KPAT_output_lexical_result(char msg[], KPAT_LT lt, ostream &os,
	const void *p_sent);
KPAT_API
int KPAT_output_lexical_result_1_best(char msg[], KPAT_LT lt, ostream &os,
	const void *p_sent);

#endif // __cplusplus

#endif
