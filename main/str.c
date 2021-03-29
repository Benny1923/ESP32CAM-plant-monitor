#include "include/common.h"

#include <stdio.h>
#include <string.h>

char *newstr(size_t len) {
    return calloc(len , sizeof(char));
}

void strpad(char **des, char *src) {
    size_t len = strlen(*des) + strlen(src) + 1;
    char *res = newstr(len);
    char *temp = newstr(strlen(src)+1);
    strcpy(res, *des);
    strcpy(temp, src);
    strcat(res, temp);
    free(*des);
    *des = res;
}
