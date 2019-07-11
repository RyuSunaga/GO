/*
ver2�ł�UCT_IGO�̃����e�J�����ؒT���ŕ��񉻂���������
ver3�ł͑��̃t�@�C�����܂߂��܂܂ł̎����ō�����]�v�ȃR�[�h����薳�ʂȌv�Z���Ȃ����Ƃł���ɑ�������
*/

//�͌�v���O�������C��
#include "UCT_IGO_ver3.h"
#include "BoardManager_ver3.h"
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
using namespace std;

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
		uct->BM->ShowBoard();
		cout << "�Ղ̍����牽��ڂɒu������?(�p�X�Ȃ�0��I��)" << flush;
		cin >> x;
		cout << endl;
		cout << "�Ղ̏ォ�牽�s�ڂɒu������(�p�X�Ȃ�0��I��)?" << flush;
		cin >> y;
		cout << endl;

		if (uct->BM->Board_[y * uct->BM->WIDTH_ + x] != uct->BM->SPACE_)continue;
	

		if (x == 0 && y == 0) {
			cout << "�p�X��I������" << endl;
			//*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			break;
		}
		if (x <= 1 || x <=  9|| y <= 1 || y <= 9 /* || uct->BM->Board_[y*uct->BM->WIDTH_ + x] != uct->BM->SPACE_*/) {
			//cout << "�����ɂ͒u����I" << endl;
			break;
		}
	}
	//����
	//MemoRand.push_back(y*uct->BM->WIDTH_ + x);

	cout << y * uct->BM->WIDTH_ + x << "�ɐ΂�u���܂���" << endl;
	//uct->BM->PlayerPutStone(x, y, *turn_color);
	//*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
	//uct->BM->ShowBoard();
	return (y*uct->BM->WIDTH_ + x);
	
}





//AI���m�̐킢(�Е�����)
int main4() {


	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 15; k++) {


		UCT GAME = UCT();


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
			if (turn_color == GAME.BM->WHITE_) {
				cout << "AI1(��)�̃^�[��" << endl;

				//75000�񂾂�100�b��
				now_z = GAME.SelectActionUCT(turn_color, 10000);
				//now_z = PlayerAction(&GAME, &turn_color);
			}
			//AI2�̃^�[���̏ꍇ
			else {
				cout << "AI2(��)�̃^�[��" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(turn_color, 25000);
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

//AI���m�̐킢(�Е�����)
int main5() {


	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 30; k++) {


		UCT GAME = UCT();


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

		int GET1 = 0;
		int GET2 = 0;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------���񉻂̏ꍇ���^�[���A�R���X�g���N�^�ƃf�X�g���N�^���Ăяo�����̂ŉ��P���Ȃ��Ƃ����Ȃ�
			//-------------------------------------------------------------------------------------------------------------�����̕��񉻂�section���g�����N���X�̃R�s�[���g����������������
			//AI1�̃^�[���̏ꍇ
			if (turn_color == GAME.BM->BLACK_) {
				cout << "Player�̃^�[��" << endl;
				now_z = PlayerAction(&GAME, &turn_color);
				//now_z = GAME.SelectActionUCT(turn_color, 20000);
			}
			//AI2�̃^�[���̏ꍇ
			else {
				cout << "AI2(��)�̃^�[��" << endl;
				//--------------------------------------75000���167�b
				now_z = GAME.SelectActionUCT_Paralelle2(turn_color, 50000);
			}


			cout << "���̎�" << now_z << endl;
			cout << "���̃n�} " << GET2 << endl;
			cout << "���̃n�} " << GET1 << endl;

			
		
			if (turn_color == GAME.BM->BLACK_) {
				int h = GAME.BM->GetStone_[1];
				IsPutOK = GAME.BM->PutStone(now_z, turn_color);
				GET1 += (GAME.BM->GetStone_[1] - h);
			}
			else {
				int h = GAME.BM->GetStone_[2];
				IsPutOK = GAME.BM->PutStone(now_z, turn_color);
				GET1 += (GAME.BM->GetStone_[2] - h);
			}

			GAME.BM->ShowBoard();

			if (IsPutOK == false) {
				cout << "�G���[�I" << endl;
				ERROR_COUNT++;
				cout << "����" << endl;
				cout << "AI1(��) " << BLACK_WIN_COUNT << "��" << endl;
				cout << "AI2(��) " << WHITE_WIN_COUNT << "��" << endl;
				cout << "�G���[�� " << ERROR_COUNT << "��" << endl;
				break;
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

	return main5();


}
