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

	//初期化
	UCT::Node_Count_ = 0;
	//this->BM
	//メモリを確保
	//UCT::BM = NULL;
	//this->BM = NULL;
	UCT::BM = new BoardManager();//-----------------------------------------------------------------------------------------------------コピーコンストラクタの実装の時注意
	//this->BM = new BoardManager();
	//std::cout << "UCTのコンストラクタが呼ばれました" << std::endl;

	//---------------------------------------------------stackoverflowを防ぐためにnewで確保してみる(delete忘れずに)
	UCT::node = new Node[UCT::MAX_NODE_];//--------------------------------------------------------------------------------------------------コピーコンストラクタの実装の時注意
	
	//コピーコンストラクタの実装の時注意
	//UCT::RND = new (std::random_device);


	//初期化必要かな?									 
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

	//std::cout << "BMをデリーと" << std::endl;
	delete UCT::BM;   //------------------------------------------------------------こうするとエラーが出ないんだけどこのままはまずい
	//delete this->BM;
	//UCT::BM = NULL;
	//this->BM = NULL;
	//std::cout << "UCTのデストラクタが呼ばれました" << std::endl;

	//-------------------コンストラクタでnewしたからデストラクタではdelete
	//delete[] UCT::node;
	delete[] UCT::node;
	//UCT::node = NULL;
	//node = NULL;
	//this->node = NULL;

	//delete UCT::RND;

}

//コピーコンストラクタ----------------------------------------------------大事(二重開放を防ぎたい)
//-------------------------------------------------------------------------------------------------------------メンバ変数でもともと初期化されていない変数は
//-------------------------------------------------------------------------------------------------------------ここで初期化しないとアクセス違反を起こす(Node_Count_もその一つ)
UCT::UCT(const UCT &uct) {
	//std::cout << "UCTのコピーコンストラクタを呼び出します" << std::endl;
	this->BM = new BoardManager();
	//std::cout << "BMアドレス " << this->BM << std::endl;
	//this->SAVE_BM = *uct.BM;//-----------------------------------------------------------------------------------------------this->SAVE_BM を初期化していないから -このやり方だとメモリーエラーを起こしてしまうかもしれない
	//this->BM = &this->SAVE_BM;
	*(this->BM) = *uct.BM;//---------------------------------------このままじゃ同じアドレスになってしまうのでは？
	//std::cout << "BMアドレス " << this->BM << std::endl;
	
	//初期化必要かな?		
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
	//std::cout << "nodeアドレス " << this->node << std::endl;
	//this->SAVE_NODE = *uct.node;//------------------------------------------------------------------------------------------------this->SAVE_NODEを初期化していないのでこのやり方だとメモリーエラーをおこしてしまうかもしれない
	//this->node = &this->SAVE_NODE;
	*(this->node) = *uct.node;//---------------------------------------このままじゃ同じアドレスになってしまうのでは？
	//Node_Count_のミスを考えての実験
	this->Node_Count_ = uct.Node_Count_;
	//this->node->child_games_total = uct.node->child_games_total;
	//std::cout << "nodeアドレス " << this->node << std::endl;
	//std::cout << "node" << this.no
	//std::cout << "childのアドレス " << &this->node->child << std::endl;
	//std::cout << "Node_Count_this = " << this->Node_Count_ << std::endl;//------------------------------------------------------この二か所でNode_Count_の値が異なるのが
	//std::cout << "Node_Count_&uct = " << uct.Node_Count_ << std::endl;//--------------------------------------------------------メモリアクセス違反の原因かもしれない
	//std::cout << "Node_Count_thisのAddress = " << &this->Node_Count_ << std::endl;//------------------------------------------------------この二か所でNode_Count_の値が異なるのが
	//std::cout << "Node_Count_&uctのAddress = " << &uct.Node_Count_ << std::endl;//--------------------------------------------------------メモリアクセス違反の原因かもしれない
	//std::cout << "child_games_total_this " << this->node->child_num << std::endl;//------------ここも初期化されていないため
	//std::cout << "chold_games_total_&uct " << uct.node->child_games_total << std::endl;//------アクセス違反を起こしていた




	//これで並列化したシミュレーションの結果が全て異なればいい
	//this->RND = new std::random_device;
	//this->RND = &(std::random_device());
}

