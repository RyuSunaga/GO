#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <stdlib.h>
#include <algorithm>
#include <random>
#include <memory.h>
#include "UCT_IGO.h"
#include "BoardManager.h"
#include <stdio.h>
#include <omp.h>


UCT::UCT() {

	//������
	UCT::Node_Count_ = 0;
	//this->BM
	//���������m��
	//UCT::BM = NULL;
	//this->BM = NULL;
	UCT::BM = new BoardManager();//-----------------------------------------------------------------------------------------------------�R�s�[�R���X�g���N�^�̎����̎�����
	//this->BM = new BoardManager();
	//std::cout << "UCT�̃R���X�g���N�^���Ă΂�܂���" << std::endl;

	//---------------------------------------------------stackoverflow��h�����߂�new�Ŋm�ۂ��Ă݂�(delete�Y�ꂸ��)
	UCT::node = new Node[UCT::MAX_NODE_];//--------------------------------------------------------------------------------------------------�R�s�[�R���X�g���N�^�̎����̎�����
	
	//�R�s�[�R���X�g���N�^�̎����̎�����
	//UCT::RND = new (std::random_device);


	//�������K�v����?									 
	/*
	for (int i = 0; i < MAX_NODE_; i++) {
		node->child_games_total = 0;
		node->child_num = 0;
		node->child[i].next_node = 0;
		node->child[i].visit_count = 0;
		node->child[i].win_rate = 0;
		node->child[i].z_position = 0;
	}
	*/

	//this->node = new Node[UCT::MAX_NODE_];

}


UCT::~UCT() {

	//std::cout << "BM���f���[��" << std::endl;
	delete UCT::BM;   //------------------------------------------------------------��������ƃG���[���o�Ȃ��񂾂��ǂ��̂܂܂͂܂���
	//delete this->BM;
	//UCT::BM = NULL;
	//this->BM = NULL;
	//std::cout << "UCT�̃f�X�g���N�^���Ă΂�܂���" << std::endl;

	//-------------------�R���X�g���N�^��new��������f�X�g���N�^�ł�delete
	//delete[] UCT::node;
	delete[] UCT::node;
	//UCT::node = NULL;
	//node = NULL;
	//this->node = NULL;

	//delete UCT::RND;

}

//�R�s�[�R���X�g���N�^----------------------------------------------------�厖(��d�J����h������)
//-------------------------------------------------------------------------------------------------------------�����o�ϐ��ł��Ƃ��Ə���������Ă��Ȃ��ϐ���
//-------------------------------------------------------------------------------------------------------------�����ŏ��������Ȃ��ƃA�N�Z�X�ᔽ���N����(Node_Count_�����̈��)
UCT::UCT(const UCT &uct) {
	//std::cout << "UCT�̃R�s�[�R���X�g���N�^���Ăяo���܂�" << std::endl;
	this->BM = new BoardManager();
	//std::cout << "BM�A�h���X " << this->BM << std::endl;
	//this->SAVE_BM = *uct.BM;//-----------------------------------------------------------------------------------------------this->SAVE_BM �����������Ă��Ȃ����� -���̂������ƃ������[�G���[���N�����Ă��܂���������Ȃ�
	//this->BM = &this->SAVE_BM;
	*(this->BM) = *uct.BM;//---------------------------------------���̂܂܂��ᓯ���A�h���X�ɂȂ��Ă��܂��̂ł́H
	//std::cout << "BM�A�h���X " << this->BM << std::endl;
	
	//�������K�v����?		
	/*
	for (int i = 0; i < MAX_NODE_; i++) {
	    this->node->child_games_total    = uct.node->child_games_total;
		this->node->child_num            = uct.node->child_num;
		this->node->child[i].next_node   = uct.node->child[i].next_node;
		this->node->child[i].visit_count = uct.node->child[i].visit_count;
		this->node->child[i].win_rate    = uct.node->child[i].win_rate;
		this->node->child[i].z_position  = uct.node->child[i].z_position;
	}
	*/

	this->node = new Node[UCT::MAX_NODE_];
	//std::cout << "node�A�h���X " << this->node << std::endl;
	//this->SAVE_NODE = *uct.node;//------------------------------------------------------------------------------------------------this->SAVE_NODE�����������Ă��Ȃ��̂ł��̂������ƃ������[�G���[���������Ă��܂���������Ȃ�
	//this->node = &this->SAVE_NODE;
	*(this->node) = *uct.node;//---------------------------------------���̂܂܂��ᓯ���A�h���X�ɂȂ��Ă��܂��̂ł́H
	//Node_Count_�̃~�X���l���Ă̎���
	this->Node_Count_ = uct.Node_Count_;
	//this->node->child_games_total = uct.node->child_games_total;
	//std::cout << "node�A�h���X " << this->node << std::endl;
	//std::cout << "node" << this.no
	//std::cout << "child�̃A�h���X " << &this->node->child << std::endl;
	//std::cout << "Node_Count_this = " << this->Node_Count_ << std::endl;//------------------------------------------------------���̓񂩏���Node_Count_�̒l���قȂ�̂�
	//std::cout << "Node_Count_&uct = " << uct.Node_Count_ << std::endl;//--------------------------------------------------------�������A�N�Z�X�ᔽ�̌�����������Ȃ�
	//std::cout << "Node_Count_this��Address = " << &this->Node_Count_ << std::endl;//------------------------------------------------------���̓񂩏���Node_Count_�̒l���قȂ�̂�
	//std::cout << "Node_Count_&uct��Address = " << &uct.Node_Count_ << std::endl;//--------------------------------------------------------�������A�N�Z�X�ᔽ�̌�����������Ȃ�
	//std::cout << "child_games_total_this " << this->node->child_num << std::endl;//------------����������������Ă��Ȃ�����
	//std::cout << "chold_games_total_&uct " << uct.node->child_games_total << std::endl;//------�A�N�Z�X�ᔽ���N�����Ă���




	//����ŕ��񉻂����V�~�����[�V�����̌��ʂ��S�ĈقȂ�΂���
	//this->RND = new std::random_device;
	//this->RND = &(std::random_device());
}

