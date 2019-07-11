//BoardManager�N���X�̎���
#include <string>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "BoardManager_ver3.h"





BoardManager::BoardManager() {

	//-----------------------------------------------��񂵂��g��Ȃ��Ȃ炱������������������
	int BoardTmp[121] = {
		3,3,3,3,3,3,3,3,3,3,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,
		3,3,3,3,3,3,3,3,3,3,3 };


	std::memcpy(BoardManager::Board_, BoardTmp, sizeof(BoardManager::Board_));

	//std::cout << "�R���X�g���N�^���" << std::endl;
	//BoardManager::ShowBoard();

	BoardManager::total_playout_ = 0;
	BoardManager::GetStone_[WHITE_ - 1] = BoardManager::GetStone_[BLACK_ - 1] = 0;//-------------------------------------------�����̔z����ԈႦ�Ă���������delete�o���Ȃ�����

																				  //std::cout << "BoardManager�̃R���X�g���N�^���Ă΂�܂���" << std::endl;
}

BoardManager::~BoardManager() {
	//std::cout << "BoardManager�̃f�X�g���N�^���Ă΂�܂���" << std::endl;
}

int BoardManager::Get_total_playout() {
	return BoardManager::total_playout_;
}

BoardManager::BoardManager(const BoardManager &bm) {
	//std::cout << "BoardManager�̃R�s�[�R���X�g���N�^" << std::endl;
}

BoardManager &BoardManager::operator=(const BoardManager &bm) {
	//std::cout << "���" << std::endl;
	//std::cout << "BM���" << std::endl;
	//std::cout << "Board_�̃A�h���X " << this->Board_ << std::endl;
	//std::cout << "this->KoPoint_ " << this->KoPoint_ << std::endl;
	//std::cout << "&bm.KoPoint_ " << bm.KoPoint_ << std::endl;
	//this->KoPoint_ = bm.KoPoint_;
	return *this;
}


void BoardManager::ShowBoard() {
	int i, j;
	std::cout << std::endl;
	for (i = 1; i < WIDTH_ - 1; i++) {
		if (i != 1)std::cout << "| | | | | | | | |" << std::endl;
		for (j = 1; j < WIDTH_ - 1; j++) {
			//std::cout << 1 << std::endl;
			switch (BoardManager::Board_[i*WIDTH_ + j]) {
			case 0/*SPACE_*/:
				if (j != WIDTH_ - 2)std::cout << " -" << std::flush;
				else std::cout << " " << std::flush;
				break;
			case 1/*WHITE_*/:
				if (j != WIDTH_ - 2)std::cout << "W-" << std::flush;
				else std::cout << "W" << std::flush;
				break;
			case 2/*BLACK_*/:
				if (j != WIDTH_ - 2)std::cout << "B-" << std::flush;
				else std::cout << "B" << std::flush;
				break;
			case 3/*WALL_*/:
				std::cout << ' #' << std::flush;
				break;
			}
		}
		std::cout << std::endl;
	}
	//std::cout << "BoardManager::ShowBoard() �I��" << std::endl;
}


//-------------------------------------------------------------------------------------------------------�Ƃ����΂̐����l�����Ă��Ȃ��̂ł́H�H�H�H
//-------------------------------------------------------------------------------------------------------������p�X������ׂ��Ń^�C�~���O�Őw�n���L���ɍs���̂ł͂Ȃ����H
int BoardManager::WhichColorWin(int nowturn_color, bool IsShowData) {


	//score���傫��->���L��,score��������->���L��
	//���΂̐�->score��+1
	//���΂̐�->score��-1
	//���΂ň͂܂ꂽ�}�X1�}�X�ɂ�score��+1
	//���΂ň͂܂ꂽ�}�X1�}�X�ɂ�score��-1
	double score = 0;

	//�΂̐��𐔂���	
	//0->��,1->��
	int Stone[2];

	//�㉺���E�̐΂̐��𒲂ׂ�
	//0->��,1->��
	int AroundStone[2];
	Stone[WHITE_ - 1] = Stone[BLACK_ - 1] = 0;

	int x, y, z;
	int i;

	for (y = 1; y <= WIDTH_; y++) {
		for (x = 1; x <= WIDTH_; x++) {
			z = y * WIDTH_ + x;

			//�󔒂łȂ��ꍇ
			if (BoardManager::Board_[z] == WHITE_) {
				Stone[WHITE_ - 1]++;
				continue;
			}
			else if (BoardManager::Board_[z] == BLACK_) {
				Stone[BLACK_ - 1]++;
				continue;
			}


			//�󔒂̏ꍇ
			AroundStone[WHITE_ - 1] = AroundStone[BLACK_ - 1] = 0;

			//4�����̐΂̎�ނ𒲂ׂ�
			for (i = 0; i < 4; i++) {
				z = y * WIDTH_ + x + BoardManager::DIRECTION_[i];

				switch (BoardManager::Board_[z]) {
				case 1:
					AroundStone[WHITE_ - 1]++;
					break;
				case 2:
					AroundStone[BLACK_ - 1]++;
					break;
				}
			}

			//�󂫃}�X�ɂ�链�_
			//4��������͂܂�Ȃ��Ă��悢(���̉\���������邩��)
			if (AroundStone[WHITE_ - 1] && AroundStone[BLACK_ - 1] == 0)score++;
			else if (AroundStone[BLACK_ - 1] && AroundStone[WHITE_ - 1] == 0)score--;

		}
	}

	//�΂̐��ɂ�链�_
	score += (Stone[WHITE_ - 1] - Stone[BLACK_ - 1]);

	//���͘Z�ڔ��L���ɂȂ�
	score += BoardManager::KOMI_;

	int result_game = 0;
	//���������Ȃ�11
	if (score < 0)result_game = 1;

	if (IsShowData == true) {
		std::cout << "����" << std::endl;
		if (result_game == 1) {
			std::cout << "���̏���" << std::endl;
			IsBlackWin_ = true;
		}
		else {
			std::cout << "���̏���" << std::endl;
			IsBlackWin_ = false;
		}
		std::cout << "���ʓ_��:" << score << std::endl;
		//std::cout << "�I�ǔ�" << std::endl;
		//BoardManager::ShowBoard();
	}
	//���������Ȃ畄���𔽓]
	if (nowturn_color == WHITE_) result_game = -result_game;
	//std::cout << "���s����I��" << std::endl;
	return result_game;
}



