/*
ver2�ł�UCT_IGO�̃����e�J�����ؒT���ŕ��񉻂���������
*/

//�͌�v���O�������C��
#include "UCT_IGO.h"
#include "BoardManager.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <time.h>
#include <chrono>
#include <random>
#include <fstream>
#include <stdio.h>
#include <omp.h>
//�������[���[�N�����p
#include "crtdbg.h"
#define malloc(X) _malloc_dbg(X._NORMAL_BLOCK,__FILE__,__LINE__)
#define new ::new(_NORMAL_BLOCK,__FILE__,__LINE__)
using namespace std;

random_device rnd;     // �񌈒�I�ȗ���������𐶐�
mt19937 mt(rnd());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
uniform_int_distribution<> rand10(0, 9);        // [0, 9] �͈͂̈�l����

//AI1�̑ł��������������
vector<int> MemoAI1;

//AI2�̑ł��������������
vector<int> MemoAI2;

//rand�̑ł��������������
vector<int> MemoRand;


int WHITE_WIN_COUNT = 0;
int BLACK_WIN_COUNT = 0;


//�p�X�Ȃ�0��Ԃ��p�X�ȊO�Ȃ�1��Ԃ�
int PlayerAction(UCT* uct, int* turn_color) {

	int x, y;
	while (true) {
		cout << "�Ղ̍����牽��ڂɒu������?(�p�X�Ȃ�0��I��)" << flush;
		cin >> x;
		cout << endl;
		cout << "�Ղ̏ォ�牽�s�ڂɒu������(�p�X�Ȃ�0��I��)?" << flush;
		cin >> y;
		cout << endl;
		if (x == 0 && y == 0) {
			cout << "�p�X��I������" << endl;
			*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			return 0;
		}
		if (x < 1 || 9 < x || y < 1 || 9 < y /* || uct->BM->Board_[y*uct->BM->WIDTH_ + x] != uct->BM->SPACE_*/ ) {
			cout << "�����ɂ͒u����I" << endl;
			continue;
		}
		//����
		MemoRand.push_back(y*uct->BM->WIDTH_ + x);

		if (uct->BM->PutStone(y*uct->BM->WIDTH_ + x, *turn_color)) {
			cout << y * uct->BM->WIDTH_ + x << "�ɐ΂�u���܂���" << endl;
			uct->BM->PlayerPutStone(x, y, *turn_color);
			*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			//uct->BM->ShowBoard();
			return 1;
		}
		else {
			cout << "�΂�u���܂���ł���" << endl;
			cout << "�������I��ł�������" << endl;
		}
	}
}



//�����v���C���[
//�p�X�Ȃ�0��Ԃ��p�X�ȊO�Ȃ�1��Ԃ�
int PlayerActionRand(UCT* uct, int* turn_color) {

	int x, y;
	while (true) {
		x = int(rand10(mt));
		y = int(rand10(mt));
		if (x == 0 && y == 0) {
			return 0;
		}
		if (x < 1 || 9 < x || y < 1 || 9 < y /* || uct->BM->Board_[y*uct->BM->WIDTH_ + x] != uct->BM->SPACE_*/) {
			continue;
		}
		//����
		MemoRand.push_back(y*uct->BM->WIDTH_ + x);

		if (uct->BM->PutStone(y*uct->BM->WIDTH_ + x, *turn_color)) {
			uct->BM->PlayerPutStone(x, y, *turn_color);
			*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			//uct->BM->ShowBoard();
			return 1;
		}
		else {
		}
	}
}




