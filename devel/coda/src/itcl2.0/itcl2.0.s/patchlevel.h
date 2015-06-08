/*
 * ------------------------------------------------------------------------
 *      PACKAGE:  [incr Tcl]
 *  DESCRIPTION:  Object-Oriented Extensions to Tcl
 *
 *  [incr Tcl] provides object-oriented extensions to Tcl, much as
 *  C++ provides object-oriented extensions to C.  It provides a means
 *  of encapsulating related procedures together with their shared data
 *  in a local namespace that is hidden from the outside world.  It
 *  promotes code re-use through inheritance.  More than anything else,
 *  it encourages better organization of Tcl applications through the
 *  object-oriented paradigm, leading to code that is easier to
 *  understand and maintain.
 *
 *  This file does nothing except define a "patch level" for [incr Tcl].
 *  The patch level has the form "X.YpZ" where X.Y is the base release,
 *  and Z is a serial number that is used to sequence patches for a
 *  given release.  Thus 7.4p1 is the first patch to release 7.4,
 *  7.4p2 is the patch that follows 7.4p1, and so on.  The "pZ" is
 *  omitted in an original new release, and it is replaced with "bZ"
 *  for beta releases.  The patch level ensures that patches are
 *  applied in the correct order and only to appropriate sources.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: patchlevel.h,v 1.1.1.1 1996/08/21 19:29:44 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#define ITCL_PATCH_LEVEL "2.0"