//������Z�q�I�[�o�[���[�h
UCT &UCT::operator=(const UCT &uct) {
	std::cout << "UCT�̑�����s���܂�" << std::endl;

	//std::cout << "UCT���" << std::endl;
	//�������g���n���ꂽ�ꍇ�͏��������Ȃ�
	if (this != &uct) {
		//delete this->BM;//---------------------����K�v���킩��Ȃ�
		this->BM = new BoardManager();
		*(this->BM) = *uct.BM;
		//this->SAVE_BM_ADDRESS = new BoardManager();
		//*(this->SAVE_BM_ADDRESS) = *uct.BM;
		//this->BM = &this->SAVE_BM;
		//this->BM = uct.BM;//---------------------------------------���̂܂܂��ᓯ���A�h���X�ɂȂ��Ă��܂��̂ł́H
		//std::cout << "BM�A�h���X " << this->BM << std::endl;

		//delete[] this->node;//-----------------------����K�v���킩��Ȃ�
		this->node = new Node[UCT::MAX_NODE_];
		*(this->node) = *uct.node;
		//std::cout << "node�A�h���X " << this->node << std::endl;
		//this->SAVE_NODE = *uct.node;
		//this->node = &this->SAVE_NODE;
		//this->node = uct.node;//---------------------------------------���̂܂܂��ᓯ���A�h���X�ɂȂ��Ă��܂��̂ł́H
		//std::cout << "node�A�h���X " << this->node << std::endl;
	}

	return *this;

}

//���񉻂���ꍇ�ύX���K�v
int UCT::CreateNode() {
	
	//�ǉ������m�[�h�����E�l�ɒB���������m���߂�
	//-------------------------------------------------------------------------------------------------------------------------������������uct_search�����܂������Ȃ�
	if (UCT::Node_Count_ == UCT::MAX_NODE_)return -1;

	Node* ParentNode = &UCT::node[UCT::Node_Count_];//-------------------------------------------------------------------------�����ŕs���A�N�Z�X�����Ă��邩������Ȃ�
	ParentNode->child_num = 0;
	
	int x, y, z;
	for (y = 0; y < UCT::BM->WIDTH_; y++) {
		for (x = 0; x < UCT::BM->WIDTH_; x++) {
			z = y * UCT::BM->WIDTH_ + x;
			if (UCT::BM->Board_[z] != UCT::BM->SPACE_) {
				continue;
			}
			//std::cout << "PushChild���Ăяo���܂�" << std::endl;
			PushChild(ParentNode, z);
		}
	}
	//Pass�̏ꍇ
	PushChild(ParentNode, 0);

	UCT::Node_Count_++;

	//�쐬�����m�[�h�̔ԍ�
	return (UCT::Node_Count_ - 1);
}

void UCT::PushChild(Node* ParentNode, int z) {
	//std::cout << "UCT::PushChild���Ă΂�܂���" << std::endl;
	int n = ParentNode->child_num;
	//std::cout << n << std::endl;
	//-------------------------------------------------------------------------------------������ւ�ŗ�O��������̂͏������Ƒ�����֌W���Ă��邩������Ȃ�
	ParentNode->child[n].z_position = z;
	ParentNode->child[n].visit_count = 0;
	ParentNode->child[n].win_rate = 0;
	ParentNode->child[n].next_node = UCT::EMPTY_NODE_;
	ParentNode->child_num++;
	//std::cout << "UCT::PushChild���I���܂���" << std::endl;
}

//UCB�l���v�Z����
double UCT::CalcUCB(Child* child,Node* ParentNode) {
	return child->win_rate + UCT::c*sqrt(log(ParentNode->child_games_total) / child->visit_count);
}


