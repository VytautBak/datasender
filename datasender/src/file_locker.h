#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef FILE_LOCKER_H
#define FILE_LOCKER_H
#define LOCKFILE "/var/run/datasender.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int is_only_instance();
int lock_file(int fd);
int unlock_file();
int file_is_unlocked(int fd);
int is_only_instance();

#endif
