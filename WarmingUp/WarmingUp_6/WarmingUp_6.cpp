#include <iostream>
#include <windows.h>
#include <string>
#include <random>

using namespace std;

#define READY -1000

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

void initialize_board();
void make_main_road();
void make_sub_road();
void make_final_road();
bool check_road();
void print_board();
void collision_player();

void make_real_road();

class Player {
public:
	int x{ READY };
	int y{ READY };
	char type{};
	int dir{};
};

char board[30][30]{};
Player player{};
Player road{};

int main()
{
	srand(static_cast<int>(time(NULL)));
	char input{};
	
	road.x = 0;
	road.y = 0;
	initialize_board();
	while (true) {
		make_real_road();
		//if (!check_road()) continue;
		print_board();
		if (board[28][29] == '0' || board[29][28] == '0') break;
		Sleep(50);
	}

	while (true) {
		print_board();
		cout << "��ɾ �Է��ϼ��� : ";
		cin >> input;

		if (input == 'p') {
			//// ���� �ߴ���..... �츸���.................
			//player.x = READY;
			//player.y = READY;
			//while (true) {
			//	make_final_road();
			//	if (check_road()) break;
			//}

			road.x = 0;
			road.y = 0;
			initialize_board();
			while (true) {
				make_real_road();
				//if (!check_road()) continue;
				print_board();
				if (board[28][29] == '0' || board[29][28] == '0') break;
				Sleep(50);
			}
		}
		
		else if (input == 'r') {
			player.x = 0;
			player.y = 0;
			player.type = '*';
		}

		else if (input == 'w') {
			if (player.y != READY && player.y <= 0) continue;
			--player.y;
			player.dir = UP;
			collision_player();
		}

		else if (input == 'a') {
			if (player.x != READY && player.x <= 0) continue;
			--player.x;
			player.dir = LEFT;
			collision_player();
		}

		else if (input == 's') {
			if (player.y >= 29) continue;
			++player.y;
			player.dir = DOWN;
			collision_player();
		}

		else if (input == 'd') {
			if (player.x >= 29) continue;
			++player.x;
			player.dir = RIGHT;
			collision_player();
		}

		else if (input == 'q') {
			exit(true);
		}

		else continue;
	}

	return 0;
}


void initialize_board() {
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			board[j][i] = '1';
		}
	}

	board[0][0] = '0';
	board[29][29] = '0';

	for (int i = 0; i < 30; ++i) {
		int tempx{};
		int tempy{};
		tempx = rand() % 30;
		tempy = rand() % 30;
		if (tempx == 0 && tempy == 0) continue;
		if (tempx == 29 && tempy == 29) continue;
		board[tempy][tempx] = 'X';
	}
}

void make_main_road()
{
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if (board[j][i] == '0') {
				switch (rand() % 2 + 2) {
				case DOWN:
					if (board[j + 1][i] == 'X') continue;
					board[j + 1][i] = '0';
					break;
				case RIGHT:
					if (board[j][i + 1] == 'X') continue;
					board[j][i + 1] = '0';
					break;
				}
			}
		}
	}
}

void make_sub_road()
{
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if (board[j][i] == '0') {
				switch (rand() % 4) {
				case UP:
					// ���� ĭ�� 0�� ��� �𼭸��� �ƴ�.
					if (board[j][i + 1] == '0') {
						// ���� ������ ó��
						if (j - 1 <= 0) continue;
						// X(��ֹ�) �� ���� ó��
						else if (board[j - 1][i] == 'X') continue;
						// ���� ����ε� ����
						for (int k = 1; k < 4; ++k) {
							board[j - k][i] = '0';
						}
					}
					break;
				case LEFT:
					// ���� ĭ�� 0�� ��� �𼭸��� �ƴ�.
					if (board[j + 1][i] == '0') {
						// ���� ������ ó��
						if (i - 1 <= 0) continue;
						// X(��ֹ�) �� ���� ó��
						else if (board[j][i - 1] == 'X') continue;
						// �������� ����ε� ����
						for (int k = 1; k < 4; ++k) {
							board[j][i - k] = '0';
						}
					}
					break;
				}
			}
		}
	}
}

