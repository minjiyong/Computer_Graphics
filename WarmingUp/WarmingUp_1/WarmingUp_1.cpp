#include <iostream>
#include <random>
#include <cstdlib> 
#include <string>

#define x_max 4
#define y_max 4

using namespace std;

int matrix[y_max][x_max]{ 0 };
int matrix2[y_max][x_max]{ 0 };

int res_matrix[y_max][x_max]{ 0 };


void initialize_matrix();
void del_res_matrix();

void add_matrix();
void sub_matrix();
void mult_matrix();
void T_matrix();

int det_3x3matrix_inmat1(int y, int x);
int det_4x4matrix_inmat1();
int det_3x3matrix_inmat2(int y, int x);
int det_4x4matrix_inmat2();

void even_or_odd(int counter);

void mult_num_to_matrix(int num);

void print_matrix();


int main()
{
	string input{};
	string next{};

	int even_or_odd_counter{ 0 };

	initialize_matrix();

	while (true) {
		if (even_or_odd_counter == 0) print_matrix();
		else if (even_or_odd_counter != 0) even_or_odd(even_or_odd_counter);

		cout << "명령어를 입력하세요 : ";
		cin >> input;
		if (input == "s") {
			initialize_matrix();
			del_res_matrix();
		}

		else if (input == "m") {
			del_res_matrix();
			mult_matrix();
		}

		else if (input == "a") {
			add_matrix();
		}

		else if (input == "d") {
			sub_matrix();
		}

		else if (input == "r") {
			int det1{};
			int det2{};
			det1 = det_4x4matrix_inmat1();
			det2 = det_4x4matrix_inmat2();
			cout << endl;
			cout << "첫 번째 행렬의 행렬식의 값 : " << det1 << endl;
			cout << "두 번째 행렬의 행렬식의 값 : " << det2 << endl;
			cout << "다음으로 넘어가려면 아무 값이나 입력하십시오 : ";
			cin >> next;
		}

		else if (input == "t") {
			T_matrix();
		}

		else if (input == "e") {
			++even_or_odd_counter;
			if (even_or_odd_counter == 3) even_or_odd_counter = 0;
		}

		else if (input >= "1" && input <= "9") {
			int num{ stoi(input) };
			mult_num_to_matrix(num);
		}

		else if (input == "q") {
			exit(true);
		}

		else continue;
	}

	return 0;
}


void initialize_matrix() {
	srand(static_cast<int>(time(NULL)));
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			switch (rand() % 2) {
			case 0:
				matrix[j][i] = 0;
				break;
			case 1:
				matrix[j][i] = 1;
				break;
			}
		}
	}

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			switch (rand() % 2) {
			case 0:
				matrix2[j][i] = 0;
				break;
			case 1:
				matrix2[j][i] = 1;
				break;
			}
		}
	}

	int temp_Y{};
	int temp_X{};
	for (int i = 0; i < 2; ++i) {
		temp_Y = rand() % 4;
		temp_X = rand() % 4;
		matrix[temp_Y][temp_X] = 2;
	}
	for (int i = 0; i < 2; ++i) {
		temp_Y = rand() % 4;
		temp_X = rand() % 4;
		matrix2[temp_Y][temp_X] = 2;
	}
}


void del_res_matrix() {
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			res_matrix[j][i] = 0;
		}
	}
}

void add_matrix() {
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			res_matrix[j][i] = matrix[j][i] + matrix2[j][i];
		}
	}
}

void sub_matrix() {
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			res_matrix[j][i] = matrix[j][i] - matrix2[j][i];
		}
	}
}

void mult_matrix() {
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			for (int k = 0; k < 4; ++k) {
				res_matrix[j][i] += matrix[j][k] * matrix2[k][i];
			}
		}
	}
}

void T_matrix() {
	int temp1[4][4]{};
	int temp2[4][4]{};

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			temp1[j][i] = matrix[j][i];
		}
	}
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			matrix[j][i] = temp1[i][j];
		}
	}

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			temp2[j][i] = matrix2[j][i];
		}
	}
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			matrix2[j][i] = temp2[i][j];
		}
	}
}


