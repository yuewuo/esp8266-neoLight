#include "neo_exec.h"
struct neo_color frame[NEO_N];  // 记录当前一帧的信息
struct neo_var neo_slot[NEO_SLOT];  // 记录每一个procedure的信息

void frame_clear(struct neo_color *frame) {
    int i=0;
    for (i=0; i<NEO_N; ++i) {
        frame[i].r = frame[i].g = frame[i].b = 0;
    }
}

void neo_exec_init() {
    int i;
    frame_clear(frame);
    for (i=0; i<NEO_SLOT; ++i) {
        neo_slot[i].valid = 0;
    }
}

static int find_valid_slot() {
    int i;
    for (i=0; i<NEO_SLOT; ++i) {
        if (!neo_slot[i].valid) return i;
    }
    return -1;
}

void neo_exec_load(const char* str) {
    int i, version, j;
    char *ptr, *ptr2;
    sscanf(str, "%d", &version);
    neo_printf("version: %d\n", version);
    ptr = strchr(str, '\n');
    if (ptr) {
        if (version == 1) {
            neo_printf("load procedure of version %d\n", version);
            i = find_valid_slot();
            if (i >= 0) {  // 初始化工作
                neo_printf("load procedure to slot %d\n", i);
                // 加载名字字符串
                ptr2 = strchr(str, ' ');
                if (ptr2 != NULL && ptr - ptr2 < NEO_NAME_LENGTH - 2) {
                    *ptr = '\0';
                    strcpy(neo_slot[i].name, ptr2 + 1);
                    neo_printf("procedure name is: \"%s\"\n", neo_slot[i].name);
                    if (strlen(ptr+1) < MAX_NEO_STR_LENGTH) {
                        strcpy(neo_slot[i].str, ptr + 1);
                        neo_slot[i].version = version;
                        switch (version) {
                        case 1:
                            j = neo_exec_v1_init(i);
                            if (j == 0) {
                                neo_printf("procedure init success\n");
                                neo_slot[i].valid = 1;
                            } else neo_printf("procedure init failed with exit code %d\n", j);
                            break;
                        }
                    } else neo_printf("procedure str too long %s %d\n", __FILE__, __LINE__);
                } else neo_printf("procedure name error %s %d\n", __FILE__, __LINE__);
            } else neo_printf("no more slot to put\n");
        } else neo_printf("version error %s %d\n", __FILE__, __LINE__);
    } else neo_printf("format error %s %d\n", __FILE__, __LINE__);
}

extern void neo_exec_draw(int timeintv) {
    int i, ret;
    frame_clear(frame);
    for (i=0; i<NEO_SLOT; ++i) {
        if (neo_slot[i].valid) {
            switch (neo_slot[i].version) {
            case 1:
                ret = neo_exec_v1_draw(i, timeintv);
                if (ret != 0) {
                    neo_printf("procedure %d draw failed with code %d, invalidate it\n", i, ret);
                    neo_slot[i].valid = 0;  // 关闭这个procedure
                }
                break;
            }
        }
    }
}

extern void neo_exec_frame_dump(struct neo_color *frame) {
    int i=0;
    for (i=0; i<NEO_N; ++i) {
        neo_printf("%d: %d %d %d\n", i, frame[i].r, frame[i].g, frame[i].b);
    }
}
