#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *list = malloc(sizeof(struct list_head));
    if (list != NULL) {
        INIT_LIST_HEAD(list);  // initialize list->prev and list->next
        return list;
    } else
        return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head == NULL)  // no nodes in this list
        return;
    else {
        struct list_head *current = head->next;
        while (current != head) {
            element_t *element = container_of(current, element_t, list);
            current = current->next;
            q_release_element(element);
        }
        free(head);
    }
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    if (new_node == NULL)
        return false;

    new_node->value = malloc(strlen(s) + 1);
    if (new_node->value == NULL) {
        free(new_node);
        return false;
    }

    memcpy(new_node->value, s, (strlen(s) + 1));

    list_add(&new_node->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *new_node = malloc(sizeof(element_t));
    if (new_node == NULL)
        return false;
    new_node->value = malloc(strlen(s) + 1);
    if (new_node->value == NULL) {
        free(new_node);
        return false;
    }

    memcpy(new_node->value, s, (strlen(s) + 1));

    list_add_tail(&new_node->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    struct list_head *current = head->next;
    element_t *element = container_of(current, element_t, list);
    list_del_init(current);

    if (sp != NULL) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    struct list_head *current = head->prev;
    element_t *element = container_of(current, element_t, list);
    list_del_init(current);

    if (sp != NULL) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return 0;
    else {
        struct list_head *current = head->next;
        unsigned int count = 0;
        while (current != head) {
            current = current->next;
            count++;
        }
        return count;
    }
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    else {
        unsigned int list_length = q_size(head);
        unsigned int mid_point = (list_length - 1) / 2;
        struct list_head *current = head->next;
        for (int i = 0; i < mid_point; i++)
            current = current->next;

        element_t *element = container_of(current, element_t, list);
        list_del(current);
        q_release_element(element);
        return true;
    }
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL)
        return false;
    else {
        struct list_head *current = head->next;
        while (current != head) {
            element_t *element = container_of(current, element_t, list);
            struct list_head *compare_current = current->next;
            while (compare_current != head) {
                element_t *compare_element =
                    container_of(compare_current, element_t, list);

                if (element->value[0] != compare_element->value[0])
                    break;

                compare_current = compare_current->next;

                if (strlen(element->value) == strlen(compare_element->value) &&
                    strcmp(element->value, compare_element->value) == 0) {
                    list_del(&compare_element->list);
                    q_release_element(compare_element);
                }
            }
            current = current->next;
        }
        return true;
    }
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    unsigned int loop_count = q_size(head) / 2;
    struct list_head *current = head->next;
    struct list_head *next = current->next;
    for (int i = 0; i < loop_count; i++) {
        next->next->prev = current;
        current->prev->next = next;

        next->prev = current->prev;
        current->next = next->next;

        next->next = current;
        current->prev = next;

        current = current->next;
        next = current->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;

    struct list_head *current = head;
    unsigned int loop_count = q_size(head);

    for (int i = 0; i <= loop_count; i++) {
        struct list_head *temp = current->next;
        current->next = current->prev;
        current->prev = temp;

        current = temp;
    }
}

/*
 * Core of merge sort
 */
void merge_sort(struct list_head **head)
{
    // base case, length is zero or one
    if (*head == NULL || (*head)->next == NULL)
        return;

    // perform split here
    // fast should reach end of list, while slow is at halve of list
    struct list_head *fast = (*head)->next;
    struct list_head *slow = *head;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            fast = fast->next;
            slow = slow->next;
        }
    }

    struct list_head *left = *head;
    struct list_head *right = slow->next;
    slow->next = NULL;
    merge_sort(&left);
    merge_sort(&right);

    // merge list back
    struct list_head dummy;
    struct list_head *current = &dummy;
    while (1) {
        // base case, left or right list has no node
        if (left == NULL) {
            current->next = right;
            break;
        }
        if (right == NULL) {
            current->next = left;
            break;
        }

        element_t *left_element = container_of(left, element_t, list);
        element_t *right_element = container_of(right, element_t, list);
        if (strcmp(left_element->value, right_element->value) > 0) {
            current->next = right;
            right = right->next;
        } else {
            current->next = left;
            left = left->next;
        }
        current = current->next;
    }
    *head = dummy.next;
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    // Use merge sort
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    // make this list a singly-linked list
    head->prev->next = NULL;
    merge_sort(&(head->next));

    struct list_head *prev = head;
    struct list_head *current = head->next;
    while (current != NULL) {
        current->prev = prev;
        prev = current;
        current = current->next;
    }
    prev->next = head;
    head->prev = prev;
}
