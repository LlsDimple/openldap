/*  ldap-int.h - defines & prototypes internal to the LDAP library */
/* $OpenLDAP$ */
/*
 * Copyright 1998-2002 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */
/*  Portions
 *  Copyright (c) 1995 Regents of the University of Michigan.
 *  All rights reserved.
 */

#ifndef	_LDAP_INT_H
#define	_LDAP_INT_H 1

#ifdef LDAP_R_COMPILE
#define LDAP_THREAD_SAFE 1
#endif

#include "../liblber/lber-int.h"

#ifdef LDAP_R_COMPILE
#include <ldap_pvt_thread.h>
#endif

#ifdef HAVE_CYRUS_SASL
	/* the need for this should be removed */
#ifdef HAVE_SASL_SASL_H
#include <sasl/sasl.h>
#else
#include <sasl.h>
#endif

#define SASL_MAX_BUFF_SIZE	65536
#define SASL_MIN_BUFF_SIZE	4096
#endif

/* 
 * Support needed if the library is running in the kernel
 */
#if LDAP_INT_IN_KERNEL
	/* 
	 * Platform specific function to return a pointer to the
	 * process-specific global options. 
	 *
	 * This function should perform the following functions:
	 *  Allocate and initialize a global options struct on a per process basis
	 *  Use callers process identifier to return its global options struct
	 *  Note: Deallocate structure when the process exits
	 */
#	define LDAP_INT_GLOBAL_OPT() ldap_int_global_opt()
   struct ldapoptions *ldap_int_global_opt(void);
#else
#	define LDAP_INT_GLOBAL_OPT() (&ldap_int_global_options)
#endif

#define ldap_debug	((LDAP_INT_GLOBAL_OPT())->ldo_debug)

#include "ldap_log.h"

#undef Debug
#define Debug( level, fmt, arg1, arg2, arg3 ) \
	do { if ( ldap_debug & level ) \
	ldap_log_printf( NULL, (level), (fmt), (arg1), (arg2), (arg3) ); \
	} while ( 0 )

#define LDAP_Debug( subsystem, level, fmt, arg1, arg2, arg3 )\
	ldap_log_printf( NULL, (level), (fmt), (arg1), (arg2), (arg3) )

#include "ldap.h"

#include "ldap_pvt.h"

LDAP_BEGIN_DECL

#define LDAP_URL_PREFIX         "ldap://"
#define LDAP_URL_PREFIX_LEN     (sizeof(LDAP_URL_PREFIX)-1)
#define LDAPS_URL_PREFIX		"ldaps://"
#define LDAPS_URL_PREFIX_LEN	(sizeof(LDAPS_URL_PREFIX)-1)
#define LDAPI_URL_PREFIX	"ldapi://"
#define LDAPI_URL_PREFIX_LEN	(sizeof(LDAPI_URL_PREFIX)-1)
#ifdef LDAP_CONNECTIONLESS
#define LDAPC_URL_PREFIX	"cldap://"
#define LDAPC_URL_PREFIX_LEN	(sizeof(LDAPC_URL_PREFIX)-1)
#endif
#define LDAP_URL_URLCOLON		"URL:"
#define LDAP_URL_URLCOLON_LEN	(sizeof(LDAP_URL_URLCOLON)-1)

#define LDAP_REF_STR		"Referral:\n"
#define LDAP_REF_STR_LEN	(sizeof(LDAP_REF_STR)-1)
#define LDAP_LDAP_REF_STR	LDAP_URL_PREFIX
#define LDAP_LDAP_REF_STR_LEN	LDAP_URL_PREFIX_LEN

#define LDAP_DEFAULT_REFHOPLIMIT 5

#define LDAP_BOOL_REFERRALS		0
#define LDAP_BOOL_RESTART		1
#define LDAP_BOOL_TLS			3

