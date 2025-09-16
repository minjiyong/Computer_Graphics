#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

char board[30][30]{};

void initialize_board();
void set_board(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int calculate_board();
void print_board();

int main()
{
	initialize_board();
	print_board();

	int x1{};
	int y1{};
	int x2{};
	int y2{};

	int x3{};
	int y3{};
	int x4{};
	int y4{};
	char input{};

	cout << "ù ��° �׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
	cin >> x1 >> y1 >> x2 >> y2;
	cout << "�� ��° �׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
	cin >> x3 >> y3 >> x4 >> y4;

	set_board(x1, y1, x2, y2, x3, y3, x4, y4);

	while (true) {
		print_board();
		cout << "��ɾ �Է����ּ��� : ";
		cin >> input;

		if (input == 'x') {
			if (x1 == 0) {
				x1 = 29;
				--x2;
			}
			else if (x2 == 0) {
				x2 = 29;
				--x1;
			}
			else {
				--x1;
				--x2;
			}
		}

		else if (input == 'X') {
			if (x1 == 29) {
				x1 = 0;
				++x2;
			}
			else if (x2 == 29) {
				x2 = 0;
				++x1;
			}
			else {
				++x1;
				++x2;
			}
		}

		else if (input == 'y') {
			if (y1 == 0) {
				y1 = 29;
				--y2;
			}
			else if (y2 == 0) {
				y2 = 29;
				--y1;
			}
			else {
				--y1;
				--y2;
			}
		}

		else if (input == 'Y') {
			if (y1 == 29) {
				y1 = 0;
				++y2;
			}
			else if (y2 == 29) {
				y2 = 0;
				++y1;
			}
			else {
				++y1;
				++y2;
			}
		}

		else if (input == 's') {
			if (x1 >= x2) continue;
			if (y1 >= y2) continue;
			--x2;
			--y2;
		}

		else if (input == 'S') {
			if (x2 >= 29) continue;
			if (y2 >= 29) continue;
			++x2;
			++y2;
		}

		else if (input == 'w') {
			if (x3 == 0) {
				x3 = 29;
				--x4;
			}
			else if (x4 == 0) {
				x4 = 29;
				--x3;
			}
			else {
				--x3;
				--x4;
			}
		}

		else if (input == 'W') {
			if (x3 == 29) {
				x3 = 0;
				++x4;
			}
			else if (x4 == 29) {
				x4 = 0;
				++x3;
			}
			else {
				++x3;
				++x4;
			}
		}

		else if (input == 'a') {
			if (y3 == 0) {
				y3 = 29;
				--y4;
			}
			else if (y4 == 0) {
				y4 = 29;
				--y3;
			}
			else {
				--y3;
				--y4;
			}
		}

		else if (input == 'A') {
			if (y3 == 29) {
				y3 = 0;
				++y4;
			}
			else if (y4 == 29) {
				y4 = 0;
				++y3;
			}
			else {
				++y3;
				++y4;
			}
		}

		else if (input == 'd') {
			if (x3 >= x4) continue;
			if (y3 >= y4) continue;
			--x4;
			--y4;
		}

		else if (input == 'D') {
			if (x4 >= 29) continue;
			if (y4 >= 29) continue;
			++x4;
			++y4;
		}

		else if (input == 'r') {
			initialize_board();
			cout << "ù ��° �׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
			cin >> x1 >> y1 >> x2 >> y2;
			cout << "�� ��° �׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
			cin >> x3 >> y3 >> x4 >> y4;
		}

		else if (input == 'q') {
			exit(true);
		}

		else continue;

		set_board(x1, y1, x2, y2, x3, y3, x4, y4);
	}


	return 0;
}

void initialize_board() {
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			board[j][i] = '.';
		}
	}
}

void set_board(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			// x, y �� ���̿� �ִ� ������ ���� 0���� �ٲٱ�
			if (x1 <= i && i <= x2 && y1 <= j && j <= y2) board[j][i] = '0';
			else if (x3 <= i && i <= x4 && y3 <= j && j <= y4) board[j][i] = 'X';
			// �ƴϸ� .���� �ٲٱ�
			else board[j][i] = '.';
			
			// �簢�� 1 ���� �̵� �� x1�� x2���� Ŀ���� ��쿡 ���� ó��
			if (x1 > x2) {
				if (0 <= i && i <= x2 && y1 <= j && j <= y2) board[j][i] = '0';
				if (x1 <= i && i <= 29 && y1 <= j && j <= y2) board[j][i] = '0';
			}
			// �簢�� 1 �� �̵� �� y1�� y2���� Ŀ���� ��쿡 ���� ó��
			if (y1 > y2) {
				if (x1 <= i && i <= x2 && 0 <= j && j <= y2) board[j][i] = '0';
				if (x1 <= i && i <= x2 && y1 <= j && j <= 29) board[j][i] = '0';
			}

			// �簢�� 2 ���� �̵� �� x1�� x2���� Ŀ���� ��쿡 ���� ó��
			if (x3 > x4) {
				if (0 <= i && i <= x4 && y3 <= j && j <= y4) board[j][i] = 'X';
				if (x3 <= i && i <= 29 && y3 <= j && j <= y4) board[j][i] = 'X';
			}
			// �簢�� 2 �� �̵� �� y1�� y2���� Ŀ���� ��쿡 ���� ó��
			if (y3 > y4) {
				if (x3 <= i && i <= x4 && 0 <= j && j <= y3) board[j][i] = 'X';
				if (x3 <= i && i <= x4 && y3 <= j && j <= 29) board[j][i] = 'X';
			}

			// ��ġ�� # ����
			if (x1 <= i && i <= x2 && y1 <= j && j <= y2 && x3 <= i && i <= x4 && y3 <= j && j <= y4) board[j][i] = '#';
			// x1 > x2 && y1 > y2 ���ÿ� ���� ó�� �̱���
		}
	}
}

int calculate_board()
{
	int num{};
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if (board[j][i] == '0') ++num;
		}
	}
	return num;
}


void print_board() {
	system("cls");
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			if(board[j][i] == '#') SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			
			cout << board[j][i] << " ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		cout << endl;
	}
}