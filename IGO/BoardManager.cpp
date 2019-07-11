//BoardManagerクラスの実装
#include <string>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "BoardManager.h"





BoardManager::BoardManager() {

	//-----------------------------------------------一回しか使わないならこうした方がいいかも
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

	//std::cout << "コンストラクタ語後" << std::endl;
	//BoardManager::ShowBoard();

	BoardManager::total_playout_ = 0;
	BoardManager::GetStone_[WHITE_-1] = BoardManager::GetStone_[BLACK_-1] = 0;//-------------------------------------------ここの配列を間違えていたせいでdelete出来なかった

	//std::cout << "BoardManagerのコンストラクタが呼ばれました" << std::endl;
}

BoardManager::~BoardManager() {
	//std::cout << "BoardManagerのデストラクタが呼ばれました" << std::endl;
}

int BoardManager::Get_total_playout() {
	return BoardManager::total_playout_;
}

BoardManager::BoardManager(const BoardManager &bm) {
	//std::cout << "BoardManagerのコピーコンストラクタ" << std::endl;
}

BoardManager &BoardManager::operator=(const BoardManager &bm) {
	//std::cout << "代入" << std::endl;
	//std::cout << "BM代入" << std::endl;
	//std::cout << "Board_のアドレス " << this->Board_ << std::endl;
	//std::cout << "this->KoPoint_ " << this->KoPoint_ << std::endl;
	//std::cout << "&bm.KoPoint_ " << bm.KoPoint_ << std::endl;
	//this->KoPoint_ = bm.KoPoint_;
	return *this;
}


void BoardManager::ShowBoard() {
	int i, j;
	std::cout << std::endl;
	for (i = 1; i < WIDTH_-1; i++) {
		if(i != 1)std::cout << "| | | | | | | | |" << std::endl;
		for (j = 1; j < WIDTH_-1; j++) {
			//std::cout << 1 << std::endl;
			switch (BoardManager::Board_[i*WIDTH_ + j]){
				case 0/*SPACE_*/:
					if(j != WIDTH_-2)std::cout << " -" << std::flush;
					else std::cout << " " << std::flush;
					break;
				case 1/*WHITE_*/:
					if(j != WIDTH_-2)std::cout << "W-" << std::flush;
					else std::cout << "W" << std::flush;
					break;
				case 2/*BLACK_*/:
					if(j != WIDTH_-2)std::cout << "B-" << std::flush;
					else std::cout << "B" << std::flush;
					break;
				case 3/*WALL_*/:
					std::cout << ' #' << std::flush;
					break;
			}
		}
		std::cout << std::endl;
	}
	//std::cout << "BoardManager::ShowBoard() 終了" << std::endl;
}