int det_3x3matrix_inmat1(int y, int x) {
	int temp[3][3]{};
	int temp1[9]{};
	int det{};
	int index{ 0 };

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			if (y == j || x == i) continue;
			temp1[index] = matrix[j][i];
			++index;
		}
	}

	index = 0;
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 3; ++i) {
			temp[j][i] = temp1[index];
			++index;
		}
	}

	det = temp[0][0] * (temp[1][1] * temp[2][2] - temp[1][2] * temp[2][1]) - temp[0][1] * (temp[1][0] * temp[2][2] - temp[1][2] * temp[2][0]) + temp[0][2] * (temp[1][0] * temp[2][1] - temp[1][1] * temp[2][0]);
	return det;
}

int det_4x4matrix_inmat1() {
	int det{};
	det = matrix[0][0] * det_3x3matrix_inmat1(0, 0) - matrix[0][1] * det_3x3matrix_inmat1(0, 1) + matrix[0][2] * det_3x3matrix_inmat1(0, 2) - matrix[0][3] * det_3x3matrix_inmat1(0, 3);
	return det;
}

int det_3x3matrix_inmat2(int y, int x) {
	int temp[3][3]{};
	int temp1[9]{};
	int det{};
	int index{ 0 };

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			if (y == j || x == i) continue;
			temp1[index] = matrix2[j][i];
			++index;
		}
	}

	index = 0;
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 3; ++i) {
			temp[j][i] = temp1[index];
			++index;
		}
	}

	det = temp[0][0] * (temp[1][1] * temp[2][2] - temp[1][2] * temp[2][1]) - temp[0][1] * (temp[1][0] * temp[2][2] - temp[1][2] * temp[2][0]) + temp[0][2] * (temp[1][0] * temp[2][1] - temp[1][1] * temp[2][0]);
	return det;
}

int det_4x4matrix_inmat2() {
	int det{};
	det = matrix2[0][0] * det_3x3matrix_inmat2(0, 0) - matrix2[0][1] * det_3x3matrix_inmat2(0, 1) + matrix2[0][2] * det_3x3matrix_inmat2(0, 2) - matrix2[0][3] * det_3x3matrix_inmat2(0, 3);
	return det;
}


void even_or_odd(int counter) {
	system("cls");

	switch (counter) {
	case 1: {
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (matrix[j][i] % 2 == 0) {
					cout << matrix[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}

		cout << endl;
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (matrix2[j][i] % 2 == 0) {
					cout << matrix2[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}

		cout << endl << "결과 행렬" << endl;
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (res_matrix[j][i] % 2 == 0) {
					cout << res_matrix[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}
		break;
	}

	case 2: {
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (matrix[j][i] % 2 != 0) {
					cout << matrix[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}

		cout << endl;
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (matrix2[j][i] % 2 != 0) {
					cout << matrix2[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}

		cout << endl << "결과 행렬" << endl;
		for (int j = 0; j < y_max; ++j) {
			for (int i = 0; i < x_max; ++i) {
				if (res_matrix[j][i] % 2 != 0) {
					cout << res_matrix[j][i] << '\t';
				}
				else cout << '\t';
			}
			cout << endl;
		}
		break;
	}

	}
}

void mult_num_to_matrix(int num) {
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			matrix[j][i] *= num;
		}
	}

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			matrix2[j][i] *= num;
		}
	}

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			res_matrix[j][i] *= num;
		}
	}
}

void print_matrix() {
	system("cls");

	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			cout << matrix[j][i] << '\t';
		}
		cout << endl;
	}

	cout << endl;
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			cout << matrix2[j][i] << '\t';
		}
		cout << endl;
	}

	cout << endl << "결과 행렬" << endl;
	for (int j = 0; j < y_max; ++j) {
		for (int i = 0; i < x_max; ++i) {
			cout << res_matrix[j][i] << '\t';
		}
		cout << endl;
	}
}