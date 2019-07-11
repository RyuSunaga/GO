/*
ver2ではUCT_IGOのモンテカルロ木探索で並列化を実装する
ver3では他のファイルも含めいままでの実装で作った余計なコードを削り無駄な計算を省くことでさらに速くする
*/

//囲碁プログラムメイン
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
		uct->BM->ShowBoard();
		cout << "盤の左から何列目に置きたい?(パスなら0を選択)" << flush;
		cin >> x;
		cout << endl;
		cout << "盤の上から何行目に置きたい(パスなら0を選択)?" << flush;
		cin >> y;
		cout << endl;

		if (uct->BM->Board_[y * uct->BM->WIDTH_ + x] != uct->BM->SPACE_)continue;
	

		if (x == 0 && y == 0) {
			cout << "パスを選択する" << endl;
			//*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
			break;
		}
		if (x <= 1 || x <=  9|| y <= 1 || y <= 9 /* || uct->BM->Board_[y*uct->BM->WIDTH_ + x] != uct->BM->SPACE_*/) {
			//cout << "そこには置けん！" << endl;
			break;
		}
	}
	//メモ
	//MemoRand.push_back(y*uct->BM->WIDTH_ + x);

	cout << y * uct->BM->WIDTH_ + x << "に石を置きました" << endl;
	//uct->BM->PlayerPutStone(x, y, *turn_color);
	//*turn_color = (*turn_color == uct->BM->WHITE_) ? uct->BM->BLACK_ : uct->BM->WHITE_;
	//uct->BM->ShowBoard();
	return (y*uct->BM->WIDTH_ + x);
	
}





//AI同士の戦い(片方並列化)
int main4() {


	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 15; k++) {


		UCT GAME = UCT();


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
			if (turn_color == GAME.BM->WHITE_) {
				cout << "AI1(黒)のターン" << endl;

				//75000回だと100秒弱
				now_z = GAME.SelectActionUCT(turn_color, 10000);
				//now_z = PlayerAction(&GAME, &turn_color);
			}
			//AI2のターンの場合
			else {
				cout << "AI2(白)のターン" << endl;
				now_z = GAME.SelectActionUCT_Paralelle2(turn_color, 25000);
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

//AI同士の戦い(片方並列化)
int main5() {


	int WHITE_WIN_COUNT = 0;
	int BLACK_WIN_COUNT = 0;
	int ERROR_COUNT = 0;

	for (int k = 0; k < 30; k++) {


		UCT GAME = UCT();


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

		int GET1 = 0;
		int GET2 = 0;

		while (true) {

			GAME.BM->total_playout_ = 0;

			//-------------------------------------------------------------------------------------------------------------並列化の場合毎ターン、コンストラクタとデストラクタが呼び出されるので改善しないといけない
			//-------------------------------------------------------------------------------------------------------------ここの並列化はsectionを使ったクラスのコピーを使った方がいいかも
			//AI1のターンの場合
			if (turn_color == GAME.BM->BLACK_) {
				cout << "Playerのターン" << endl;
				now_z = PlayerAction(&GAME, &turn_color);
				//now_z = GAME.SelectActionUCT(turn_color, 20000);
			}
			//AI2のターンの場合
			else {
				cout << "AI2(白)のターン" << endl;
				//--------------------------------------75000回で167秒
				now_z = GAME.SelectActionUCT_Paralelle2(turn_color, 50000);
			}


			cout << "次の手" << now_z << endl;
			cout << "白のハマ " << GET2 << endl;
			cout << "黒のハマ " << GET1 << endl;

			
		
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
				cout << "エラー！" << endl;
				ERROR_COUNT++;
				cout << "現在" << endl;
				cout << "AI1(黒) " << BLACK_WIN_COUNT << "勝" << endl;
				cout << "AI2(白) " << WHITE_WIN_COUNT << "勝" << endl;
				cout << "エラー回数 " << ERROR_COUNT << "回" << endl;
				break;
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

	return main5();


}
