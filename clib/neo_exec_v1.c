#include "neo_exec.h"

int neo_exec_v1_init(int slot) {
    struct neo_var_v1 *v;
    const char* str = neo_slot[slot].str;
    char *ptr, *ptr2;
    v = &(neo_slot[slot].var.v1);
    sscanf(str, "%d %d %d %d", &(v->mask), &(v->mask_start), &(v->mask_end), &(v->repeat));
    ptr = strchr(str, '\n');
    if (!ptr) { neo_printf("format error %s %d\n", __FILE__, __LINE__); return 1; }
    v->head = ptr + 1;  // 把\n空过去
    neo_printf("%s", v->head);
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

static void mixcolor(struct neo_color_uint *c, struct neo_color_uint *c1, struct neo_color_uint *c2, int mixer) {  // mixer is 0 ~ 1024
    c->r = ( (1024-mixer) * c1->r + mixer * c2->r ) / 1024;  // 都是255也不会导致溢出
    c->g = ( (1024-mixer) * c1->g + mixer * c2->g ) / 1024;
    c->b = ( (1024-mixer) * c1->b + mixer * c2->b ) / 1024;
}

static char* draw_frame_gradual(struct neo_color *frame, char* str) {  // 返回下一行的头指针
    int last_pos, this_pos, k_pos;
    char *ptr, *ptr2;
    struct neo_color_uint last_color, this_color, mix_color;
    sscanf(str, "%d %u %u %u", &last_pos, &(last_color.r), &(last_color.g), &(last_color.b));
    fset(frame + last_pos, &last_color);
    ptr = strchr(str, '\n');
    if (ptr == NULL) return NULL;  // should not reach here
    *ptr = '\0';  // 防止搜索出边界
    ptr2 = strchr(str, ';');
    while (ptr2) {
        ptr2 += 1;  // 把;跳过去
        sscanf(ptr2, "%d %u %u %u", &this_pos, &(this_color.r), &(this_color.g), &(this_color.b));
        for (k_pos=last_pos+1; k_pos<=this_pos; ++k_pos) {
            mixcolor(&mix_color, &last_color, &this_color, (k_pos-last_pos) * 1024 / (this_pos-last_pos));
            fset(frame + k_pos, &mix_color);
        }
        last_pos = this_pos;
        last_color = this_color;
        ptr2 = strchr(ptr2, ';');
    }
    *ptr = '\n';  // 恢复换行符
    return ptr+1;  // 下一行头指针
}

int neo_exec_v1_draw(int slot, int timeintv) {
    struct neo_var_v1 *v;
    char *ptr;
    v = &(neo_slot[slot].var.v1);
    if (v->head[0] == 'f') {  // frame
        if (v->head[1] == 'g') {  // gradual
            frame_clear(neo_slot[slot].frame1);  // 清空frame
            ptr = draw_frame_gradual(neo_slot[slot].frame1, v->head + 3);
        } else {
            neo_printf("unknown sleep subtype\n"); return 1;
        }
    } else if (v->head[0] == 's') {  // sleep
        if (v->head[1] == 'g') {  // gradual

        } else if (v->head[1] == 'r') {  // raw

        } else {
            neo_printf("unknown sleep subtype\n"); return 2;
        }
    } else {
        neo_printf("unknown type\n"); return 3;
    }
    return 0;
}