//�����e�J�����T��
//�G���[�̏ꍇ-1��Ԃ�
int UCT::UCT_search(int node_num, int color) {
	//std::cout << "UCT_search" << std::endl;
	//Node* ParentNode = NULL;
	//Node* ParentNode = (Node*)std::malloc(sizeof(Node));
	Node* ParentNode =  &UCT::node[node_num];//--------------------�{���͂�����
	

	//std::cout << "�ԍ� : " << node_num << std::endl;
	//std::cout << "�q�ǖʂ̐� : " << ParentNode->child_num << std::endl;

	std::random_device RND;

	Child* child;
	re_try:
		//�X�e�b�v1(UCB�l���ł������m�[�h��I��)
		//���W�J�̃m�[�h������΂����D��I�ɂ����
		double UCB_MAX = -10000;
		int select_child_index = -1;//�I������q�m�[�h�̃C���f�b�N�X
		
		//std::cout << "child_num :" << ParentNode->child_num << std::endl;
		for (int i = 0; i < ParentNode->child_num; i++) {//----------------------------------------�q�����ł��Ă��Ȃ��̂Ŏ��s�ɂȂ��Ă��܂�2018/10/22/20:16
			Child* child = &ParentNode->child[i];

			
			if (child->z_position == UCT::ERROR_NODE_)continue;
			double UCB = 0;

			//�܂���x���K��Ă��Ȃ��ꍇ
			if (child->visit_count == 0) {
				//rand()�͈�x���K��Ă��Ȃ��m�[�h����������ꍇ
				//�����������_���ɑI�Ԃ��߂Ɏg��
				//---------------------------------------------------------------------------------------------------------------------�����Ŏ��Ԃ𗐐��V�[�h�Ƃ��Ă��܂��ƕ��񉻂����Ƃ��ɂ��ׂẴv���Z�b�T�Ō��ʂ������ɂȂ��Ă��܂�
				
				//UCB = 10000 + rand();
				UCB = 1000000 + (RND()/1000);

				
				//----------------------------------------------------------------���������̒l�𐶐����邱�Ƃ�����̂ł������C�����Ȃ��Ƃ����Ȃ�
				//std::cout << RND() << std::endl;
		
			}
			else {
				//UCB = UCT::CalcUCB(child, ParentNode);
				const double c = 0.31;
				UCB = child->win_rate + c * sqrt(log(ParentNode->child_games_total) / child->visit_count);
			}
			//std::cout << "UCB:" << UCB << std::endl;
			//UCB���X�V����
			if (UCB > UCB_MAX) {
				UCB_MAX = UCB;
				select_child_index = i;
			}
		}

		if (select_child_index == -1) {
			std::cout << "�I�����s" << std::endl;
			return -1;
		}


		//����ƌ��܂�����
		child = &ParentNode->child[select_child_index];
		int PutStonePosition = child->z_position;
		//���̍��W�ɐ΂�u��
		//�u���΂̐F��color�Ɠ���
		bool PutResult = UCT::BM->PutStone(PutStonePosition, color);     //-----------------------------------------------���񉻂���ꍇ���������P���Ȃ��Ƃ����Ȃ�2�̃X���b�h�������Ղ��������ƂɂȂ�
		//�΂�u���̂Ɏ��s�����炻�̍��W���u�����Ƃ̂ł��Ȃ����W�Ƃ������Ƃ�ۑ�����
		if (PutResult == false) {
			child->z_position = UCT::ERROR_NODE_;
			goto re_try;
		}
	



	//����������������
	int win;

	//�v���C�A�E�g���s���ꍇ
	if (child->visit_count /*== 0*/ < 20) {
		//std::cout << "�v���C�A�E�g���s���܂�" << std::endl;
		//std::cout << "���݂̐F : " << color << std::endl;
		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		//std::cout << "���̐F : " << reverse_color << std::endl;
		/*�厖(���ނ̏������l����Ƃ��Ɏg�������ȍl����)
		UCT::playout()�͈����̐F����v���C�A�E�g�����߂Ĉ����̐F�������Ă��邩�ǂ����𔻒肷��
		�Ⴆ��color(���݂̔�)������reverse_color�����̏ꍇUCT::playout(reverse_color)�ň���(��)�������Ă����1���Ԃ��Ă���
		���������Ă����0���Ԃ��Ă���
		�Ⴆ��color(���݂̔�)������reverse_color���̏ꍇUCT::playout(reverse_color)�ň���(��)�������Ă����-1���Ԃ��Ă���
		���������Ă����0���Ԃ��Ă���
		��̓�̗Ⴉ�珟���̌v�Z�ł͍��A�����ꂼ�ꂪ�Ⴄ�����͈̔͂��l����Ƃ������Ƃ�������
		�ǂ�����������0���Ԃ��Ă��ď����������邪�������ꍇ����1�𔒂�-1��Ԃ����ꂼ��̑����Ă��鏟���͈̔͂�
		������傫������悤�ɕω���������
		*/
		win = -UCT::playout(reverse_color);//---------------------------------------------------------------------------------------------------
	}
	//�v���C�A�E�g���s��Ȃ��ꍇ
	else {
		//�����q�������Ȃ��Ȃ�ǉ����čċA
		if (child->next_node == UCT::EMPTY_NODE_) {
			child->next_node = UCT::CreateNode();
		}

		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		int child_next = child->next_node;//���̎�̌�̃m�[�h�̔ԍ�
		win = -UCT::UCT_search(child_next, reverse_color);
	}

	//������Ԃ����A�����ꂼ��ł̏����͈̔�
	//���̏���-> 0.0 -- 1.0
	//���̏���-> -1.0 -- 0.0

	//�����̍X�V��
	//UCT�؂ŉ�����l���X�V���Ă����C���[�W
	child->win_rate = (child->win_rate*child->visit_count + win) / (child->visit_count + 1);
	child->visit_count++;
	ParentNode->child_games_total++;
	//UCT::BM->total_playout_++;
	//std::free(ParentNode);
	return win;
}