#define LDAP_BOOLEANS	unsigned long
#define LDAP_BOOL(n)	(1 << (n))
#define LDAP_BOOL_GET(lo, bool)	\
	((lo)->ldo_booleans & LDAP_BOOL(bool) ? -1 : 0)
#define LDAP_BOOL_SET(lo, bool) ((lo)->ldo_booleans |= LDAP_BOOL(bool))
#define LDAP_BOOL_CLR(lo, bool) ((lo)->ldo_booleans &= ~LDAP_BOOL(bool))
#define LDAP_BOOL_ZERO(lo) ((lo)->ldo_booleans = 0)

/*
 * This structure represents both ldap messages and ldap responses.
 * These are really the same, except in the case of search responses,
 * where a response has multiple messages.
 */

struct ldapmsg {
	ber_int_t		lm_msgid;	/* the message id */
	ber_tag_t		lm_msgtype;	/* the message type */
	BerElement	*lm_ber;	/* the ber encoded message contents */
	struct ldapmsg	*lm_chain;	/* for search - next msg in the resp */
	struct ldapmsg	*lm_next;	/* next response */
	time_t	lm_time;	/* used to maintain cache */
};

/*
 * structure representing get/set'able options
 * which have global defaults.
 */
struct ldapoptions {
	short ldo_valid;
#define LDAP_UNINITIALIZED	0x0
#define LDAP_INITIALIZED	0x1
#define LDAP_VALID_SESSION	0x2
	int   ldo_debug;
#ifdef LDAP_CONNECTIONLESS
#define	LDAP_IS_UDP(ld)		((ld)->ld_options.ldo_is_udp)
	void*			ldo_peer;	/* struct sockaddr* */
	char*			ldo_cldapdn;
	int			ldo_is_udp;
#endif

	/* per API call timeout */
	struct timeval		*ldo_tm_api;
	struct timeval		*ldo_tm_net;

	ber_int_t		ldo_version;
	ber_int_t		ldo_deref;
	ber_int_t		ldo_timelimit;
	ber_int_t		ldo_sizelimit;

#ifdef HAVE_TLS
   	int			ldo_tls_mode;
#endif

	LDAPURLDesc *ldo_defludp;
	int		ldo_defport;
	char*	ldo_defbase;
	char*	ldo_defbinddn;	/* bind dn */

#ifdef HAVE_CYRUS_SASL
	char*	ldo_def_sasl_mech;		/* SASL Mechanism(s) */
	char*	ldo_def_sasl_realm;		/* SASL realm */
	char*	ldo_def_sasl_authcid;	/* SASL authentication identity */
	char*	ldo_def_sasl_authzid;	/* SASL authorization identity */

	/* SASL Security Properties */
	struct sasl_security_properties	ldo_sasl_secprops;
#endif

	int		ldo_refhoplimit;	/* limit on referral nesting */

	/* LDAPv3 server and client controls */
	LDAPControl	**ldo_sctrls;
	LDAPControl **ldo_cctrls;

	/* LDAP rebind callback function */
	LDAP_REBIND_PROC *ldo_rebind_proc;
	void *ldo_rebind_params;

	LDAP_BOOLEANS ldo_booleans;	/* boolean options */
};


/*
 * structure for representing an LDAP server connection
 */
typedef struct ldap_conn {
	Sockbuf		*lconn_sb;
#ifdef HAVE_TLS
   	/* tls context */
   	void		*lconn_tls_ctx;
#endif
#ifdef HAVE_CYRUS_SASL
	void		*lconn_sasl_ctx;
#endif
	int			lconn_refcnt;
	time_t		lconn_lastused;	/* time */
	int			lconn_rebind_inprogress;	/* set if rebind in progress */
	char		***lconn_rebind_queue;		/* used if rebind in progress */
	int			lconn_status;
#define LDAP_CONNST_NEEDSOCKET		1
#define LDAP_CONNST_CONNECTING		2
#define LDAP_CONNST_CONNECTED		3
	LDAPURLDesc		*lconn_server;
#ifdef LDAP_API_FEATURE_X_OPENLDAP_V2_KBIND
	char			*lconn_krbinstance;
#endif
	BerElement		*lconn_ber;	/* ber receiving on this conn. */

	struct ldap_conn *lconn_next;
} LDAPConn;


