#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>

FILE*tetrisptr;

#define LEFT 75		// Keyboard Input
#define RIGHT 77
#define UP 72
#define DOWN 80
#define p 112
#define P 80
#define ESC 27

#define false 0
#define true 1

#define ACTIVE_BLOCK1 -2 // Block State
#define ACTIVE_BLOCK2 -3
#define CEILLING -1
#define EMPTY 0
#define WALL 1
#define INACTIVE_BLOCK1 2
#define INACTIVE_BLOCK2 3


#define MAIN_X 11	// Game borad Size
#define MAIN_Y 23
#define MAIN_X_ADJ 3
#define MAIN_Y_ADJ 1

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 // Status info position

int turn = 1;
int sixty = 1;

int STATUS_Y_GOAL; // Status info position

int STATUS_Y_SCORE1;
int STATUS_Y_SCORE2;
int STATUS_Y_ITEM1;
int STATUS_Y_ITEM2;

int blocks[7][4][4][4] = {
	{ { 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },
	{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 } },
	{ { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 },
	{ 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 } },
	{ { 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0 } },
	{ { 0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0 },{ 0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0 } },
	{ { 0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0 },
	{ 0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0 },{ 0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0 } }
}; // Shape of block

int b_type; // block type
int b_rotation; // block rotation
int b_type_next; //new block

int main_org[MAIN_Y][MAIN_X];
int main_cpy[MAIN_Y][MAIN_X];

int bx, by; // x,y position of moving block

int key; // keyboard input 

int speed; // Game Speed 

int score1; // Game Score
int score2;
int final_score1;
int final_score2;

int item1 = 0; // number of item
int item2 = 0;

int itemcount1 = 0;
int itemcount2 = 0;

int val1 = 0;
int val2 = 0;

int new_block_on = 0;
int crush_on = 0;



void title(void);
void reset(void);
void reset_main(void); // 
void reset_main_cpy(void);
void draw_map(void);
void draw_main(void);
void new_block(void);
void check_key(void);

int check_crush(int bx, int by, int rotation);
void drop_block(void);
void move_block(int dir);
void check_line(void);

void check_game_over(void);
void pause(void);

void skill(void);
void end(void);