int UCT::SelectActionUCT(int color, int loop = 1000) {
	
	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();

	//��O
	try {
		if (next == -1)throw 10;
	}
	catch(int error){
		std::cout << "��O���������܂���" << std::endl;
	}

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(BoardCopy));

	//�ݒ肵���񐔕������e�J�����ؒT�������s����
	for (i = 0; i < loop; i++) {

	
		//�R�E�̓_��ۊǂ��Ă���
		int KOPOINT2 = UCT::BM->KoPoint_;

		//--------------------------------------------------------------------------������next��-1�̂��Ƃ����邽��UCT_search�ŗ�O����������
		UCT::UCT_search(next, color);

	
		//���������T������������ŏ��̋ǖʂ�Ԃ�
		std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
		UCT::BM->KoPoint_ = KOPOINT2;
	}

	//�K��񐔂���ԑ����m�[�h��I������
	int select_index = -1;
	int max_visit_count = -999;
	Node* ParentNode = &UCT::node[next];

	for (i = 0; i < ParentNode->child_num; i++) {
		Child* child = &ParentNode->child[i];
		if (child->visit_count > max_visit_count) {
			select_index = i;
			max_visit_count = child->visit_count;
		}
	}

	//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));



	int SelectPosition = ParentNode->child[select_index].z_position;

	std::cout << "SelectActionUCT()�I��" << std::endl;
	std::cout << "max_visit_count " << max_visit_count << std::endl;
	return SelectPosition;
}




//---------------------------------------------------------------------------------------------------Root���񉻂��s�������̂ŕ��񉻂̒��ł͍��Ǝ��̃m�[�h���������łق��͈قȂ�؂����ꂼ����K�v������
//---------------------------------------------------------------------------------------------------���񉻂̒��ł͂��ꂼ��ʂ̖؂�����K�v����̂�UCT_IGO�I�u�W�F�N�g�𓮓I�Ɋm�ۂ��čŌ��
//---------------------------------------------------------------------------------------------------�@��ɂ܂Ƃ߂�Ηǂ��̂ł͂Ȃ���
int UCT::SelectActionUCT_Paralelle(int color, int loop = 1000) {

	std::cout << "���񉻔Ł@SelectActionUCT_Paralelle" << std::endl;

	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));



	//�ݒ肵���񐔕������e�J�����ؒT�������s����
	//--------------------------------------------------------------------------------------------����for���[�v�̃u���b�N���ŕ��񉻂��s������
	//--------------------------------------------------------------------------------------------���񉻂��s���̂Ń����o�ϐ��̒l�����������Ă��܂��悤�ȕ��������������Ȃ��Ƃ����Ȃ�

	//���񉻂��s��

	#pragma omp parallel  for /*num_threads(3)*/
	//-----------------------------------------------------------------------------------------�������񉻂ł��Ȃ���..
	for (i = 0; i < loop; i++) {
		//std::cout << i + 1 << "���" << std::endl;

			//���I�Ɋm�ۂ���΃����o�ϐ����g��Ȃ��čς�
			//new������delete�킷����
		int* Board_Paralelle = new int[121];

		std::memcpy(Board_Paralelle, UCT::BM->Board_, sizeof(UCT::BM->Board_));

		//std::cout << Board_Paralelle[33];
		//�R�E�̓_��ۊǂ��Ă���
		//�������܂�������
		//------------------------------------------------------�������������ŃR�E�̓_��ۑ�����̂͏�����������ɖ߂����߂ł��邩�烁���o�ϐ������������Ă��邱�̕����͕��񉻗p�ɏC�����Ȃ��Ƃ����Ȃ�

		//new������delete�Y����
		int* KOPOINT2 = new int;
		*KOPOINT2 = UCT::BM->KoPoint_;


		UCT::UCT_search_Parallel(next, color, KOPOINT2, Board_Paralelle);//���񉻔�

		//std::cout << i << std::endl;
		//UCT::BM->ShowBoard_Paralelle(Board_Paralelle);


		//�m�ۂ�����delete
		delete KOPOINT2;
		KOPOINT2 = NULL;

		//�m�ۂ�����delete
		delete[] Board_Paralelle;
		Board_Paralelle = NULL;

	}
	
		//�K��񐔂���ԑ����m�[�h��I������
	int select_index = -1;
	int max_visit_count = -999;
	Node* ParentNode = &UCT::node[next];

	for (i = 0; i < ParentNode->child_num; i++) {
		Child* child = &ParentNode->child[i];
		if (child->visit_count > max_visit_count) {
			select_index = i;
			max_visit_count = child->visit_count;
		}
	}
	std::cout << select_index << std::endl;
	//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�-----------------------------���񉻂̏ꍇ�K�v�Ȃ�����
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	int SelectPosition = ParentNode->child[select_index].z_position;

	std::cout << "�I�����܂���" << std::endl;

	return SelectPosition;

	
}

