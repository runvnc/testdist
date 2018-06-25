void initOrUpdate(volatile int* last, volatile int* elapsed, int now) {
  if (*last == NULL) {
    *last = now;
    *elapsed = 0;
  } else {
    *elapsed = now - *last;
  }
}