void gotoxy(int x, int y) {
	COORD pos = { 2 * x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; // hide cursor

void setcursortype(CURSOR_TYPE c) { // hide cursor
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int main(void) {

	srand((unsigned)time(NULL));
	setcursortype(NOCURSOR);
	title();
	reset();

	while (1) {
		for (int i = 0; i < 5; i++) {
			check_key();
			draw_main();

			Sleep(speed);
			if (crush_on&&check_crush(bx, by + 1, b_rotation) == false) Sleep(100);
		}
		drop_block();

		check_game_over();

		if (new_block_on == 1) {
			turn *= -1;
			sixty += 1;
			draw_map();
			skill();
			new_block();
		}
	}
}

void title(void) {
	int x = 5; //
	int y = 4; //


	gotoxy(x, y + 0); printf("┌──────────────┐");
	gotoxy(x, y + 1); printf("│                            │");
	gotoxy(x, y + 2); printf("│        T E T R I S         │");
	gotoxy(x, y + 3); printf("│                            │");
	gotoxy(x, y + 4); printf("└──────────────┘");


	gotoxy(x + 1, y + 8); printf("Please Enter Any Key to Start..");


	gotoxy(x + 1, y + 11); printf("PLAYER1");
	gotoxy(x + 1, y + 13); printf("  ⓦ   : Shift");
	gotoxy(x + 1, y + 14); printf("ⓐ  ⓓ : Left / Right");
	gotoxy(x + 1, y + 15); printf("  ⓢ   : Drop");
	gotoxy(x + 1, y + 16); printf("  ⓟ   : Pause");
	gotoxy(x + 1, y + 17); printf("  ESC  : Quit");
	gotoxy(x + 1, y + 18); printf("  ⓕ   : Use Item");

	gotoxy(x + 15, y + 11); printf("PLAYER2");
	gotoxy(x + 15, y + 13); printf("  △   : Shift");
	gotoxy(x + 15, y + 14); printf("◁  ▷ : Left / Right");
	gotoxy(x + 15, y + 15); printf("  ▽   : Drop");
	gotoxy(x + 15, y + 16); printf("  ⓟ   : Pause");
	gotoxy(x + 15, y + 17); printf("  ESC  : Quit");
	gotoxy(x + 15, y + 18); printf("  ⓜ   : Use Item");

	gotoxy(x + 20, y - 1); printf(" ⊂_＼");
	gotoxy(x + 20, y + 0); printf("   ＼＼Λ＿Λ");
	gotoxy(x + 20, y + 1); printf("  ＼(‘ㅅ' ) 두둠칫");
	gotoxy(x + 20, y + 2); printf("　　/     ＼");
	gotoxy(x + 20, y + 3); printf(" 　/  　/＼＼");
	gotoxy(x + 20, y + 4); printf("　　/　  /　　＼_つ");
	gotoxy(x + 20, y + 5); printf("　 /　  / 두둠칫");
	gotoxy(x + 20, y + 6); printf("　/　  /|");
	gotoxy(x + 20, y + 7); printf("　(　   (");
	gotoxy(x + 20, y + 8); printf("　|　 ^  ＼");
	gotoxy(x + 20, y + 9); printf("　| /  ＼ )");
	gotoxy(x + 20, y + 10); printf("　||　　 ||");
	gotoxy(x + 20, y + 11); printf(" ノ )  ノ )");


	while (true) {
		if (kbhit()) break;
	}
	while (kbhit()) getch();
}

void reset(void) {

	time_t timer;
	struct tm *t;
	timer = time(NULL);
	t = localtime(&timer);

	if ((tetrisptr = fopen("tetris_log.txt", "a")) == NULL) {
		printf("Could Not Open File\n");
		return -1;
	}
	else
	{
		fprintf(tetrisptr, "\nStart Time : %4d/%2d/%2d %2d:%2d:%2d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	}
	fclose(tetrisptr);

	score1 = 0;
	score2 = 0;
	final_score1 = 0;
	final_score2 = 0;
	key = 0;
	crush_on = 0;
	turn = 1;
	sixty = 1;
	item1 = 0;
	item2 = 0;
	itemcount1 = 0;
	itemcount2 = 0;
	speed = 100;

	system("cls");
	reset_main();
	draw_map();
	draw_main();

	b_type_next = rand() % 7;
	new_block();

}

void reset_main(void) {
	int i, j;

	for (i = 0; i < MAIN_Y; i++) { // Game board initialize
		for (j = 0; j < MAIN_X; j++) {
			main_org[i][j] = 0;
			main_cpy[i][j] = 100;
		}
	}
	for (j = 1; j < MAIN_X; j++) {
		main_org[3][j] = CEILLING;
	}
	for (i = 1; i < MAIN_Y - 1; i++) {
		main_org[i][0] = WALL;
		main_org[i][MAIN_X - 1] = WALL;
	}
	for (j = 0; j < MAIN_X; j++) {
		main_org[MAIN_Y - 1][j] = WALL;
	}
}

void reset_main_cpy(void) {

	for (int i = 0; i < MAIN_Y; i++) {
		for (int j = 0; j < MAIN_X; j++) {
			main_cpy[i][j] = 100;
		}
	}
}

void draw_map(void) { // Game Status 
	int y = 3;

	if (turn == 1) {
		gotoxy(STATUS_X_ADJ + 15, y + 3); printf("+------- N O W -------+ ");
		gotoxy(STATUS_X_ADJ + 15, y + 4); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 5); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 6); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 7); printf("|   Player 1's turn   | ");
		gotoxy(STATUS_X_ADJ + 15, y + 8); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 9); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 10); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 11); printf("+---------------------+ ");
	}
	if (turn == -1) {
		gotoxy(STATUS_X_ADJ + 15, y + 3); printf("+------- N O W -------+ ");
		gotoxy(STATUS_X_ADJ + 15, y + 4); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 5); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 6); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 7); printf("|   Player 2's turn   | ");
		gotoxy(STATUS_X_ADJ + 15, y + 8); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 9); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 10); printf("|                     | ");
		gotoxy(STATUS_X_ADJ + 15, y + 11); printf("+---------------------+ ");
	}

	gotoxy(STATUS_X_ADJ, y + 2); printf("+-  N E X T  -+ ");
	gotoxy(STATUS_X_ADJ, y + 3); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 4); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 5); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 6); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 7); printf("+-------------+ ");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE1 = y + 9); printf("P1 score : %4d", score1);
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE2 = y + 10); printf("P2 score : %4d", score2);

	gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM1 = y + 12); printf("P1 item  : %4d", item1);
	gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM2 = y + 13); printf("P2 item  : %4d", item2);

	gotoxy(STATUS_X_ADJ, y + 15); printf("  □PLAYER1                   |");
	gotoxy(STATUS_X_ADJ, y + 16); printf("                              |");
	gotoxy(STATUS_X_ADJ, y + 17); printf("  ⓦ   : Shift                |");
	gotoxy(STATUS_X_ADJ, y + 18); printf("ⓐ  ⓓ : Left / Right      P  : Pause");
	gotoxy(STATUS_X_ADJ, y + 19); printf("  ⓢ   : Soft Drop        ESC : Quit");
	gotoxy(STATUS_X_ADJ, y + 20); printf("  ⓕ   : Use Item             |");
	gotoxy(STATUS_X_ADJ, y + 21); printf("                              |");

	gotoxy(STATUS_X_ADJ + 20, y + 15); printf("  ○PLAYER2");
	gotoxy(STATUS_X_ADJ + 20, y + 17); printf("  △   : Shift        ");
	gotoxy(STATUS_X_ADJ + 20, y + 18); printf("◁  ▷ : Left / Right");
	gotoxy(STATUS_X_ADJ + 20, y + 19); printf("  ▽   : Soft Drop");
	gotoxy(STATUS_X_ADJ + 20, y + 20); printf("  ⓜ   : Use Item");
}