/*
 * structure used to track outstanding requests
 */
typedef struct ldapreq {
	ber_int_t		lr_msgid;	/* the message id */
	int		lr_status;	/* status of request */
#define LDAP_REQST_COMPLETED	0
#define LDAP_REQST_INPROGRESS	1
#define LDAP_REQST_CHASINGREFS	2
#define LDAP_REQST_NOTCONNECTED	3
#define LDAP_REQST_WRITING	4
	int		lr_outrefcnt;	/* count of outstanding referrals */
	ber_int_t		lr_origid;	/* original request's message id */
	int		lr_parentcnt;	/* count of parent requests */
	ber_tag_t		lr_res_msgtype;	/* result message type */
	ber_int_t		lr_res_errno;	/* result LDAP errno */
	char		*lr_res_error;	/* result error string */
	char		*lr_res_matched;/* result matched DN string */
	BerElement	*lr_ber;	/* ber encoded request contents */
	LDAPConn	*lr_conn;	/* connection used to send request */
	struct ldapreq	*lr_parent;	/* request that spawned this referral */
	struct ldapreq	*lr_child;	/* first child request */
	struct ldapreq	*lr_refnext;	/* next referral spawned */
	struct ldapreq	*lr_prev;	/* previous request */
	struct ldapreq	*lr_next;	/* next request */
} LDAPRequest;

/*
 * structure for client cache
 */
#define LDAP_CACHE_BUCKETS	31	/* cache hash table size */
typedef struct ldapcache {
	LDAPMessage	*lc_buckets[LDAP_CACHE_BUCKETS];/* hash table */
	LDAPMessage	*lc_requests;			/* unfulfilled reqs */
	long		lc_timeout;			/* request timeout */
	ber_len_t		lc_maxmem;			/* memory to use */
	ber_len_t		lc_memused;			/* memory in use */
	int		lc_enabled;			/* enabled? */
	unsigned long	lc_options;			/* options */
#define LDAP_CACHE_OPT_CACHENOERRS	0x00000001
#define LDAP_CACHE_OPT_CACHEALLERRS	0x00000002
}  LDAPCache;

/*
 * structure containing referral request info for rebind procedure
 */
typedef struct ldapreqinfo {
	ber_len_t	ri_msgid;
	int			ri_request;
	char 		*ri_url;
} LDAPreqinfo;

/*
 * structure representing an ldap connection
 */

struct ldap {
	Sockbuf		*ld_sb;		/* socket descriptor & buffer */

	struct ldapoptions ld_options;

#define ld_valid		ld_options.ldo_valid
#define ld_debug		ld_options.ldo_debug

#define ld_deref		ld_options.ldo_deref
#define ld_timelimit	ld_options.ldo_timelimit
#define ld_sizelimit	ld_options.ldo_sizelimit

#define ld_defbinddn	ld_options.ldo_defbinddn
#define ld_defbase		ld_options.ldo_defbase
#define ld_defhost		ld_options.ldo_defhost
#define ld_defport		ld_options.ldo_defport

#define ld_refhoplimit	ld_options.ldo_refhoplimit

#define ld_sctrls		ld_options.ldo_sctrls
#define ld_cctrls		ld_options.ldo_cctrls
#define ld_rebind_proc	ld_options.ldo_rebind_proc
#define ld_rebind_params	ld_options.ldo_rebind_params

#define ld_version		ld_options.ldo_version

	unsigned short	ld_lberoptions;

	ber_int_t	ld_errno;
	char	*ld_error;
	char	*ld_matched;
	char	**ld_referrals;
	ber_len_t		ld_msgid;

