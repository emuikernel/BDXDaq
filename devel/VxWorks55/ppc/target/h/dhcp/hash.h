/* hash.h - DHCP server include file for hash table functions */

/* Copyright 1984 - 1997 Wind River Systems, Inc. */

/*
modification history
____________________
01b,06aug97,spm  added definitions for C++ compilation
01a,07apr97,spm  created by modifying WIDE project DHCP implementation
*/

/*
 * Modified by Akihiro Tominaga. (tomy@sfc.wide.ad.jp)
 */
/*
 * Generalized hash table ADT
 *
 * Provides multiple, dynamically-allocated, variable-sized hash tables on
 * various data and keys.
 *
 * This package attempts to follow some of the coding conventions suggested
 * by Bob Sidebotham and the AFS Clean Code Committee.
 *
 *
 *
 * Copyright 1988 by Carnegie Mellon.
 *
 * Permission to use, copy, modify, and distribute this program for any
 * purpose and without fee is hereby granted, provided that this copyright
 * and permission notice appear on all copies and supporting documentation,
 * the name of Carnegie Mellon not be used in advertising or publicity
 * pertaining to distribution of the program without specific prior
 * permission, and notice be given in supporting documentation that copying
 * and distribution is by permission of Carnegie Mellon and Stanford
 * University.  Carnegie Mellon makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */
#ifndef __INChashh
#define __INChashh

#ifdef __cplusplus
extern "C" {
#endif

#define HASHTBL_SIZE            127     /* this is a prime number */

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef NULL
#define NULL    0
#endif

/*
 * Define "hash_datum" as a universal data type
 */
#ifdef __STDC__
typedef void hash_datum;
#else
typedef char hash_datum;
#endif

struct hash_member {
    struct hash_member *next;
    hash_datum  *data;
};

struct hash_tbl {
    struct hash_member *head[HASHTBL_SIZE];
};

extern int  hash_exst();
extern int  hash_ins();
extern int  hash_del();
extern hash_datum  *hash_pickup();
extern hash_datum  *hash_find();

#ifdef __cplusplus
}
#endif

#endif
