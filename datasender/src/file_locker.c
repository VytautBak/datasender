#include "file_locker.h"

struct flock get_file_lock(int type)
{
        struct flock fl;
        fl.l_whence = SEEK_SET; /* beginning of file */
        fl.l_start = 0;         /* offset from l_whence */
        fl.l_len = 0;           /* length, 0 = to EOF */
        fl.l_pid = getpid();    /* PID */
        fl.l_type = type;       /* lock type */
        return fl;
}

int lock_file(int fd)
{
        struct flock fl = get_file_lock(F_WRLCK);
        int rc = fcntl(fd, F_SETLKW, &fl);
        return rc;
}

int unlock_file()
{
        struct flock fl = get_file_lock(F_UNLCK);
        int fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
        int rc = fcntl(fd, F_SETLK, &fl);
        return rc;
}

/*Checks if the file is currently locked by getting its lock type (F_UNLCK means it is not locked) */
int file_is_unlocked(int fd)
{
        struct flock lck;
        lck.l_type = F_WRLCK;
        lck.l_whence = 0; /* beginning of file */
        lck.l_start = 0;  /* offset from l_whence */
        lck.l_len = 0;    /* length, 0 = to EOF */
        lck.l_pid = 0;    /* PID */
        (void)fcntl(fd, F_GETLK, &lck);
        if (lck.l_type == F_UNLCK) return 0;
        return -1;
}

/*Checks if there is another instance of the daemon by checking if the file is locked */
int is_only_instance()
{
        int fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

        if (fd < 0) {
                fprintf(stderr, "ERROR: Cannot open file %s: %s\n", LOCKFILE, strerror(errno));
                return fd;
        }

        int rc = file_is_unlocked(fd);

        if (rc != 0) {
                fprintf(stderr, "ERROR: There is an instance of the daemon already running! Exiting.\n");
                return rc;
        }

        rc = lock_file(fd);
        if (rc != 0) {
                fprintf(stderr, "ERROR: Could not lock file. Reason: %s\n", strerror(errno));
        }

        return rc;
}
