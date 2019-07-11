//�Ղ��Ǘ�����N���X

#ifndef __BOARD_H_INCLUDED__
#define __BOARD_H_INCLUDED__
#include <vector>




class BoardManager {

public:
	int total_playout_;//�v���C�A�E�g�̉�

	//�{�[�h�̏�Ԃ�\��
    //�C���f�b�N�X�͍��W���������k�������̂�
    //0->��,1->��,2->��,3->��
	int Board_[121];
	//int* Board_;------------------------------------------------------------------------���I�z����m�ۂ������������̂��ȁH

	const int SPACE_ = 0;
	const int WHITE_ = 1;
	const int BLACK_ = 2;
	const int WALL_ = 3;

	const int BOARDSIZE_ = 9;//9�H�Ղ�����
	const int WIDTH_ = 11;//�Ώ̐��������₷���悤��11�ɂ���
	const int BOARDPOINT_ = WIDTH_ * WIDTH_;//�{�[�h�̓_�̐�-----------------------------------------------����͂���Ȃ�����
	const int DIRECTION_[4] = { /*WIDTH_*/11,/*-WIDTH_*/-11,-1,1};

	int KoPoint_;//�R�E�őł��Ƃ��o���Ȃ��ʒu(�ꎟ���Ɉ��k�������W���g��)


	//�΋ǂ̌��ʂ��������邽�߂̕ϐ�
	bool IsBlackWin_;


private:

	//�����{�[�h
	//3->��,0->��
	/*int BoardTmp[121] = { 
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
		3,3,3,3,3,3,3,3,3,3,3};	
	*/

	//�{�[�h�̊e�}�X������Ƃ��ɂ��łɌ����_���ǂ����𔻒f����̂Ɏg��
	bool CheckBoard_[121];
	//std::vector<bool> CheckBoard_(121,false);
	
	//���̓Q�[���I����6.5��������
	const double KOMI_ = 6.5;
	int GetStone_[2];//0->��,1->��

					   //�{�[�h�ɕ\�����镶��
	//const char Cha_[3] = { '.','��','��' };


public:

	BoardManager();

	~BoardManager();

	//�R�s�[�R���X�g���N�^
	BoardManager(const BoardManager &bm);

	//������Z�q�I�[�o�[���[�h
	BoardManager &operator=(const BoardManager &bm);


	//���݂̃v���C�A�E�g�̉񐔂𓾂�
	int Get_total_playout();

public:
	//���݂̃{�[�h��\������
	void ShowBoard();

	//�^����ꂽ�{�[�h��\������
	void ShowBoard_Paralelle(int* Board);

public:
	//�v���C�A�E�g���J�n������Ԃ����������ǂ������肷��
	//��������->0,��������->-1
	//��������->0,��������->1,
	int WhichColorWin(int nowturn_color,bool IsShowData);

	//���񉻗p��WhichColorWin()
	int WhichColorWin_Paralelle(int nowturn_color, bool IsShowData, int* Board_Paralelle);

public:
	//���s������false��Ԃ��A����ȊO�Ȃ��true��Ԃ�
	bool PutStone(int z, int color);

	//���񉻗p��PutStone()
	bool PutStone_Parallel(int z, int color, int* KoPoint, int* Board_Paralelle);

	//�{�[�h�̃T�C�Y��Ԃ�
	int GetBoardSize();

public:
	//�΂���菜���֐�
	void RemoveStone(int z, int color);

	//���񉻗p��RemoveStone()
	void RemoveStone_Paralelle(int z, int color, int* Board_Parallel);


public:
	//�ʖڂ̐��𐔂���
	void CountDame(int now_z, int* dame_count, int* stone_count);

private:
	//CountDame()�̓���
	void CountDameSub(int now_z, int color, int* dame_count, int* stone_count);

public:
	void CountDame_Parallel(int now_z, int* dame_count, int* stone_count, int* Board_Paralle);

private:
	//CountDame_Parallel()�̓���
	void CountDame_Parallel_Sub(int now_z, int color, int* dame_count, int* stone_count, int* Board_Parallel, bool* CheckBoard_Parallel);



public:
	//�v���C���[���΂�u���֐�
	void PlayerPutStone(int x, int y, int player_color);

};

#endif




