#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <cstdlib> 
#include <windows.h>
#include <string>

using namespace std;

string str_line_arr[10]{};

void print_str_line();

int main()
{
	ifstream in{ "data.txt" };
	if (!in) {
		cout << "파일을 찾을 수 없습니다." << endl;
	}

	for (int j = 0; j < 10; ++j) {
		getline(in, str_line_arr[j]);
	}

	in.close();
	

	bool check_c{ false };
	bool check_d{ false };
	bool check_e{ false };
	bool check_f{ false };
	bool check_g{ false };
	int check_r{ 0 };

	print_str_line();

	string input{};
	string word{};

	while (true) {
		cout << "명령어를 입력하세요 : ";
		cin >> input;
		system("cls");

		if (input == "c") {
			// 대문자로 시작하는 단어 색깔 바꿔 출력하기
			if (!check_c) {
				int cnt{};

				for (int i = 0; i < 10; ++i) {
					istringstream iss(str_line_arr[i]);
					while (iss >> word) {
						if ("A" <= word && word <= "Z") {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
							cout << word << " ";
							++cnt;
						}
						else {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
							cout << word << " ";
						}
					}
					cout << endl;
				}

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << "대문자인 단어의 갯수 : " << cnt << "개" << endl;
				check_c = true;
			}

			else if (check_c) {
				print_str_line();
				check_c = false;
			}
		}

		else if (input == "d") {
			if (!check_d) {
				for (int i = 0; i < 10; ++i) {
					for (int j = str_line_arr[i].length(); j >= 0; --j) {
						cout << str_line_arr[i][j];
					}
					cout << endl;
				}

				check_d = true;
			}
			else if (check_d) {
				print_str_line();
				check_d = false;
			}
		}

		else if (input == "e") {
			if (!check_e) {
				for (int i = 0; i < 10; ++i) {
					int cnt{};
					for (int j = 0; j < str_line_arr[i].length(); ++j) {
						cout << str_line_arr[i][j];
						++cnt;
						if (cnt % 3 == 0) cout << "@@";
					}
					cout << endl;
				}

				check_e = true;
			}
			else if (check_e) {
				print_str_line();
				check_e = false;
			}
		}

		else if (input == "f") {
			if (!check_f) {
				for (int i = 0; i < 10; ++i) {
					istringstream iss(str_line_arr[i]);
					while (iss >> word) {
						for (int j = word.length(); j >= 0; --j) {
							cout << word[j];
						}
						cout << " ";
					}
					cout << endl;
				}

				check_f = true;
			}
			else if (check_f) {
				print_str_line();
				check_f = false;
			}
		}

		else if (input == "g") {
			if (!check_g) {
				char org{};
				char change{};
				cout << "어떤 문자를 바꿀까요? : ";
				cin >> org;
				cout << "뭘로 바꿀까요? : ";
				cin >> change;

				for (int i = 0; i < 10; ++i) {
					for (int j = 0; j < str_line_arr[i].length(); ++j) {
						if (str_line_arr[i][j] == org) cout << change;
						else cout << str_line_arr[i][j];
					}
					cout << endl;
				}

				check_g = true;
			}
			else if (check_g) {
				print_str_line();
				check_g = false;
			}
		}

		else if (input == "h") {
			int word_num{};

			for (int i = 0; i < 10; ++i) {
				istringstream iss(str_line_arr[i]);
				while (iss >> word) {
					++word_num;
					cout << word << " ";
				}
				cout << '\t' << "단어의 개수는 : " << word_num << " 개";
				word_num = 0;
				cout << endl;
			}
		}

		else if (input == "r") {
			int word_num[10]{};
			string temp_str_line_arr[10];

			for (int i = 0; i < 10; ++i) {
				temp_str_line_arr[i] = str_line_arr[i];

				istringstream iss(temp_str_line_arr[i]);
				while (iss >> word) {
					++word_num[i];
				}
			}

			if (check_r % 3 == 0) {
				for (int i = 0; i < 10 - 1; ++i) {
					for (int j = 0; j < 10 - i - 1; ++j) {
						if (word_num[j] > word_num[j + 1]) {
							int temp{};
							string temps{};

							temp = word_num[j];
							word_num[j] = word_num[j + 1];
							word_num[j + 1] = temp;

							temps = temp_str_line_arr[j];
							temp_str_line_arr[j] = temp_str_line_arr[j + 1];
							temp_str_line_arr[j + 1] = temps;
						}
					}
				}
			}

			else if (check_r % 3 == 1) {
				for (int i = 0; i < 10 - 1; ++i) {
					for (int j = 0; j < 10 - i - 1; ++j) {
						if (word_num[j] < word_num[j + 1]) {
							int temp{};
							string temps{};

							temp = word_num[j];
							word_num[j] = word_num[j + 1];
							word_num[j + 1] = temp;

							temps = temp_str_line_arr[j];
							temp_str_line_arr[j] = temp_str_line_arr[j + 1];
							temp_str_line_arr[j + 1] = temps;
						}
					}
				}
			}

			else if (check_r % 3 == 2) {
				print_str_line();
				++check_r;
				continue;
			}

			for (int i = 0; i < 10; ++i) {
				cout << temp_str_line_arr[i] << '\t';
				cout << "단어의 개수는 : " << word_num[i] << " 개" << endl;
			}
			++check_r;
		}

		else if (input == "s") {
			int cnt{};
			string str{};
			cout << "찾을 단어를 입력하세요 : ";
			cin >> str;

			for (int i = 0; i < 10; ++i) {
				istringstream iss(str_line_arr[i]);
				bool checkupper{ false };

				while (iss >> word) {
					if (str == word) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						cout << word << " ";
						++cnt;
					}
					else {
						for (int j = 0; j < word.length(); ++j) {
							if ('A' <= word[j] && word[j] <= 'Z') {
								word[j] = tolower(word[j]);
								if (str == word) {
									word[j] = toupper(word[j]);
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
									cout << word << " ";
									++cnt;
									checkupper = true;
								}
							}
						}

						if(checkupper) checkupper = false;
						else if (!checkupper) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
							cout << word << " ";
						}
					}
				}
				cout << endl;
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			cout << str;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
			cout << "의 갯수 : " << cnt << "개" << endl;
		}

		else if (input == "q") {
			print_str_line();
			exit(true);
		}

		else {
			print_str_line();
			continue;
		}
	}
	
	return 0;
}

void print_str_line() {
	for (int i = 0; i < 10; ++i) {
		cout << str_line_arr[i] << endl;
	}
}