//select�ɂ��Root���񉻂�_��
int UCT::SelectActionUCT_Paralelle2(UCT copy_uct, int color, int loop = 1000) {

	//std::cout << "���񉻔Ł@SelectActionUCT_Paralelle�Q" << std::endl;

	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();

	Node* ParentNode_Master = &UCT::node[next];


	
	int i;
	//int BoardCopy[121];
	//std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));
	
	//std::cout << "test1" << std::endl;


	//�ݒ肵���񐔕������e�J�����ؒT�������s����
	//--------------------------------------------------------------------------------------------����for���[�v�̃u���b�N���ŕ��񉻂��s������
	//--------------------------------------------------------------------------------------------���񉻂��s���̂Ń����o�ϐ��̒l�����������Ă��܂��悤�ȕ��������������Ȃ��Ƃ����Ȃ�
	//���񉻂��s��

	//---------------------------------------------------------------------------------------�}�X�^�[�X���b�h�Ƃ���ȊO�ŏꍇ�������������߂�������ʂɎg��Ȃ��ōς�
	//new�łł��Ȃ���
	//UCT* uct = new UCT[3];
	//uct[0] = copy_uct;

	#pragma omp parallel
	{
		//-----------------------�����ŃZ�N�V�������Ƃɕ��񉻂��s�����Ƃ��Ƃ�����
		//-----------------------UCT�N���X�̃m�[�h�̒l���X�V����
		#pragma omp sections
		{
			
			#pragma omp section
			{
				int this_thread = omp_get_thread_num();
				//std::cout << this_thread << std::endl;
				//UCT uct1 = copy_uct;
				//uct1.SelectActionUCT_Parallel_SUB(next, color, loop);
				//uct[0].SelectActionUCT_Parallel_SUB(next, color, loop);

				int BoardCopy[121];
				std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));

				//�}�X�^�[�X���b�h�̏ꍇ
				if (this_thread == /*0*/5) {
					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (i = 0; i < loop; i++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "�}�X�^�[�X���b�h�I��" << std::endl;
				}
				else {
					//std::cout << "Node_Count_copy_uct " << copy_uct.Node_Count_ << std::endl;
					//UCT uct = copy_uct;
					UCT uct = *this;
				
					//uct.UCT_search(color, loop);


					//uct.SelectActionUCT(color, loop);
					//uct.UCT_search(next, color);





					//����


					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (int j = 0; j < loop; j++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));






					
					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "�X���b�h(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "�X���b�h(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;

					//�}�X�^�[�X���b�h�Ɍv�Z����(���K���)��ǉ�����
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}
					//std::cout << this_thread << "�I��" << std::endl;
				}
			}
			#pragma omp section
			{
				int this_thread = omp_get_thread_num();
				//std::cout << this_thread << std::endl;
				//UCT uct1 = copy_uct;
				//uct1.SelectActionUCT_Parallel_SUB(next, color, loop);
				//uct[0].SelectActionUCT_Parallel_SUB(next, color, loop);

				int BoardCopy[121];
				std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));

				//�}�X�^�[�X���b�h�̏ꍇ
				if (this_thread == /*0*/5) {

					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (i = 0; i < loop; i++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "�}�X�^�[�X���b�h�I��" << std::endl;
				}
				else {
					//std::cout << "Node_Count_copy_uct " << copy_uct.Node_Count_ << std::endl;
					//uct[this_thread - 1].SelectActionUCT_Parallel_SUB(next, color, loop);
					//UCT uct = copy_uct;
					UCT uct = *this;

					//uct.UCT_search(color, loop);

					//uct.SelectActionUCT_Parallel_SUB(next, color, loop);
					//uct.SelectActionUCT(color, loop);
					//uct.UCT_search(next, color);




					//����


					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (int j = 0; j < loop; j++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));





					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "�X���b�h(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "�X���b�h(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;

					//�}�X�^�[�X���b�h�Ɍv�Z����(���K���)��ǉ�����
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}
					//std::cout << this_thread << "�I��" << std::endl;
				}

			}
			#pragma omp section
			{
				int this_thread = omp_get_thread_num();
				//std::cout << this_thread << std::endl;
				//UCT uct1 = copy_uct;
				//uct1.SelectActionUCT_Parallel_SUB(next, color, loop);
				//uct[0].SelectActionUCT_Parallel_SUB(next, color, loop);

				int BoardCopy[121];
				std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));

				//�}�X�^�[�X���b�h�̏ꍇ
				if (this_thread == /*0*/5) {
					//std::cout << "�}�X�^�[�X���b�h" << std::endl;

					for (i = 0; i < loop; i++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "�}�X�^�[�X���b�h�I��" << std::endl;
				}
				else {
					//std::cout << "Node_Count_copy_uct " << copy_uct.Node_Count_ << std::endl;
					//uct[this_thread - 1].SelectActionUCT_Parallel_SUB(next, color, loop);
					//UCT uct = copy_uct;
					UCT uct = *this;

					//uct.UCT_search(color, loop);

					//uct.SelectActionUCT_Parallel_SUB(next, color, loop);
					//uct.SelectActionUCT(color, loop);
					//uct.UCT_search(next, color);


					//����




					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (int j = 0; j < loop; j++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));





					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "�X���b�h(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "�X���b�h(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;


					//�}�X�^�[�X���b�h�Ɍv�Z����(���K���)��ǉ�����
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}


					//std::cout << this_thread << "�I��" << std::endl;

				}

			}
			#pragma omp section
			{
				int this_thread = omp_get_thread_num();
				//std::cout << this_thread << std::endl;
				//UCT uct1 = copy_uct;
				//uct1.SelectActionUCT_Parallel_SUB(next, color, loop);
				//uct[0].SelectActionUCT_Parallel_SUB(next, color, loop);

				int BoardCopy[121];
				std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));

				//�}�X�^�[�X���b�h�̏ꍇ
				if (this_thread == /*0*/5) {
					//std::cout << "�}�X�^�[�X���b�h" << std::endl;

					for (i = 0; i < loop; i++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}

					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					//std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "�}�X�^�[�X���b�h�I��" << std::endl;
				}
				else {
					//std::cout << "Node_Count_copy_uct " << copy_uct.Node_Count_ << std::endl;
					//uct[this_thread - 1].SelectActionUCT_Parallel_SUB(next, color, loop);
					//UCT uct = copy_uct;
					UCT uct = *this;

					//uct.UCT_search(color, loop);


					//uct.SelectActionUCT_Parallel_SUB(next, color, loop);
					//uct.SelectActionUCT(color, loop);
					//uct.UCT_search(next, color);

					//Node* ParentNode_Sub = &uct[this_thread - 1].node[next];
					//std::cout << "&uct.node[next] : " << &uct.node[next] << std::endl;


					//����



					//std::cout << "�}�X�^�[�X���b�h" << std::endl;
					for (int j = 0; j < loop; j++) {
						//�R�E�̓_��ۊǂ��Ă���
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//���������T������������ŏ��̋ǖʂ�Ԃ�
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}



					//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));


					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "�X���b�h(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "�X���b�h(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;


					//�}�X�^�[�X���b�h�Ɍv�Z����(���K���)��ǉ�����
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}

					//-------------------------------------------------------------------------------------------------�ǂ̃X���b�h�ł�ParentNode_Sub�̒��g�̒l�������ɂȂ��Ă���̂�
					//-------------------------------------------------------------------------------------------------�V�~�����[�V����������̍s���Ă��Ȃ��B�������C�����Ȃ��Ƃ����Ȃ�
					
					//-------------------------------------------------------------------------------------------------�������b�Őݒ肳��Ă��邽�߂ɂ��ׂẴV���~���[�V�������ʂ������ɂȂ��Ă���̂�������Ȃ�

					//std::cout << this_thread << "�I��" << std::endl;


				}

			}
			
		}
	}


	//delete[] uct;


	//�K��񐔂���ԑ����m�[�h��I������
	int select_index = -1;
	int max_visit_count = -999;
	Node* ParentNode = &UCT::node[next];

	for (i = 0; i < ParentNode->child_num; i++) {
		Child* child = &ParentNode->child[i];
		if (child->visit_count > max_visit_count) {
			select_index = i;
			max_visit_count = child->visit_count;
		}
	}
	std::cout << select_index << std::endl;
	//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�-----------------------------���񉻂̏ꍇ�K�v�Ȃ�����
	//std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	//---------------------------------------------------------------------------------------------------------------------------------�����Ŋe�q�ǂ��̏�����0�Ȃ͕̂��񉻕����̌��ʂ�����Ă��Ȃ�����
	int SelectPosition = ParentNode->child[select_index].z_position;

	//int sum = 0;
	//for (int k = 0; k < ParentNode->child_num; k++)sum += ParentNode->child[k].visit_count;
	//std::cout << "�q���̖K��񐔂̍��v " << sum << std::endl;

	std::cout << "SelectActionUCT_Paralelle2()�I��" << std::endl;
	std::cout << "max_visit_count " << max_visit_count << std::endl;

	return SelectPosition;


}