//AI���m�̐킢(�������̕������܂���������)
int main3() {

	//���񉻃e�X�g
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;
	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;

	for (int k = 0; k < 50; k++) {

		chrono::system_clock::time_point start, end;
		//�v���X�^�[�g
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//��s
		int turn_color = GAME.BM->BLACK_;
		//���݂��v���C���[�̃^�[���Ȃ��true;
		bool IsPlayerTurn = true;

		//���܂őł�����̐��𐔂���
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "AI1 ��" << endl;
		cout << "AI2 ��" << endl;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------���񉻂̏ꍇ���^�[���A�R���X�g���N�^�ƃf�X�g���N�^���Ăяo�����̂ŉ��P���Ȃ��Ƃ����Ȃ�
			//-------------------------------------------------------------------------------------------------------------�����̕��񉻂�section���g�����N���X�̃R�s�[���g����������������
			//AI1�̃^�[���̏ꍇ
			if (turn_color == GAME.BM->BLACK_) {
				cout << "AI1(��)�̃^�[��" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 3000);
			}
			//AI2�̃^�[���̏ꍇ
			else {
				cout << "AI2(��)�̃^�[��" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 15000);
			}


			cout << "���̎�" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "�G���[�I" << endl;
				break;
			}


			if (turn_color == GAME.BM->BLACK_) {
				GAME.BM->ShowBoard();
				MemoAI1.push_back(now_z);
			}
			else {
				GAME.BM->ShowBoard();
				MemoAI2.push_back(now_z);
			}


			//�I�ǔ���
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color, true);
				cout << "�΋ǏI��" << endl;
				if (GAME.BM->IsBlackWin_) {
					BLACK_WIN_COUNT++;
				}
				else {
					WHITE_WIN_COUNT++;
				}
				break;
			}

			before_z = now_z;

			//�F��ς���-----------------------------------------------
			turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
			//cout << "turn_color : " << turn_color << endl;


			before_z = now_z;


			/*
			char file_name_sub1[100] = "c:\\users\\ryu03\\desktop\\IGO\\Datas\\B1000_250 vs W2000_250\\BLACK1000_250 vs WHITE2000_250 ";

			int num = k;
			int save_num = num;
			int dig = 0;

			while (save_num != 0) {
			dig++;
			save_num /= 10;
			}

			char file_num[10] = "";

			//char* file_num = new char[dig];

			/*for (int i = 0; i < dig; i++) {
			file_num[i] = ((num%10) + '0');
			num /= 10;
			}*/

			/*
			dig = 0;
			while (num != 0) {
			file_num[dig] = ((num % 10) + '0');
			dig++;
			num /= 10;
			}

			char file_name_sub2[] = ".txt";


			strcat_s(file_name_sub1, file_num);
			strcat_s(file_name_sub1, file_name_sub2);
			*/
			/*
			ofstream ofs(file_name_sub1);
			ofstream outputfile(file_name_sub1);

			outputfile << "���s:" << flush;
			if (GAME.BM->IsBlackWin_) {
			outputfile << "���̏���" << endl;

			BLACK_WIN_COUNT++;
			}
			else {
			outputfile << "���̏���" << endl;;

			WHITE_WIN_COUNT++;
			}

			outputfile << "�_��:" << flush;
			outputfile << GAME.BM->game_score_ << endl;;


			cout << "AI1" << endl;
			outputfile << "AI1�̊���" << endl;
			for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI1.clear();
			cout << endl;
			cout << "AI2" << endl;
			outputfile << "AI2�̊���" << endl;
			for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI2.clear();
			cout << endl;

			end = chrono::system_clock::now();
			double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			cout << "�v������ : " << elapsed << endl;
			outputfile << "�v������" << endl;
			outputfile << elapsed << endl;


			outputfile.close();

			//delete[] file_num;
			*/
		}
	}

	cout << "�ΐ팋��" << endl;
	cout << "AI1(��) " << BLACK_WIN_COUNT << "��" << endl;
	cout << "AI2(��) " << WHITE_WIN_COUNT << "��" << endl;



	return 0;
	//�܂����̔���50��,50��
}


