#ifndef KEPT_API_H
#define KEPT_API_H
#include "kema-api.h"

#ifdef KEPT_MAKE_DLL
	#ifdef  __cplusplus
		#define KEPT_API extern "C" __declspec(dllexport)
	#else
		#define KEPT_API __declspec(dllexport)
	#endif
#else
	#ifdef  __cplusplus
		#define KEPT_API extern "C"
	#else
		#define KEPT_API
	#endif
#endif

typedef enum {	// run type
	KEPT_RT_BOTH,		// by learning-based -> by rule-based
	KEPT_RT_LEARNING,	// by learning-based only
	KEPT_RT_RULE,		// by rule-based only
} KEPT_run_type;

typedef struct {
	KEPT_run_type m_run_type;		// default = KEPT_RT_BOTH
	int m_trace_level;				// 0(default), 1, 2 -- for rule-based
} KEPT_option;

// KEPT functions #########################################################

KEPT_API
int KEPT_load_data_sys(char msg[], void **p_hd_sys, char *path,
	char *data_file);
KEPT_API
int KEPT_load_data(char msg[], void **p_hd, void *hd_sys, char *path,
	char *user_rule_file);
KEPT_API
int KEPT_unload_data(char msg[], void *hd);

// In the following, 'hd' must be the data handle acquired by 'KEPT_load_data'.

KEPT_API	// if p_option == NULL, use default option
int KEPT_create(char msg[], void **p_h, KEPT_option *p_option);
KEPT_API
int KEPT_destroy(char msg[], void *h);

KEPT_API
int KEPT_tag(char msg[], void *h, void *hd, KEMA_sent *p_sent);

KEPT_API	// if p_option == NULL, use default option
int KEPT_set_option(char msg[], void *h, KEPT_option *p_option);
KEPT_API
int KEPT_get_option(char msg[], void *h, KEPT_option *p_option);
KEPT_API
KEPT_option KEPT_get_default_option();

KEPT_API
char* KEPT_get_version(char para_data_location[]);

KEPT_API	// return "??" if bad tag
int KEPT_get_tag_name(char msg[], void *hd, int tag, const char **p_tag_name);
KEPT_API	// return 0 if bad name
int KEPT_get_tag_id(char msg[], void *hd, const char *tag_name, int *p_tag);

KEPT_API	// return "??" if bad tag, don't check if valid
const char *KEPT_tag_name(void *hd, int tag);
KEPT_API	// return 0 if bad name, don't check if valid
int KEPT_tag_id(void *hd, const char *tag_name);

KEPT_API	// if file == NULL then use 'stdout'
int KEPT_show_user_data(char msg[], void *hd, const char *file);

#endif
