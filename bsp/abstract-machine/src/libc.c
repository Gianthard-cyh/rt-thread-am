#include <klib.h>
#include <rtthread.h>

char *strchr(const char *s, int c) {

  while (*s != '\0') {
    if (*s == c) {
      return (char *)s;
    }
    s++;
  }

  if (c == '\0') {
    return (char *)s;
  }

  return 0;
}

char *strrchr(const char *s, int c) {

  int offset = strlen(s);
  while (*s != '\0') {
    if (*(s + offset) == c) {
      return (char *)s;
    }
    offset--;
  }

  return 0;
}

char *strstr(const char *haystack, const char *needle) {
  return rt_strstr(haystack, needle);
}

long strtol(const char *restrict nptr, char **restrict endptr, int base) {
  const char *s = nptr;
  long acc = 0;
  int c;
  long cutoff;
  int neg = 0, any, cutlim;

  do {
    c = *s++;
  } while (c == ' ');

  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+') {
    c = *s++;
  }

  if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  } else if ((base == 0 || base == 2) && c == '0' && (*s == 'b' || *s == 'B')) {
    c = s[1];
    s += 2;
    base = 2;
  } else if (base == 0) {
    base = c == '0' ? 8 : 10;
  }

  cutoff = neg ? -(unsigned long)(-2147483648L) : (2147483647L);
  cutlim = cutoff % base;
  cutoff /= base;

  for (acc = 0, any = 0;; c = *s++) {
    if (c >= '0' && c <= '9') {
      c -= '0';
    } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
      c -= (c >= 'A' && c <= 'Z') ? 'A' - 10 : 'a' - 10;
    } else {
      break;
    }
    if (c >= base) {
      break;
    }
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;
    } else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }

  if (any < 0) {
    acc = neg ? (-2147483648L) : (2147483647L);
  } else if (neg) {
    acc = -acc;
  }

  if (endptr != 0) {
    *endptr = (char *)(any ? s - 1 : nptr);
  }

  return acc;
}

char *strncat(char *restrict dst, const char *restrict src, size_t sz) {
  char *dp = dst;

  while (*dp != '\0') {
    dp++;
  }

  while (sz-- > 0 && *src != '\0') {
    *dp++ = *src++;
  }

  *dp = '\0';

  return dst;
}