//AI���m�̐킢(�Е�����)
int main4() {

	//���񉻃e�X�g
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;
	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 10; k++) {

		chrono::system_clock::time_point start, end;
		//�v���X�^�[�g
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//��s
		int turn_color = GAME.BM->BLACK_;
		//���݂��v���C���[�̃^�[���Ȃ��true;
		bool IsPlayerTurn = true;

		//���܂őł�����̐��𐔂���
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "AI1 ��" << endl;
		cout << "AI2 ��" << endl;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------���񉻂̏ꍇ���^�[���A�R���X�g���N�^�ƃf�X�g���N�^���Ăяo�����̂ŉ��P���Ȃ��Ƃ����Ȃ�
			//-------------------------------------------------------------------------------------------------------------�����̕��񉻂�section���g�����N���X�̃R�s�[���g����������������
			
			//AI1�̃^�[���̏ꍇ
			if (turn_color == GAME.BM->BLACK_) {
				cout << "AI1(��)�̃^�[��" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 70000);
			}
			//AI2�̃^�[���̏ꍇ
			else {
				cout << "AI2(��)�̃^�[��" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME, turn_color, 12500);
			}


			cout << "���̎�" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "�G���[�I" << endl;
				ERROR_COUNT++;
				cout << "����" << endl;
				cout << "AI1(��) " << BLACK_WIN_COUNT << "��" << endl;
				cout << "AI2(��) " << WHITE_WIN_COUNT << "��" << endl;
				cout << "�G���[�� " << ERROR_COUNT << "��" << endl;
				break;
			}


			if (turn_color == GAME.BM->BLACK_) {
				GAME.BM->ShowBoard();
				MemoAI1.push_back(now_z);
			}
			else {
				GAME.BM->ShowBoard();
				MemoAI2.push_back(now_z);
			}


			//�I�ǔ���
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color, true);
				cout << "�΋ǏI��" << endl;
				if (GAME.BM->IsBlackWin_) {
					BLACK_WIN_COUNT++;
				}
				else {
					WHITE_WIN_COUNT++;
				}

				cout << "����" << endl;
				cout << "AI1(��) " << BLACK_WIN_COUNT << "��" << endl;
				cout << "AI2(��) " << WHITE_WIN_COUNT << "��" << endl;
				cout << "�G���[�� " << ERROR_COUNT << "��" << endl;

				break;
			}

			before_z = now_z;

			//�F��ς���-----------------------------------------------
			turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
			//cout << "turn_color : " << turn_color << endl;


			before_z = now_z;


			/*
			char file_name_sub1[100] = "c:\\users\\ryu03\\desktop\\IGO\\Datas\\B1000_250 vs W2000_250\\BLACK1000_250 vs WHITE2000_250 ";

			int num = k;
			int save_num = num;
			int dig = 0;

			while (save_num != 0) {
			dig++;
			save_num /= 10;
			}

			char file_num[10] = "";

			//char* file_num = new char[dig];

			/*for (int i = 0; i < dig; i++) {
			file_num[i] = ((num%10) + '0');
			num /= 10;
			}*/

			/*
			dig = 0;
			while (num != 0) {
			file_num[dig] = ((num % 10) + '0');
			dig++;
			num /= 10;
			}

			char file_name_sub2[] = ".txt";


			strcat_s(file_name_sub1, file_num);
			strcat_s(file_name_sub1, file_name_sub2);
			*/
			/*
			ofstream ofs(file_name_sub1);
			ofstream outputfile(file_name_sub1);

			outputfile << "���s:" << flush;
			if (GAME.BM->IsBlackWin_) {
			outputfile << "���̏���" << endl;

			BLACK_WIN_COUNT++;
			}
			else {
			outputfile << "���̏���" << endl;;

			WHITE_WIN_COUNT++;
			}

			outputfile << "�_��:" << flush;
			outputfile << GAME.BM->game_score_ << endl;;


			cout << "AI1" << endl;
			outputfile << "AI1�̊���" << endl;
			for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI1.clear();
			cout << endl;
			cout << "AI2" << endl;
			outputfile << "AI2�̊���" << endl;
			for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI2.clear();
			cout << endl;

			end = chrono::system_clock::now();
			double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			cout << "�v������ : " << elapsed << endl;
			outputfile << "�v������" << endl;
			outputfile << elapsed << endl;


			outputfile.close();

			//delete[] file_num;
			*/
		}
	}

	cout << "�ΐ팋��" << endl;
	cout << "AI1(��) " << BLACK_WIN_COUNT << "��" << endl;
	cout << "AI2(��) " << WHITE_WIN_COUNT << "��" << endl;
	cout << "�G���[�� " << ERROR_COUNT << "��" << endl;


	return 0;
}