bool BoardManager::PutStone(int z, int color) {

	//std::cout << "PutStone�J�n" << std::endl;

	//�p�X
	if (z == 0) {
		BoardManager::KoPoint_ = 0;
		return true;
	}

	int Around[4][3];//�㉺���E�̑ʖڐ�(�ċz�X),�ΐ�,�F
	int reverse_color = (color == WHITE_) ? BLACK_ : WHITE_;

	int space_count = 0;//4�����̋󔒂̐�
	int kabe_count = 0;//4�����̕ǂ̐�
	int safe_count = 0;//�ʖڂ�2�ȏ�ň��S�Ȗ����̐�
	int take_count = 0;//��邱�Ƃ��\�Ȑ΂̍��v
	int KOPOINT2;//�R�E�̌��ƂȂ�_�̍��W

	int i;
	for (i = 0; i < 4; i++) {
		Around[i][0] = Around[i][1] = Around[i][2] = 0;

		int next_z = z + BoardManager::DIRECTION_[i];
		int next_color = BoardManager::Board_[next_z];

		//�󔒂��ǂ̏ꍇ
		if (next_color == SPACE_) {
			space_count++;
			continue;
		}
		else if (next_color == WALL_) {
			kabe_count++;
			continue;
		}

		int dame_count = 0;
		//�q�����Ă���΂̑���
		int stone_count = 0;
		BoardManager::CountDame(next_z, &dame_count, &stone_count);//��ԍŏ��̌Ăяo�������̐΂��܂߂Đ�����

		Around[i][0] = dame_count;
		Around[i][1] = stone_count;
		Around[i][2] = next_color;

		//�R�E�ɂȂ�\��������ꍇ---------------------------------------
		//-----------------------------------------------------------------������u���\��̏ꏊ�ׂ̗��Ⴄ�F�̌�΂ł��̌�΂̌ċz�_��1�����Ȃ��Ƃ������Ƃ�
		//-----------------------------------------------------------------���̓_�͍����������u���_�ł������č�����u���_�̓R�E�ɂȂ�\���̂���_�ƌ�����(����)
		if (next_color == reverse_color && dame_count == 1) {              /*                                                                     */
			take_count += stone_count;									   /*       ���Z                                                          */
			KOPOINT2 = next_z;                                             /*     ����  �Z                                                        */
		}																   /*       ���������ɒu�����Ƃ��l����ꍇ�ō������ɂ��čl���Ă���Ƃ�  */
																		   //���S�ȏꍇ													   /*           ���̏ꍇ�R�E�ł͂Ȃ����R�E�̌��ɂ͂Ȃ�                  */
		else if (next_color == color && dame_count >= 2) {
			safe_count++;
		}
	}

	//std::cout << "test1" << std::endl;

	//���E��
	if (space_count == 0 && safe_count == 0 && take_count == 0)return false;
	if (z == BoardManager::KoPoint_)return false;
	if (kabe_count + safe_count == 4)return false;//�����ڂ̂Ƃ��ł������ɖ��߂�K�v���Ȃ��ꍇ(�ቺ)
												  //std::cout << "test1.5" << std::endl;
	if (BoardManager::Board_[z] != SPACE_)return false;
																										  /*           */    /*        �@*/   /*          */
																									      /*     ��    */    /*        �@*/   /*          */
																										  /*   ��  ��  */    /*        ��*/   /*    ��    */
																										  /*     ��    */    /*      ��  */   /*  ��  ��  */
																										//std::cout << "test2" << std::endl;
	for (i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];
		int dame_count = Around[i][0];
		int connected_stone_count = Around[i][1];
		int nextcolor = Around[i][2];

		//std::cout << "�ʖڂ̐� : " << dame_count << std::endl;//------------------------------------���������0
		//std::cout << "�q�����Ă���΂̐� : " << connected_stone_count << std::endl;//---------------���������0
		//std::cout << "���̐F : " << nextcolor << std::endl;
		//std::cout << "���̐F : " << reverse_color << std::endl;

		//�l���Ă�������̐΂���邱�Ƃ��o����ꍇ
		if (nextcolor == reverse_color && dame_count == 1 && BoardManager::Board_[next_z]) {
			//std::cout << "�΂Ƃ�Ł[�[�[�[�[�[�[-------------------------------------------------------------------------------" << std::endl;
			BoardManager::RemoveStone(next_z, reverse_color);
			//------------------------------------------------------------------------------------------------------------------------------------------------------�����̓V���~���[�V�����̂��Ƃ��Ƃɖ߂��Ȃ��Ƃ����Ȃ�����
			BoardManager::GetStone_[color - 1] += connected_stone_count;
		}

	}

	//�����܂ł��Ă���ƒu����
	//std::cout << color << std::endl;
	BoardManager::Board_[z] = color;//-------------------------------------�����ɐ΂̐��l�ȊO�������Ă��܂�
									//std::cout << "�΂�" << z << "�ɒu���܂�" << color << std::endl;
									//std::cout << "Board_[" << z << "] : " << BoardManager::Board_[z] << std::endl;
									//BoardManager::ShowBoard();


									//�̂���͂Ƃ�����̂��Ƃ��l����
	int dame_count = 0;
	int stone_count = 0;
	BoardManager::CountDame(z, &dame_count, &stone_count);

	//�R�E�̏ꍇ(connected_stone_count��Board[z]���g���܂ނ��Ƃɒ���)
	//connected_stone_count == 1�Ƃ������Ƃ͐΂�1�ŌǗ����Ă���Ƃ�������
	if (stone_count == 1 && dame_count == 1 && take_count == 1) {                 //���̏�(����͊��S�ȃR�E)    /*     ���Z         */
		BoardManager::KoPoint_ = KOPOINT2;                                                                        /*   ����  �Z       */
	}                                                                                                             /*     ���Z         */
	else {                                                                                                        /*                  */
		BoardManager::KoPoint_ = 0;
	}

	//�����ɂ����܂���!
	//std::cout << "PutStone�I��" << std::endl;
	return true;

}




