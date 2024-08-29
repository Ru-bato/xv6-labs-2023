#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/fs.h"

// get filename in path
char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // find the positoin of the last '/' in the path
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // if the length of filename is longer than DIRSIZ, cast
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

// recursively find
void 
find(char *path, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // open path
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // get status
     if(fstat(fd, &st) < 0) {
        fprintf(1, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
        // if filename and matching
        if(strcmp(fmtname(path), target) == 0) {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        // if dialog, find recursively
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // read dialog
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            // skip "." and ".."
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            // call find recursively
            find(buf, target);
        }
        break;
    }
    close(fd);
}

int 
main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: please input path and filename\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}