//代入演算子オーバーロード
UCT &UCT::operator=(const UCT &uct) {
	std::cout << "UCTの代入を行います" << std::endl;

	//std::cout << "UCT代入" << std::endl;
	//自分自身が渡された場合は処理をしない
	if (this != &uct) {
		//delete this->BM;//---------------------これ必要かわからない
		this->BM = new BoardManager();
		*(this->BM) = *uct.BM;
		//this->SAVE_BM_ADDRESS = new BoardManager();
		//*(this->SAVE_BM_ADDRESS) = *uct.BM;
		//this->BM = &this->SAVE_BM;
		//this->BM = uct.BM;//---------------------------------------このままじゃ同じアドレスになってしまうのでは？
		//std::cout << "BMアドレス " << this->BM << std::endl;

		//delete[] this->node;//-----------------------これ必要かわからない
		this->node = new Node[UCT::MAX_NODE_];
		*(this->node) = *uct.node;
		//std::cout << "nodeアドレス " << this->node << std::endl;
		//this->SAVE_NODE = *uct.node;
		//this->node = &this->SAVE_NODE;
		//this->node = uct.node;//---------------------------------------このままじゃ同じアドレスになってしまうのでは？
		//std::cout << "nodeアドレス " << this->node << std::endl;
	}

	return *this;

}

//並列化する場合変更が必要
int UCT::CreateNode() {
	
	//追加したノードが限界値に達したかを確かめる
	//-------------------------------------------------------------------------------------------------------------------------ここが原因でuct_searchがうまくいかない
	if (UCT::Node_Count_ == UCT::MAX_NODE_)return -1;

	Node* ParentNode = &UCT::node[UCT::Node_Count_];//-------------------------------------------------------------------------ここで不正アクセスをしているかもしれない
	ParentNode->child_num = 0;
	
	int x, y, z;
	for (y = 0; y < UCT::BM->WIDTH_; y++) {
		for (x = 0; x < UCT::BM->WIDTH_; x++) {
			z = y * UCT::BM->WIDTH_ + x;
			if (UCT::BM->Board_[z] != UCT::BM->SPACE_) {
				continue;
			}
			//std::cout << "PushChildを呼び出します" << std::endl;
			PushChild(ParentNode, z);
		}
	}
	//Passの場合
	PushChild(ParentNode, 0);

	UCT::Node_Count_++;

	//作成したノードの番号
	return (UCT::Node_Count_ - 1);
}

void UCT::PushChild(Node* ParentNode, int z) {
	//std::cout << "UCT::PushChildが呼ばれました" << std::endl;
	int n = ParentNode->child_num;
	//std::cout << n << std::endl;
	//-------------------------------------------------------------------------------------ここらへんで例外がおこるのは初期化と代入が関係しているかもしれない
	ParentNode->child[n].z_position = z;
	ParentNode->child[n].visit_count = 0;
	ParentNode->child[n].win_rate = 0;
	ParentNode->child[n].next_node = UCT::EMPTY_NODE_;
	ParentNode->child_num++;
	//std::cout << "UCT::PushChildが終わりました" << std::endl;
}

//UCB値を計算する
double UCT::CalcUCB(Child* child,Node* ParentNode) {
	return child->win_rate + UCT::c*sqrt(log(ParentNode->child_games_total) / child->visit_count);
}


