#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include "logger.h"

void log_write(const char *level, const char *message)
{
    mkdir("log", 0755);

    FILE *f = fopen(LOG_FILE, "a");
    if (!f) {
        perror("log_write: fopen");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(f, "[%s] [%s] %s\n", timestamp, level, message);
    fclose(f);
}
