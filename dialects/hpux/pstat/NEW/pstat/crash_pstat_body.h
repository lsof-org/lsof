/* crash_pstat_body.h : pst_crash* body declarations and user constants */

/*  @(#)B11.23_LR   */

/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	PST_MAXCLASSES
 *	PS_EARLY_DUMP, PS_CONF_CHANGED, PS_HEADER_VALID
 *	PS_BOOTTIME, PS_RUNTIME
 *
 * Size-invariant types
 *  Defines:
 *	pst_crashinfo
 *	pst_crashdev
 *
 * Size-variant types
 *  Defines:
 *	none
 *
 *  Uses:
 *	__psdev
 *
 */


/* pst_crashinfo support */

#include <sys/pstat/pstat_constants.h>


/*
 * This structure describes the system crash dump configuration.
 * It is only available as 64-bit data (_PSTAT64 defined).
 */


struct pst_crashinfo {
        int64_t      psc_flags;		/* Dump config. flags, see below */
        struct __psdev psc_headerdev;	/* Device containing dump header */
  	int64_t      psc_headeroffset;	/* Byte Offset of dump hdr on device */
  	int64_t      psc_ncrashdevs;	/* Number of dump devices */
  	int64_t      psc_totalsize;	/* Total amount of dump space (kB) */
  	int64_t      psc_included;	/* Page classes to be included */
  	int64_t      psc_excluded;	/* Page classes to be excluded */
  	int64_t      psc_default;	/* Defaults for unspec'd classes */
  	int64_t      psc_nclasses;	/* Number of classes */
  	int64_t      psc_pgcount[PST_MAXCLASSES];
					/* Number of pages in each class */
	int64_t      psc_reserved;      /* Reserved for future use */
};

/* Flag values for psc_flags: */
#define PS_EARLY_DUMP   0x1		/* An early dump was taken */
#define PS_CONF_CHANGED 0x2		/* Config. changed since boot */
#define PS_HEADER_VALID 0x4		/* headerdev and headeroffset valid */
/* CDPI - flag value for compress option */
#define PS_COMPRESS	0x8		/* Compressed dump option */

/*
 * This structure describes a crash dump device.
 * It is only available as 64-bit data (_PSTAT64 defined).
 */
struct pst_crashdev {
        int64_t      psc_idx;		/* Index of this device. */
        struct __psdev psc_device;	/* Device number of physical device */
  	int64_t      psc_offset;	/* Offset (kB) of area on phys. dev. */
  	int64_t      psc_size;		/* Size (kB) of dump area */
        struct psdev psc_lv;		/* Logical volume dev number, if any */
	int64_t      psc_source;        /* How'd this device get config'd? */
};

/* Flag values for psc_source: */
#define PS_BOOTTIME 0x1                 /* Device configured at boot */
#define PS_RUNTIME  0x2                 /* Device configured by crashconf(2) */

