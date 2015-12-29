#include "tetris.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <curses.h>
#include <ncurses.h>
#include <termios.h>
#endif

#define GAME_WIDTH 16
#define GAME_HEIGHT 16

#ifndef _WIN32
struct termios save;
void cleanup_io() {
    tcsetattr(fileno(stdin), TCSANOW, &save);
}

void signal_quit(int) {
    cleanup_io();
}

void set_ioconfig() {
    struct termios custom;
    int fd = fileno(stdin);
    tcgetattr(fd, &save);
    custom = save;
    custom.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(fd,TCSANOW, &custom);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}
#endif

void my_sleep(unsigned long ms)
{
#ifdef _WIN32    Sleep(ms);
#else    timespec tm;
    tm.tv_sec = ms/1000;
    tm.tv_nsec = 1000000 * (ms % 1000);    nanosleep(&tm, 0);
#endif
}

void on_keycontrol_pressed(LPTETRIS t, char cmd)
{    switch (cmd) {
	case ' ':
		tetris_rotate_block(t);
		break;
	case 'a':
		tetris_block_lr(t, -1);
		break;
	case 'd':
		tetris_block_lr(t, 1);
		break;
	case 's':
		tetris_block_down(t);
		break;
	default:
		break;
	}
}

int main(void)
{    srand((unsigned int)time(NULL));
    int level = 2, count = 0;

    printf("Select difficult level [1-2]: ");
#ifdef _WIN32
	scanf_s("%d", &level);
#else
	scanf("%d", &level);
#endif

    if (level <= 0 || level > 2) {
        level = 1;
    }

#ifdef _WIN32
	system("cls");
#else
	printf("\n\n\n");
#endif

    TETRIS t;
#ifndef _WIN32
    set_ioconfig();
#endif
    tetris_init(&t, GAME_WIDTH, GAME_HEIGHT, level);
    tetris_new_block(&t);

    while (!t.gameover_flag)
    {
        my_sleep(200/level);
#ifdef _WIN32
		while (_kbhit()) {
			on_keycontrol_pressed(&t, _getch());
			Sleep(10);
		}
#else
		char cmd = 0;
		while ((cmd = getchar()) > 0) {
			on_keycontrol_pressed(&t, cmd);
		}
#endif
        if (++count % 5 == 0) {
            tetris_block_down(&t);
            tetris_check_lines(&t);
        } else {
            if (count >= 100) count = 1;
        }
        tetris_print(&t);
    }

    printf("Game over\n");
	my_sleep(2000);
    tetris_clean(&t);
    return 0;
}

