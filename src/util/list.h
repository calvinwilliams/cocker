#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

/*
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/poison.h>
#include <linux/const.h>
#include <linux/kernel.h>
*/
#include <stdio.h>

#ifndef container_of
#define container_of(ptr, type, member) ((type *)( (char *)(ptr) - offsetof(type,member) ))
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head {
	struct list_head *next, *prev;
};

#define WRITE_ONCE(_var_,_val_)		(_var_) = (_val_)
#define READ_ONCE(_var_)		(_var_)

#define POISON_POINTER_DELTA 0
#define LIST_POISON1  ((struct list_head *) 0x100 + POISON_POINTER_DELTA)
#define LIST_POISON2  ((struct list_head *) 0x200 + POISON_POINTER_DELTA)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

void INIT_LIST_HEAD(struct list_head *list);
void list_add(struct list_head *_new, struct list_head *head);
void list_add_tail(struct list_head *_new, struct list_head *head);
void list_del(struct list_head *entry);
void list_replace(struct list_head *old, struct list_head *_new);
void list_replace_init(struct list_head *old, struct list_head *_new);
void list_del_init(struct list_head *entry);
void list_move(struct list_head *list, struct list_head *head);
void list_move_tail(struct list_head *list, struct list_head *head);
int list_is_last(const struct list_head *list, const struct list_head *head);
int list_empty(const struct list_head *head);
int list_empty_careful(const struct list_head *head);
void list_rotate_left(struct list_head *head);
int list_is_singular(const struct list_head *head);
void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry);
void list_splice(const struct list_head *list, struct list_head *head);
void list_splice_tail(struct list_head *list, struct list_head *head);
void list_splice_init(struct list_head *list, struct list_head *head);
void list_splice_tail_init(struct list_head *list, struct list_head *head);

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @p_node:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(p_node, type, member) \
	list_entry((p_node)->member.next, type, member)

/**
 * list_prev_entry - get the prev element in list
 * @p_node:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(p_node, type, member) \
	list_entry((p_node)->member.prev, type, member)

/**
 * list_for_each	-	iterate over a list
 * @p_node:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(p_node, head) \
	for (p_node = (head)->next; p_node != (head); p_node = p_node->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @p_node:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(p_node, head) \
	for (p_node = (head)->prev; p_node != (head); p_node = p_node->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @p_node:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(p_node, n, head) \
	for (p_node = (head)->next, n = p_node->next; p_node != (head); \
		p_node = n, n = p_node->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @p_node:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(p_node, n, head) \
	for (p_node = (head)->prev, n = p_node->prev; \
	     p_node != (head); \
	     p_node = n, n = p_node->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @p_node:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(p_node, head, type, member)				\
	for (p_node = list_first_entry(head, type, member);	\
	     &p_node->member != (head);					\
	     p_node = list_next_entry(p_node, type, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @p_node:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(p_node, head, member)			\
	for (p_node = list_last_entry(head, typeof(*p_node), member);		\
	     &p_node->member != (head); 					\
	     p_node = list_prev_entry(p_node, member))

/**
 * list_prepare_entry - prepare a p_node entry for use in list_for_each_entry_continue()
 * @p_node:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_head within the struct.
 *
 * Prepares a p_node entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(p_node, head, member) \
	((p_node) ? : list_entry(head, typeof(*p_node), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @p_node:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(p_node, head, member) 		\
	for (p_node = list_next_entry(p_node, member);			\
	     &p_node->member != (head);					\
	     p_node = list_next_entry(p_node, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @p_node:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(p_node, head, member)		\
	for (p_node = list_prev_entry(p_node, member);			\
	     &p_node->member != (head);					\
	     p_node = list_prev_entry(p_node, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @p_node:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(p_node, head, member) 			\
	for (; &p_node->member != (head);					\
	     p_node = list_next_entry(p_node, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @p_node:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(p_node, n, head, type, member)			\
	for (p_node = list_first_entry(head, type, member),	\
		n = list_next_entry(p_node, type, member);			\
	     &p_node->member != (head); 					\
	     p_node = n, n = list_next_entry(n, type, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @p_node:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(p_node, n, head, member) 		\
	for (p_node = list_next_entry(p_node, member), 				\
		n = list_next_entry(p_node, member);				\
	     &p_node->member != (head);						\
	     p_node = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @p_node:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(p_node, n, head, member) 			\
	for (n = list_next_entry(p_node, member);					\
	     &p_node->member != (head);						\
	     p_node = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @p_node:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(p_node, n, head, member)		\
	for (p_node = list_last_entry(head, typeof(*p_node), member),		\
		n = list_prev_entry(p_node, member);			\
	     &p_node->member != (head); 					\
	     p_node = n, n = list_prev_entry(n, member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @p_node:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_head within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (p_node) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(p_node, n, member)				\
	n = list_next_entry(p_node, member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#define HLIST_POISON1  ((struct hlist_node *) 0x100 + POISON_POINTER_DELTA)
#define HLIST_POISON2  ((struct hlist_node **) 0x200 + POISON_POINTER_DELTA)

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

void INIT_HLIST_NODE(struct hlist_node *h);
int hlist_unhashed(const struct hlist_node *h);
int hlist_empty(const struct hlist_head *h);
void hlist_del(struct hlist_node *n);
void hlist_del_init(struct hlist_node *n);
void hlist_add_head(struct hlist_node *n, struct hlist_head *h);
void hlist_add_before(struct hlist_node *n, struct hlist_node *next);
void hlist_add_behind(struct hlist_node *n, struct hlist_node *prev);
void hlist_add_fake(struct hlist_node *n);
int hlist_fake(struct hlist_node *h);
int hlist_is_singular_node(struct hlist_node *n, struct hlist_head *h);
void hlist_move_list(struct hlist_head *old, struct hlist_head *_new);

#define hlist_entry(ptr, type, member) ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos ; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos=(head)->first, n=((pos)?((pos)->next):NULL) ; pos ; pos = n, n=((pos)?((pos)->next):NULL) )

#define hlist_entry_safe(ptr, type, member) (ptr==NULL?NULL:hlist_entry(ptr, type, member))

/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, type, member)			\
	for (pos = hlist_entry_safe((head)->first, type, member);	\
	     pos;							\
	     pos = hlist_entry_safe((pos)->member.next, type, member))

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(pos, member)			\
	for (pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member);\
	     pos;							\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(pos, member)				\
	for (; pos;							\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(pos, n, head, member) 		\
	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member);\
	     pos && ({ n = pos->member.next; 1; });			\
	     pos = hlist_entry_safe(n, typeof(*pos), member))

#endif
