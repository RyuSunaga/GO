/*
ver2ではUCT_IGOのモンテカルロ木探索で並列化を実装する
*/

//囲碁プログラムメイン
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
//メモリーリーク発見用
#include "crtdbg.h"
#define malloc(X) _malloc_dbg(X._NORMAL_BLOCK,__FILE__,__LINE__)
#define new ::new(_NORMAL_BLOCK,__FILE__,__LINE__)
using namespace std;

random_device rnd;     // 非決定的な乱数生成器を生成
mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
uniform_int_distribution<> rand10(0, 9);        // [0, 9] 範囲の一様乱数

//AI1の打った手をメモする
vector<int> MemoAI1;

//AI2の打った手をメモする
vector<int> MemoAI2;

//randの打った手をメモする
vector<int> MemoRand;


int WHITE_WIN_COUNT = 0;
int BLACK_WIN_COUNT = 0;


//パスなら0を返すパス以外なら1を返す
int PlayerAction(UCT* uct, int* turn_color) {

	int x, y;
	while (true) {
		cout << "盤の左から何列目に置きたい?(パスなら0を選択)" << flush;
		cin >> x;
		cout << endl;
		cout << "盤の上から何行目に置きたい(パスなら0を選択)?" << flush;
		cin >> y;
		cout << endl;
		if (x == 0 && y == 0) {
			cout << "パスを選択する" << endl;
			*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			return 0;
		}
		if (x < 1 || 9 < x || y < 1 || 9 < y /* || uct->BM->Board_[y*uct->BM->WIDTH_ + x] != uct->BM->SPACE_*/ ) {
			cout << "そこには置けん！" << endl;
			continue;
		}
		//メモ
		MemoRand.push_back(y*uct->BM->WIDTH_ + x);

		if (uct->BM->PutStone(y*uct->BM->WIDTH_ + x, *turn_color)) {
			cout << y * uct->BM->WIDTH_ + x << "に石を置きました" << endl;
			uct->BM->PlayerPutStone(x, y, *turn_color);
			*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			//uct->BM->ShowBoard();
			return 1;
		}
		else {
			cout << "石を置けませんでした" << endl;
			cout << "もう一回選んでください" << endl;
		}
	}
}



//乱数プレイヤー
//パスなら0を返すパス以外なら1を返す
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
		//メモ
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




//AI同士の戦い(こっちの方がうまくいくかも)
int main3() {

	//並列化テスト
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;
	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;

	for (int k = 0; k < 50; k++) {

		chrono::system_clock::time_point start, end;
		//計測スタート
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//先行
		int turn_color = GAME.BM->BLACK_;
		//現在がプレイヤーのターンならばtrue;
		bool IsPlayerTurn = true;

		//今まで打った手の数を数える
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "AI1 黒" << endl;
		cout << "AI2 白" << endl;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------並列化の場合毎ターン、コンストラクタとデストラクタが呼び出されるので改善しないといけない
			//-------------------------------------------------------------------------------------------------------------ここの並列化はsectionを使ったクラスのコピーを使った方がいいかも
			//AI1のターンの場合
			if (turn_color == GAME.BM->BLACK_) {
				cout << "AI1(黒)のターン" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 3000);
			}
			//AI2のターンの場合
			else {
				cout << "AI2(白)のターン" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 15000);
			}


			cout << "次の手" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "エラー！" << endl;
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


			//終局判定
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color, true);
				cout << "対局終了" << endl;
				if (GAME.BM->IsBlackWin_) {
					BLACK_WIN_COUNT++;
				}
				else {
					WHITE_WIN_COUNT++;
				}
				break;
			}

			before_z = now_z;

			//色を変える-----------------------------------------------
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

			outputfile << "勝敗:" << flush;
			if (GAME.BM->IsBlackWin_) {
			outputfile << "黒の勝ち" << endl;

			BLACK_WIN_COUNT++;
			}
			else {
			outputfile << "白の勝ち" << endl;;

			WHITE_WIN_COUNT++;
			}

			outputfile << "点数:" << flush;
			outputfile << GAME.BM->game_score_ << endl;;


			cout << "AI1" << endl;
			outputfile << "AI1の棋譜" << endl;
			for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI1.clear();
			cout << endl;
			cout << "AI2" << endl;
			outputfile << "AI2の棋譜" << endl;
			for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI2.clear();
			cout << endl;

			end = chrono::system_clock::now();
			double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			cout << "計測時間 : " << elapsed << endl;
			outputfile << "計測時間" << endl;
			outputfile << elapsed << endl;


			outputfile.close();

			//delete[] file_num;
			*/
		}
	}

	cout << "対戦結果" << endl;
	cout << "AI1(黒) " << BLACK_WIN_COUNT << "勝" << endl;
	cout << "AI2(白) " << WHITE_WIN_COUNT << "勝" << endl;



	return 0;
	//まさかの白が50戦,50勝
}