//モンテカルロ探索
//エラーの場合-1を返す
int UCT::UCT_search(int node_num, int color) {
	//std::cout << "UCT_search" << std::endl;
	//Node* ParentNode = NULL;
	//Node* ParentNode = (Node*)std::malloc(sizeof(Node));
	Node* ParentNode =  &UCT::node[node_num];//--------------------本当はこっち
	

	//std::cout << "番号 : " << node_num << std::endl;
	//std::cout << "子局面の数 : " << ParentNode->child_num << std::endl;

	std::random_device RND;

	Child* child;
	re_try:
		//ステップ1(UCB値が最も高いノードを選ぶ)
		//未展開のノードがあればそれを優先的にえらぶ
		double UCB_MAX = -10000;
		int select_child_index = -1;//選択する子ノードのインデックス
		
		//std::cout << "child_num :" << ParentNode->child_num << std::endl;
		for (int i = 0; i < ParentNode->child_num; i++) {//----------------------------------------子供ができていないので失敗になってしまう2018/10/22/20:16
			Child* child = &ParentNode->child[i];

			
			if (child->z_position == UCT::ERROR_NODE_)continue;
			double UCB = 0;

			//まだ一度も訪れていない場合
			if (child->visit_count == 0) {
				//rand()は一度も訪れていないノードが複数ある場合
				//それらをランダムに選ぶために使う
				//---------------------------------------------------------------------------------------------------------------------ここで時間を乱数シードとしてしまうと並列化したときにすべてのプロセッサで結果が同じになってしまう
				
				//UCB = 10000 + rand();
				UCB = 1000000 + (RND()/1000);

				
				//----------------------------------------------------------------乱数が負の値を生成することがあるのでそこを修正しないといけない
				//std::cout << RND() << std::endl;
		
			}
			else {
				//UCB = UCT::CalcUCB(child, ParentNode);
				const double c = 0.31;
				UCB = child->win_rate + c * sqrt(log(ParentNode->child_games_total) / child->visit_count);
			}
			//std::cout << "UCB:" << UCB << std::endl;
			//UCBを更新する
			if (UCB > UCB_MAX) {
				UCB_MAX = UCB;
				select_child_index = i;
			}
		}

		if (select_child_index == -1) {
			std::cout << "選択失敗" << std::endl;
			return -1;
		}


		//やっと決まったね
		child = &ParentNode->child[select_child_index];
		int PutStonePosition = child->z_position;
		//その座標に石を置く
		//置く石の色はcolorと同じ
		bool PutResult = UCT::BM->PutStone(PutStonePosition, color);     //-----------------------------------------------並列化する場合ここを改善しないといけない2つのスレッドが同じ盤を扱うことになる
		//石を置くのに失敗したらその座標が置くことのできない座標ということを保存する
		if (PutResult == false) {
			child->z_position = UCT::ERROR_NODE_;
			goto re_try;
		}
	



	//勝ったか負けたか
	int win;

	//プレイアウトを行う場合
	if (child->visit_count /*== 0*/ < 20) {
		//std::cout << "プレイアウトを行います" << std::endl;
		//std::cout << "現在の色 : " << color << std::endl;
		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		//std::cout << "次の色 : " << reverse_color << std::endl;
		/*大事(二種類の勝率を考えるときに使えそうな考え方)
		UCT::playout()は引数の色からプレイアウトを初めて引数の色が勝っているかどうかを判定する
		例えばcolor(現在の番)が白でreverse_colorが黒の場合UCT::playout(reverse_color)で引数(黒)が勝っていれば1が返ってきて
		黒が負けていれば0が返ってくる
		例えばcolor(現在の番)が黒でreverse_color白の場合UCT::playout(reverse_color)で引数(白)が勝っていれば-1が返ってきて
		白が負けていれば0が返ってくる
		上の二つの例から勝率の計算では黒、白それぞれが違う勝率の範囲を考えるということが分かる
		どちらも負ければ0が返ってきて勝率が下がるが勝った場合黒は1を白は-1を返しそれぞれの属している勝率の範囲で
		勝率を大きくするように変化を加える
		*/
		win = -UCT::playout(reverse_color);//---------------------------------------------------------------------------------------------------
	}
	//プレイアウトを行わない場合
	else {
		//もし子供がいないなら追加して再帰
		if (child->next_node == UCT::EMPTY_NODE_) {
			child->next_node = UCT::CreateNode();
		}

		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		int child_next = child->next_node;//この手の後のノードの番号
		win = -UCT::UCT_search(child_next, reverse_color);
	}

	//自分手番が黒、白それぞれでの勝率の範囲
	//黒の勝率-> 0.0 -- 1.0
	//白の勝率-> -1.0 -- 0.0

	//勝率の更新や
	//UCT木で下から値を更新していくイメージ
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

	//例外
	try {
		if (next == -1)throw 10;
	}
	catch(int error){
		std::cout << "例外が発生しました" << std::endl;
	}

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(BoardCopy));

	//設定した回数分モンテカルロ木探索を実行する
	for (i = 0; i < loop; i++) {

	
		//コウの点を保管しておく
		int KOPOINT2 = UCT::BM->KoPoint_;

		//--------------------------------------------------------------------------ここでnextが-1のことがあるためUCT_searchで例外が発生する
		UCT::UCT_search(next, color);

	
		//もっかい探索したいから最初の局面を返す
		std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
		UCT::BM->KoPoint_ = KOPOINT2;
	}

	//訪問回数が一番多いノードを選択する
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

	//一番最初の局面に直してあげないとゲームが終了したままになってしまう
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));



	int SelectPosition = ParentNode->child[select_index].z_position;

	std::cout << "SelectActionUCT()終了" << std::endl;
	std::cout << "max_visit_count " << max_visit_count << std::endl;
	return SelectPosition;
}




