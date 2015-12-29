#include "tetris.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>#else#include <stdint.h>
#endif

#ifdef _WIN32
inline double round(double x)
{    return (floor(x + 0.5));
}
#endif

struct block_data_def {
    int defs[4];
};

struct block_data_def block_def[] = {
		{ 02, 12, 22, 32 },
		{ 11, 12, 21, 22 },
		{ 11, 12, 22, 23 },
		{ 12, 13, 21, 22 },
		{ 12, 21, 22, 23 },
		{ 02, 12, 22, 23 },
		{ 02, 12, 21, 22 }
};

int test_hit(LPTETRIS t)
{
    struct block b = t->block;
    for (uint8_t i = 0; i < 5; i++)
    for (uint8_t j = 0; j < 5; j++)  {
        if (b.data[i][j] && (i+b.y >= t->high || j+b.x >= t->width
                             || i+b.y < 0 || j+b.x < 0
                             || t->game[i+b.y][j+b.x])) {
            return 0;
        }
    }
    return 1;
}

int test_gameover(LPTETRIS t)
{    for (int i = 0; i < 5; i++) {        for (int j = 0; j < 5; j++) {            if (t->block.data[i][j]) {                t->gameover_flag = i + t->block.y < 1;                return t->gameover_flag;            }        }    }    return 0;
}

void update_block_data(struct block *bl){
    struct block_data_def data = block_def[bl->type];
    for (uint8_t i = 0; i < 5; i++)
    for (uint8_t j = 0; j < 5; j++)
        bl->data[i][j] = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        int x1 = data.defs[i] % 10 - 2;
        int y1 = 2 - data.defs[i] / 10;
        if (!x1 && !y1) {
            bl->data[x1+2][2-y1] = 1;
            continue;
        }

        double d = sqrt((double)(x1*x1 + y1*y1));
        double a = asin(x1 / d);
        if (y1 < 0) {
			if (a) a = (fabs(a) / a) * PI - a;
			else a = PI;
        }
        double b = a + bl->direction * PI;

        int v1 = (int)round(d * sin(b)) + 2;
        int u1 = 2 - (int)round(d * cos(b));

        bl->data[u1][v1] = 1;
    }
}

void tetris_init(LPTETRIS t, int w, int h, int l)
{
    t->width = w;
    t->high = h;
    t->level = l;
    t->score = 0;
    t->gameover_flag = 0;

    t->game = (uchar **)malloc(sizeof(uchar *) * h);
    for (int i = 0; i < h; i++) {
        t->game[i] = (uchar *)malloc(sizeof(uchar) * w);
        for (int j = 0; j < w; j++) {
            t->game[i][j] = 0;
        }
    }
}

void tetris_clean(LPTETRIS t)
{    for (int i = 0; i < t->high; i++) {        free(t->game[i]);    }    free(t->game);
}

void clear_screen(int, int h)
{
#ifdef _WIN32
	HANDLE							hStdOut;
	//CONSOLE_SCREEN_BUFFER_INFO		csbi;
	//DWORD							count;
	//DWORD							cellCount;
	COORD							homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/*if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;*/

	SetConsoleCursorPosition(hStdOut, homeCoords);
#else
	for (int i = 0; i < h + 3; i++) {
		printf("\033[F\033[J");
	}
#endif
}

void tetris_print(LPTETRIS t)
{
    clear_screen(t->width, t->high);

    printf("[LEVEL: %d | SCORE: %d]\n", t->level, t->score);
    for (int i = 0; i < t->width + 2; i++) {
        printf("=");
    }
    printf("\n");

    struct block b = t->block;

    for (int i = 0; i < t->high; i++) {
        printf("|");
        for (int j = 0; j < t->width; j++) {
            int bx = j - b.x;
            int by = i - b.y;
            if (bx >= 0 && bx < 5 && by >= 0 && by < 5 && b.data[by][bx]) {
                printf("X");
                continue;
            }
            if (t->game[i][j]) printf("X");
            else printf(" ");
        }
        printf("|\n");
    }

    for (int i = 0; i < t->width + 2; i++) {
        printf("=");
    }
    printf("\n");
    fflush(stdout);
}

void tetris_new_block(LPTETRIS t)
{
    memset(&t->block, 0, sizeof(struct block));
    t->block.type = (int)(rand() % 7);
    t->block.direction = 0.0f;
    t->block.x = t->width / 2;
    t->block.y = -1;
    update_block_data(&t->block);
}

void tetris_rotate_block(LPTETRIS t)
{
    if (t->block.type == TYPE_SQUARE)
        return;

    t->block.direction += 0.5f;    if (t->block.direction > 1.5f) {        t->block.direction = 0;    }    update_block_data(&t->block);

    if (!test_hit(t)) {
        t->block.direction -= 0.5f;
        if (t->block.direction < 0) {            t->block.direction = 1.5f;        }        update_block_data(&t->block);
	}
}

void tetris_block_down(LPTETRIS t)
{
    t->block.y += 1;
    int x = t->block.x;
    int y = t->block.y;
    if (test_hit(t)) return;    else if (test_gameover(t)) return;

    y -= 1;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++)  {
            if (t->block.data[i][j] == 1)
                t->game[i + y][j + x] = 1;
        }
    }
    tetris_new_block(t);
}

void tetris_block_lr(LPTETRIS t, int d)
{
    t->block.x += d;
    if (!test_hit(t)) t->block.x -= d;
}

void del_row(LPTETRIS t, int rowid)
{
    for (int i = rowid; i > 0; i--)
        for (int j = 0; j < t->width; j++)
            t->game[i][j] = t->game[i-1][j];
    for (int j = 0; j < t->width; j++)
        t->game[0][j] = 0;
}

void tetris_check_lines(LPTETRIS t)
{
    int num_line_del = 0;
    int i = t->high - 1;
    while (i > 0) {
        int ok = 1;
        for (int j = 0; j < t->width; j++)
            if (t->game[i][j] == 0) {
                ok = 0;
                break;
            }
        if (ok) {
            num_line_del++;
            del_row(t, i);
        }
        else --i;
    }
    t->score += num_line_del * num_line_del;
}