void BoardManager::ShowBoard_Paralelle(int* Board_Paralelle) {
	int i, j;
	std::cout << std::endl;
	for (i = 1; i < WIDTH_ - 1; i++) {
		if (i != 1)std::cout << "| | | | | | | | |" << std::endl;
		for (j = 1; j < WIDTH_ - 1; j++) {
			//std::cout << 1 << std::endl;
			switch (Board_Paralelle[i*WIDTH_ + j]) {
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
	//std::cout << "BoardManager::ShowBoard() 終了" << std::endl;
}

int BoardManager::WhichColorWin(int nowturn_color,bool IsShowData) {


	//scoreが大きい->白有利,scoreが小さい->黒有利
	//白石の数->scoreを+1
	//黒石の数->scoreを-1
	//白石で囲まれたマス1マスにつきscoreを+1
	//黒石で囲まれたマス1マスにつきscoreを-1
	double score = 0;
	
	//石の数を数える	
	//0->白,1->黒
	int Stone[2];

	//上下左右の石の数を調べる
	//0->白,1->黒
	int AroundStone[2];
	Stone[WHITE_-1] = Stone[BLACK_-1] = 0;

	int x, y, z;
	int i;

	for (y = 1; y <= WIDTH_; y++) {
		for (x = 1; x <= WIDTH_; x++) {
			z = y * WIDTH_ + x;
			
			//空白でない場合
			if (BoardManager::Board_[z] == WHITE_) {
				Stone[WHITE_-1]++;
				continue;
			}
			else if (BoardManager::Board_[z] == BLACK_) {
				Stone[BLACK_-1]++;
				continue;
			}
		

			//空白の場合
			AroundStone[WHITE_-1] = AroundStone[BLACK_-1] = 0;

			//4方向の石の種類を調べる
			for (i = 0; i < 4; i++) {
				z = y * WIDTH_ + x + BoardManager::DIRECTION_[i];
				
				switch (BoardManager::Board_[z]) {
					case 1:
						AroundStone[WHITE_-1]++;
						break;
					case 2:
						AroundStone[BLACK_-1]++;
						break;
				}
			}
		
			//空きマスによる得点
			//4方向から囲まれなくてもよい(隅の可能せいもあるから)
			if (AroundStone[WHITE_-1] && AroundStone[BLACK_-1] == 0)score++;
			else if (AroundStone[BLACK_-1] && AroundStone[WHITE_-1] == 0)score--;

		}
	}

	//石の数による得点
	score += (Stone[WHITE_-1] - Stone[BLACK_-1]);

	//白は六目半有利になる
	score += BoardManager::KOMI_;

	int result_game = 0;
	//黒が勝ちなら11
	if (score < 0)result_game = 1;

	if (IsShowData == true) {
		std::cout << "結果" << std::endl;
		if (result_game == 1) {
			std::cout << "黒の勝ち" << std::endl;
			IsBlackWin_ = true;
		}
		else {
			std::cout << "白の勝ち" << std::endl;
			IsBlackWin_ = false;
		}
		std::cout << "結果点数:" << score << std::endl;
		//std::cout << "終局盤" << std::endl;
		//BoardManager::ShowBoard();
	}
	//白が勝ちなら符号を反転
	if (nowturn_color == WHITE_) result_game = -result_game;
	//std::cout << "勝敗判定終了" << std::endl;
	return result_game;
}

//並列化用,WhichColorWin()
int BoardManager::WhichColorWin_Paralelle(int nowturn_color, bool IsShowData, int* Board_Paralelle) {

	//scoreが大きい->白有利,scoreが小さい->黒有利
	//白石の数->scoreを+1
	//黒石の数->scoreを-1
	//白石で囲まれたマス1マスにつきscoreを+1
	//黒石で囲まれたマス1マスにつきscoreを-1
	double score = 0;

	//石の数を数える	
	//0->白,1->黒
	int Stone[2];

	//上下左右の石の数を調べる
	//0->白,1->黒
	int AroundStone[2];
	Stone[WHITE_ - 1] = Stone[BLACK_ - 1] = 0;

	int x, y, z;
	int i;

	for (y = 1; y <= WIDTH_; y++) {
		for (x = 1; x <= WIDTH_; x++) {
			z = y * WIDTH_ + x;

			//空白でない場合
			if (/*BoardManager::Board_[z]*/ Board_Paralelle[z] == WHITE_) {
				Stone[WHITE_ - 1]++;
				continue;
			}
			else if (/*BoardManager::Board_[z]*/ Board_Paralelle[z] == BLACK_) {
				Stone[BLACK_ - 1]++;
				continue;
			}


			//空白の場合
			AroundStone[WHITE_ - 1] = AroundStone[BLACK_ - 1] = 0;

			//4方向の石の種類を調べる
			for (i = 0; i < 4; i++) {
				z = y * WIDTH_ + x + BoardManager::DIRECTION_[i];

				switch (/*BoardManager::Board_[z]*/ Board_Paralelle[z]) {
				case 1:
					AroundStone[WHITE_ - 1]++;
					break;
				case 2:
					AroundStone[BLACK_ - 1]++;
					break;
				}
			}

			//空きマスによる得点
			//4方向から囲まれなくてもよい(隅の可能せいもあるから)
			if (AroundStone[WHITE_ - 1] && AroundStone[BLACK_ - 1] == 0)score++;
			else if (AroundStone[BLACK_ - 1] && AroundStone[WHITE_ - 1] == 0)score--;

		}
	}

	//石の数による得点
	score += (Stone[WHITE_ - 1] - Stone[BLACK_ - 1]);

	//白は六目半有利になる
	score += BoardManager::KOMI_;

	int result_game = 0;
	//黒が勝ちなら11
	if (score < 0)result_game = 1;

	if (IsShowData == true) {
		std::cout << "結果" << std::endl;
		if (result_game == 1) {
			std::cout << "黒の勝ち" << std::endl;
			IsBlackWin_ = true;
		}
		else {
			std::cout << "白の勝ち" << std::endl;
			IsBlackWin_ = false;
		}
		std::cout << "結果点数:" << score << std::endl;
		//std::cout << "終局盤" << std::endl;
		//BoardManager::ShowBoard();
	}
	//白が勝ちなら符号を反転
	if (nowturn_color == WHITE_) result_game = -result_game;
	//std::cout << "勝敗判定終了" << std::endl;
	return result_game;

}


bool BoardManager::PutStone(int z, int color) {
	
	//std::cout << "PutStone開始" << std::endl;

	//パス
	if (z == 0) {
		BoardManager::KoPoint_ = 0;
		return true;
	}

	int Around[4][3];//上下左右の駄目数(呼吸店),石数,色
	int reverse_color = (color == WHITE_) ? BLACK_ : WHITE_;

	int space_count = 0;//4方向の空白の数
	int kabe_count = 0;//4方向の壁の数
	int safe_count = 0;//駄目が2以上で安全な味方の数
	int take_count = 0;//取ることが可能な石の合計
	int KOPOINT2;//コウの候補となる点の座標
	
	int i;
	for (i = 0; i < 4; i++) {
		Around[i][0] = Around[i][1] = Around[i][2] = 0;
		
		int next_z = z + BoardManager::DIRECTION_[i];
		int next_color = BoardManager::Board_[next_z];

		//空白か壁の場合
		if (next_color == SPACE_) {
			space_count++;
			continue;
		}
		else if (next_color == WALL_) {
			kabe_count++;
			continue;
		}

		int dame_count = 0;
		//繋がっている石の総数
		int stone_count = 0;
		BoardManager::CountDame(next_z, &dame_count, &stone_count);//一番最初の呼び出し引数の石も含めて数える

		Around[i][0] = dame_count;
		Around[i][1] = stone_count;
		Around[i][2] = next_color;

		//コウになる可能性がある場合---------------------------------------
		//-----------------------------------------------------------------今から置く予定の場所の隣が違う色の碁石でその碁石の呼吸点が1つしかないということは
		//-----------------------------------------------------------------その点は今か自分が置く点であるよって今から置く点はコウになる可能性のある点と言える(下麗)
		if (next_color == reverse_color && dame_count == 1) {              /*                                                                     */
			take_count += stone_count;									   /*       ●〇                                                          */
			KOPOINT2 = next_z;                                             /*     ●●  〇                                                        */
		}																   /*       ●↑ここに置くことを考える場合で左方向について考えているとき  */
		//安全な場合													   /*           この場合コウではないがコウの候補にはなる                  */
		else if (next_color == color && dame_count >= 2) {
			safe_count++;
		}
	}

	//std::cout << "test1" << std::endl;

	//自殺手
	if (space_count == 0 && safe_count == 0 && take_count == 0)return false;
	if (z == BoardManager::KoPoint_)return false;
	if (kabe_count + safe_count == 4)return false;//欠け目のときでただちに埋める必要がない場合(例下)
	//std::cout << "test1.5" << std::endl;
	if (BoardManager::Board_[z] != SPACE_)return false;          
		                                                                   /*           */    /*        　*/   /*          */
		                                                                   /*     ●    */    /*        　*/   /*          */
		                                                                   /*   ●  ●  */    /*        ●*/   /*    ●    */
		                                                                   /*     ●    */    /*      ●  */   /*  ●  ●  */

	//std::cout << "test2" << std::endl;
	for (i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];
		int dame_count = Around[i][0];
		int connected_stone_count = Around[i][1];
		int nextcolor = Around[i][2];

		//std::cout << "駄目の数 : " << dame_count << std::endl;//------------------------------------ここが常に0
		//std::cout << "繋がっている石の数 : " << connected_stone_count << std::endl;//---------------ここも常に0
		//std::cout << "次の色 : " << nextcolor << std::endl;
		//std::cout << "次の色 : " << reverse_color << std::endl;

		//考えている方向の石を取ることが出来る場合
		if (nextcolor == reverse_color && dame_count == 1 && BoardManager::Board_[next_z]) {
			//std::cout << "石とるでーーーーーーー-------------------------------------------------------------------------------" << std::endl;
			BoardManager::RemoveStone(next_z, reverse_color);
			BoardManager::GetStone_[color - 1] += connected_stone_count;
		}
	}

	//ここまできてやっと置ける
	//std::cout << color << std::endl;
	BoardManager::Board_[z] = color;//-------------------------------------ここに石の数値以外が入ってしまう
	//std::cout << "石を" << z << "に置きます" << color << std::endl;
	//std::cout << "Board_[" << z << "] : " << BoardManager::Board_[z] << std::endl;
	//BoardManager::ShowBoard();


	//のこりはとった後のことを考える
	int dame_count = 0;
	int stone_count = 0;
	BoardManager::CountDame(z, &dame_count, &stone_count);

	//コウの場合(connected_stone_countはBoard[z]自身も含むことに注意)
	//connected_stone_count == 1ということは石が1個で孤立しているということ
	if (stone_count == 1 && dame_count == 1 && take_count == 1) {                 //左の状況(これは完全なコウ)    /*     ●〇         */
		BoardManager::KoPoint_ = KOPOINT2;                                                                        /*   ●●  〇       */
	}                                                                                                             /*     ●〇         */
	else {                                                                                                        /*                  */
		BoardManager::KoPoint_ = 0;
	}

	//無事におけました!
	//std::cout << "PutStone終了" << std::endl;
	return true;

}

//並列化用のPutStone()
//-----------------------------------------------------------------------------------------基本的にメンバ変数を扱うのは並列化中はよくないかも.....
//-----------------------------------------------------------------------------------------BoardManagerのメンバ変数であるBoard_にアクセスしていた部分を引数に変更した2018/1030/22:51
//-----------------------------------------------------------------------------------------まずはメンバ変数にアクセスしている部分を他の関連する関数も含めて変更する必要がある
bool BoardManager::PutStone_Parallel(int z, int color, int* KoPoint_Parallel, int* Board_Paralelle){
	//std::cout << "PutStone開始" << std::endl;

	//パス
	if (z == 0) {
		//並列化用にする
		//BoardManager::KoPoint_ = 0;
		*KoPoint_Parallel = 0;
		return true;
	}

	int Around[4][3];//上下左右の駄目数(呼吸店),石数,色
	int reverse_color = (color == WHITE_) ? BLACK_ : WHITE_;

	int space_count = 0;//4方向の空白の数
	int kabe_count = 0;//4方向の壁の数
	int safe_count = 0;//駄目が2以上で安全な味方の数
	int take_count = 0;//取ることが可能な石の合計
	int KOPOINT2;//コウの候補となる点の座標

	int i;
	for (i = 0; i < 4; i++) {
		Around[i][0] = Around[i][1] = Around[i][2] = 0;

		int  next_z= z + BoardManager::DIRECTION_[i];
		int next_color = Board_Paralelle[next_z];

		//空白か壁の場合
		if (next_color == SPACE_) {
			space_count++;
			continue;
		}
		else if (next_color == WALL_) {
			kabe_count++;
			continue;
		}

		int dame_count = 0;
		//繋がっている石の総数
		int stone_count = 0;

		
		BoardManager::CountDame_Parallel(next_z, &dame_count, &stone_count, Board_Paralelle);//一番最初の呼び出し引数の石も含めて数える-----------------------------------ここも並列化用に変えた方がいいかも--->変更済み

		Around[i][0] = dame_count;
		Around[i][1] = stone_count;
		Around[i][2] = next_color;

		//コウになる可能性がある場合---------------------------------------
		//-----------------------------------------------------------------今から置く予定の場所の隣が違う色の碁石でその碁石の呼吸点が1つしかないということは
		//-----------------------------------------------------------------その点は今か自分が置く点であるよって今から置く点はコウになる可能性のある点と言える(下麗)
		if (next_color == reverse_color && dame_count == 1) {              /*                                                                     */
			take_count += stone_count;									   /*       ●〇                                                          */
			KOPOINT2 = next_z;                                             /*     ●●  〇                                                        */
		}																   /*       ●↑ここに置くことを考える場合で左方向について考えているとき  */
									   //安全な場合	   					 /*           この場合コウではないがコウの候補にはなる                  */
		else if (next_color == color && dame_count >= 2) {
			safe_count++;
		}
	}

	//std::cout << "test1" << std::endl;

	//自殺手
	if (space_count == 0 && safe_count == 0 && take_count == 0)return false;
	if (z == /*BoardManager::KoPoint_*/ *KoPoint_Parallel)return false;
	if (kabe_count + safe_count == 4)return false;//欠け目のときでただちに埋める必要がない場合(例下)
												  //std::cout << "test1.5" << std::endl;
	if (Board_Paralelle[z] != SPACE_)return false;
																										 /*           */    /*        　*/   /*          */
																										 /*     ●    */    /*        　*/   /*          */
																										 /*   ●  ●  */    /*        ●*/   /*    ●    */
																									     /*     ●    */    /*      ●  */   /*  ●  ●  */

																																			//std::cout << "test2" << std::endl;
	for (i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];
		int dame_count = Around[i][0];
		int connected_stone_count = Around[i][1];
		int nextcolor = Around[i][2];
		//std::cout << "駄目の数 : " << dame_count << std::endl;//
		//std::cout << "繋がっている石の数 : " << connected_stone_count << std::endl;
		//std::cout << "次の色 : " << nextcolor << std::endl;
		//std::cout << "次の色 : " << reverse_color << std::endl;

		//考えている方向の石を取ることが出来る場合
		if (nextcolor == reverse_color && dame_count == 1 && Board_Paralelle[next_z]) {
			//std::cout << "石とるでーーーーーーー---------" << std::endl;
			BoardManager::RemoveStone_Paralelle(next_z, reverse_color, Board_Paralelle);//----------------------------------------------------------------------------------------------この関数も並列化用に書き換えた方がいいかも-->変更済み
			BoardManager::GetStone_[color - 1] += connected_stone_count;//----------------------------------------------------------------------------------GetStoneも並列化用に変更した方がいいかも
		}
	}

	//ここまできてやっと置ける
	//std::cout << color << std::endl;
	Board_Paralelle[z] = color;//-------------------------------------ここに石の数値以外が入ってしまう
									//std::cout << "石を" << z << "に置きます" << color << std::endl;
									//std::cout << "Board_[" << z << "] : " << BoardManager::Board_[z] << std::endl;
									//BoardManager::ShowBoard();


									//のこりはとった後のことを考える
	int dame_count = 0;
	int stone_count = 0;
	BoardManager::CountDame_Parallel(z, &dame_count, &stone_count, Board_Paralelle);//--------------------------------------------------------------------------------------------------この関数も並列化用に書き換える->完了

	//コウの場合(connected_stone_countはBoard[z]自身も含むことに注意)
	//connected_stone_count == 1ということは石が1個で孤立しているということ
	if (stone_count == 1 && dame_count == 1 && take_count == 1) {                 //左の状況(これは完全なコウ)    /*     ●〇         */
		/*BoardManager::KoPoint_*/ *KoPoint_Parallel = KOPOINT2;                                                  /*   ●●  〇       */
	}                                                                                                             /*     ●〇         */
	else {                                                                                                        /*                  */
		/*BoardManager::KoPoint_*/ *KoPoint_Parallel = 0;
	}

	//無事におけました!
	//std::cout << "PutStone終了" << std::endl;
	return true;

}


//指定した色の石と繋がっている石をすべて取り除く
void BoardManager::RemoveStone(int z, int RemoveColor) {

	BoardManager::Board_[z] = SPACE_;

	for (int i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];
		
		if (BoardManager::Board_[next_z] == RemoveColor) {
			BoardManager::RemoveStone(next_z, RemoveColor);
		}
	}
}