//AI同士の戦い(片方並列化)
int main4() {

	//並列化テスト
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;
	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 10; k++) {

		chrono::system_clock::time_point start, end;
		//計測スタート
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//先行
		int turn_color = GAME.BM->BLACK_;
		//現在がプレイヤーのターンならばtrue;
		bool IsPlayerTurn = true;

		//今まで打った手の数を数える
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "AI1 黒" << endl;
		cout << "AI2 白" << endl;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------並列化の場合毎ターン、コンストラクタとデストラクタが呼び出されるので改善しないといけない
			//-------------------------------------------------------------------------------------------------------------ここの並列化はsectionを使ったクラスのコピーを使った方がいいかも
			
			//AI1のターンの場合
			if (turn_color == GAME.BM->BLACK_) {
				cout << "AI1(黒)のターン" << endl;
				now_z = GAME.SelectActionUCT(turn_color, 70000);
			}
			//AI2のターンの場合
			else {
				cout << "AI2(白)のターン" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME, turn_color, 12500);
			}


			cout << "次の手" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "エラー！" << endl;
				ERROR_COUNT++;
				cout << "現在" << endl;
				cout << "AI1(黒) " << BLACK_WIN_COUNT << "勝" << endl;
				cout << "AI2(白) " << WHITE_WIN_COUNT << "勝" << endl;
				cout << "エラー回数 " << ERROR_COUNT << "回" << endl;
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


			//終局判定
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color, true);
				cout << "対局終了" << endl;
				if (GAME.BM->IsBlackWin_) {
					BLACK_WIN_COUNT++;
				}
				else {
					WHITE_WIN_COUNT++;
				}

				cout << "現在" << endl;
				cout << "AI1(黒) " << BLACK_WIN_COUNT << "勝" << endl;
				cout << "AI2(白) " << WHITE_WIN_COUNT << "勝" << endl;
				cout << "エラー回数 " << ERROR_COUNT << "回" << endl;

				break;
			}

			before_z = now_z;

			//色を変える-----------------------------------------------
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

			outputfile << "勝敗:" << flush;
			if (GAME.BM->IsBlackWin_) {
			outputfile << "黒の勝ち" << endl;

			BLACK_WIN_COUNT++;
			}
			else {
			outputfile << "白の勝ち" << endl;;

			WHITE_WIN_COUNT++;
			}

			outputfile << "点数:" << flush;
			outputfile << GAME.BM->game_score_ << endl;;


			cout << "AI1" << endl;
			outputfile << "AI1の棋譜" << endl;
			for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI1.clear();
			cout << endl;
			cout << "AI2" << endl;
			outputfile << "AI2の棋譜" << endl;
			for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
			}
			outputfile << endl;
			MemoAI2.clear();
			cout << endl;

			end = chrono::system_clock::now();
			double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			cout << "計測時間 : " << elapsed << endl;
			outputfile << "計測時間" << endl;
			outputfile << elapsed << endl;


			outputfile.close();

			//delete[] file_num;
			*/
		}
	}

	cout << "対戦結果" << endl;
	cout << "AI1(黒) " << BLACK_WIN_COUNT << "勝" << endl;
	cout << "AI2(白) " << WHITE_WIN_COUNT << "勝" << endl;
	cout << "エラー回数 " << ERROR_COUNT << "回" << endl;


	return 0;
}