//�w�肵���F�̐΂ƌq�����Ă���΂����ׂĎ�菜��
void BoardManager::RemoveStone(int z, int RemoveColor) {

	BoardManager::Board_[z] = SPACE_;

	for (int i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];

		if (BoardManager::Board_[next_z] == RemoveColor) {
			BoardManager::RemoveStone(next_z, RemoveColor);
		}
	}
}


//�ʖڂ͐΂̌ċz�X�Ƃ��Ă��l����
//now_z�ɂ�����ʖڂ̐��ƌq�����Ă���΂̑��������߂�
void BoardManager::CountDame(int now_z, int* dame_count, int* stone_count) {


	//*dame_count = 0;
	//*stone_count = 0;

	//CheckBoard��false�ŏ���������->�T���ς݂���true�ɂȂ�
	for (int i = 0; i < WIDTH_*WIDTH_; i++) {
		BoardManager::CheckBoard_[i] = false;
	}

	//�ʒunow_z�ł̈�t�̐F�����߂�
	int color = BoardManager::Board_[now_z];

	BoardManager::CountDameSub(now_z, color, dame_count, stone_count);
}

//�����Ɠ����΂�����������ċA�ɂ���Ē��ׂ�΂���
void BoardManager::CountDameSub(int now_z, int color, int* dame_count, int* stone_count) {
	//std::cout << "BoardManager::CountDameSub" << std::endl;
	//�t���O�𗧂Ă�
	BoardManager::CheckBoard_[now_z] = true;
	(*stone_count)++;//-----------------------------------------------------*stone_count++ ���Ǝ��s����+���D��炵��
					 //std::cout << "�΂�����" << *stone_count << std::endl;
	int i;
	for (i = 0; i < 4; i++) {

		int next_z = now_z + BoardManager::DIRECTION_[i];

		//���łɌ������Ƃ��邩�m�F
		if (BoardManager::CheckBoard_[next_z])continue;

		//�ʖ�
		if (BoardManager::Board_[next_z] == 0) {
			//std::cout << "�ʖڂ�����" << *dame_count << std::endl;
			(*dame_count)++;
			BoardManager::CheckBoard_[next_z] = true;
		}

		//�����Ɠ����F�̐΂�����΍ċA�����Ă��̐΂̏㉺���E�ɂ��Ă����ׂ�
		if (BoardManager::Board_[next_z] == color) {
			//std::cout << "�ċA" << std::endl;
			BoardManager::CountDameSub(next_z, color, dame_count, stone_count);
		}

	}

}


int BoardManager::GetBoardSize() {
	return BoardManager::BOARDSIZE_;
}