void make_final_road()
{
	while (true) {
		initialize_board();
		make_main_road();

		if (!check_road()) continue;
		if (board[28][29] == '0' || board[29][28] == '0') break;
	}

	make_sub_road();
}

bool check_road()
{
	int garo{};
	int sero{};
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			// ������ üũ
			if (board[j][i] == '0' && board[j][i + 1] == '0') {
				++garo;
			}
			else if (board[j][i] == '0' && board[j][i + 1] != '0') {
				garo = 0;
			}
			
			// ���� ���ǽ����� �ϸ� ���� 5ȸ (����0�� 6��) �� �� false��ȯ, ������ 3���� ���̸� �ð��� �ʹ� ���� �ɸ�. �ּ�ó���� ������� ���� ���
			if(garo > 4) return false;
		}
	}

	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			// ������ üũ
			if (board[j][i] == '0' && board[j + 1][i] == '0') {
				++sero;
			}
			else if (board[j][i] == '0' && board[j + 1][i] != '0') {
				sero = 0;
			}

			if (sero > 4) return false;
		}
	}

	return true;
}

void print_board() {
	system("cls");
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if(board[j][i] == '0') SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
			else if(board[j][i] == 'X') SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			
			if (j == player.y && i == player.x) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				cout << player.type << " ";
			}
			else cout << board[j][i] << " ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		cout << endl;
	}
}

void collision_player()
{
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if (j == player.y && i == player.x) {
				if (board[j][i] != '0') {
					switch (player.dir) {
					case UP:
						++player.y;
						break;
					case LEFT:
						++player.x;
						break;
					case DOWN:
						--player.y;
						break;
					case RIGHT:
						--player.x;
						break;
					}
				}
			}
		}
	}

	if (player.y == 29 && player.x == 29) {
		cout << "�������� �����߽��ϴ�!" << endl;
		Sleep(5000);
		exit(true);
	}
}

void make_real_road()
{
	switch (rand() % 4) {
	case UP:
		if (road.dir == UP || road.dir == DOWN) return;
		road.dir = UP;
		for (int i = 0; i < rand() % 3 + 2; ++i) {
			if (road.y - 1 < 0) return;

			if (board[road.y - 1][road.x] == 'X') {
				road.dir = DOWN;
				return;
			}

			if (board[road.y - 1][road.x - 1] == '0' || board[road.y - 1][road.x + 1] == '0') {
				return;
			}
			--road.y;
			board[road.y][road.x] = '0';
		}
	break;

	case LEFT:
		if (road.dir == LEFT || road.dir == RIGHT) return;
		road.dir = LEFT;
		for (int i = 0; i < rand() % 3 + 2; ++i) {
			if (road.x - 1 < 0) return;

			if (board[road.y][road.x - 1] == 'X') {
				road.dir = RIGHT;
				return;
			}

			if (board[road.y - 1][road.x - 1] == '0' || board[road.y + 1][road.x - 1] == '0') {
				return;
			}

			--road.x;
			board[road.y][road.x] = '0';
		}
	break;

	case DOWN:
		if (road.dir == DOWN) return;
		road.dir = DOWN;
		for (int i = 0; i < rand() % 3 + 2; ++i) {
			if (road.y + 1 > 29) return;
			if (board[road.y + 1][road.x] == 'X') {
				road.dir = DOWN;
				return;
			}
			++road.y;
			board[road.y][road.x] = '0';
		}
	break;

	case RIGHT:
		if (road.dir == RIGHT) return;
		road.dir = RIGHT;
		for (int i = 0; i < rand() % 3 + 2; ++i) {
			if (road.x + 1 > 29) return;
			if (board[road.y][road.x + 1] == 'X') {
				road.dir = RIGHT;
				return;
			}
			++road.x;
			board[road.y][road.x] = '0';
		}
	break;
	}
}