int main() {

	return main4();



	//���񉻃e�X�g
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;

	for (int k = 1; k <= 1; k++) {

		chrono::system_clock::time_point start, end;
		//�v���X�^�[�g
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//��s
		int turn_color = GAME.BM->BLACK_;
		//���݂��v���C���[�̃^�[���Ȃ��true;
		bool IsPlayerTurn = true;

		//���܂őł�����̐��𐔂���
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "�v���C���[ ��" << endl;
		cout << "AI ��" << endl;



		//�R���s���[�^���m�̐킢--------------------------------------------------------------UCT�N���X��2���Ȃ��Ƒʖڂ���
		//2��A���p�X�������Α΋ǏI��
		while (true) {

			//GAME->BM->ShowBoard();

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------���񉻂̏ꍇ���^�[���A�R���X�g���N�^�ƃf�X�g���N�^���Ăяo�����̂ŉ��P���Ȃ��Ƃ����Ȃ�
			//-------------------------------------------------------------------------------------------------------------�����̕��񉻂�section���g�����N���X�̃R�s�[���g����������������
			//AI1�̃^�[���̏ꍇ
			if (turn_color == GAME.BM->BLACK_)now_z = GAME.SelectActionUCT(turn_color, 5000);
			//if (turn_color == GAME.BM->BLACK_)now_z = GAME.SelectActionUCT_Paralelle(turn_color,10000);//���񉻔�
			//AI2�̃^�[���̏ꍇ
			else {
				cout << "GAME.SelectActionUCT_Paralelle2���Ăт܂� " << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME, turn_color, 5000);
			}
			//else now_z = GAME.SelectActionUCT(turn_color, 5000);
			//else now_z = GAME.SelectActionUCT_Paralelle(turn_color, 10000);//���񉻔�
			//else now_z = GAME.SelectActionUCT(turn_color, 10000);


			cout << "���̎�" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "�G���[�I" << endl;
				break;
			}

			if (turn_color == GAME.BM->BLACK_) {
				GAME.BM->ShowBoard();
				MemoAI1.push_back(now_z);
			}
			else {
				GAME.BM->ShowBoard();
				MemoAI2.push_back(now_z);
			}

			cout << "OK" << endl;
			//GAME.BM->ShowBoard();

			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color, true);
				cout << "�΋ǏI��" << endl;
				break;
			}

			before_z = now_z;

			//�F��ς���-----------------------------------------------
			turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
			//cout << "turn_color : " << turn_color << endl;
		}


		/*
		//AI�Ɛl�Ԃ̏ꍇ
		while (true) {
			if (IsPlayerTurn) {
				cout << "�v���C���[�̃^�[��" << endl;

				now_z = PlayerActionRand(&GAME, &turn_color);
				//now_z = PlayerAction(&GAME, &turn_color);


				IsPlayerTurn = !IsPlayerTurn;
			}
			else {
				cout << "AI�̃^�[��" << endl;

				//GAME.BM->total_playout_ = 0;
				//----------------------------------------------------------------------���񉻂Ȃ���30000�𒴂���Ǝ~�܂��Ă��܂�
				//now_z = GAME.SelectActionUCT(turn_color,30500);
				//cout << (now_z/9) << now_z % 9 << "�ɒu���܂�" << endl;
				//MemoAI2.push_back(now_z);
				//cout << "���̎�" << now_z << endl;

				//----------------------------------------------------------------------------parallel2
				cout << "GAME.SelectActionUCT_Paralelle2���Ăт܂�" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME ,turn_color, 2500);

				//����
				//MemoAI1.push_back(now_z);

				IsPutOK = GAME.BM->PutStone(now_z, turn_color);
				if (IsPutOK == false) {
					GAME.BM->ShowBoard();
					cout << "�G���[�I" << endl;

					cout << "�v���C���[" << endl;
					for (auto r : MemoRand) {
						cout << r << " " << flush;
					}
					cout << endl;
					cout << "AI1" << endl;
					for (auto r : MemoAI1) {
						cout << r << " " << flush;
					}
					cout << endl;

					end = chrono::system_clock::now();
					double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

					cout << "�v������ : " << elapsed << endl;

					int n;
					cout << "������H�iYes->1�ANO->0�j" << endl;
					cin >> n;
					if (n == 1)continue;
					else break;
				}

				//cout << "OK" << endl;

				//�F��ς���-----------------------------------------------
				turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
				IsPlayerTurn = !IsPlayerTurn;
			}

			//GAME.BM->ShowBoard();

			//�I�ǔ���
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color,true);
				cout << "�΋ǏI��" << endl;
				break;
			}
			before_z = now_z;
		}
		*/

		cout << "�G���[�I" << endl;

		/*
		char file_name_sub1[100] = "c:\\users\\ryu03\\desktop\\IGO\\Datas\\B1000_250 vs W2000_250\\BLACK1000_250 vs WHITE2000_250 ";

		int num = k;
		int save_num = num;
		int dig = 0;

		while (save_num != 0) {
			dig++;
			save_num /= 10;
		}

		char file_num[10] = "";

		//char* file_num = new char[dig];

		/*for (int i = 0; i < dig; i++) {
			file_num[i] = ((num%10) + '0');
			num /= 10;
		}*/

		/*
		dig = 0;
		while (num != 0) {
			file_num[dig] = ((num % 10) + '0');
			dig++;
			num /= 10;
		}

		char file_name_sub2[] = ".txt";


		strcat_s(file_name_sub1, file_num);
		strcat_s(file_name_sub1, file_name_sub2);
		*/
		/*
		ofstream ofs(file_name_sub1);
		ofstream outputfile(file_name_sub1);

		outputfile << "���s:" << flush;
		if (GAME.BM->IsBlackWin_) {
			outputfile << "���̏���" << endl;

			BLACK_WIN_COUNT++;
		}
		else {
			outputfile << "���̏���" << endl;;

			WHITE_WIN_COUNT++;
		}

		outputfile << "�_��:" << flush;
		outputfile << GAME.BM->game_score_ << endl;;


		cout << "AI1" << endl;
		outputfile << "AI1�̊���" << endl;
		for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
		}
		outputfile << endl;
		MemoAI1.clear();
		cout << endl;
		cout << "AI2" << endl;
		outputfile << "AI2�̊���" << endl;
		for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
		}
		outputfile << endl;
		MemoAI2.clear();
		cout << endl;

		end = chrono::system_clock::now();
		double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		cout << "�v������ : " << elapsed << endl;
		outputfile << "�v������" << endl;
		outputfile << elapsed << endl;


		outputfile.close();

		//delete[] file_num;
		*/

		cout << "Address BM" << GAME.BM << endl;
		cout << "GAME�̃f���[�Ɗ���" << endl;
	}

	cout << "�ŏI����" << endl;
	cout << "AI1(��) : " << BLACK_WIN_COUNT << "��" << endl;
	cout << "AI2(��) : " << WHITE_WIN_COUNT << "��" << endl;

	return 0;

}