	/* do not mess with these */
	LDAPRequest	*ld_requests;	/* list of outstanding requests */
	LDAPMessage	*ld_responses;	/* list of outstanding responses */

	ber_int_t		*ld_abandoned;	/* array of abandoned requests */

	LDAPCache	*ld_cache;	/* non-null if cache is initialized */

	/* do not mess with the rest though */

	LDAPConn	*ld_defconn;	/* default connection */
	LDAPConn	*ld_conns;	/* list of server connections */
	void		*ld_selectinfo;	/* platform specifics for select */
#ifdef HAVE_TLS
	void		*ld_ssl;	/* SSL session for next connection */
#endif
};
#define LDAP_VALID(ld)	( (ld)->ld_valid == LDAP_VALID_SESSION )

#ifdef LDAP_R_COMPILE
#ifdef HAVE_RES_QUERY
LDAP_V ( ldap_pvt_thread_mutex_t ) ldap_int_resolv_mutex;
#endif

#ifdef HAVE_CYRUS_SASL
LDAP_V( ldap_pvt_thread_mutex_t ) ldap_int_sasl_mutex;
#endif
#endif

/*
 * in init.c
 */

LDAP_V ( struct ldapoptions ) ldap_int_global_options;

LDAP_F ( void ) ldap_int_initialize LDAP_P((struct ldapoptions *, int *));
LDAP_F ( void ) ldap_int_initialize_global_options LDAP_P((
	struct ldapoptions *, int *));

/* memory.c */
	/* simple macros to realloc for now */
#define LDAP_MALLOC(s)		(LBER_MALLOC((s)))
#define LDAP_CALLOC(n,s)	(LBER_CALLOC((n),(s)))
#define LDAP_REALLOC(p,s)	(LBER_REALLOC((p),(s)))
#define LDAP_FREE(p)		(LBER_FREE((p)))
#define LDAP_VFREE(v)		(LBER_VFREE((void **)(v)))
#define LDAP_STRDUP(s)		(LBER_STRDUP((s)))
#define LDAP_STRNDUP(s,l)	(LBER_STRNDUP((s),(l)))

/*
 * in error.c
 */
LDAP_F (void) ldap_int_error_init( void );

/*
 * in unit-int.c
 */
LDAP_F (void) ldap_int_utils_init LDAP_P(( void ));


/*
 * in print.c
 */
LDAP_F (int) ldap_log_printf LDAP_P((LDAP *ld, int level, const char *fmt, ...)) LDAP_GCCATTR((format(printf, 3, 4)));

/*
 * in cache.c
 */
LDAP_F (void) ldap_add_request_to_cache LDAP_P(( LDAP *ld, ber_tag_t msgtype,
        BerElement *request ));
LDAP_F (void) ldap_add_result_to_cache LDAP_P(( LDAP *ld, LDAPMessage *result ));
LDAP_F (int) ldap_check_cache LDAP_P(( LDAP *ld, ber_tag_t msgtype, BerElement *request ));

/*
 * in controls.c
 */
LDAP_F (int) ldap_int_get_controls LDAP_P((
	BerElement *be,
	LDAPControl ***ctrlsp));

LDAP_F (int) ldap_int_put_controls LDAP_P((
	LDAP *ld,
	LDAPControl *const *ctrls,
	BerElement *ber ));

LDAP_F (int) ldap_int_client_controls LDAP_P((
	LDAP *ld,
	LDAPControl **ctrlp ));

/*
 * in dsparse.c
 */
LDAP_F (int) ldap_int_next_line_tokens LDAP_P(( char **bufp, ber_len_t *blenp, char ***toksp ));

#ifdef LDAP_API_FEATURE_X_OPENLDAP_V2_KBIND
/*
 * in kerberos.c
 */
LDAP_F (char *) ldap_get_kerberosv4_credentials LDAP_P((
	LDAP *ld,
	LDAP_CONST char *who,
	LDAP_CONST char *service,
	ber_len_t *len ));