void draw_main(void) {

	for (int j = 1; j<MAIN_X - 1; j++) {
		if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
	}
	for (int i = 0; i<MAIN_Y; i++) {
		for (int j = 0; j<MAIN_X; j++) {
			if (main_cpy[i][j] != main_org[i][j]) {

				gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);
				switch (main_org[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case CEILLING:
					printf(". ");
					break;
				case WALL:
					printf("▩");
					break;
				case ACTIVE_BLOCK1:
					printf("■");
					break;
				case INACTIVE_BLOCK1:
					printf("□");
					break;
				case ACTIVE_BLOCK2:
					printf("●");
					break;
				case INACTIVE_BLOCK2:
					printf("○");
					break;
				}
			}
		}
	}
	for (int i = 0; i<MAIN_Y; i++) {
		for (int j = 0; j<MAIN_X; j++) {
			main_cpy[i][j] = main_org[i][j];
		}
	}
}

void new_block(void) {
	int i, j;

	bx = (MAIN_X / 2) - 1;
	by = 0;
	b_type = b_type_next;
	b_type_next = rand() % 7;
	b_rotation = 0;

	new_block_on = 0;

	if (turn == 1) {
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK1;
			}
		}
		for (i = 1; i < 3; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type_next][0][i][j] == 1) {
					gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					printf("●");
				}
				else {
					gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					printf("  ");
				}
			}
		}
	}
	else if (turn == -1) {
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK2;
			}
		}
		for (i = 1; i < 3; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type_next][0][i][j] == 1) {
					gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					printf("■");
				}
				else {
					gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					printf("  ");
				}
			}
		}
	}
}