//---------------------------------------------------------------------------------------------------Root並列化を行いたいので並列化の中では根と次のノードだけ同じでほかは異なる木をそれぞれ作る必要がある
//---------------------------------------------------------------------------------------------------並列化の中ではそれぞれ別の木をつくる必要がるのでUCT_IGOオブジェクトを動的に確保して最後に
//---------------------------------------------------------------------------------------------------　一つにまとめれば良いのではないか
int UCT::SelectActionUCT_Paralelle(int color, int loop = 1000) {

	std::cout << "並列化版　SelectActionUCT_Paralelle" << std::endl;

	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));



	//設定した回数分モンテカルロ木探索を実行する
	//--------------------------------------------------------------------------------------------次のforループのブロック内で並列化を行いたい
	//--------------------------------------------------------------------------------------------並列化を行うのでメンバ変数の値を書き換えてしまうような部分を書き換えないといけない

	//並列化を行う

	#pragma omp parallel  for /*num_threads(3)*/
	//-----------------------------------------------------------------------------------------ここ並列化できなそう..
	for (i = 0; i < loop; i++) {
		//std::cout << i + 1 << "回目" << std::endl;

			//動的に確保すればメンバ変数を使わなくて済む
			//newしたらdeleteわすれるな
		int* Board_Paralelle = new int[121];

		std::memcpy(Board_Paralelle, UCT::BM->Board_, sizeof(UCT::BM->Board_));

		//std::cout << Board_Paralelle[33];
		//コウの点を保管しておく
		//ここもまずいかも
		//------------------------------------------------------そもそもここでコウの点を保尊するのは書き換えた後に戻すためであるからメンバ変数を書き換えているこの部分は並列化用に修正しないといけない

		//newしたらdelete忘れるな
		int* KOPOINT2 = new int;
		*KOPOINT2 = UCT::BM->KoPoint_;


		UCT::UCT_search_Parallel(next, color, KOPOINT2, Board_Paralelle);//並列化版

		//std::cout << i << std::endl;
		//UCT::BM->ShowBoard_Paralelle(Board_Paralelle);


		//確保したらdelete
		delete KOPOINT2;
		KOPOINT2 = NULL;

		//確保したらdelete
		delete[] Board_Paralelle;
		Board_Paralelle = NULL;

	}
	
		//訪問回数が一番多いノードを選択する
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
	//一番最初の局面に直してあげないとゲームが終了したままになってしまう-----------------------------並列化の場合必要ないかも
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	int SelectPosition = ParentNode->child[select_index].z_position;

	std::cout << "終了しました" << std::endl;

	return SelectPosition;

	
}

