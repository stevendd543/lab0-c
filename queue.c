#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes,  Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q_head = malloc(sizeof(struct list_head));
    /*return queue head address if successfully allocate memory else return
     NULL*/
    if (q_head) {
        INIT_LIST_HEAD(q_head);
        return q_head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    element_t *iter, *tmp;
    if (l) {
        /*access whole list*/
        list_for_each_entry_safe (iter, tmp, l, list) {
            /*free element containg list node*/
            q_release_element(container_of(&iter->list, typeof(*iter), list));
        };
        free(l);
    }
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    char *v = malloc(sizeof(char) * (strlen(s) + 1));
    if (!v) {
        free(e);
        return false;
    }
    strncpy(v, s, strlen(s) + 1);
    e->value = v;
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    char *v = malloc(sizeof(char) * (strlen(s) + 1));
    if (!v) {
        free(e);
        return false;
    }
    e->value = v;
    strncpy(v, s, strlen(s) + 1);
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = container_of(head->next, element_t, list);
    list_del_init(head->next);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        strncpy(sp + bufsize - 1, "\0", 1);
    }

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = container_of(head->prev, element_t, list);
    list_del_init(head->prev);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        strncpy(sp + bufsize - 1, "\0", 1);
    }

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    int mid = (int) (q_size(head) / 2);
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (!mid) {
            list_del_init(&entry->list);
            q_release_element(entry);
            break;
        }
        mid--;
    }
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    element_t *node, *safe;
    bool flag = false;
    list_for_each_entry_safe (node, safe, head, list) {
        if (&safe->list != head && strcmp(node->value, safe->value) == 0) {
            list_del(&node->list);
            q_release_element(node);
            flag = true;
        } else if (flag) {
            list_del(&node->list);
            q_release_element(node);
            flag = false;
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    bool flag = false;
    struct list_head *node, *tmp = head;
    list_for_each (node, head) {
        if (flag) {
            list_move(node, tmp);
            tmp = node->next;
            node = node->next;
        }
        flag = !flag;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }

}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *flag;
    int count = 1;
    flag = head;
    list_for_each_safe (node, safe, head) {
        list_move(node, flag);
        if (count % k == 0)
            flag = safe->prev;
        count++;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
void mergeLists(struct list_head *L1, struct list_head *L2)
{
    if (!L1 || !L2)
        return;
    struct list_head *Lnode = L1->next, *Rnode = L2->next;
    while (Lnode != L1 && Rnode != L2) {
        if (strcmp(list_entry(Lnode, element_t, list)->value,
                   list_entry(Rnode, element_t, list)->value) < 0) {
            Lnode = Lnode->next;
        } else {
            Rnode = Rnode->next;
            list_move(Rnode->prev, Lnode->prev);
        }
    }
    if (q_size(L2) != 0) {
        list_splice_tail(L2, L1);
    }
}

void merge_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    LIST_HEAD(headtwo);
    list_cut_position(&headtwo, head, slow->prev);
    merge_sort(head);
    merge_sort(&headtwo);
    mergeLists(head, &headtwo);
    // return right;
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    merge_sort(head);
}
/* data */
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *ptr = head->prev, *left = head->prev->prev;
    while (left != head) {
        if (strcmp(list_entry(ptr, element_t, list)->value,
                   list_entry(left, element_t, list)->value) < 0) {
            list_del(&list_entry(left, element_t, list)->list);
            q_release_element(list_entry(left, element_t, list));
        } else {
            ptr = left;
        }
        left = ptr->prev;
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *ptr = head->prev, *left = head->prev->prev;
    while (left != head) {
        if (strcmp(list_entry(ptr, element_t, list)->value,
                   list_entry(left, element_t, list)->value) > 0) {
            list_del(&list_entry(left, element_t, list)->list);
            q_release_element(list_entry(left, element_t, list));
        } else {
            ptr = left;
        }
        left = ptr->prev;
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Merge all the queues into one sorted queue,  which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    queue_contex_t *queue_head = list_entry(head->next, queue_contex_t, chain);
    for (struct list_head *curr = head->next->next; curr != head;
         curr = curr->next) {
        queue_contex_t *queue = list_entry(curr, queue_contex_t, chain);
        mergeLists(queue_head->q, queue->q);
        INIT_LIST_HEAD(queue->q);
        queue->size = 0;
    }
    return queue_head->size;
    // https://leetcode.com/problems/merge-k-sorted-lists/
}
