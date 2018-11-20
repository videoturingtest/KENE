#ifndef KEMA_API_H
#define KEMA_API_H

#ifdef KEMA_MAKE_DLL
	#ifdef  __cplusplus
		#define KEMA_API extern "C" __declspec(dllexport)
	#else
		#define KEMA_API __declspec(dllexport)
	#endif
#else
	#ifdef  __cplusplus
		#define KEMA_API extern "C"
	#else
		#define KEMA_API
	#endif
#endif

// KEMA data #########################################################

enum {	// alalysis type values
	KEMA_AT_DIC_SYS		= 'D',	// system dictionary
	KEMA_AT_DIC_USR		= 'd',	// user dictionary
	KEMA_AT_GUESS_SYS	= 'G',	// system guess rule
	KEMA_AT_GUESS_USR	= 'g',	// user guess rule
	KEMA_AT_DEFAULT_TAG	= 'T',	// default tag
};

typedef struct {
	char		*m_str;
	int			 m_tag;
	void		*m_aux;
} KEMA_lemma;

typedef struct {
	char		*m_str;
	char		*m_str_surf; // surface string
	KEMA_lemma	*m_p_lemma;
	int			 m_n_lemma;
	int			 m_off_surf; // surface string offset
	// m_level, m_type, m_id: set only when m_perform_profile is true
	char		 m_level;	 // analysis level, 0 if not profiled
							 // -- 1(best, in dictionary) ~ 9(default tag)
	char		 m_type;	 // alalysis type
	int			 m_id;		 // id for dictionary entry or guess rule
							 // invalid if m_source is KEMA_AT_DEFAULT_TAG
	void		*m_aux;
} KEMA_token;

typedef struct {
	char		*m_str;
	KEMA_token	*m_p_token;
	int			 m_n_token;
	int			 m_off;
	void		*m_aux;
} KEMA_word;

typedef struct {
	char*		 m_str;
	KEMA_word	*m_p_word;
	int			 m_n_word;
	void		*m_aux;
} KEMA_sent;

struct KEMA_option {
	char m_split_sent_by_newline;	// default = true(1)
	char m_perform_profile;			// default = false(0)
	char m_perform_trace;			// default = false(0)
};

// KEMA functions #########################################################

KEMA_API
int KEMA_load_data_sys(char msg[], void **p_hd_sys, char *path,
	char *data_file);
KEMA_API
int KEMA_load_data(char msg[], void **p_hd, void *hd_sys, char *default_tag,
	char *path, char *tag_definition_file, char *dictionary_file,
	char *sentence_rule_file, char *token_rule_file, char *guess_rule_file);
KEMA_API
int KEMA_unload_data(char msg[], void *hd);

// In the following, 'hd' must be the data handle acquired by 'KEMA_load_data'.

KEMA_API	// if p_option == NULL, use default option
int KEMA_create(char msg[], void **p_h, KEMA_option *p_option);
KEMA_API
int KEMA_destroy(char msg[], void *h);

KEMA_API	// if len <= 0, use 'strlen(str)' as 'len'
int KEMA_split_sent(char msg[], void *h, void *hd,
	char *str, int len, int *p_done);
KEMA_API	// if len <= 0, use 'strlen(str)' as 'len'
int KEMA_analyze_doc(char msg[], void *h, void *hd,
	KEMA_sent *p_sent, char *str, int len, int *p_done);
KEMA_API	// if len <= 0, use 'strlen(str)' as 'len'
int KEMA_analyze_sent(char msg[], void *h, void *hd,
	KEMA_sent *p_sent, char *str, int len);

KEMA_API	// if p_option == NULL, use default option
int KEMA_set_option(char msg[], void *h, KEMA_option *p_option);
KEMA_API
int KEMA_get_option(char msg[], void *h, KEMA_option *p_option);
KEMA_API
KEMA_option KEMA_get_default_option();

KEMA_API
char* KEMA_get_version(char para_data_location[]);

KEMA_API	// return "??" if bad tag
int KEMA_get_tag_name(char msg[], void *hd, int tag, const char **p_tag_name);
KEMA_API	// return 0 if bad name
int KEMA_get_tag_id(char msg[], void *hd, const char *tag_name, int *p_tag);

KEMA_API	// return "??" if bad tag, don't check if valid
const char *KEMA_tag_name(void *hd, int tag);
KEMA_API	// return 0 if bad name, don't check if valid
int KEMA_tag_id(void *hd, const char *tag_name);

KEMA_API	// if file == NULL then use 'stdout'
int KEMA_show_user_data(char msg[], void *hd, const char *file);

#endif
