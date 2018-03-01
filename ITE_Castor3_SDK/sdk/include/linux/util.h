#ifndef _LINUX_UTIL_H_
#define _LINUX_UTIL_H_


#define max_t(type, x, y)  ((x) > (y) ? (x) : (y))


static inline int copy_from_user(void *des, void *src, int len) {
    memcpy(des, src, len);
    return 0;
}

static inline int copy_to_user(void *des, void *src, int len) {
    memcpy(des, src, len);
    return 0;
}



#endif // _LINUX_UTIL_H_