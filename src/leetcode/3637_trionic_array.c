static bool gt(int a, int b) { return a > b; }
static bool lt(int a, int b) { return a < b; }
static bool eq(int a, int b) { return a == b; }

static bool _map(int* nums, int sz, size_t* i, bool (*cmp)(int, int)) {
  bool code = false;
  while (*i < sz) {
    if (cmp(nums[*i], nums[(*i) - 1]))
      code = true;
    else if (eq(nums[*i], nums[(*i) - 1])) {
      code = false;
      break;
    } else {
      break;
    }
    *i += 1;
  }
  return code;
}

bool isTrionic(int* nums, int numsSize) {
  bool res = false, p = 0, q = 0;
  if (numsSize > 3) {
    size_t i = 1;
    p = _map(nums, numsSize, &i, gt);
    if (p) q = _map(nums, numsSize, &i, lt);
    if (q) res = _map(nums, numsSize, &i, gt);
    res = res && i == (size_t)numsSize;
  }
  return res;
}