//selectによりRoot並列化を狙う
int UCT::SelectActionUCT_Paralelle2(UCT copy_uct, int color, int loop = 1000) {

	//std::cout << "並列化版　SelectActionUCT_Paralelle２" << std::endl;

	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();

	Node* ParentNode_Master = &UCT::node[next];


	
	int i;
	//int BoardCopy[121];
	//std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(UCT::BM->Board_));
	
	//std::cout << "test1" << std::endl;


	//設定した回数分モンテカルロ木探索を実行する
	//--------------------------------------------------------------------------------------------次のforループのブロック内で並列化を行いたい
	//--------------------------------------------------------------------------------------------並列化を行うのでメンバ変数の値を書き換えてしまうような部分を書き換えないといけない
	//並列化を行う

	//---------------------------------------------------------------------------------------マスタースレッドとそれ以外で場合分けした方がめもりも無駄に使わないで済む
	//newでできないか
	//UCT* uct = new UCT[3];
	//uct[0] = copy_uct;

	#pragma omp parallel
	{
		//-----------------------ここでセクションごとに並列化を行いもともとあった
		//-----------------------UCTクラスのノードの値を更新する
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

				//マスタースレッドの場合
				if (this_thread == /*0*/5) {
					//std::cout << "マスタースレッド" << std::endl;
					for (i = 0; i < loop; i++) {
						//コウの点を保管しておく
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "マスタースレッド終了" << std::endl;
				}
				else {
					//std::cout << "Node_Count_copy_uct " << copy_uct.Node_Count_ << std::endl;
					//UCT uct = copy_uct;
					UCT uct = *this;
				
					//uct.UCT_search(color, loop);


					//uct.SelectActionUCT(color, loop);
					//uct.UCT_search(next, color);





					//実験


					//std::cout << "マスタースレッド" << std::endl;
					for (int j = 0; j < loop; j++) {
						//コウの点を保管しておく
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));






					
					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "スレッド(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "スレッド(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;

					//マスタースレッドに計算結果(総訪問回数)を追加する
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}
					//std::cout << this_thread << "終了" << std::endl;
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

				//マスタースレッドの場合
				if (this_thread == /*0*/5) {

					//std::cout << "マスタースレッド" << std::endl;
					for (i = 0; i < loop; i++) {
						//コウの点を保管しておく
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "マスタースレッド終了" << std::endl;
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




					//実験


					//std::cout << "マスタースレッド" << std::endl;
					for (int j = 0; j < loop; j++) {
						//コウの点を保管しておく
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));





					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "スレッド(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "スレッド(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;

					//マスタースレッドに計算結果(総訪問回数)を追加する
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}
					//std::cout << this_thread << "終了" << std::endl;
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

				//マスタースレッドの場合
				if (this_thread == /*0*/5) {
					//std::cout << "マスタースレッド" << std::endl;

					for (i = 0; i < loop; i++) {
						//コウの点を保管しておく
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}
					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "マスタースレッド終了" << std::endl;
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


					//実験




					//std::cout << "マスタースレッド" << std::endl;
					for (int j = 0; j < loop; j++) {
						//コウの点を保管しておく
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}

					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));





					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "スレッド(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "スレッド(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;


					//マスタースレッドに計算結果(総訪問回数)を追加する
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}


					//std::cout << this_thread << "終了" << std::endl;

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

				//マスタースレッドの場合
				if (this_thread == /*0*/5) {
					//std::cout << "マスタースレッド" << std::endl;

					for (i = 0; i < loop; i++) {
						//コウの点を保管しておく
						int KOPOINT2 = UCT::BM->KoPoint_;

						UCT::UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
						UCT::BM->KoPoint_ = KOPOINT2;
					}

					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					//std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
					//std::cout << "マスタースレッド終了" << std::endl;
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


					//実験



					//std::cout << "マスタースレッド" << std::endl;
					for (int j = 0; j < loop; j++) {
						//コウの点を保管しておく
						int KOPOINT2 = uct.BM->KoPoint_;

						uct.UCT_search(next, color);


						//もっかい探索したいから最初の局面を返す
						std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));
						uct.BM->KoPoint_ = KOPOINT2;
					}



					//一番最初の局面に直してあげないとゲームが終了したままになってしまう
					std::memcpy(uct.BM->Board_, BoardCopy, sizeof(uct.BM->Board_));


					Node* ParentNode_Sub = &uct.node[next];
					//std::cout << "スレッド(next)" << this_thread << " : " << next << std::endl;
					//std::cout << "スレッド(&uct.node[next])" << this_thread << " : " << &uct.node[next] << std::endl;


					//マスタースレッドに計算結果(総訪問回数)を追加する
					for (int j = 0; j < ParentNode_Master->child_num; j++) {
						//Child* child = &ParentNode_Master->child[i];

						ParentNode_Master->child[j].visit_count += ParentNode_Sub->child[j].visit_count;

					}

					//-------------------------------------------------------------------------------------------------どのスレッドでもParentNode_Subの中身の値が同じになっているので
					//-------------------------------------------------------------------------------------------------シミュレーションが正常の行われていない。ここを修正しないといけない
					
					//-------------------------------------------------------------------------------------------------乱数が秒で設定されているためにすべてのシュミレーション結果が同じになっているのかもしれない

					//std::cout << this_thread << "終了" << std::endl;


				}

			}
			
		}
	}


	//delete[] uct;


	//訪問回数が一番多いノードを選択する
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
	//一番最初の局面に直してあげないとゲームが終了したままになってしまう-----------------------------並列化の場合必要ないかも
	//std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	//---------------------------------------------------------------------------------------------------------------------------------ここで各子どもの勝率が0なのは並列化部分の結果をいれていないから
	int SelectPosition = ParentNode->child[select_index].z_position;

	//int sum = 0;
	//for (int k = 0; k < ParentNode->child_num; k++)sum += ParentNode->child[k].visit_count;
	//std::cout << "子供の訪問回数の合計 " << sum << std::endl;

	std::cout << "SelectActionUCT_Paralelle2()終了" << std::endl;
	std::cout << "max_visit_count " << max_visit_count << std::endl;

	return SelectPosition;


}


