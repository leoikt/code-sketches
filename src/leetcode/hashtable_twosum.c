// src: https://leetcode.com/problems/two-sum/
#define HASH_SIZE 32768
#define HASH_SHIFT 17  // (32 - 15)

static inline int get_hash(int key) {
  return ((unsigned int)key * 2654435761u) >> HASH_SHIFT;
}

typedef struct {
  int key;   // nums[i]
  int val;   // i
  int step;  // N test
} HashEntry;

static HashEntry table[HASH_SIZE];
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
static int test_count = 0;
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
  test_count++;
  *returnSize = 2;
  int* res = (int*)malloc(2 * sizeof(int));
  if (res == NULL) {
    *returnSize = 0;
    return NULL;
  }
  for (int i = 0; i < numsSize; i++) {
    int diff = target - nums[i];

    int h = get_hash(diff);
    while (table[h].step == test_count) {
      if (table[h].key == diff) {
        res[0] = table[h].val;
        res[1] = i;
        return res;
      }
      h = (h + 1) & (HASH_SIZE - 1);
    }

    int h_insert = get_hash(nums[i]);
    while (table[h_insert].step == test_count) {
      if (table[h_insert].key == nums[i]) break;
      h_insert = (h_insert + 1) & (HASH_SIZE - 1);
    }

    table[h_insert].key = nums[i];
    table[h_insert].val = i;
    table[h_insert].step = test_count;
  }

  return res;
}
