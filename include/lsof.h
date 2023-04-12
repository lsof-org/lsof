/** @file
 * lsof.h - header file for lsof
 */

/*
 * Copyright 1994 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Victor A. Abell
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors nor Purdue University are responsible for any
 *    consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Credit to the authors and Purdue
 *    University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

/*
 * $Id: lsof.h,v 1.70 2018/03/26 21:50:45 abe Exp $
 */

#if !defined(LSOF_H)
#    define LSOF_H 1

/** File access mode */
enum lsof_file_access_mode {
    LSOF_FILE_ACCESS_NONE = 0,  /**< None */
    LSOF_FILE_ACCESS_READ = 1,  /**< Read */
    LSOF_FILE_ACCESS_WRITE = 2, /**< Write */
    LSOF_FILE_ACCESS_READ_WRITE =
        LSOF_FILE_ACCESS_READ | LSOF_FILE_ACCESS_WRITE, /**< Read and write */
};

/** File lock mode */
enum lsof_lock_mode {
    LSOF_LOCK_NONE,          /**< None */
    LSOF_LOCK_UNKNOWN,       /**< Unknown */
    LSOF_LOCK_READ_PARTIAL,  /**< Read lock on part of the file */
    LSOF_LOCK_READ_FULL,     /**< Read lock on the entire file */
    LSOF_LOCK_WRITE_PARTIAL, /**< Write lock on part of the file */
    LSOF_LOCK_WRITE_FULL,    /**< Write lock on the entire file */
    LSOF_LOCK_READ_WRITE,    /**< Read and write lock */
    LSOF_LOCK_SOLARIS_NFS,   /**< Solaris NFS lock */
    LSOF_LOCK_SCO_PARTIAL, /**< SCO OpenServer Xenix lock on part of the file */
    LSOF_LOCK_SCO_FULL,    /**< SCO OpenServer Xenix lock on the entire file */
};

/** File descriptor type */
enum lsof_fd_type {
    LSOF_FD_NUMERIC,    /**< Numeric fd, e.g fd returned by open() in process */
    LSOF_FD_UNKNOWN,    /**< Unknown fd type */
    LSOF_FD_CWD,        /**< Current working directory */
    LSOF_FD_ERROR,      /**< Failed to get fd information */
    LSOF_FD_ROOT_DIR,   /**< Root directory */
    LSOF_FD_PARENT_DIR, /**< Parent directory */
    LSOF_FD_PROGRAM_TEXT, /**< Program text */
    LSOF_FD_LIBRARY_TEXT, /**< Library text */
    LSOF_FD_MEMORY,       /**< Memory */
    LSOF_FD_DELETED,      /**< Deleted file */
    LSOF_FD_FILEPORT,     /**< Darwin fileport */
    LSOF_FD_TASK_CWD,     /**< Per task/thread cwd */
    LSOF_FD_CTTY,         /**< Character TTY */
    LSOF_FD_JAIL_DIR,     /**< Jail directory */
    LSOF_FD_VIRTUAL_8086, /**< Virtual 8086 */
    LSOF_FD_MERGE_386,    /**< MERGE386 vm86 region */
    LSOF_FD_MMAP_SPECIAL, /**< mmap-ed special device segment */
};

#endif