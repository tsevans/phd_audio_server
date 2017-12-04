#include "dlist.h"
#include <assert.h>

/*
 * Check if list element is head.
 *
 * @param elem - List element to check.
 * @return - True if elem is head of list, false otherwise.
 */
static inline bool is_head(struct list_elem* elem)
{
    return elem != NULL && elem->prev == NULL && elem->next != NULL;
}

/*
 * Check if list element is interior element.
 *
 * @param elem - List element to check.
 * @return - True if elem is interior, false otherwise.
 */
static inline bool is_interior(struct list_elem* elem)
{
    return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

/*
 * Check if list element is tail.
 *
 * @param elem - List element to check.
 * @return - True if elem is tail of list, false otherwise.
 */
static inline bool is_tail(struct list_elem *elem)
{
    return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

/*
 * Initialize an empty list.
 *
 * @param elem - List to be initialized.
 */
void list_init(struct list* list)
{
    assert(list != NULL);
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

/*
 * Access the beginning of the list (head).
 *
 * @param elem - List to get beginning from.
 * @return - Pointer to element at beginning of list.
 */
struct list_elem* list_begin(struct list* list)
{
    assert(list != NULL);
    return list->head.next;
}

/*
 * Access element after elem in a list.
 *
 * @param elem - Element to get next element from.
 * @return - Element in list after elem.
 */
struct list_elem* list_next(struct list_elem* elem)
{
    assert(is_head(elem) || is_interior(elem));
    return elem->next;
}

/*
 * Access the end of the list (tail).
 *
 * @param elem - List to get end from.
 * @return - Pointer to element at end of list.
 */
struct list_elem* list_end(struct list* list)
{
    assert(list != NULL);
    return &list->tail;
}

/*
 * Access list beginning when iterating in reverse order.
 *
 * @param list - List from which access occurs.
 * @return - Pointer to reverse beginning element of list.
 */
struct list_elem* list_rbegin(struct list* list)
{
    assert(list != NULL);
    return list->tail.prev;
}

/*
 * Access element before elem in a list.
 *
 * @param elem - Element to get predecessor for.
 * @return - Element in list before elem.
 */
struct list_elem* list_prev(struct list_elem* elem)
{
    assert(is_interior(elem) || is_tail(elem));
    return elem->prev;
}

/*
 * Access list end when iterating in reverse order.
 *
 * @param list - List from which access occurs.
 * @return - Pointer to reverse end element of list.
 */
struct list_elem* list_rend(struct list* list)
{
    assert(list != NULL);
    return &list->head;
}

/*
 * Access list head.
 *
 * @param list - List from which access occurs.
 * @return - Pointer to head element of list.
 */
struct list_elem* list_head(struct list* list)
{
    assert(list != NULL);
    return &list->head;
}

/*
 * Access list tail.
 *
 * @param list - List from which access occurs.
 * @return - Pointer to tail element of list.
 */
struct list_elem* list_tail(struct list* list)
{
    assert(list != NULL);
    return &list->tail;
}

/*
 * Insert element into a list.
 *
 * @param before - Insert before this element, which is interior or tail.
 * @param elem - Element to insert into list.
 */
void list_insert(struct list_elem* before, struct list_elem* elem)
{
    assert(is_interior(before) || is_tail(before));
    assert(elem != NULL);

    elem->prev = before->prev;
    elem->next = before;
    before->prev->next = elem;
    before->prev = elem;
}

/*
 * Insert element at front of list.
 *
 * @param list - List to insert to.
 * @param elem - Element to insert at front.
 */
void list_push_front(struct list* list, struct list_elem* elem)
{
    list_insert(list_begin(list), elem);
}

/*
 * Insert element at back of list.
 *
 * @param list - List to insert to.
 * @param elem - Element to insert at back.
 */
void list_push_back(struct list* list, struct list_elem* elem)
{
    list_insert(list_end(list), elem);
}

/*
 * Remove element from a list.
 *
 * @param elem - Element to remove from list.
 * @return - Element following element that was removed.
 */
struct list_elem* list_remove(struct list_elem* elem)
{
    assert(is_interior(elem));
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    return elem->next;
}

/*
 * Remove element at front of list.
 *
 * @param list - List to remove from.
 * @return - Element that was removed.
 */
struct list_elem* list_pop_front(struct list* list)
{
    struct list_elem* front = list_front(list);
    list_remove(front);
    return front;
}

/*
 * Remove element at back of list.
 *
 * @param list - List to remove from.
 * @return - Element that was removed.
 */
struct list_elem* list_pop_back(struct list* list)
{
    struct list_elem* back = list_back(list);
    list_remove(back);
    return back;
}

/*
 * Access element at front of list.
 *
 * @param list - List to access element from.
 * @return - Element at front of list.
 */
struct list_elem* list_front(struct list* list)
{
    assert(!list_empty(list));
    return list->head.next;
}

/*
 * Access element at back of list.
 *
 * @param list - List to access element from.
 * @return - Element at back of list.
 */
struct list_elem* list_back(struct list* list)
{
    assert(!list_empty(list));
    return list->tail.prev;
}

/*
 * Count number of elements in list.
 *
 * @param list - List to be counted.
 * @return - Number of elements in list.
 */
size_t list_size(struct list* list)
{
    struct list_elem* e;
    size_t cnt = 0;

    for (e = list_begin(list); e != list_end(list); e = list_next(e))
        cnt++;

    return cnt;
}

/*
 * Check if a list is empty.
 *
 * @param list - List to be checked.
 * @return - True if list is empty, false otherwise.
 */
bool list_empty(struct list* list)
{
    return list_begin(list) == list_end(list);
}

/*
 * Swap two element pointers in a list.
 *
 * @param a - First element to be swapped.
 * @param b - Second element to be swapped.
 */
static void swap(struct list_elem** a, struct list_elem** b)
{
    struct list_elem* t = *a;
    *a = *b;
    *b = t;
}

/*
 * Reverse order of a list.
 *
 * @param list - List to be reversed.
 */
void list_reverse(struct list* list)
{
    if (!list_empty(list))
    {
        struct list_elem* e;
        for (e = list_begin(list); e != list_end(list); e = e->prev)
            swap(&e->prev, &e->next);
        swap(&list->head.next, &list->tail.prev);
        swap(&list->head.next->prev, &list->tail.prev->next);
    }
}