void check_key(void) {
	key = 0;

	if (kbhit()) {
		key = getch();
		if (turn == 1) {
			switch (key) {
			case 'w':
				if (check_crush(bx, by, (b_rotation + 1) % 4) == true) move_block(UP);
				else if (crush_on == 1 && check_crush(bx, by - 1, (b_rotation + 1) % 4) == true) move_block(100);
				break;
			case 'a':
				if (check_crush(bx - 1, by, b_rotation) == true) move_block(LEFT);
				break;
			case 's':
				if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
				break;
			case 'd':
				if (check_crush(bx + 1, by, b_rotation) == true) move_block(RIGHT);
				break;
			case 'f':
				if (item1 > 0 && val1 == 0 && val2 == 0) {
					item1--;
					val1 = 1;
				}
				gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM1); printf("P1 item  : %4d", item1);
				break;
			case 'm':
				if (item2 > 0 && val1 == 0 && val2 == 0) {
					item2--;
					val2 = 1;
				}
				gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM2); printf("P2 item  : %4d", item2);
				break;
			case p:
				pause();
				break;
			case ESC:
				end();
			}

		}

		else if (turn == -1) {
			if (key == 224) { // direction key
				do { key = getch(); } while (key == 224);
				switch (key) {
				case LEFT:
					if (check_crush(bx - 1, by, b_rotation) == true) move_block(LEFT);
					break;
				case RIGHT:
					if (check_crush(bx + 1, by, b_rotation) == true) move_block(RIGHT);
					break;
				case DOWN:
					if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
					break;
				case UP:
					if (check_crush(bx, by, (b_rotation + 1) % 4) == true) move_block(UP);
					else if (crush_on == 1 && check_crush(bx, by - 1, (b_rotation + 1) % 4) == true) move_block(100);
					break;
				}
			}

			else { // Not direction key
				switch (key) {
				case 'f':
					if (item1>0 && val1 == 0 && val2 == 0) {
						item1--;
						val1 = 1;
					}
					gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM1); printf("P1 item  : %4d", item1);
					break;
				case 'm':
					if (item2>0 && val1 == 0 && val2 == 0) {
						item2--;
						val2 = 1;
					}
					gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM2); printf("P2 item  : %4d", item2);
					break;
				case p:
					pause();
					break;
				case ESC:
					end();
				}
			}

		}
	}
	while (kbhit()) getch();
}


int check_crush(int bx, int by, int b_rotation) {
	for (int i = 0; i<4; i++) {
		for (int j = 0; j<4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1 && main_org[by + i][bx + j]>0) return false;
		}
	}
	return true;
};

void drop_block(void) {

	if (crush_on&&check_crush(bx, by + 1, b_rotation) == true) crush_on = 0;
	if (crush_on&&check_crush(bx, by + 1, b_rotation) == false) {
		for (int i = 0; i<MAIN_Y; i++) {
			for (int j = 0; j<MAIN_X; j++) {
				if (main_org[i][j] == ACTIVE_BLOCK1) main_org[i][j] = INACTIVE_BLOCK1;
				if (main_org[i][j] == ACTIVE_BLOCK2) main_org[i][j] = INACTIVE_BLOCK2;
			}
		}
		crush_on = 0;
		check_line();
		new_block_on = 1;
		return;
	}
	if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
	if (check_crush(bx, by + 1, b_rotation) == false) crush_on++;
}


void move_block(int dir) {
	int i, j;

	switch (dir) {
	case LEFT:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) {
					if (turn == 1) main_org[by + i][bx + j - 1] = ACTIVE_BLOCK1;
					else if (turn == -1) main_org[by + i][bx + j - 1] = ACTIVE_BLOCK2;
				}
			}
		}
		bx--;
		break;

	case RIGHT:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) {
					if (turn == 1) main_org[by + i][bx + j + 1] = ACTIVE_BLOCK1;
					else if (turn == -1) main_org[by + i][bx + j + 1] = ACTIVE_BLOCK2;
				}
			}
		}
		bx++;
		break;

	case DOWN:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) {
					if (turn == 1) main_org[by + i + 1][bx + j] = ACTIVE_BLOCK1;
					else if (turn == -1) main_org[by + i + 1][bx + j] = ACTIVE_BLOCK2;
				}
			}
		}
		by++;
		break;

	case UP:
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) {
					if (turn == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK1;
					else if (turn == -1) main_org[by + i][bx + j] = ACTIVE_BLOCK2;
				}
			}
		}
		break;

	case 100:

		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i<4; i++) {
			for (j = 0; j<4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) {
					if (turn == 1) main_org[by + i - 1][bx + j] = ACTIVE_BLOCK1;
					else if (turn == -1) main_org[by + i - 1][bx + j] = ACTIVE_BLOCK2;
				}
			}
		}
		by--;
		break;
	}
}

