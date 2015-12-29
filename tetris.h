#ifndef TETRIS_H
#define TETRIS_H

#define DIRECTION_RIGHT_90  0.5f
#define DIRECTION_LEFT_90   1.5f
#define DIRECTION_180       1.0f
#define DIRECTION_NORMAL    0.0f

#define TYPE_LINE_4     0
#define TYPE_SQUARE     1
#define TYPE_TRIANGLE   2
#define TYPE_L1         3
#define TYPE_L2         4
#define TYPE_H1         5
#define TYPE_H2         6

#define PI 3.14159265358979323846

typedef unsigned char uchar;

struct block {
    int x;
    int y;
    int type;
    float direction;
    uchar data[5][5];
};

typedef struct tetris {
    struct block block;
    uchar **game;
    int width;
    int high;
    int score;
    int level;
    int gameover_flag;
} TETRIS, *LPTETRIS;

void tetris_init(LPTETRIS t, int w, int h, int l);
void tetris_clean(LPTETRIS t);
void tetris_print(LPTETRIS t);
void tetris_new_block(LPTETRIS t);
void tetris_rotate_block(LPTETRIS t);
void tetris_block_down(LPTETRIS t);
void tetris_block_lr(LPTETRIS t, int d);
void tetris_check_lines(LPTETRIS t);

#endif // TETRIS_H
