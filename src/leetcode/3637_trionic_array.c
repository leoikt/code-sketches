static bool gt(int a, int b) { return a > b; }
static bool lt(int a, int b) { return a < b; }
enum { ASC = 0, DESC = 1 };
static bool (*f[])(int, int) = {[ASC] = gt, [DESC] = lt};

static bool _map(int* nums, int sz, size_t* i, int state) {
  bool res = false;
  while ((*i) < sz && f[state](nums[*i], nums[(*i) - 1])) {
    res = true;
    (*i)++;
  }
  return res;
}

bool isTrionic(int* nums, int numsSize) {
  bool res = false;
  if (numsSize > 3) {
    size_t i = 1;
    bool p = _map(nums, numsSize, &i, ASC);
    if (p) {
      bool q = _map(nums, numsSize, &i, DESC);
      if (q) res = _map(nums, numsSize, &i, ASC);
      res = res && ((size_t)numsSize == i);
    }
  }
  return res;
}