void check_line(void) {
	int i, j, k, l;

	int block_amount; // block num of one line

	for (i = MAIN_Y - 2; i>3;) {
		block_amount = 0;
		for (j = 1; j<MAIN_X - 1; j++) {
			if (main_org[i][j]>0) block_amount++;
		}
		if (block_amount == MAIN_X - 2) {

			if (turn == 1) final_score1 += 1;
			else if (turn == -1) final_score2 += 1;

			for (k = i; k>1; k--) {  // Block Down
				for (l = 1; l<MAIN_X - 1; l++) {
					if (main_org[k - 1][l] != CEILLING) main_org[k][l] = main_org[k - 1][l];
					if (main_org[k - 1][l] == CEILLING) main_org[k][l] = EMPTY;
				}
			}
		}
		else i--;
	}
	if (final_score1 - score1 >= 2) {
		item1++;
		itemcount1++;
	}
	if (final_score2 - score2 >= 2) {
		item2++;
		itemcount2++;
	}

	score1 = final_score1;
	score2 = final_score2;

	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE1); printf("P1 score : %4d", score1);
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE2); printf("P2 score : %4d", score2);

	gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM1); printf("P1 item  : %4d", item1);
	gotoxy(STATUS_X_ADJ, STATUS_Y_ITEM2); printf("P2 item  : %4d", item2);
}



void check_game_over(void) {

	int gameover = 0;

	int x = 5;
	int y = 5;

	for (int i = 1; i<MAIN_X - 2; i++) {
		if (main_org[3][i]>0) gameover = 1;
	}

	if (gameover == 1 || sixty>60) {  // end condition 

		gotoxy(x + 10, y + 0);  printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		gotoxy(x + 10, y + 1);  printf("▤                              ▤");
		gotoxy(x + 10, y + 2);  printf("▤   +-----------------------+  ▤");
		gotoxy(x + 10, y + 3);  printf("▤   |  G A M E  O V E R..   |  ▤");
		gotoxy(x + 10, y + 4);  printf("▤   +-----------------------+  ▤");
		gotoxy(x + 10, y + 5);  printf("▤                              ▤");
		gotoxy(x + 10, y + 6);  printf("▤    P1 SCORE: %d               ▤", score1);
		gotoxy(x + 10, y + 7);  printf("▤    P2 SCORE: %d               ▤", score2);
		gotoxy(x + 10, y + 8);  printf("▤                              ▤");
		gotoxy(x + 10, y + 9);  printf("▤                              ▤");
		gotoxy(x + 10, y + 10); printf("▤   Press any key to restart.. ▤");
		gotoxy(x + 10, y + 11); printf("▤                              ▤");
		gotoxy(x + 10, y + 12); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");

		if ((tetrisptr = fopen("tetris_log.txt", "a")) == NULL) {
			printf("Could Not Open File\n");
			return -1;
		}
		else
		{
			fprintf(tetrisptr, "P1 Score:%4d\tObtained Item:%4d\tUsed Item:%4d\n", score1, itemcount1, itemcount1-item1);
			fprintf(tetrisptr, "P2 Score:%4d\tObtained Item:%4d\tUsed Item:%4d\n", score2, itemcount2, itemcount2-item2);
		}
		fclose(tetrisptr);
		Sleep(1000);

		while (kbhit()) getch();
		key = getch();
		reset();
	}
}