int main() {

	return main4();



	//並列化テスト
	//#pragma omp parallel num_threads(2)
	//cout << "Hello" << endl;
	//return 0;

	for (int k = 1; k <= 1; k++) {

		chrono::system_clock::time_point start, end;
		//計測スタート
		start = chrono::system_clock::now();

		srand((unsigned)time(NULL));

		UCT GAME = UCT();
		//UCT GAME1 = UCT();
		//UCT GAME2 = UCT();


		//先行
		int turn_color = GAME.BM->BLACK_;
		//現在がプレイヤーのターンならばtrue;
		bool IsPlayerTurn = true;

		//今まで打った手の数を数える
		int select_count = 0;

		//GAME->BM->ShowBoard();
		int before_z = -1;
		int now_z = -1;
		bool IsPutOK;

		cout << "プレイヤー 黒" << endl;
		cout << "AI 白" << endl;



		//コンピュータ同士の戦い--------------------------------------------------------------UCTクラスを2つ作らないと駄目かも
		//2回連続パスが続けば対局終了
		while (true) {

			//GAME->BM->ShowBoard();

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------並列化の場合毎ターン、コンストラクタとデストラクタが呼び出されるので改善しないといけない
			//-------------------------------------------------------------------------------------------------------------ここの並列化はsectionを使ったクラスのコピーを使った方がいいかも
			//AI1のターンの場合
			if (turn_color == GAME.BM->BLACK_)now_z = GAME.SelectActionUCT(turn_color, 5000);
			//if (turn_color == GAME.BM->BLACK_)now_z = GAME.SelectActionUCT_Paralelle(turn_color,10000);//並列化版
			//AI2のターンの場合
			else {
				cout << "GAME.SelectActionUCT_Paralelle2を呼びます " << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME, turn_color, 5000);
			}
			//else now_z = GAME.SelectActionUCT(turn_color, 5000);
			//else now_z = GAME.SelectActionUCT_Paralelle(turn_color, 10000);//並列化版
			//else now_z = GAME.SelectActionUCT(turn_color, 10000);


			cout << "次の手" << now_z << endl;

			IsPutOK = GAME.BM->PutStone(now_z, turn_color);

			if (IsPutOK == false) {
				cout << "エラー！" << endl;
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
				cout << "対局終了" << endl;
				break;
			}

			before_z = now_z;

			//色を変える-----------------------------------------------
			turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
			//cout << "turn_color : " << turn_color << endl;
		}


		/*
		//AIと人間の場合
		while (true) {
			if (IsPlayerTurn) {
				cout << "プレイヤーのターン" << endl;

				now_z = PlayerActionRand(&GAME, &turn_color);
				//now_z = PlayerAction(&GAME, &turn_color);


				IsPlayerTurn = !IsPlayerTurn;
			}
			else {
				cout << "AIのターン" << endl;

				//GAME.BM->total_playout_ = 0;
				//----------------------------------------------------------------------並列化なしで30000を超えると止まってしまう
				//now_z = GAME.SelectActionUCT(turn_color,30500);
				//cout << (now_z/9) << now_z % 9 << "に置きます" << endl;
				//MemoAI2.push_back(now_z);
				//cout << "次の手" << now_z << endl;

				//----------------------------------------------------------------------------parallel2
				cout << "GAME.SelectActionUCT_Paralelle2を呼びます" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(GAME ,turn_color, 2500);

				//メモ
				//MemoAI1.push_back(now_z);

				IsPutOK = GAME.BM->PutStone(now_z, turn_color);
				if (IsPutOK == false) {
					GAME.BM->ShowBoard();
					cout << "エラー！" << endl;

					cout << "プレイヤー" << endl;
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

					cout << "計測時間 : " << elapsed << endl;

					int n;
					cout << "続ける？（Yes->1、NO->0）" << endl;
					cin >> n;
					if (n == 1)continue;
					else break;
				}

				//cout << "OK" << endl;

				//色を変える-----------------------------------------------
				turn_color = (turn_color == GAME.BM->BLACK_) ? GAME.BM->WHITE_ : GAME.BM->BLACK_;
				IsPlayerTurn = !IsPlayerTurn;
			}

			//GAME.BM->ShowBoard();

			//終局判定
			if (before_z == 0 && now_z == 0) {
				GAME.BM->WhichColorWin(turn_color,true);
				cout << "対局終了" << endl;
				break;
			}
			before_z = now_z;
		}
		*/

		cout << "エラー！" << endl;

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

		outputfile << "勝敗:" << flush;
		if (GAME.BM->IsBlackWin_) {
			outputfile << "黒の勝ち" << endl;

			BLACK_WIN_COUNT++;
		}
		else {
			outputfile << "白の勝ち" << endl;;

			WHITE_WIN_COUNT++;
		}

		outputfile << "点数:" << flush;
		outputfile << GAME.BM->game_score_ << endl;;


		cout << "AI1" << endl;
		outputfile << "AI1の棋譜" << endl;
		for (auto r : MemoAI1) {
			cout << r << " " << flush;
			outputfile << r << " ";
		}
		outputfile << endl;
		MemoAI1.clear();
		cout << endl;
		cout << "AI2" << endl;
		outputfile << "AI2の棋譜" << endl;
		for (auto r : MemoAI2) {
			cout << r << " " << flush;
			outputfile << r << " ";
		}
		outputfile << endl;
		MemoAI2.clear();
		cout << endl;

		end = chrono::system_clock::now();
		double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		cout << "計測時間 : " << elapsed << endl;
		outputfile << "計測時間" << endl;
		outputfile << elapsed << endl;


		outputfile.close();

		//delete[] file_num;
		*/

		cout << "Address BM" << GAME.BM << endl;
		cout << "GAMEのデリーと完了" << endl;
	}

	cout << "最終結果" << endl;
	cout << "AI1(黒) : " << BLACK_WIN_COUNT << "回" << endl;
	cout << "AI2(白) : " << WHITE_WIN_COUNT << "回" << endl;

	return 0;

}
