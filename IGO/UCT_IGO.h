//UCT�T�����s���N���X

#ifndef __UCT_IGO_H_INCLUDED__
#define __UCT_IGO_H_INCLUDED__

#include "BoardManager.h"
#include <random>

//���̎�̍ő吔(9�H�Ղ�����9x9+1)
const int NEXT_NODE_NUM_MAX = 83;

//�Ղ��Ƃ̎��̎�̏����Ǘ�����
struct Child {
	int z_position;//�I���������W
	int visit_count;//���̎��K�ꂽ��
	double win_rate;//���̎�̏���
	int next_node;//���̋ǖʂ̃m�[�h�̔ԍ�
};




//�Ղ̏����Ǘ�����m�[�h
struct Node {
	int child_num;//�q�ǖʂ̐�
	Child child[83];
	//--------------------------------------������new�o�����烁������ߖ�ł���
	//Child* child;
	int child_games_total;//���̋ǖʂ̉񐔂̍��v
};


class UCT {
public:
	BoardManager* BM;

	//������R�s�[�R���X�g���N�^�ŕς���������������
	// �񌈒�I�ȗ���������𐶐�
	//std::random_device* RND;


private:


	//�ǉ��ł���m�[�h�̍ő�l-----------------------------------------------------------------node�̃T�C�Y�ύX�����炱�����ύX
	//���ꂪ���Ȃ���uct_search��-1���o�Ă��Ă��܂�����
	const int MAX_NODE_ = 3000;
	//�ǉ������m�[�h�̐�//---------------------------�R�s�[�R���X�g���N�^�ŏ���������
	int Node_Count_;
	//���̃m�[�h�����݂��Ȃ��ꍇ
	const int EMPTY_NODE_ = -1;
	//���[���ᔽ
	const int ERROR_NODE_ = -1;
	//UCB�l�v�Z�̎��̒萔
	const double c = 0.5;

	//�ǂ̂��炢�̗v�f�����悢�񂾂�------------------------------------------------------------------(255�͓�����!!!)
	//Node node[400];
	Node* node;

	Node* root_;

	//�R�s�[�R���X�g���N�^�œ��I�ɕϐ����m�ۂ����ꍇ�K�v�ɂȂ�
	//------------------------------------------------------------------����͏��������Ȃ��Ƃ����Ȃ��C������
	BoardManager SAVE_BM;
	Node SAVE_NODE;
	//Child CHILD_SAVE;

public:
	UCT();

	~UCT();

	//�R�s�[�R���X�g���N�^
	UCT(const UCT &uct);

	//������Z�q�I�[�o�[���[�h
	UCT &operator=(const UCT &uct);

	//AppendNode�p�ɐV����Node�����֐�
	int CreateNode();

	//Node�Ɏq����ǉ�����֐�
	void PushChild(Node* ParentNode, int z);

	//UCB�l���v�Z����֐�
	double CalcUCB(Child* child, Node* ParentNode);
	

	//�����e�J�����T�����s���֐�
	int UCT_search(int node_num, int color);


	//�����e�J�����ؒT�������x���s������̍őP���Ԃ��֐�
	int SelectActionUCT(int color, int loop);


	//SelectActionUCT()�̕��񉻔�
	int SelectActionUCT_Paralelle(int color, int loop);

	int SelectActionUCT_Paralelle2(UCT uct_copy, int color, int loop);

	//����v�Z�ŏo�������ʂ��}�X�^�[�X���b�h�ɉ�����֐�
	void SelectActionUCT_Parallel_SUB(int next, int color, int loop);



	//UCT_search()�̕��񉻔�
	int UCT_search_Parallel(int node_num, int color, int* KoPoint2 ,int* Board_Paralelle);


	//�v���C�A�E�g���s���֐�
	int playout(int color);

	//playout()�̕��񉻔�
	int playout_Paralelle(int color, int* KOPOINT2, int* Board_Paralelle);

};


#endif
