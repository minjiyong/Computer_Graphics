#include <iostream>
#include <random>
#include <windows.h>
#include <string>

using namespace std;

class Card {
public:
	char word{};
	bool correct{false};
	int color{15};
};

Card board[5][5]{};
int coin{ 5 };
int score{ 0 };


void initialize_board();
void print_ans_board();
void print_board();
void check_correct(string ans1, string ans2);


int main()
{
	initialize_board();
	print_ans_board();

	string input{};
	string ans1{};
	string ans2{};
	
	cout << "�ƹ� Ű�� �Է��ϸ� ������ ���۵˴ϴ�...";
	cin >> input;

	while (true) {
		print_board();
		cout << "��ɾ �Է��ϼ���." << endl;
		cout << "1. ������, 2. �ٽ� ����, 3. ���� ���� : ";
		cin >> input;

		if (input == "1") {
			cout << "������ ĭ�� �Է��ϼ��� : ";
			cin >> ans1 >> ans2;

			check_correct(ans1, ans2);
			--coin;
		}

		else if (input == "2") {
			initialize_board();
			print_ans_board();
			Sleep(5000);
		}

		else if (input == "3") {
			exit(true);
		}

		else continue;

		if (coin <= 0) {
			cout << "���� ��ȸ ����!!" << endl;
			exit(true);
		}
	}

	return 0;
}


void initialize_board() {
	srand(static_cast<int>(time(NULL)));
	char temp[25]{ 'A', 'A' ,'B', 'B', 'C', 'C', 'D', 'D','E', 'E', 'F', 'F', 'G', 'G', 'H', 'H', 'I', 'I', 'J', 'J', 'K', 'K','L', 'L', '@' };
	for (int j = 0; j < 5; ++j) {
		for (int i = 0; i < 5; ++i) {
			board[j][i].word = '-';
			board[j][i].correct = false;
			board[j][i].color = 15;
		}
	}
	
	for (int i = 0; i < 25; ++i) {
		int y = rand() % 5;
		int x = rand() % 5;
		if (board[y][x].word == '-') {
			board[y][x].word = temp[i];
		}
		else --i;
	}

	score = 0;
	coin = 5;
}

void print_ans_board() {
	system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	cout << "  a b c d e" << endl;
	for (int j = 0; j < 5; ++j) {
		for (int i = 0; i < 5; ++i) {
			if (i == 0) cout << j + 1;
			cout << " " << board[j][i].word;
		}
		cout << endl;
	}
}

void print_board() {
	system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	cout << "  a b c d e" << endl;
	for (int j = 0; j < 5; ++j) {
		for (int i = 0; i < 5; ++i) {
			if (i == 0) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << j + 1;
			}

			if (board[j][i].correct) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[j][i].color);
				cout << " " << board[j][i].word;
			}
			else if (!board[j][i].correct) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << " " << "*";
			}
		}
		cout << endl;
	}

	cout << "���� Ƚ�� : " << coin << "\t���� : " << score << endl;
}

void check_correct(string ans1, string ans2) {
	int x1{};
	int y1{};
	int x2{};
	int y2{};

	x1 = static_cast<int>(ans1[0] - 'a');
	y1 = static_cast<int>(ans1[1] - '0' - 1);
	x2 = static_cast<int>(ans2[0] - 'a');
	y2 = static_cast<int>(ans2[1] - '0' - 1);

	int word_color = rand() % 6 + 1;

	board[y1][x1].correct = true;
	board[y2][x2].correct = true;
	board[y1][x1].color = word_color;
	board[y2][x2].color = word_color;

	system("cls");
	cout << "  a b c d e" << endl;
	for (int j = 0; j < 5; ++j) {
		for (int i = 0; i < 5; ++i) {
			if (i == 0) cout << j + 1;

			if (board[j][i].correct) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[j][i].color);
				cout << " " << board[j][i].word;
			}
			else if (!board[j][i].correct) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << " " << "*";
			}
		}
		cout << endl;
	}
	Sleep(1000);

	if (board[y1][x1].word == '@' || board[y2][x2].word == '@') {
		board[y1][x1].correct = true;
		board[y2][x2].correct = true;
		board[y1][x1].color = word_color;
		board[y2][x2].color = word_color;

		int x{};
		int y{};
		if (board[y1][x1].word == '@') {
			x = x2;
			y = y2;
		}
		else if (board[y2][x2].word == '@') {
			x = x1;
			y = y1;
		}
		for (int j = 0; j < 5; ++j) {
			for (int i = 0; i < 5; ++i) {
				if (board[j][i].word == board[y][x].word) {
					board[j][i].correct = true;
					board[j][i].color = word_color;
				}
			}
		}
		++score;
	}

	else if (board[y1][x1].word == board[y2][x2].word) {
		board[y1][x1].correct = true;
		board[y2][x2].correct = true;
		board[y1][x1].color = word_color;
		board[y2][x2].color	= word_color;
		++score;
	}

	else if (board[y1][x1].word != board[y2][x2].word) {
		board[y1][x1].correct = false;
		board[y2][x2].correct = false;
		board[y1][x1].color = 15;
		board[y2][x2].color = 15;
	}
}