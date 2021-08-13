#ifndef __LIST_H__
#define __LIST_H__

//#include "lib.h"

struct list_head
{
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name)	\
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static inline void __add_list(struct list_head *new, struct list_head *prev, struct list_head *next)
{
	prev->next = new;
	next->prev = new;
	new->next  = next;
	new->prev  = prev;

}

static inline void add_list(struct list_head *new, struct list_head *head)
{
	__add_list(new, head, head->next);
}

static inline void add_list_tail(struct list_head *new, struct list_head *head)
{
	__add_list(new, head->prev, head);
}

static inline void __del_entry(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void del_list(struct list_head *list)
{
	__del_entry(list->prev, list->next);
	INIT_LIST_HEAD(list);
}

static inline int is_list_empty(struct list_head *head)
{
	return (head->prev == head);
}

static inline int is_list_last(struct list_head *list, struct list_head *head)
{

	return (list->next == head);
}

#define list_for_each(pos, head)				\
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)			\
	for (pos = (head)->next, n = pos->next; pos != (head);	\
				pos = n, n = pos->next)
		

#endif
