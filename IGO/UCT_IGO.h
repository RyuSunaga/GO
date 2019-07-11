//UCT探索を行うクラス

#ifndef __UCT_IGO_H_INCLUDED__
#define __UCT_IGO_H_INCLUDED__

#include "BoardManager.h"
#include <random>

//次の手の最大数(9路盤だから9x9+1)
const int NEXT_NODE_NUM_MAX = 83;

//盤ごとの次の手の情報を管理する
struct Child {
	int z_position;//選択した座標
	int visit_count;//この手を訪れた回数
	double win_rate;//この手の勝率
	int next_node;//次の局面のノードの番号
};




//盤の情報を管理するノード
struct Node {
	int child_num;//子局面の数
	Child child[83];
	//--------------------------------------ここもnew出来たらメモリを節約できる
	//Child* child;
	int child_games_total;//次の局面の回数の合計
};


class UCT {
public:
	BoardManager* BM;

	//これもコピーコンストラクタで変えた方がいいかも
	// 非決定的な乱数生成器を生成
	//std::random_device* RND;


private:


	//追加できるノードの最大値-----------------------------------------------------------------nodeのサイズ変更したらここも変更
	//これが少ないとuct_searchで-1が出てきてしまうかも
	const int MAX_NODE_ = 3000;
	//追加したノードの数//---------------------------コピーコンストラクタで初期化する
	int Node_Count_;
	//次のノードが存在しない場合
	const int EMPTY_NODE_ = -1;
	//ルール違反
	const int ERROR_NODE_ = -1;
	//UCB値計算の時の定数
	const double c = 0.5;

	//どのくらいの要素数がつよいんだろ------------------------------------------------------------------(255は動いた!!!)
	//Node node[400];
	Node* node;

	Node* root_;

	//コピーコンストラクタで動的に変数を確保した場合必要になる
	//------------------------------------------------------------------これは初期化しないといけない気がする
	BoardManager SAVE_BM;
	Node SAVE_NODE;
	//Child CHILD_SAVE;

public:
	UCT();

	~UCT();

	//コピーコンストラクタ
	UCT(const UCT &uct);

	//代入演算子オーバーロード
	UCT &operator=(const UCT &uct);

	//AppendNode用に新しいNodeを作る関数
	int CreateNode();

	//Nodeに子供を追加する関数
	void PushChild(Node* ParentNode, int z);

	//UCB値を計算する関数
	double CalcUCB(Child* child, Node* ParentNode);
	

	//モンテカルロ探索を行う関数
	int UCT_search(int node_num, int color);


	//モンテカルロ木探索を何度も行い現状の最善手を返す関数
	int SelectActionUCT(int color, int loop);


	//SelectActionUCT()の並列化版
	int SelectActionUCT_Paralelle(int color, int loop);

	int SelectActionUCT_Paralelle2(UCT uct_copy, int color, int loop);

	//並列計算で出した結果をマスタースレッドに加える関数
	void SelectActionUCT_Parallel_SUB(int next, int color, int loop);



	//UCT_search()の並列化版
	int UCT_search_Parallel(int node_num, int color, int* KoPoint2 ,int* Board_Paralelle);


	//プレイアウトを行う関数
	int playout(int color);

	//playout()の並列化版
	int playout_Paralelle(int color, int* KOPOINT2, int* Board_Paralelle);

};


#endif
