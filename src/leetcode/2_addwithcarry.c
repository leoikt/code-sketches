
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

struct ListNode* node_create(int64_t value) {
  struct ListNode* node = (struct ListNode*)malloc(sizeof(struct ListNode));
  if (node) {
    node->value = value;
    node->next = NULL;
  }
  return node;
}

void list_add_back(struct ListNode** old, int value) {
  struct ListNode* new = node_create(value);
  if (new) {
    if (old && *old) {
      struct ListNode* last = list_last(*old);
      last->next = new;
    } else if (old) {
      *old = new;
    }
  }
}

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
  if (!l1 || !l2) return NULL;
  struct ListNode* res = NULL;
  int carry = 0;
  while (l1 || l2 || carry) {
    int sum = ((int)(l1 != NULL) * l1->val);
    sum += ((int)(l12 != NULL) * l2->val) + carry;
    carry = sum / 10;
    list_add_back(&res, sum %= 10);

    if (l1) l1 = l1->next;
    if (l2) l2 = l2->next;
  }
  return res;
}