#include "neo_exec.h"

int neo_exec_v1_init(int slot) {
    struct neo_var_v1 *v;
    const char* str = neo_slot[slot].str;
    char *ptr, *ptr2;
    v = &(neo_slot[slot].var.v1);
    sscanf(str, "%d %d %d %d", &(v->mask), &(v->mask_start), &(v->mask_end), &(v->repeat));
    ptr = strchr(str, '\n');
    if (!ptr) { neo_printf("format error %s %d\n", __FILE__, __LINE__); return 1; }
    v->orihead = v->head = ptr + 1;  // 把\n空过去
    v->sleep = 0;  // 初始化睡眠计时
    return 0;
}

static void fset(struct neo_color *a, struct neo_color_uint *b) {
    a->r = b->r; a->g = b->g; a->b = b->b;
}

static void fadd(struct neo_color *a, struct neo_color *b, int mask) {  // mask = 128 为全加
    unsigned int R,G,B;
    R = ( b->r * 128 + mask * a->r ) / 128;
    G = ( b->g * 128 + mask * a->g ) / 128;
    B = ( b->b * 128 + mask * a->b ) / 128;
    a->r = R > 255 ? 255 : R;
    a->g = G > 255 ? 255 : G;
    a->b = B > 255 ? 255 : B;
}

static void mixcolor_uint(struct neo_color_uint *c, struct neo_color_uint *c1, struct neo_color_uint *c2, int mixer) {  // mixer is 0 ~ 1024
    c->r = ( (1024-mixer) * c1->r + mixer * c2->r ) / 1024;  // 都是255也不会导致溢出
    c->g = ( (1024-mixer) * c1->g + mixer * c2->g ) / 1024;
    c->b = ( (1024-mixer) * c1->b + mixer * c2->b ) / 1024;
}

static void mixcolor(struct neo_color *c, struct neo_color *c1, struct neo_color *c2, int mixer) {  // mixer is 0 ~ 1024
    c->r = ( (1024-mixer) * c1->r + mixer * c2->r ) / 1024;  // 都是255也不会导致溢出
    c->g = ( (1024-mixer) * c1->g + mixer * c2->g ) / 1024;
    c->b = ( (1024-mixer) * c1->b + mixer * c2->b ) / 1024;
}

static void hex2color(int hex, struct neo_color_uint *color) {
    color->r = 0x00FF & (hex >> 16);
    color->g = 0x00FF & (hex >> 8);
    color->b = 0x00FF & hex;
}

static char* draw_frame_gradual(struct neo_color *frame, char* str) {  // 返回下一行的头指针
    int last_pos, this_pos, k_pos, hex;
    char *ptr, *ptr2;
    struct neo_color_uint last_color, this_color, mix_color;
    // neo_printf("%s\n", str);
    sscanf(str, "%d %x", &last_pos, &hex);
    hex2color(hex, &last_color);
    fset(frame + last_pos, &last_color);
    ptr = strchr(str, '\n');
    if (ptr == NULL) return NULL;  // should not reach here
    *ptr = '\0';  // 防止搜索出边界
    ptr2 = strchr(str, ';');
    while (ptr2) {
        ptr2 += 1;  // 把;跳过去
        sscanf(ptr2, "%d %x", &this_pos, &hex);
        hex2color(hex, &this_color);
        for (k_pos=last_pos+1; k_pos<=this_pos; ++k_pos) {
            mixcolor_uint(&mix_color, &last_color, &this_color, (k_pos-last_pos) * 1024 / (this_pos-last_pos));
            fset(frame + k_pos, &mix_color);
        }
        last_pos = this_pos;
        last_color = this_color;
        ptr2 = strchr(ptr2, ';');
    }
    *ptr = '\n';  // 恢复换行符
    return ptr+1;  // 下一行头指针
}

static void mask_frame(struct neo_color *f1, struct neo_color *f2, int mask, int mask_start, int mask_end) {
    int i;
    for (i=0; i<NEO_N; ++i) {
        if (i >= mask_start && i <= mask_end) {
            fadd(f1+i, f2+i, mask);
        } else fadd(f1+i, f2+i, 128);  // 完整保留总的frame
    }
}

static void mask_frame_mixer(struct neo_color *f1, struct neo_color *f2, int mask, int mask_start, int mask_end, struct neo_color *f3, int mixer) { // mixer is 0 ~ 1024
    int i;
    struct neo_color c;
    for (i=0; i<NEO_N; ++i) {
        mixcolor(&c, f2+i, f3+i, mixer);
        if (i >= mask_start && i <= mask_end) {
            fadd(f1+i, &c, mask);
        } else fadd(f1+i, &c, 128);  // 完整保留总的frame
    }
}

int neo_exec_v1_draw(int slot, int timeintv) {
    struct neo_var_v1 *v;
    char *ptr;
    v = &(neo_slot[slot].var.v1);
    if (v->head[0] == 'f') {  // frame
        if (v->head[1] == 'g') {  // gradual
            frame_clear(neo_slot[slot].frame1);  // 清空frame
            ptr = draw_frame_gradual(neo_slot[slot].frame1, v->head + 3);
            mask_frame(frame, neo_slot[slot].frame1, v->mask, v->mask_start, v->mask_end);
            if (ptr == NULL) return 1;
            v->head = ptr;
        } else {
            neo_printf("unknown sleep subtype\n"); return 1;
        }
    } else if (v->head[0] == 's') {  // sleep
        if (v->head[1] == 'g') {  // gradual
            if (v->sleep == 0) {  // 初始化
                sscanf(v->head + 3, "%d", &(v->tosleep));
                ptr = strchr(v->head, '\n');
                if (ptr == NULL) return 4;
                v->nexthead = ptr+1;
                if (v->nexthead[0] == 'f' && v->nexthead[1] == 'g') {
                    frame_clear(neo_slot[slot].frame2);  // 清空frame
                    draw_frame_gradual(neo_slot[slot].frame2, v->nexthead + 3);
                } else {
                    neo_printf("cannot find proper next frame\n"); return 5;
                }
            }
            v->sleep += timeintv;
            // 使用 va.frame1 和 va.frame2 组合出来新的frame
            mask_frame_mixer(frame, neo_slot[slot].frame1, v->mask, v->mask_start, v->mask_end, neo_slot[slot].frame2, 1024 * v->sleep / v->tosleep);
            if (v->sleep + timeintv >= v->tosleep) {
                v->head = v->nexthead;
                v->sleep = 0;
            }
        } else if (v->head[1] == 'r') {  // raw
            if (v->sleep == 0) {  // 初始化
                sscanf(v->head + 3, "%d", &(v->tosleep));
            }
            v->sleep += timeintv;
            mask_frame(frame, neo_slot[slot].frame1, v->mask, v->mask_start, v->mask_end);
            if (v->sleep + timeintv >= v->tosleep) {
                ptr = strchr(v->head, '\n');
                if (ptr == NULL) return 2;
                v->head = ptr+1;
                v->sleep = 0;
            }
        } else {
            neo_printf("unknown sleep subtype\n"); return 2;
        }
    } else if (v->head[0] == '\0') {
        if (v->repeat) {
            // neo_printf("DEBUG:st repeat = %d\n", v->repeat);
            if (v->repeat > 0) --v->repeat;
            v->head = v->orihead;
            return neo_exec_v1_draw(slot, timeintv);
        } else { neo_printf("procedure done\n"); return -1; }
    } else {
        // neo_printf("%s\n", v->head);
        neo_printf("unknown type\n"); return 3;
    }
    return 0;
}
