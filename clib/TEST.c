#include <stdio.h>
#include "neo_exec.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
char buf[8192];

int main() {
    neo_exec_init();
    int fd = open("../pro1.json.txt", O_RDONLY);
    read(fd, buf, sizeof(buf));
    // printf("%s", buf);
    neo_exec_load(buf);
    neo_exec_draw();
    neo_exec_frame_dump();
    close(fd);
}

