#ifndef __LOGGER__
#define __LOGGER__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <error.h>
#include <sys/file.h>
#define log_buffer_size 8192
#define LOG_DEBUG(x...)    log_write(LOG_DEBUG, __FILE__, __LINE__, ##x)
#define LOG_TRACE(x...)    log_write(LOG_TRACE, __FILE__, __LINE__, ##x)
#define LOG_NOTICE(x...)   log_write(LOG_NOTICE, __FILE__, __LINE__, ##x)
#define LOG_MONITOR(x...)  log_write(LOG_MONITOR, __FILE__ __LINE__, ##x)
#define LOG_WARNING(x...)  log_write(LOG_WARNING, __FILE__, __LINE__, ##x)
#define LOG_FATAL(x...)    log_write(LOG_FATAL, __FILE__, __LINE__, ##x)

int log_open(const char *filename);
void log_close();
void log_add_info(const char *info);
static void log_write(int, const char *, const int, const char *, ...);

const int LOG_FATAL   = 0;
const int LOG_WARNING = 1;
const int LOG_MONITOR = 2;
const int LOG_NOTICE  = 3;
const int LOG_TRACE   = 4;
const int LOG_DEBUG   = 5;

static FILE *log_fp = NULL;
static char *log_filename = NULL;
static int  log_opened = 0;
static char log_buffer[log_buffer_size];
static char log_extra_info[log_buffer_size];
static char LOG_LEVEL_NOTE[][10] = 
    { "FATAL", "WARNING", "MONITOR", "NOTICE", "TRACE", "DEBUG" };

int log_open(const char* filename) {
    if (log_opened == 1) {
        fprintf(stderr, "logger: log already opened\n");
        return 0;
    }
    int len = strlen(filename);
    log_filename = (char *)malloc(sizeof(char) * len + 1);
    strcpy(log_filename, filename);
    log_fp = fopen(log_filename, "a");
    if (log_fp == NULL) {
        fprintf(stderr, "log_file: %s", log_filename);
        perror("can't not open log file");
        exit(1);
    }
    atexit(log_close);
    log_opened = 1;
    log_extra_info[0] = 0;
    LOG_NOTICE("log_open");
    return 1;
}

void log_close() {
    if (log_opened) {
        LOG_TRACE("log_close");
        fclose(log_fp);
        free(log_filename);
        log_fp = NULL;
        log_filename = NULL;
        log_opened = 0;
    }
}

static void log_write(int level, const char *file,
        const int line, const char *fmt, ...) {
    if (log_opened == 0) {
        fprintf(stderr, "log_open not called yet\n");
        exit(1);
    }
    static char buffer[log_buffer_size];
    static char datetime[100];
    static char line_str[20];
    static time_t now;
    now = time(NULL);

    strftime(datetime, 99, "%Y-%m-%d %H:%M:%S", localtime(&now));
    snprintf(line_str, 19, "%d", line);
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_buffer, log_buffer_size, fmt, ap);
    va_end(ap);

    size_t count = snprintf(buffer, log_buffer_size,
            "%s [%s] [%s:%d]%s %s\n",
            LOG_LEVEL_NOTE[level], datetime, file, line, log_extra_info, log_buffer);
    int log_fd = log_fp -> _fileno;
    if (flock(log_fd, LOCK_EX) == 0) {
        if (write(log_fd, buffer, count) < 0) {
            perror("write error");
            exit(1);
        }
        flock(log_fd, LOCK_UN);
    }
    else {
        perror("flock error");
        exit(1);
    }
}

void log_add_info(const char *info) {
    int len = strlen(log_extra_info);
    snprintf(log_extra_info + len, log_buffer_size - len, "\n [%s]", info);
}

#endif
