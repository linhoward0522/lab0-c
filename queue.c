#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // https://leetcode.com/problems/merge-k-sorted-lists/
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

struct list_head *merge_list(struct list_head *left, struct list_head *right)
{
    struct list_head *head = NULL, **ptr = &head;

    for (; left && right; ptr = &(*ptr)->next) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            *ptr = left;
            left = left->next;
        } else {
            *ptr = right;
            right = right->next;
        }
    }

    *ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);

    return head;
}
struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast, *slow = head;

    for (fast = head->next; fast && fast->next; fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *left, *right;

    right = slow->next;
    slow->next = NULL;

    left = merge_sort(head);
    right = merge_sort(right);

    return merge_list(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    struct list_head *cur = head, *n = head->next;

    while (n) {
        n->prev = cur;
        cur = n;
        n = n->next;
    }

    cur->next = head;
    head->prev = cur;
}

/*Quick sort recursive*/

/*void q_sort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head list_less, list_greater;
    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    element_t *pivot = list_first_entry(head, element_t, list);
    list_del(&pivot->list);

    element_t *itm = NULL, *is = NULL;
    list_for_each_entry_safe (itm, is, head, list) {
        if (strcmp(itm->value, pivot->value) < 0)
            list_move_tail (&itm->list, &list_less);
        else
            list_move_tail (&itm->list, &list_greater);
    }

    q_sort(&list_less);
    q_sort(&list_greater);

    list_add(&pivot->list, head);
    list_splice(&list_less, head);
    list_splice_tail(&list_greater, head);
}
*/

/*Quick sort non-recursive*/
/*
#define MAX_DEPTH 512
void q_sort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head stack[MAX_DEPTH];
    for (int i = 0; i < MAX_DEPTH; i++)
        INIT_LIST_HEAD(&stack[i]);
    int top = -1;
    list_splice_init(head, &stack[++top]);

    struct list_head partition;
    INIT_LIST_HEAD(&partition);

    while (top >= 0) {
        INIT_LIST_HEAD(&partition);
        list_splice_init(&stack[top--], &partition);
        if (!list_empty(&partition) && !list_is_singular(&partition)) {
            struct list_head list_less, list_greater;
            INIT_LIST_HEAD(&list_less);
            INIT_LIST_HEAD(&list_greater);
            element_t *pivot = list_first_entry(&partition, element_t, list);
            list_del(&pivot->list);
            INIT_LIST_HEAD(&pivot->list);

            element_t *itm = NULL, *is = NULL;
            list_for_each_entry_safe (itm, is, &partition, list) {
                if (strcmp(itm->value, pivot->value) < 0)
                    list_move(&itm->list, &list_less);
                else
                    list_move(&itm->list, &list_greater);
            }

            list_move_tail(&pivot->list, &list_less);
            if (!list_empty(&list_greater))
                list_splice_tail(&list_greater, &stack[++top]);
            if (!list_empty(&list_less))
                list_splice_tail(&list_less, &stack[++top]);
        } else {
            top++;
            list_splice_tail(&partition, &stack[top]);
            while (top >= 0 && list_is_singular(&stack[top])) {
                element_t *tmp = list_first_entry(&stack[top], element_t, list);
                INIT_LIST_HEAD(&stack[top--]);
                list_move_tail(&tmp->list, head);
            }
        }
    }
}*/

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *cur = head->prev, *n = cur->prev;

    while (n != head) {
        if (strcmp(list_entry(n, element_t, list)->value,
                   list_entry(cur, element_t, list)->value) < 0) {
            list_del_init(n);
            q_release_element(list_entry(n, element_t, list));
        } else
            cur = n;
        n = cur->prev;
    }
    return q_size(head);
}

int q_merge(struct list_head *head)
{
    if (list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *pos = NULL;

    struct list_head *tmp = first->q->next;
    first->q->prev->next = NULL;
    list_for_each_entry (pos, head, chain) {
        if (first == pos)
            continue;
        pos->q->prev->next = NULL;
        tmp = merge_list(tmp, pos->q->next);
        INIT_LIST_HEAD(pos->q);
    }
    first->q->next = tmp;
    struct list_head *cur = first->q, *n = cur->next;

    while (n) {
        n->prev = cur;
        cur = n;
        n = n->next;
    }

    cur->next = first->q;
    first->q->prev = cur;

    return q_size(tmp);
}