#endif /* LDAP_API_FEATURE_X_OPENLDAP_V2_KBIND */


/*
 * in open.c
 */
LDAP_F (int) ldap_open_defconn( LDAP *ld );
LDAP_F (int) ldap_int_open_connection( LDAP *ld,
	LDAPConn *conn, LDAPURLDesc *srvlist, int async );

/*
 * in os-ip.c
 */
LDAP_V (int) ldap_int_tblsize;
LDAP_F (int) ldap_int_timeval_dup( struct timeval **dest, const struct timeval *tm );
LDAP_F (int) ldap_connect_to_host( LDAP *ld, Sockbuf *sb,
	int proto, const char *host, unsigned long address, int port,
	int async );

#if defined(LDAP_API_FEATURE_X_OPENLDAP_V2_KBIND) || \
	defined(HAVE_TLS) || defined(HAVE_CYRUS_SASL)
LDAP_V (char *) ldap_int_hostname;
LDAP_F (char *) ldap_host_connected_to( Sockbuf *sb );
#endif

LDAP_F (void) ldap_int_ip_init( void );
LDAP_F (int) ldap_int_select( LDAP *ld, struct timeval *timeout );
LDAP_F (void *) ldap_new_select_info( void );
LDAP_F (void) ldap_free_select_info( void *sip );
LDAP_F (void) ldap_mark_select_write( LDAP *ld, Sockbuf *sb );
LDAP_F (void) ldap_mark_select_read( LDAP *ld, Sockbuf *sb );
LDAP_F (void) ldap_mark_select_clear( LDAP *ld, Sockbuf *sb );
LDAP_F (int) ldap_is_read_ready( LDAP *ld, Sockbuf *sb );
LDAP_F (int) ldap_is_write_ready( LDAP *ld, Sockbuf *sb );

/*
 * in os-local.c
 */
#ifdef LDAP_PF_LOCAL
LDAP_F (int) ldap_connect_to_path( LDAP *ld, Sockbuf *sb,
	const char *path, int async );
#endif /* LDAP_PF_LOCAL */

/*
 * in request.c
 */
LDAP_F (ber_int_t) ldap_send_initial_request( LDAP *ld, ber_tag_t msgtype,
	const char *dn, BerElement *ber );
LDAP_F (BerElement *) ldap_alloc_ber_with_options( LDAP *ld );
LDAP_F (void) ldap_set_ber_options( LDAP *ld, BerElement *ber );

LDAP_F (int) ldap_send_server_request( LDAP *ld, BerElement *ber, ber_int_t msgid, LDAPRequest *parentreq, LDAPURLDesc *srvlist, LDAPConn *lc, LDAPreqinfo *bind );
LDAP_F (LDAPConn *) ldap_new_connection( LDAP *ld, LDAPURLDesc *srvlist, int use_ldsb, int connect, LDAPreqinfo *bind );
LDAP_F (LDAPRequest *) ldap_find_request_by_msgid( LDAP *ld, ber_int_t msgid );
LDAP_F (void) ldap_free_request( LDAP *ld, LDAPRequest *lr );
LDAP_F (void) ldap_free_connection( LDAP *ld, LDAPConn *lc, int force, int unbind );
LDAP_F (void) ldap_dump_connection( LDAP *ld, LDAPConn *lconns, int all );
LDAP_F (void) ldap_dump_requests_and_responses( LDAP *ld );
LDAP_F (int) ldap_chase_referrals( LDAP *ld, LDAPRequest *lr,
	char **errstrp, int sref, int *hadrefp );
LDAP_F (int) ldap_chase_v3referrals( LDAP *ld, LDAPRequest *lr,
	char **refs, int sref, char **referralsp, int *hadrefp );