//����v�Z�ŏo�������ʂ��}�X�^�[�X���b�h�ɉ�����֐�
void UCT::SelectActionUCT_Parallel_SUB(int next, int color, int loop = 1000) {

	//���̊֐����Ăяo���Ƃ����̓�͂��łɌ��߂Ă���
	/*
	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();
	*/

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(BoardCopy));

	//�ݒ肵���񐔕������e�J�����ؒT�������s����
	for (i = 0; i < loop; i++) {


		//�R�E�̓_��ۊǂ��Ă���
		int KOPOINT2 = UCT::BM->KoPoint_;

		UCT::UCT_search(next, color);


		//���������T������������ŏ��̋ǖʂ�Ԃ�
		std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
		UCT::BM->KoPoint_ = KOPOINT2;
	}

	//���񉻂̏ꍇreturn�����s���̂ł͂Ȃ��}�X�^�[�X���b�h�Ɍv�Z����
	//���݂̃m�[�h�̒���̎q����total_visit�������Ă��΂���

	return;


	/*
	//�K��񐔂���ԑ����m�[�h��I������
	int select_index = -1;
	int max_visit_count = -999;
	Node* ParentNode = &UCT::node[next];

	for (i = 0; i < ParentNode->child_num; i++) {
		Child* child = &ParentNode->child[i];
		if (child->visit_count > max_visit_count) {
			select_index = i;
			max_visit_count = child->visit_count;
		}
	}

	//��ԍŏ��̋ǖʂɒ����Ă����Ȃ��ƃQ�[�����I�������܂܂ɂȂ��Ă��܂�
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	int SelectPosition = ParentNode->child[select_index].z_position;

	return SelectPosition;
	*/

}

