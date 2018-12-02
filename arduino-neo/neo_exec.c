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

int neo_exec_set(unsigned char* payload, int length, int slot) {
    int version, j;
    char *ptr, *ptr2;

    // 首先检查长度
    if (length >= MAX_NEO_STR_LENGTH) { neo_printf("procedure payload too long %s %d\n", __FILE__, __LINE__); return -3; }

    payload[length] = '\0';  // 保证有一个结尾
    ptr = strchr(payload, '\n');  // 确保字符串存在第一行
    if (ptr == NULL) { neo_printf("format error %s %d\n", __FILE__, __LINE__); return -7; }

    if (slot < 0 || slot >= NEO_SLOT) { neo_printf("invalid slot number(%d)\n", slot); return -3; }
    neo_printf("load procedure to slot %d\n", slot);

    // 加载名字字符串，不管version为什么，第一行必须存在，而且为 <version> <name>\n
    ptr2 = strchr(payload, ' ');
    if (ptr2 == NULL || ptr - ptr2 >= NEO_NAME_LENGTH - 2) { neo_printf("procedure name error %s %d\n", __FILE__, __LINE__); return -4; }
    *ptr2 = '\0';  // 封住空格的位置，确保读入数字
    sscanf(payload, "%d", &version);
    neo_slot[slot].version = version;
    // *ptr2 = ' ';
    *ptr = '\0';  // 封住名字的结尾
    strcpy(neo_slot[slot].name, ptr2 + 1);
    // *ptr = '\n';
    neo_printf("procedure name is: \"%s\"\n", neo_slot[slot].name);

    // copy content，这个很可能是二进制，长度需要计算好，不包括\n前面的version和name信息
    neo_slot[slot].content_length = length - (ptr + 1 - (char*)payload);
    memcpy(neo_slot[slot].content, ptr + 1, neo_slot[slot].content_length);
    neo_slot[slot].content[neo_slot[slot].content_length] = '\0';  // 保证有结尾

    switch (version) {  // 这个里面，version、name和content、content_length变量已经设置，但valid还没有设置
        case 1:
            j = neo_exec_v1_init(slot);
            break;
        case 2:
            // TODO: @张烨实现version2解码 j = neo_exec_v2_init(slot);
            break;
        default:
            neo_printf("invalid version number %d\n", version);
            return -10;
    }

    neo_printf("load procedure of version %d\n", version);
    if (j == 0) {
        neo_printf("procedure init success\n");
    } else { neo_printf("procedure init failed with exit code %d\n", j); return j; }

    neo_slot[slot].valid = 1;
    return 0;
}

int neo_exec_load(unsigned char* payload, int length) {
    int slot;
    slot = find_valid_slot();
    if (slot < 0) {
        neo_printf("no more slot to put\n"); return -5;
    }
    return neo_exec_set(payload, length, slot);  // 直接设置这个slot
}

void neo_exec_draw(int timeintv) {
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

void neo_exec_frame_dump(struct neo_color *frame) {
    int i=0;
    for (i=0; i<NEO_N; ++i) {
        neo_printf("%d: %d %d %d\n", i, frame[i].r, frame[i].g, frame[i].b);
    }
}

void neo_info(char* buf) {
    int i=0;
    for (i=0; i<NEO_SLOT; ++i) {
        if (neo_slot[i].valid) {
            buf += sprintf(buf, "%d %d %s\n", (int)neo_slot[i].valid, neo_slot[i].version, neo_slot[i].name);
        } else buf += sprintf(buf, "0\n");
    }
}

void neo_exec_delete(int slot) {
    if (slot >= 0 && slot < NEO_SLOT) {
        neo_slot[slot].valid = 0;
    }
}