//並列化用のRemoveStone()
void BoardManager::RemoveStone_Paralelle(int z, int RemoveColor, int* Board_Paralelle) {
	/*BoardManager::Board_[z]*/ Board_Paralelle[z] = SPACE_;

	for (int i = 0; i < 4; i++) {
		int next_z = z + BoardManager::DIRECTION_[i];

		if (/*BoardManager::Board_[next_z]*/ Board_Paralelle[z] == RemoveColor) {
			BoardManager::RemoveStone_Paralelle(next_z, RemoveColor, Board_Paralelle);
		}
	}
}

//駄目は石の呼吸店としてか考える
//now_zにおける駄目の数と繋がっている石の総数を求める
void BoardManager::CountDame(int now_z, int* dame_count, int* stone_count) {

	
	//*dame_count = 0;
	//*stone_count = 0;

	//CheckBoardをfalseで初期化する->探索済みだとtrueになる
	for(int i = 0; i < WIDTH_*WIDTH_; i++){
		BoardManager::CheckBoard_[i] = false;
	}

	//位置now_zでの医師の色を求める
	int color = BoardManager::Board_[now_z];

	BoardManager::CountDameSub(now_z, color, dame_count, stone_count);
}

//自分と同じ石が見つかったら再帰によって調べればいい
void BoardManager::CountDameSub(int now_z, int color, int* dame_count, int* stone_count) {
	//std::cout << "BoardManager::CountDameSub" << std::endl;
	//フラグを立てる
	BoardManager::CheckBoard_[now_z] = true;
	(*stone_count)++;//-----------------------------------------------------*stone_count++ だと失敗する+が優先らしい
					 //std::cout << "石がある" << *stone_count << std::endl;
	int i;
	for (i = 0; i < 4; i++) {

		int next_z = now_z + BoardManager::DIRECTION_[i];

		//すでに見たことあるか確認
		if (BoardManager::CheckBoard_[next_z])continue;

		//駄目
		if (BoardManager::Board_[next_z] == 0) {
			//std::cout << "駄目がある" << *dame_count << std::endl;
			(*dame_count)++;
			BoardManager::CheckBoard_[next_z] = true;
		}

		//自分と同じ色の石があれば再帰をしてその石の上下左右についても調べる
		if (BoardManager::Board_[next_z] == color) {
			//std::cout << "再帰" << std::endl;
			BoardManager::CountDameSub(next_z, color, dame_count, stone_count);
		}

	}

}

