#ifndef UTILS_H
#define UTILS_H

#define min(x, y)           (x) < (y) ? (x) : (y)
#define max(x, y)           (x) > (y) ? (x) : (y)

#define bound(value, limit) max(min(value, limit), -1 * limit)

#define print(fmt, ...)     printf_P(PSTR(fmt), ##__VA_ARGS__)

#endif /* UTILS_H */