//UCT_search()�̕��񉻔�
//------------------------------------------------------------------node_num�ɕ��񉻗p�̍H�v���K�v�ɂȂ邩��
int UCT::UCT_search_Parallel(int node_num, int color,int* KOPOINT2 , int* Board_Paralelle) {

	Node* ParentNode = &UCT::node[node_num];
	//std::cout << "�ԍ� : " << node_num << std::endl;
	//std::cout << "�q�ǖʂ̐� : " << ParentNode->child_num << std::endl;

	Child* child;
re_try:
	//�X�e�b�v1(UCB�l���ł������m�[�h��I��)
	//���W�J�̃m�[�h������΂����D��I�ɂ����
	double UCB_MAX = -10000;
	int select_child_index = -1;//�I������q�m�[�h�̃C���f�b�N�X

	//std::cout << "child_num :" << ParentNode->child_num << std::endl;
	for (int i = 0; i < ParentNode->child_num; i++) {//----------------------------------------�q�����ł��Ă��Ȃ��̂Ŏ��s�ɂȂ��Ă��܂�2018/10/22/20:16
		Child* child = &ParentNode->child[i];

		//std::cout << child->z_position << std::endl;
		if (child->z_position == UCT::ERROR_NODE_)continue;
		double UCB = 0;

		//�܂���x���K��Ă��Ȃ��ꍇ
		if (child->visit_count == 0) {
			//rand()�͈�x���K��Ă��Ȃ��m�[�h����������ꍇ
			//�����������_���ɑI�Ԃ��߂Ɏg��
			UCB = 10000 + rand();
		}
		else {
			//UCB = UCT::CalcUCB(child, ParentNode);
			const double c = 0.31;
			UCB = child->win_rate + c * sqrt(log(ParentNode->child_games_total) / child->visit_count);
		}
		//std::cout << "UCB:" << UCB << std::endl;
		//UCB���X�V����
		if (UCB > UCB_MAX) {
			UCB_MAX = UCB;
			select_child_index = i;
		}
	}

	if (select_child_index == -1) {
		//std::cout << "�I�����s" << std::endl;
		return -1;
	}


	//����ƌ��܂�����
	child = &ParentNode->child[select_child_index];
	int PutStonePosition = child->z_position;
	//���̍��W�ɐ΂�u��
	//�u���΂̐F��color�Ɠ���
	//���񉻗pPutStone()��p����
	bool PutResult = UCT::BM->PutStone_Parallel(PutStonePosition, color, KOPOINT2 , Board_Paralelle);     //-----------------------------------------------���񉻂���ꍇ���������P���Ȃ��Ƃ����Ȃ�2�̃X���b�h�������Ղ��������ƂɂȂ�
																	 //�΂�u���̂Ɏ��s�����炻�̍��W���u�����Ƃ̂ł��Ȃ����W�Ƃ������Ƃ�ۑ�����
	if (PutResult == false) {
		child->z_position = UCT::ERROR_NODE_;
		goto re_try;
	}




	//����������������
	int win;

	//�v���C�A�E�g���s���ꍇ
	if (child->visit_count == 0) {
		//std::cout << "�v���C�A�E�g���s���܂�" << std::endl;
		//std::cout << "���݂̐F : " << color << std::endl;
		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		//std::cout << "���̐F : " << reverse_color << std::endl;
		/*�厖(���ނ̏������l����Ƃ��Ɏg�������ȍl����)
		UCT::playout()�͈����̐F����v���C�A�E�g�����߂Ĉ����̐F�������Ă��邩�ǂ����𔻒肷��
		�Ⴆ��color(���݂̔�)������reverse_color�����̏ꍇUCT::playout(reverse_color)�ň���(��)�������Ă����1���Ԃ��Ă���
		���������Ă����0���Ԃ��Ă���
		�Ⴆ��color(���݂̔�)������reverse_color���̏ꍇUCT::playout(reverse_color)�ň���(��)�������Ă����-1���Ԃ��Ă���
		���������Ă����0���Ԃ��Ă���
		��̓�̗Ⴉ�珟���̌v�Z�ł͍��A�����ꂼ�ꂪ�Ⴄ�����͈̔͂��l����Ƃ������Ƃ�������
		�ǂ�����������0���Ԃ��Ă��ď����������邪�������ꍇ����1�𔒂�-1��Ԃ����ꂼ��̑����Ă��鏟���͈̔͂�
		������傫������悤�ɕω���������
		*/
		win = -UCT::playout_Paralelle(reverse_color, KOPOINT2, Board_Paralelle);//---------------------------------------------------------------------------------------------------���񉻗p�̕ύX�K�v
	}
	//�v���C�A�E�g���s��Ȃ��ꍇ
	else {
		//�����q�������Ȃ��Ȃ�ǉ����čċA
		if (child->next_node == UCT::EMPTY_NODE_) {
			child->next_node = UCT::CreateNode();//-----------------------------------------------------------------------------------------------���񉻗p�̕ύX�K�v
		}

		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		int child_next = child->next_node;//���̎�̌�̃m�[�h�̔ԍ�
		win = -UCT::UCT_search_Parallel(child_next, reverse_color, KOPOINT2, Board_Paralelle);//���񉻗p�ɕύX
	}

	//������Ԃ����A�����ꂼ��ł̏����͈̔�
	//���̏���-> 0.0 -- 1.0
	//���̏���-> -1.0 -- 0.0

	//�����̍X�V��
	//UCT�؂ŉ�����l���X�V���Ă����C���[�W
	child->win_rate = (child->win_rate*child->visit_count + win) / (child->visit_count + 1);
	child->visit_count++;
	ParentNode->child_games_total++;
	//UCT::BM->total_playout_++;
	return win;
}



