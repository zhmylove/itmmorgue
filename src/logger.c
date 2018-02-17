#include "itmmorgue.h"

void logger_init() {
    char *log_file = CONF_SVAL("file_server_log");

    if (! *log_file) {
        return;
    }

    if ((log_fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
        panicf("Unable to open %s!", log_file);
    }

    logger(" ======= GAME STARTED ======= ");
}

void logger(char *str) {
    if (log_fd < 0) {
        return;
    }

    char buf[8192];
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    int len = snprintf(buf, sizeof(buf), "%lu: %s\n", tv.tv_sec, str);
    if (write(log_fd, buf, len) < 0) {
        panic("Unable to write log output!");
    }

    if (server_only == 1) {
        if (write(2, buf, len) < 0) {
            panic("Unable to write log to stderr!");
        }
    }
}