LDAP_F (int) ldap_append_referral( LDAP *ld, char **referralsp, char *s );
LDAP_F (int) ldap_int_flush_request( LDAP *ld, LDAPRequest *lr );

/*
 * in result.c:
 */
LDAP_F (char *) ldap_int_msgtype2str( ber_tag_t tag );

/*
 * in search.c
 */
LDAP_F (BerElement *) ldap_build_search_req LDAP_P((
	LDAP *ld,
	const char *base,
	ber_int_t scope,
	const char *filter,
	char **attrs,
	ber_int_t attrsonly,
	LDAPControl **sctrls,
	LDAPControl **cctrls,
	ber_int_t timelimit,
	ber_int_t sizelimit ));


/*
 * in unbind.c
 */
LDAP_F (int) ldap_ld_free LDAP_P((
	LDAP *ld,
	int close,
	LDAPControl **sctrls,
	LDAPControl **cctrls ));

LDAP_F (int) ldap_send_unbind LDAP_P((
	LDAP *ld,
	Sockbuf *sb,
	LDAPControl **sctrls,
	LDAPControl **cctrls ));

/*
 * in url.c
 */
LDAP_F (LDAPURLDesc *) ldap_url_dup LDAP_P((
	LDAPURLDesc *ludp ));

LDAP_F (LDAPURLDesc *) ldap_url_duplist LDAP_P((
	LDAPURLDesc *ludlist ));

LDAP_F (int) ldap_url_parselist LDAP_P((
	LDAPURLDesc **ludlist,
	const char *url ));

LDAP_F (int) ldap_url_parsehosts LDAP_P((
	LDAPURLDesc **ludlist,
	const char *hosts,
	int port ));

LDAP_F (char *) ldap_url_list2hosts LDAP_P((
	LDAPURLDesc *ludlist ));

LDAP_F (char *) ldap_url_list2urls LDAP_P((
	LDAPURLDesc *ludlist ));

LDAP_F (void) ldap_free_urllist LDAP_P((
	LDAPURLDesc *ludlist ));

/*
 * in cyrus.c
 */

LDAP_F (int) ldap_int_sasl_init LDAP_P(( void ));

LDAP_F (int) ldap_int_sasl_open LDAP_P((
	LDAP *ld, LDAPConn *conn,
	const char* host, ber_len_t ssf ));
LDAP_F (int) ldap_int_sasl_close LDAP_P(( LDAP *ld, LDAPConn *conn ));

LDAP_F (int) ldap_int_sasl_external LDAP_P((
	LDAP *ld, LDAPConn *conn,
	const char* authid, ber_len_t ssf ));

LDAP_F (int) ldap_int_sasl_get_option LDAP_P(( LDAP *ld,
	int option, void *arg ));
LDAP_F (int) ldap_int_sasl_set_option LDAP_P(( LDAP *ld,
	int option, void *arg ));
LDAP_F (int) ldap_int_sasl_config LDAP_P(( struct ldapoptions *lo,
	int option, const char *arg ));

LDAP_F (int) ldap_int_sasl_bind LDAP_P((
	LDAP *ld,
	const char *,
	const char *,
	LDAPControl **, LDAPControl **,

	/* should be passed in client controls */
	unsigned flags,
	LDAP_SASL_INTERACT_PROC *interact,
	void *defaults ));

/* in schema.c */
LDAP_F (char *) ldap_int_parse_numericoid LDAP_P((
	const char **sp,
	int *code,
	const int flags ));

/*
 * in tls.c
 */
LDAP_F (int) ldap_int_tls_config LDAP_P(( LDAP *ld,
	int option, const char *arg ));

LDAP_F (int) ldap_int_tls_start LDAP_P(( LDAP *ld,
	LDAPConn *conn, LDAPURLDesc *srv ));

/*
 *	in getvalues.c
 */
LDAP_F (char **) ldap_value_dup LDAP_P((
	char *const *vals ));

LDAP_END_DECL

#endif /* _LDAP_INT_H */
