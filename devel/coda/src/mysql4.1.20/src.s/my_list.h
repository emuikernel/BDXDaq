/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef _list_h_
#define _list_h_

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct st_list {
  struct st_list *prev,*next;
  void *data;
} MYLIST;

typedef int (*list_walk_action)(void *,void *);

extern MYLIST *list_add(MYLIST *root,MYLIST *element);
extern MYLIST *list_delete(MYLIST *root,MYLIST *element);
extern MYLIST *list_cons(void *data,MYLIST *root);
extern MYLIST *list_reverse(MYLIST *root);
extern void list_free(MYLIST *root,unsigned int free_data);
extern unsigned int list_length(MYLIST *);
extern int list_walk(MYLIST *,list_walk_action action,gptr argument);

#define list_rest(a) ((a)->next)
#define list_push(a,b) (a)=list_cons((b),(a))
#define list_pop(A) {MYLIST *old=(A); (A)=list_delete(old,old) ; my_free((gptr) old,MYF(MY_FAE)); }

#ifdef	__cplusplus
}
#endif
#endif