//�v���C�A�E�g���s���֐�
//------------------------------------------------------------------------------���񉻂���Ƃ����̊֐����C�����Ȃ��Ƃ����Ȃ���������Ȃ�
int UCT::playout(int start_color) {
		
	//�v���C�A�E�g���s���Ƒ�����
	UCT::BM->total_playout_++;
	int turn_color = start_color;
	int before_z = -1;//1��O�̎�->2��A���p�X���ǂ������f����Ƃ��Ɏg��
	//3�R�E�΍�
	const int LOOP_MAX = UCT::BM->BOARDSIZE_ * UCT::BM->BOARDSIZE_ + 500;
	int i;
	for(i = 0; i < LOOP_MAX; i++){
		//���ׂĂ̋󂫓_�͑ł��
		int Board2[121];
		//�u����_����
		int put_stone_num = 0;
		int x,y,z;

		for (y = 0; y < UCT::BM->WIDTH_; y++) {
			for (x = 0; x < UCT::BM->WIDTH_; x++) {
				z = y * UCT::BM->WIDTH_ + x;
				
				if (UCT::BM->Board_[z] != UCT::BM->SPACE_) {
					continue;
				}

				//�u����ꏊ�����Ԃɓ���Ă���
				Board2[put_stone_num] = z;
				put_stone_num++;
			}
		}
		
		int put_z;
		int random_put;
		while (true) {//-----------------------���ł܂Ń��[�v
			//�u����₪�Ȃ��ꍇ
			if (put_stone_num == 0) {
				put_z = 0;
			}
			else {
				//�u���ꏊ�������_���ɑI��
				random_put = rand() % put_stone_num;
				//�����_���ɑI�񂾔ԍ��̍��W���擾����
				put_z = Board2[random_put];
			}
			//std::cout << "������ł�" << turn_color << std::endl;
			bool Put_Result = UCT::BM->PutStone(put_z, turn_color);//-------------------------------------------------------------------------------------���񉻂̏ꍇ���������ɂȂ�2�̃X���b�h�������Ղ��������ƂɂȂ邩��
			if (Put_Result == true) {
				//std::cout << "�łĂ܂���" << std::endl;
				break;
			}
			//std::cout << "�łĂ܂���ł���" << std::endl;
			//�I���������W��I�΂Ȃ��悤�ɂ���
			Board2[random_put] = Board2[put_stone_num - 1];
			put_stone_num--;
			//std::cout << "put_stone_num : " << put_stone_num << std::endl;
		}

		//���A���p�X�̏ꍇ,�΋ǏI��
		if (before_z == 0 && put_z == 0) {
			break;
		}
		//�ۑ�
		before_z = put_z;

		//�F�����ւ���
		turn_color = (turn_color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;

	}

	//���s����
	int k = UCT::BM->WhichColorWin(start_color,false);
	//std::cout << "�v���C�A�E�g���I��" << std::endl;
	return k;
}


int UCT::playout_Paralelle(int start_color,int* KOPOINT2 ,int* Board_Paralelle) {


	//�v���C�A�E�g���s���Ƒ�����
	UCT::BM->total_playout_++;
	int turn_color = start_color;
	int before_z = -1;//1��O�̎�->2��A���p�X���ǂ������f����Ƃ��Ɏg��
					  //3�R�E�΍�
	const int LOOP_MAX = UCT::BM->BOARDSIZE_ * UCT::BM->BOARDSIZE_ + 500;
	int i;
	for (i = 0; i < LOOP_MAX; i++) {
		//���ׂĂ̋󂫓_�͑ł��
		int Board2[121];
		//�u����_����
		int put_stone_num = 0;
		int x, y, z;

		for (y = 0; y < UCT::BM->WIDTH_; y++) {
			for (x = 0; x < UCT::BM->WIDTH_; x++) {
				z = y * UCT::BM->WIDTH_ + x;

				if (/*UCT::BM->Board_[z]*/ Board_Paralelle[z] != UCT::BM->SPACE_) {
					continue;
				}

				//�u����ꏊ�����Ԃɓ���Ă���
				Board2[put_stone_num] = z;
				put_stone_num++;
			}
		}

		int put_z;
		int random_put;
		while (true) {//-----------------------���ł܂Ń��[�v
					  //�u����₪�Ȃ��ꍇ
			if (put_stone_num == 0) {
				put_z = 0;
			}
			else {
				//�u���ꏊ�������_���ɑI��
				random_put = rand() % put_stone_num;
				//�����_���ɑI�񂾔ԍ��̍��W���擾����
				put_z = Board2[random_put];
			}
			//std::cout << "������ł�" << turn_color << std::endl;
			bool Put_Result = UCT::BM->PutStone_Parallel(put_z, turn_color,KOPOINT2, Board_Paralelle);//-------------------------------------------------------------------------------------���񉻂̏ꍇ���������ɂȂ�2�̃X���b�h�������Ղ��������ƂɂȂ�
			if (Put_Result == true) {
				//std::cout << "�łĂ܂���" << std::endl;
				break;
			}
			//std::cout << "�łĂ܂���ł���" << std::endl;
			//�I���������W��I�΂Ȃ��悤�ɂ���
			Board2[random_put] = Board2[put_stone_num - 1];
			put_stone_num--;
			//std::cout << "put_stone_num : " << put_stone_num << std::endl;
		}

		//���A���p�X�̏ꍇ,�΋ǏI��
		if (before_z == 0 && put_z == 0) {
			break;
		}
		//�ۑ�
		before_z = put_z;

		//�F�����ւ���
		turn_color = (turn_color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;

	}

	//���s����
	int k = UCT::BM->WhichColorWin_Paralelle(start_color, false, Board_Paralelle);//-------------------------------------------------------------���񉻕K�v
	//std::cout << "�v���C�A�E�g���I��" << std::endl;
	return k;

}