void pause(void) {

	int x = 5;
	int y = 5;

	gotoxy(x + 10, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	gotoxy(x + 10, y + 1); printf("▤                              ▤");
	gotoxy(x + 10, y + 2); printf("▤  +-----------------------+   ▤");
	gotoxy(x + 10, y + 3); printf("▤  |       P A U S E       |   ▤");
	gotoxy(x + 10, y + 4); printf("▤  +-----------------------+   ▤");
	gotoxy(x + 10, y + 5); printf("▤                              ▤");
	gotoxy(x + 10, y + 6); printf("▤  Press any key to resume..   ▤");
	gotoxy(x + 10, y + 7); printf("▤                              ▤");
	gotoxy(x + 10, y + 8); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");

	getch();

	system("cls");
	reset_main_cpy();
	draw_main();
	draw_map();

	for (int i = 1; i<3; i++) {
		for (int j = 0; j<4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
				printf("■");
			}
			else {
				gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void skill(void) {

	if (val1 == 0 && val2 == 0) return 0;

	int i, j;
	char blocknum = 0;

	int x = 5;
	int y = 5;
	int seconds;

	if (val1 == 1) {
		gotoxy(x, y + 1);  printf("            +--------------------------------+");
		gotoxy(x, y + 2);  printf("            |      PLAYER1  used  item       |");
		gotoxy(x, y + 3);  printf("            +--------------------------------+");
		val1 = 0;
	}
	else if (val2 == 1) {
		gotoxy(x, y + 1);  printf("            +--------------------------------+");
		gotoxy(x, y + 2);  printf("            |      PLAYER2  used  item       |");
		gotoxy(x, y + 3);  printf("            +--------------------------------+");
		val2 = 0;
	}

	Sleep(1000);

	gotoxy(x, y - 1);  printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	gotoxy(x, y + 0);  printf("▤                                                      ▤");
	gotoxy(x, y + 1);  printf("▤          +--------------------------------+          ▤");
	gotoxy(x, y + 2);  printf("▤          |          Choose Item           |          ▤");
	gotoxy(x, y + 3);  printf("▤          +--------------------------------+          ▤");
	gotoxy(x, y + 4);  printf("▤                                                      ▤");
	gotoxy(x, y + 5);  printf("▤     Select Block                                     ▤");
	gotoxy(x, y + 6);  printf("▤                                                      ▤");
	gotoxy(x, y + 7);  printf("▤     1           2           3           4            ▤");
	gotoxy(x, y + 8);  printf("▤        ■                                    ■      ▤");
	gotoxy(x, y + 9);  printf("▤        ■          ■■         ■         ■■      ▤");
	gotoxy(x, y + 10); printf("▤        ■          ■■       ■■■       ■        ▤");
	gotoxy(x, y + 11); printf("▤        ■                                            ▤");
	gotoxy(x, y + 12); printf("▤                                                      ▤");
	gotoxy(x, y + 13); printf("▤                                                      ▤");
	gotoxy(x, y + 14); printf("▤                                                      ▤");
	gotoxy(x, y + 15); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");

	for (seconds = 5; seconds > 0; seconds--) {
		int index = 0;
		gotoxy(x, y + 13); printf("▤                Time Remaining : %d (s)                ▤", seconds);
		Sleep(1000);
		while (kbhit()) {
			blocknum = getch();
			index = 1;
		}
		if (index == 1) {
			break;
		}
	}

	switch (blocknum) {
	case '1':
		b_type_next = 1;
		break;
	case '2':
		b_type_next = 0;
		break;
	case '3':
		b_type_next = 6;
		break;
	case '4':
		b_type_next = 2;
		break;
	default:
		break;
	}

	system("cls");
	reset_main_cpy();
	draw_main();
	draw_map();
}

void end(void) {
	system("cls");

	if ((tetrisptr = fopen("tetris_log.txt", "a")) == NULL) {
		printf("Could Not Open File\n");
		return -1;
	}
	else
	{
		fprintf(tetrisptr, "Surrendered by pressing ESC. Result not recorded.\n");
	}
	fclose(tetrisptr);

	printf("\n");
	printf("   BBBBBBBBBBBBBBBBB\n");
	printf("   B::::::::::::::::B\n");
	printf("   B::::::BBBBBB:::::B\n");
	printf("   BB:::::B     B:::::B\n");
	printf("     B::::B     B:::::Byyyyyyy           yyyyyyy eeeeeeeeeeee\n");
	printf("     B::::B     B:::::B y:::::y         y:::::yee::::::::::::ee\n");
	printf("     B::::BBBBBB:::::B   y:::::y       y:::::ye::::::eeeee:::::ee\n");
	printf("     B:::::::::::::BB     y:::::y     y:::::ye::::::e     e:::::e\n");
	printf("     B::::BBBBBB:::::B     y:::::y   y:::::y e:::::::eeeee::::::e\n");
	printf("     B::::B     B:::::B     y:::::y y:::::y  e:::::::::::::::::e\n");
	printf("     B::::B     B:::::B      y:::::y:::::y   e::::::eeeeeeeeeee\n");
	printf("     B::::B     B:::::B       y:::::::::y    e:::::::e\n");
	printf("   BB:::::BBBBBB::::::B        y:::::::y     e::::::::e\n");
	printf("   B:::::::::::::::::B          y:::::y       e::::::::eeeeeeee\n");
	printf("   B::::::::::::::::B          y:::::y         ee:::::::::::::e\n");
	printf("   BBBBBBBBBBBBBBBBB          y:::::y            eeeeeeeeeeeeee\n");
	printf("                             y:::::y\n");
	printf("                            y:::::y\n");
	printf("                           y:::::y\n");
	printf("                          y:::::y\n");
	printf("                         yyyyyyy\n\n\n\n");
	Sleep(3000);
	exit(0);
}