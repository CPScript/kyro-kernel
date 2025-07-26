#include <string.h>
#include <stdint.h>

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) {
        len++;
    }
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *orig_dest = dest;
    while ((*dest++ = *src++));
    return orig_dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *orig_dest = dest;
    while (n && (*dest++ = *src++)) {
        n--;
    }
    while (n--) {
        *dest++ = '\0';
    }
    return orig_dest;
}

char *strcat(char *dest, const char *src) {
    char *orig_dest = dest;
    while (*dest) {
        dest++;
    }
    while ((*dest++ = *src++));
    return orig_dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *orig_dest = dest;
    while (*dest) {
        dest++;
    }
    while (n && (*dest++ = *src++)) {
        n--;
    }
    if (n == 0) {
        *dest = '\0';
    }
    return orig_dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == c) {
            return (char*)s;
        }
        s++;
    }
    if (c == '\0') {
        return (char*)s;
    }
    return NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == c) {
            last = s;
        }
        s++;
    }
    if (c == '\0') {
        return (char*)s;
    }
    return (char*)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) {
        return (char*)haystack;
    }
    
    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        
        if (!*n) {
            return (char*)haystack;
        }
        
        haystack++;
    }
    
    return NULL;
}
