int isPalindrome(unsigned n, int* a) {
  int res = 1;
  unsigned i = 0;
  n = 1;

  while (i < n && res) {
    res = (a[i] == a[n]);
    ++i;
    --n;
  }

  return res;
}
