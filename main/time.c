#include "include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

const char *YMDEXAMPLE = "2021-04-01";
const char *HMEXAMPLE = "17:30";
const char *MEXAMPLE  = "30";

//nah just lazy
typedef struct tm tms;

tms *getCurrentTime() {
    time_t now;
    time(&now);
    struct tm *timeinfo = malloc(sizeof(struct tm));
    localtime_r(&now, timeinfo);
    return timeinfo;
}

//convert hour:minute to struct tm
tms convertStrtoHAM(char *str) {
    tms ret;
    time_t zero = 0;
    localtime_r(&zero, &ret);
    ret.tm_hour = atoi(str);
    ret.tm_min  = atoi(strchr(str, ':')+sizeof(char));
    return ret;
}

char *getMinute() {
    char *timestamp = newstr(strlen(MEXAMPLE) + 1);
    strftime(timestamp, strlen(MEXAMPLE) + 1, "%M", getCurrentTime());
    return timestamp;
}

int intGetMinute() {
    return getCurrentTime()->tm_min;
}

char *getHourandMinute() {
    char *timestamp = newstr(strlen(HMEXAMPLE)+1);
    strftime(timestamp, strlen(HMEXAMPLE) + 1, "%H:%M", getCurrentTime());
    return timestamp;
}

char *getYearMonthDay() {
    char *timestamp = newstr(strlen(YMDEXAMPLE)+1);
    strftime(timestamp, strlen(YMDEXAMPLE) + 1, "%Y-%m-%d", getCurrentTime());
    return timestamp;
}

void addMinute(char *target, int min) {
    tms temp;
    time_t tt;
    temp = *getCurrentTime();
    temp.tm_sec = 0;
    temp.tm_hour = convertStrtoHAM(target).tm_hour;
    temp.tm_min = convertStrtoHAM(target).tm_min;
    tt = mktime(&temp) + (min * 60);
    localtime_r(&tt, &temp);
    strftime(target, strlen(HMEXAMPLE) + 1, "%H:%M", &temp);
}

int isWorkTime(char *start, char *end) {
    tms temp;
    time_t st, et;
    temp = *getCurrentTime();
    temp.tm_sec = 0;
    temp.tm_hour = convertStrtoHAM(start).tm_hour;
    temp.tm_min = convertStrtoHAM(start).tm_min;
    st = mktime(&temp);
    temp.tm_hour = convertStrtoHAM(end).tm_hour;
    temp.tm_min = convertStrtoHAM(end).tm_min;
    et = mktime(&temp);
    if (et < st) {
        et += 86400;
    }
    time_t now;
    time(&now);
    if (now > st && now < et) {
        return 1;
    } else {
        return 0;
    }
}