//並列計算で出した結果をマスタースレッドに加える関数
void UCT::SelectActionUCT_Parallel_SUB(int next, int color, int loop = 1000) {

	//この関数を呼び出すときこの二つはすでに決めてある
	/*
	UCT::Node_Count_ = 0;
	int next = UCT::CreateNode();
	*/

	int i;
	int BoardCopy[121];
	std::memcpy(BoardCopy, UCT::BM->Board_, sizeof(BoardCopy));

	//設定した回数分モンテカルロ木探索を実行する
	for (i = 0; i < loop; i++) {


		//コウの点を保管しておく
		int KOPOINT2 = UCT::BM->KoPoint_;

		UCT::UCT_search(next, color);


		//もっかい探索したいから最初の局面を返す
		std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));
		UCT::BM->KoPoint_ = KOPOINT2;
	}

	//並列化の場合return下を行うのではなくマスタースレッドに計算した
	//現在のノードの直後の子供のtotal_visitを加えてやればいい

	return;


	/*
	//訪問回数が一番多いノードを選択する
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

	//一番最初の局面に直してあげないとゲームが終了したままになってしまう
	std::memcpy(UCT::BM->Board_, BoardCopy, sizeof(UCT::BM->Board_));

	int SelectPosition = ParentNode->child[select_index].z_position;

	return SelectPosition;
	*/

}

