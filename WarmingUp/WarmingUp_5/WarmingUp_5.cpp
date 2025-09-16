#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

char board[30][30]{};

void initialize_board();
void set_board(int x1, int y1, int x2, int y2);
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
	char input{};

	cout << "�׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
	cin >> x1 >> y1 >> x2 >> y2;

	set_board(x1, y1, x2, y2);

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

		else if (input == 'i') {
			if (x1 >= x2) continue;
			--x2;
		}

		else if (input == 'I') {
			if (x2 >= 29) continue;
			++x2;
		}

		else if (input == 'j') {
			if (y1 >= y2) continue;
			--y2;
		}

		else if (input == 'J') {
			if (y2 >= 29) continue;
			++y2;
		}

		else if (input == 'A') {
			if (y2 >= 29) continue;
			if (x1 >= x2) continue;
			++y2;
			--x2;
		}

		else if (input == 'a') {
			if (y1 >= y2) continue;
			if (x2 >= 29) continue;
			--y2;
			++x2;
		}

		else if (input == 'm') {
			int num{};
			string next{};
			num = calculate_board();
			cout << "�簢���� ������ " << num << " �Դϴ�." << endl;
			cout << "�������� �Ѿ���� �ƹ� ���̳� �Է��Ͻʽÿ� : ";
			cin >> next;
		}

		else if (input == 'n') {
			int num{};
			double res{};
			string next{};
			num = calculate_board();
			res = num / 9;
			cout << "�簢���� ���� ������ " << res << "% �Դϴ�." << endl;
			cout << "�������� �Ѿ���� �ƹ� ���̳� �Է��Ͻʽÿ� : ";
			cin >> next;
		}

		else if (input == 'r') {
			initialize_board();
			cout << "�׸��� ũ�⸦ �Է����ּ���. (0 ~ 29) : ";
			cin >> x1 >> y1 >> x2 >> y2;
		}

		else if (input == 'q') {
			exit(true);
		}

		else continue;
	
		set_board(x1, y1, x2, y2);
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

void set_board(int x1, int y1, int x2, int y2) {
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 30; ++i) {
			// x, y �� ���̿� �ִ� ������ ���� 0���� �ٲٱ�
			if (x1 <= i && i <= x2 && y1 <= j && j <= y2) board[j][i] = '0';
			// �ƴϸ� .���� �ٲٱ�
			else board[j][i] = '.';

			// ���� �̵� �� x1�� x2���� Ŀ���� ��쿡 ���� ó��
			if (x1 > x2) {
				if (0 <= i && i <= x2 && y1 <= j && j <= y2) board[j][i] = '0';
				if (x1 <= i && i <= 29 && y1 <= j && j <= y2) board[j][i] = '0';
			}

			// �� �̵� �� y1�� y2���� Ŀ���� ��쿡 ���� ó��
			if (y1 > y2) {
				if (x1 <= i && i <= x2 && 0 <= j && j <= y2) board[j][i] = '0';
				if (x1 <= i && i <= x2 && y1 <= j && j <= 29) board[j][i] = '0';
			}
			
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
			cout << board[j][i] << " ";
		}
		cout << endl;
	}
}