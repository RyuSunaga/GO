//盤を管理するクラス

#ifndef __BOARD_H_INCLUDED__
#define __BOARD_H_INCLUDED__
#include <vector>




class BoardManager {

public:
	int total_playout_;//プレイアウトの回数

					   //ボードの状態を表す
					   //インデックスは座標を次元圧縮したものを
					   //0->空白,1->白,2->黒,3->壁
	int Board_[121];
	//int* Board_;------------------------------------------------------------------------動的配列を確保した方がいいのかな？

	const int SPACE_ = 0;
	const int WHITE_ = 1;
	const int BLACK_ = 2;
	const int WALL_ = 3;

	const int BOARDSIZE_ = 9;//9路盤だから
	const int WIDTH_ = 11;//対称性を扱いやすいように11にする
	const int BOARDPOINT_ = WIDTH_ * WIDTH_;//ボードの点の数-----------------------------------------------これはいらないかも
	const int DIRECTION_[4] = { /*WIDTH_*/11,/*-WIDTH_*/-11,-1,1 };

	int KoPoint_;//コウで打つことが出来ない位置(一次元に圧縮した座標を使う)


				 //対局の結果をメモするための変数
	bool IsBlackWin_;


private:

	//初期ボード
	//3->壁,0->空白
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

	//ボードの各マスを見るときにすでに見た点かどうかを判断するのに使う
	bool CheckBoard_[121];
	//std::vector<bool> CheckBoard_(121,false);
public:
	//白はゲーム終了時6.5分増える
	const double KOMI_ = 6.5;
	int GetStone_[2];//0->白,1->黒

					 //ボードに表示する文字
					 //const char Cha_[3] = { '.','○','●' };


public:

	BoardManager();

	~BoardManager();

	//コピーコンストラクタ
	BoardManager(const BoardManager &bm);

	//代入演算子オーバーロード
	BoardManager &operator=(const BoardManager &bm);


	//現在のプレイアウトの回数を得る
	int Get_total_playout();

public:
	//現在のボードを表示する
	void ShowBoard();



public:
	//プレイアウトを開始した手番が勝ったかどうか判定する
	//白が勝ち->0,白が負け->-1
	//黒が負け->0,黒が勝ち->1,
	int WhichColorWin(int nowturn_color, bool IsShowData);


public:
	//失敗したらfalseを返す、それ以外ならばtrueを返す
	bool PutStone(int z, int color);


	//ボードのサイズを返す
	int GetBoardSize();

public:
	//石を取り除く関数
	void RemoveStone(int z, int color);



public:
	//駄目の数を数える
	void CountDame(int now_z, int* dame_count, int* stone_count);

private:
	//CountDame()の内部
	void CountDameSub(int now_z, int color, int* dame_count, int* stone_count);







};

#endif



