#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *pos, *n;

    list_for_each_entry_safe (pos, n, l, list)
        q_release_element(pos);

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));

    if (!new)
        return false;

    int len = strlen(s);

    new->value = malloc(sizeof(char) * len + 1);

    if (!new->value) {
        q_release_element(new);
        return false;
    }
    strncpy(new->value, s, len + 1);

    list_add(&new->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));

    if (!new)
        return false;

    int len = strlen(s);

    new->value = malloc((sizeof(char) * len + 1));

    if (!new->value) {
        q_release_element(new);
        return false;
    }
    strncpy(new->value, s, len + 1);

    list_add_tail(&new->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_first_entry(head, element_t, list);

    list_del_init(head->next);
    /*If sp is non-NULL and an element is removed, copy the removed string to
     *sp (up to a maximum of bufsize-1 characters, plus a null terminator.)*/
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_last_entry(head, element_t, list);

    list_del_init(head->prev);
    /*If sp is non-NULL and an element is removed, copy the removed string to
     *sp (up to a maximum of bufsize-1 characters, plus a null terminator.)*/
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *pos;
    int length = 0;

    list_for_each (pos, head)
        length++;

    return length;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;

    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }

    list_del_init(backward);
    q_release_element(list_entry(backward, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *element, *next;
    bool flag = false;

    list_for_each_entry_safe (element, next, head, list) {
        if (&next->list != head && strcmp(element->value, next->value) == 0) {
            list_del_init(&element->list);
            q_release_element(element);
            flag = true;
        } else if (flag) {
            list_del_init(&element->list);
            q_release_element(element);
            flag = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    for (struct list_head *current = head->next;
         current != head && current->next != head; current = current->next)
        list_move(current, current->next);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *pos, *n;

    list_for_each_safe (pos, n, head)
        list_move(pos, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2)
        return;

    int length = q_size(head);
    for (struct list_head *pos = head->next; pos != head && pos->next != head;
         pos = pos->next) {
        struct list_head **current = &pos->next, *tmp = pos->prev;
        for (int i = 1; i < k; i++) {
            if (length >= k)
                list_move(*current, tmp);
        }
        length -= k;
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
