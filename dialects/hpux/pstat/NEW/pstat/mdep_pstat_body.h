/* mdep_pstat_body.h : pst_stable body declarations and user manifest constants */

/*   @(#)B11.23_LR   */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	psd_device, psd_offset
 *	PS_OSID_NONE, PS_OSID_HPUX, PS_OSID_MPEXL
 *	PS_EARLY_DUMP
 *	PS_FASTSIZE_256KB, PS_FASTSIZE_512KB, PS_FASTSIZE_1MB, PS_FASTSIZE_2MB
 *	PS_FASTSIZE_4MB, PS_FASTSIZE_8MB, PS_FASTSIZE_16MB, PS_FASTSIZE_32MB
 *	PS_FASTSIZE_64MB, PS_FASTSIZE_128MB, PS_FASTSIZE_256MB
 *	PS_FASTSIZE_512MB, PS_FASTSIZE_1GB, PS_FASTSIZE_2GB, PS_FASTSIZE_ALL
 *	pss_buffer, pss_primary, pss_osdep, pss_dump, pss_fastsize
 * 	pss_console, pss_alternate, pss_keyboard
 *	PS_PA_RISC
 *	PST_MAX_PA_RISC_STABLE
 *
 * Size-invariant types
 *  Defines
 *	pst_ss_path
 *	pst_osdep
 *	pst_dump
 *	pst_stable_PA_RISC
 *
 * Size-variant types
 *  Defines:
 *	__pst_stable
 *
 *  Uses:
 *	_T_LONG_T
 *
 */


/* pst_stable support */



#if !defined(_PST_STABLE_INVARIANT_STRUCTS)
#define _PST_STABLE_INVARIANT_STRUCTS
/*
 * This structure describes a hardware path in a stable store.
 */
struct pst_ss_path {
        unsigned char   psp_autoboot:1;	/* Auto-Boot flag (primary path only) */
	unsigned char   psp_autosearch:1; /* Auto-Search flag (primary only) */
	unsigned char   psp_pad:2;	/* Reserved */
	unsigned char   psp_timer:4;	/* Boot timer (primary & console only)
					 * (log(base2) of # of seconds)  */
	unsigned char   psp_busconv[6];	/* Bus converters leading to boot mod */
	unsigned char   psp_module;	/* Boot module */
	uint32_t        psp_layer[6];	/* Layers past boot module */
};

/*
 * This structure describes the OS-dependent portion of the PA-RISC stable
 * store.
 */
struct pst_osdep {
        unsigned short	pso_osid;	/* Operating system ID */
	unsigned short  pso_pad1;	/* Reserved */
	uint32_t	pso_pad2[5];	/* Reserved */
};

/*
 * This structure describes the HP-UX dump information stored in the OS-
 * dependent portion of the PA-RISC stable store.
 */
struct pst_dump {
        unsigned short	psd_osid;	/* Operating system ID, must be HPUX */
	unsigned short	psd_flags;	/* Flags half-word, see below. */
	uint32_t	psd_pad;	/* Reserved */
	uint32_t	psd_device_high;
	uint32_t	psd_device_low;	/* Device containing dump */
	uint32_t	psd_offset_high;
	uint32_t	psd_offset_low;	/* Offset of dump on device */
};


/*
 * This structure describes the information in a PA-RISC system's stable
 * storage area.
 */
struct pst_stable_PA_RISC {
        struct pst_ss_path Pss_primary;	/* Primary boot path */
	unsigned char   Pss_pad1[32];	/* Reserved */
	union {
	  struct pst_osdep Pss_osdep;	/* OS-Dependent information */
	  struct pst_dump  Pss_dump;	/* HP-UX dump header information */
	}               Pss_osdep;
	unsigned char   Pss_pad2[7];	/* Reserved */
	unsigned char   Pss_pad3:4;	/* Reserved */
	unsigned char   Pss_fastsize:4;	/* Fast size value (see below) */
	struct pst_ss_path Pss_console;	/* Console/display path */
	struct pst_ss_path Pss_alternate; /* Alternate boot path */
	struct pst_ss_path Pss_keyboard;  /* Keyboard path */
	unsigned char   Pss_pad4[64];	/* Reserved */
};	

#endif /* !defined(_PST_STABLE_INVARIANT_STRUCTS) */

#if !defined(_PST_STABLE_PARISC_CONSTANTS_DEFINED)
#define _PST_STABLE_PARISC_CONSTANTS_DEFINED

#define psd_device psd_device_low
#define psd_offset psd_offset_low

/*
 * Values for pso_osid and psd_osid.
 */
#define PS_OSID_NONE	0x0		/* No OS-dependent data */
#define PS_OSID_HPUX	0x1		/* HP-UX dependent data */
#define PS_OSID_MPEXL	0x2		/* MPE-XL dependent data */

/*
 * Values for psd_flags.
 */
#define PS_EARLY_DUMP	0x1		/* Last dump was an early dump;
					 * psd_device and _offset are invalid */

#define PS_FASTSIZE_256KB	0x0
#define PS_FASTSIZE_512KB	0x1
#define PS_FASTSIZE_1MB		0x2
#define PS_FASTSIZE_2MB		0x3
#define PS_FASTSIZE_4MB		0x4
#define PS_FASTSIZE_8MB		0x5
#define PS_FASTSIZE_16MB	0x6
#define PS_FASTSIZE_32MB	0x7
#define PS_FASTSIZE_64MB	0x8
#define PS_FASTSIZE_128MB	0x9
#define PS_FASTSIZE_256MB	0xA
#define PS_FASTSIZE_512MB	0xB
#define PS_FASTSIZE_1GB		0xC
#define PS_FASTSIZE_2GB		0xD
#define PS_FASTSIZE_ALL		0xE	/* or 0xF */



#define pss_buffer	pss_un.Pss_PA_RISC_buffer
#define pss_primary	pss_un.Pss_PA_RISC_stable.Pss_primary
#define pss_osdep	pss_un.Pss_PA_RISC_stable.Pss_osdep.Pss_osdep
#define pss_dump	pss_un.Pss_PA_RISC_stable.Pss_osdep.Pss_dump
#define pss_fastsize	pss_un.Pss_PA_RISC_stable.Pss_fastsize
#define pss_console	pss_un.Pss_PA_RISC_stable.Pss_console
#define pss_alternate	pss_un.Pss_PA_RISC_stable.Pss_alternate
#define pss_keyboard	pss_un.Pss_PA_RISC_stable.Pss_keyboard

/*
 * Values for pss_type
 */
#define PS_PA_RISC	0x1	/* PA-RISC stable store */


/*
 * Max bytes in a PA-RISC stable store area.
 */
#define PST_MAX_PA_RISC_STABLE 256

#endif /* !defined(_PST_STABLE_PARISC_CONSTANTS_DEFINED) */

/*
 * This structure describes the information in any system's stable storage
 * area.
 */

struct __pst_stable {
        _T_LONG_T	pss_size;	/* Number of bytes of stable store */
	_T_LONG_T	pss_type;	/* Type of stable store, see below */
	_T_LONG_T	pss_pad[8];	/* Reserved */
        union {
	  unsigned char Pss_PA_RISC_buffer[PST_MAX_PA_RISC_STABLE];
				        /* Uninterpreted stable store data */
	  struct pst_stable_PA_RISC Pss_PA_RISC_stable;
				        /* PA-RISC stable store data */
	}		pss_un;
};
