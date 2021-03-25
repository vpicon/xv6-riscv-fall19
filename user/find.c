#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

static int filename_size;


/**
 * Given a file and a string representing path to a file,
 * returns 1 if the file at end of path has the same name
 * as the given file argument. Return 0 otherwise.
 */
int has_filename(const char *path, const char *file) {
    return 1;
}


/*
char *fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;// DO NOTHING

    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;

    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}
*/



void find(const char *path, const char *file) {
    char buf[512];

    // read given path
    int fd;
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Get info of opened file
    struct stat st;
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_FILE:   // base case of recursion
            if (has_filename(path, file))
                printf("%s\n", path);
            break;

        case T_DIR:    // call recurrsively each node in directory tree

            // Check if path too long to keep recursive calls
            if(strlen(path) + 1 + filename_size + 1 > sizeof(buf)) {    // path + / + file + '\0'
              printf("find: reached path too long in path %s\n", path);
              break;
            }

            // Construct new path for each node in the directory-tree
            strcpy(buf, path);
            char *p = buf + strlen(buf);
            *p++ = '/';

            struct dirent de;
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {  // read nodes in directory-tree
                if(de.inum == 0)
                    continue;

                memmove(p, de.name, filename_size);
                p[filename_size] = '\0';

                find(p, file);
            }

        default:
            break;
    }

    close(fd);
}



int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(2, "usage %s: <path> <file>\n", argv[0]);
        exit(1);
    }

    // Call find with given args
    filename_size = strlen(argv[2]);
    find(argv[1], argv[2]);

    exit(0);
}