//並列化用のCountDame()
void BoardManager::CountDame_Parallel(int now_z, int* dame_count, int* stone_count, int* Board_Parallel) {

	//*dame_count = 0;
	//*stone_count = 0;

	//CheckBoardをfalseで初期化する->探索済みだとtrueになる
	//並列化用の場合ここでCheckBoardを定義してもいいかもしれない
	//newで確保したらdelete忘れるな
	bool* CheckBoard_Parallel = new bool[121];

	for (int i = 0; i < WIDTH_*WIDTH_; i++) {
		//----------------------------------------------------------------------------------------------この変数も並列化用に変更する必要がある
		//----------------------------------------------------------------------------------------------すべてのマスをfalseで初期化
		/*BoardManager::CheckBoard_[i]*/ CheckBoard_Parallel[i] = false;
	}

	//位置now_zでの医師の色を求める---------------------------------並列化ように変更
	int color = Board_Parallel[now_z]; /*BoardManager::Board_[now_z]*/

	
	BoardManager::CountDame_Parallel_Sub(now_z, color, dame_count, stone_count, Board_Parallel, CheckBoard_Parallel);//----------------------------------------------------ここも並列化用に変更

	//確保したら解放することを忘れるな
	delete[] CheckBoard_Parallel;
	CheckBoard_Parallel = NULL;
}


