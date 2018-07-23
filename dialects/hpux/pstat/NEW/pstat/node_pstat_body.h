/* node_pstat_body.h : pst_node body declarations and user manifest constants */

/*   @(#)B11.23_LR   */
/* NO MULTIPLE-INCLUDE PROTECTION (intentional) */

/*
 * Constants
 *	none
 *
 * Size-invariant types
 *  Defines:
 *	pst_node
 *
 *  Uses:
 *  	none
 *
 * Size-variant types
 *  Defines: 
 *	none
 *
 *  Uses:
 *	none
 *
 */



/*
 * This structure describes SCA(ccNUMA) node information
 * It is only available as 64-bit data (_PSTAT64 defined)
 *
 */


struct	pst_node {
	uint64_t	psn_idx;	  /* value currently undefined	*/
	uint64_t	psn_logical_node; /* logical node id            */
	uint64_t	psn_physical_node;/* physical node id           */
	uint64_t	psn_cpu_cnt;	  /* number of node active cpus	*/
	uint64_t	psn_private_mem_size;
					  /* node private memory in (MB)*/
	uint64_t	psn_gmem_start;   /* start of node global       */
					  /*    memory(in MB)           */
	uint64_t	psn_gmem_size;	  /* max size of node global    */
					  /*    memory(in MB)           */
	uint64_t	psn_reserved[3];  /* Reserved for future use    */
	uint64_t	psn_padding[6];	  /* Padding to next pow(2)	*/
};