//UCT_search()の並列化版
//------------------------------------------------------------------node_numに並列化用の工夫が必要になるかも
int UCT::UCT_search_Parallel(int node_num, int color,int* KOPOINT2 , int* Board_Paralelle) {

	Node* ParentNode = &UCT::node[node_num];
	//std::cout << "番号 : " << node_num << std::endl;
	//std::cout << "子局面の数 : " << ParentNode->child_num << std::endl;

	Child* child;
re_try:
	//ステップ1(UCB値が最も高いノードを選ぶ)
	//未展開のノードがあればそれを優先的にえらぶ
	double UCB_MAX = -10000;
	int select_child_index = -1;//選択する子ノードのインデックス

	//std::cout << "child_num :" << ParentNode->child_num << std::endl;
	for (int i = 0; i < ParentNode->child_num; i++) {//----------------------------------------子供ができていないので失敗になってしまう2018/10/22/20:16
		Child* child = &ParentNode->child[i];

		//std::cout << child->z_position << std::endl;
		if (child->z_position == UCT::ERROR_NODE_)continue;
		double UCB = 0;

		//まだ一度も訪れていない場合
		if (child->visit_count == 0) {
			//rand()は一度も訪れていないノードが複数ある場合
			//それらをランダムに選ぶために使う
			UCB = 10000 + rand();
		}
		else {
			//UCB = UCT::CalcUCB(child, ParentNode);
			const double c = 0.31;
			UCB = child->win_rate + c * sqrt(log(ParentNode->child_games_total) / child->visit_count);
		}
		//std::cout << "UCB:" << UCB << std::endl;
		//UCBを更新する
		if (UCB > UCB_MAX) {
			UCB_MAX = UCB;
			select_child_index = i;
		}
	}

	if (select_child_index == -1) {
		//std::cout << "選択失敗" << std::endl;
		return -1;
	}


	//やっと決まったね
	child = &ParentNode->child[select_child_index];
	int PutStonePosition = child->z_position;
	//その座標に石を置く
	//置く石の色はcolorと同じ
	//並列化用PutStone()を用いる
	bool PutResult = UCT::BM->PutStone_Parallel(PutStonePosition, color, KOPOINT2 , Board_Paralelle);     //-----------------------------------------------並列化する場合ここを改善しないといけない2つのスレッドが同じ盤を扱うことになる
																	 //石を置くのに失敗したらその座標が置くことのできない座標ということを保存する
	if (PutResult == false) {
		child->z_position = UCT::ERROR_NODE_;
		goto re_try;
	}




	//勝ったか負けたか
	int win;

	//プレイアウトを行う場合
	if (child->visit_count == 0) {
		//std::cout << "プレイアウトを行います" << std::endl;
		//std::cout << "現在の色 : " << color << std::endl;
		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		//std::cout << "次の色 : " << reverse_color << std::endl;
		/*大事(二種類の勝率を考えるときに使えそうな考え方)
		UCT::playout()は引数の色からプレイアウトを初めて引数の色が勝っているかどうかを判定する
		例えばcolor(現在の番)が白でreverse_colorが黒の場合UCT::playout(reverse_color)で引数(黒)が勝っていれば1が返ってきて
		黒が負けていれば0が返ってくる
		例えばcolor(現在の番)が黒でreverse_color白の場合UCT::playout(reverse_color)で引数(白)が勝っていれば-1が返ってきて
		白が負けていれば0が返ってくる
		上の二つの例から勝率の計算では黒、白それぞれが違う勝率の範囲を考えるということが分かる
		どちらも負ければ0が返ってきて勝率が下がるが勝った場合黒は1を白は-1を返しそれぞれの属している勝率の範囲で
		勝率を大きくするように変化を加える
		*/
		win = -UCT::playout_Paralelle(reverse_color, KOPOINT2, Board_Paralelle);//---------------------------------------------------------------------------------------------------並列化用の変更必要
	}
	//プレイアウトを行わない場合
	else {
		//もし子供がいないなら追加して再帰
		if (child->next_node == UCT::EMPTY_NODE_) {
			child->next_node = UCT::CreateNode();//-----------------------------------------------------------------------------------------------並列化用の変更必要
		}

		int reverse_color = (color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;
		int child_next = child->next_node;//この手の後のノードの番号
		win = -UCT::UCT_search_Parallel(child_next, reverse_color, KOPOINT2, Board_Paralelle);//並列化用に変更
	}

	//自分手番が黒、白それぞれでの勝率の範囲
	//黒の勝率-> 0.0 -- 1.0
	//白の勝率-> -1.0 -- 0.0

	//勝率の更新や
	//UCT木で下から値を更新していくイメージ
	child->win_rate = (child->win_rate*child->visit_count + win) / (child->visit_count + 1);
	child->visit_count++;
	ParentNode->child_games_total++;
	//UCT::BM->total_playout_++;
	return win;
}



//プレイアウトを行う関数
//------------------------------------------------------------------------------並列化するときこの関数も修正しないといけないかもしれない
int UCT::playout(int start_color) {
		
	//プレイアウトを行うと増える
	UCT::BM->total_playout_++;
	int turn_color = start_color;
	int before_z = -1;//1手前の手->2手連続パスかどうか判断するときに使う
	//3コウ対策
	const int LOOP_MAX = UCT::BM->BOARDSIZE_ * UCT::BM->BOARDSIZE_ + 500;
	int i;
	for(i = 0; i < LOOP_MAX; i++){
		//すべての空き点は打つ候補
		int Board2[121];
		//置ける点も数
		int put_stone_num = 0;
		int x,y,z;

		for (y = 0; y < UCT::BM->WIDTH_; y++) {
			for (x = 0; x < UCT::BM->WIDTH_; x++) {
				z = y * UCT::BM->WIDTH_ + x;
				
				if (UCT::BM->Board_[z] != UCT::BM->SPACE_) {
					continue;
				}

				//置ける場所を順番に入れていく
				Board2[put_stone_num] = z;
				put_stone_num++;
			}
		}
		
		int put_z;
		int random_put;
		while (true) {//-----------------------手を打つまでループ
			//置く候補がない場合
			if (put_stone_num == 0) {
				put_z = 0;
			}
			else {
				//置く場所をランダムに選ぶ
				random_put = rand() % put_stone_num;
				//ランダムに選んだ番号の座標を取得する
				put_z = Board2[random_put];
			}
			//std::cout << "今から打つ手" << turn_color << std::endl;
			bool Put_Result = UCT::BM->PutStone(put_z, turn_color);//-------------------------------------------------------------------------------------並列化の場合ここが問題になる2つのスレッドが同じ盤を扱うことになるから
			if (Put_Result == true) {
				//std::cout << "打てました" << std::endl;
				break;
			}
			//std::cout << "打てませんでした" << std::endl;
			//選択した座標を選ばないようにする
			Board2[random_put] = Board2[put_stone_num - 1];
			put_stone_num--;
			//std::cout << "put_stone_num : " << put_stone_num << std::endl;
		}

		//二回連続パスの場合,対局終了
		if (before_z == 0 && put_z == 0) {
			break;
		}
		//保尊
		before_z = put_z;

		//色を入れ替える
		turn_color = (turn_color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;

	}

	//勝敗判定
	int k = UCT::BM->WhichColorWin(start_color,false);
	//std::cout << "プレイアウトを終了" << std::endl;
	return k;
}


int UCT::playout_Paralelle(int start_color,int* KOPOINT2 ,int* Board_Paralelle) {


	//プレイアウトを行うと増える
	UCT::BM->total_playout_++;
	int turn_color = start_color;
	int before_z = -1;//1手前の手->2手連続パスかどうか判断するときに使う
					  //3コウ対策
	const int LOOP_MAX = UCT::BM->BOARDSIZE_ * UCT::BM->BOARDSIZE_ + 500;
	int i;
	for (i = 0; i < LOOP_MAX; i++) {
		//すべての空き点は打つ候補
		int Board2[121];
		//置ける点も数
		int put_stone_num = 0;
		int x, y, z;

		for (y = 0; y < UCT::BM->WIDTH_; y++) {
			for (x = 0; x < UCT::BM->WIDTH_; x++) {
				z = y * UCT::BM->WIDTH_ + x;

				if (/*UCT::BM->Board_[z]*/ Board_Paralelle[z] != UCT::BM->SPACE_) {
					continue;
				}

				//置ける場所を順番に入れていく
				Board2[put_stone_num] = z;
				put_stone_num++;
			}
		}

		int put_z;
		int random_put;
		while (true) {//-----------------------手を打つまでループ
					  //置く候補がない場合
			if (put_stone_num == 0) {
				put_z = 0;
			}
			else {
				//置く場所をランダムに選ぶ
				random_put = rand() % put_stone_num;
				//ランダムに選んだ番号の座標を取得する
				put_z = Board2[random_put];
			}
			//std::cout << "今から打つ手" << turn_color << std::endl;
			bool Put_Result = UCT::BM->PutStone_Parallel(put_z, turn_color,KOPOINT2, Board_Paralelle);//-------------------------------------------------------------------------------------並列化の場合ここが問題になる2つのスレッドが同じ盤を扱うことになる
			if (Put_Result == true) {
				//std::cout << "打てました" << std::endl;
				break;
			}
			//std::cout << "打てませんでした" << std::endl;
			//選択した座標を選ばないようにする
			Board2[random_put] = Board2[put_stone_num - 1];
			put_stone_num--;
			//std::cout << "put_stone_num : " << put_stone_num << std::endl;
		}

		//二回連続パスの場合,対局終了
		if (before_z == 0 && put_z == 0) {
			break;
		}
		//保尊
		before_z = put_z;

		//色を入れ替える
		turn_color = (turn_color == UCT::BM->BLACK_) ? UCT::BM->WHITE_ : UCT::BM->BLACK_;

	}

	//勝敗判定
	int k = UCT::BM->WhichColorWin_Paralelle(start_color, false, Board_Paralelle);//-------------------------------------------------------------並列化必要
	//std::cout << "プレイアウトを終了" << std::endl;
	return k;

}