//並列化用のCountDameSub()
void BoardManager::CountDame_Parallel_Sub(int now_z, int color, int* dame_count, int* stone_count, int* Board_Parallel, bool* CheckBoard_Parallel) {
	//std::cout << "BoardManager::CountDameSub" << std::endl;
	//フラグを立てる
	//BoardManager::CheckBoard_[now_z] = true;
	CheckBoard_Parallel[now_z] = true;
	(*stone_count)++;//-----------------------------------------------------*stone_count++ だと失敗する+が優先らしい
	//std::cout << "石がある" << *stone_count << std::endl;
	int i;
	for (i = 0; i < 4; i++) {

		int next_z = now_z + BoardManager::DIRECTION_[i];
		
		//すでに見たことあるか確認
		if (/*BoardManager::CheckBoard_[next_z]*/ CheckBoard_Parallel[next_z])continue;
		
		//駄目
		if (/*BoardManager::Board_[next_z]*/ Board_Parallel[next_z] == 0) {
			//std::cout << "駄目がある" << *dame_count << std::endl;
			(*dame_count)++;
			/*BoardManager::CheckBoard_[next_z]*/ CheckBoard_Parallel[next_z] = true;
		}

		//自分と同じ色の石があれば再帰をしてその石の上下左右についても調べる
		if (/*BoardManager::Board_[next_z]*/ Board_Parallel[next_z] == color) {
			//std::cout << "再帰" << std::endl;
			BoardManager::CountDame_Parallel_Sub(next_z, color, dame_count, stone_count, Board_Parallel, CheckBoard_Parallel);
		}

	}

}

int BoardManager::GetBoardSize() {
	return BoardManager::BOARDSIZE_;
}


void BoardManager::PlayerPutStone(int x, int y, int player_color) {
	int z = y * WIDTH_ + x;
	BoardManager::Board_[z] = player_color;
}


