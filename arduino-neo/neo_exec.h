#ifdef __cplusplus
extern "C" {
#endif

/* general */
#include "stdio.h"
#include "string.h"
#define MAX_NEO_STR_LENGTH 2048
#define NEO_N 300  // WS2812灯珠的个数，这个是编译时必需确定的
#define NEO_SLOT 5  // 最多存储的procedure个数
#define NEO_PRINTF 1
#define NEO_NAME_LENGTH 32

#if NEO_PRINTF
#define neo_printf(...) printf(__VA_ARGS__)
#else
#define neo_printf(...) 
#endif

/* version 1 */
struct neo_var_v1 {
    int mask;
    int mask_start;
    int mask_end;
    int repeat;
    char *head;
    char *orihead;
    int sleep;
    int tosleep;
    char *nexthead;
};
extern int neo_exec_v1_init(int slot);
extern int neo_exec_v1_draw(int slot, int timeintv);

/* general */

union neo_var_union {
    struct neo_var_v1 v1;
};

struct neo_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct neo_color_uint {
    unsigned int r;
    unsigned int g;
    unsigned int b;
};

struct neo_var {
    unsigned char valid;
    int version;
    char name[NEO_NAME_LENGTH];
    char content[MAX_NEO_STR_LENGTH];  // 记录原本的内容信息，不包括version那一行
    int content_length;  // 字符串的长度，MQTT会告知这一个内容
    struct neo_color frame1[NEO_N];
    struct neo_color frame2[NEO_N];
    union neo_var_union var;  // 每个版本维护自己的一套实现变量
};

extern struct neo_color frame[NEO_N];
extern void neo_exec_init();
extern struct neo_var neo_slot[NEO_SLOT];
int neo_exec_load(unsigned char* payload, int length);
int neo_exec_set(unsigned char* payload, int length, int slot);  // 制定slot number
extern void neo_exec_draw(int timeintv);
extern void neo_exec_frame_dump();
extern void frame_clear(struct neo_color *frame);
extern void neo_info(char* buf);
extern void neo_exec_delete(int slot);

#ifdef __cplusplus
}
#endif

