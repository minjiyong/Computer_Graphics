#include <iostream>
#include <cstdlib> 
#include <cmath>
#include <windows.h>
#include <string>

using namespace std;

#define DEL -1000

class Vertex {
public:
	int x{ DEL };
	int y{ DEL };
	int z{ DEL };
	double dist{ DEL };

	void print() {
		if (x == DEL) cout << "-";
		else cout << x;
		cout << " ";
		if (y == DEL) cout << "-";
		else cout << y;
		cout << " ";
		if (z == DEL) cout << "-";
		else cout << z;
		cout << '\t';
		if (dist == DEL) cout << "-";
		else cout << dist;
	}

	void insert(int _x, int _y, int _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	void del() {
		x = DEL;
		y = DEL;
		z = DEL;
	}

	void cal_dist() {
		dist = sqrt(x * x + y * y + z * z);
	}
};

void print_list();
void print_temp_list();
int check_howmany_list();
void cal_dist();

Vertex v_list[20]{};
Vertex temp_v_list[20]{};

int main()
{	
	string input{};
	string next{};

	bool check_a{ false };
	bool check_s{ false };

	while (true) {
		if (check_a || check_s) print_temp_list();
		else print_list();

		cout << "명령어를 입력하세요 : ";
		cin >> input;

		if (input == "+") {
			int x{};
			int y{};
			int z{};
			cout << "값을 입력하세요 : ";
			cin >> x >> y >> z;

			int list_num{};
			list_num = check_howmany_list();

			for (int i = list_num; i >= 0; --i) {
				v_list[i + 1] = v_list[i];
			}

			v_list[0].insert(x, y, z);
		}

		else if (input == "-") {
			int list_num{};
			list_num = check_howmany_list();

			for (int i = 0; i < list_num; ++i) {
				v_list[i] = v_list[i + 1];
			}

			v_list[list_num].del();
		}

		else if (input == "e") {
			int list_num{};
			list_num = check_howmany_list();

			int x{};
			int y{};
			int z{};
			cout << "값을 입력하세요 : ";
			cin >> x >> y >> z;

			v_list[list_num].insert(x, y, z);
		}

		else if (input == "d") {
			int list_num{};
			list_num = check_howmany_list();

			v_list[list_num - 1].del();
		}

		else if (input == "l") {
			int list_num{};
			list_num = check_howmany_list();
			cout << "리스트에 저장된 점의 개수는 : " << list_num << "개" << endl;
			cout << "다음으로 넘어가려면 아무 값이나 입력하십시오.";
			cin >> next;
		}

		else if (input == "c") {
			for (int i = 0; i < 20; ++i) {
				v_list[i].del();
			}
		}

		else if (input == "m") {
			cal_dist();
			int list_num{};
			list_num = check_howmany_list();

			for (int i = 0; i < 20; ++i) {
				temp_v_list[i] = v_list[i];
			}

			for (int i = 0; i < list_num - 1; ++i) {
				for (int j = 0; j < list_num - i - 1; ++j) {
					if (temp_v_list[j].dist < temp_v_list[j + 1].dist) {
						Vertex temp{};
						temp = temp_v_list[j];
						temp_v_list[j] = temp_v_list[j + 1];
						temp_v_list[j + 1] = temp;
					}
				}
			}

			cout << "원점에서 가장 먼 거리에 있는 점의 좌표값은 : (" << temp_v_list[0].x << ", " << temp_v_list[0].y << ", " << temp_v_list[0].z << ") 이고, 거리는 " << temp_v_list[0].dist << "입니다." << endl;
			cout << "다음으로 넘어가려면 아무 값이나 입력하십시오.";
			cin >> next;
		}

		else if (input == "n") {
			cal_dist();
			int list_num{};
			list_num = check_howmany_list();

			for (int i = 0; i < 20; ++i) {
				temp_v_list[i] = v_list[i];
			}

			for (int i = 0; i < list_num - 1; ++i) {
				for (int j = 0; j < list_num - i - 1; ++j) {
					if (temp_v_list[j].dist > temp_v_list[j + 1].dist) {
						Vertex temp{};
						temp = temp_v_list[j];
						temp_v_list[j] = temp_v_list[j + 1];
						temp_v_list[j + 1] = temp;
					}
				}
			}

			cout << "원점에서 가장 가까운 거리에 있는 점의 좌표값은 : (" << temp_v_list[0].x << ", " << temp_v_list[0].y << ", " << temp_v_list[0].z << ") 이고, 거리는 " << temp_v_list[0].dist << "입니다." << endl;
			cout << "다음으로 넘어가려면 아무 값이나 입력하십시오.";
			cin >> next;
		}

		else if (input == "a") {
			if (!check_a) {
				cal_dist();

				int list_num{};
				list_num = check_howmany_list();

				for (int i = 0; i < 20; ++i) {
					temp_v_list[i] = v_list[i];
				}

				for (int i = 0; i < list_num - 1; ++i) {
					for (int j = 0; j < list_num - i - 1; ++j) {
						if (temp_v_list[j].dist > temp_v_list[j + 1].dist) {
							Vertex temp{};
							temp = temp_v_list[j];
							temp_v_list[j] = temp_v_list[j + 1];
							temp_v_list[j + 1] = temp;
						}
					}
				}

				check_a = true;
			}
			else if (check_a) check_a = false;
		}

		else if (input == "s") {
			if (!check_s) {
				cal_dist();

				int list_num{};
				list_num = check_howmany_list();

				for (int i = 0; i < 20; ++i) {
					temp_v_list[i] = v_list[i];
				}

				for (int i = 0; i < list_num - 1; ++i) {
					for (int j = 0; j < list_num - i - 1; ++j) {
						if (temp_v_list[j].dist < temp_v_list[j + 1].dist) {
							Vertex temp{};
							temp = temp_v_list[j];
							temp_v_list[j] = temp_v_list[j + 1];
							temp_v_list[j + 1] = temp;
						}
					}
				}

				check_s = true;
			}
			else if (check_s) check_s = false;
		}

		else if (input == "q") {
			exit(true);
		}

		else continue;
	}

	return 0;
}

void print_list() {
	system("cls");
	for (int i = 0; i < 20; ++i) {
		cout << i << '\t';
		v_list[i].print();
		cout << endl;
	}
}

void print_temp_list() {
	system("cls");
	for (int i = 0; i < 20; ++i) {
		cout << i << '\t';
		temp_v_list[i].print();
		cout << endl;
	}
}

int check_howmany_list() {
	int cnt{};
	for (int i = 0; i < 20; ++i) {
		if (v_list[i].x == DEL || v_list[i].y == DEL || v_list[i].z == DEL) break;
		++cnt;
	}
	return cnt;
}

void cal_dist() {
	for (int i = 0; i < 20; ++i) {
		if (v_list[i].x == DEL || v_list[i].y == DEL || v_list[i].z == DEL) break;
		v_list[i].cal_dist();
	}
}
