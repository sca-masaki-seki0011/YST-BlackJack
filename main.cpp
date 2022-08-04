//今回は通常のブラックジャックの内容に加えて
//自分たちで独自に考えた内容も含まれます。

#include <algorithm> //ソート
#include <iostream>//入出力
#include <random> //乱数生成
#include <vector> //可変長配列
using namespace std;

#define DECK 6  //使用するデッキ数
#define CARD 52 //トランプのカードの数(ジョーカーを除く)
#define NUM DECK *CARD //カードの総数




//カードショークラス
class Shoe {

private:
  //基本的にカードシューはprivate
  int card_shoe[NUM]; //カード配列
  int card_num;       //カード枚数

public:
  void show_shoe(bool type);//カードシューを表示(デバッグ用)
  int take_card();//カードを1枚取り出す
  
  //コンストラクタ
  Shoe() {
    //配列を全て-1で初期化
    for (int i = 0; i < NUM; i++) {
      //-1の代入
      card_shoe[i] = -1;
    }
    //乱数生成器
    random_device rnd;
    //配列にカードを順番に格納(iのカードをどこに挿入するかを決める)
    for (int i = 0; i < NUM; i++) {
      // iのカードが挿入されるまでループ
      while (1) {
        //乱数によって挿入位置を選定
        int place = rnd() % (NUM); //挿入位置
        //乱数で決めた挿入位置が空であるか確認
        if (card_shoe[place] == -1) {
          //カードの格納
          card_shoe[place] = i;
          break; //ループを抜ける
        }
      }
    }
    card_num = NUM; //カード枚数の設定
  }
};

//カードシューを表示(デバッグ用)
void Shoe::show_shoe(bool type) {
  // typeで表示範囲を操作(trueならカード枚数分表示)
  if (type) {
    //カード配列を先頭から表示する
    for (int i = 0; i < card_num; i++) {
      //標準出力
      cout << card_shoe[i] << ' ';
    }
    //改行
    cout << endl;
    // falseなら配列全体
  } else {
    //カード配列を先頭から表示する
    for (int i = 0; i < NUM; i++) {
      //標準出力
      cout << card_shoe[i] << ' ';
    }
    //改行
    cout << endl;
  }
}
//カードを1枚取り出す
int Shoe::take_card() {
  //カード枚数が0だった場合,エラーとして出力
  if (card_num == 0) {
    //標準出力
    cout << "NoStackError\nShoe.card_num" << endl;
    return -1; //返り値として-1
  }
  //カード配列の先頭を格納
  int top = card_shoe[0];
  //カード配列を先頭から順に変更
  for (int i = 0; i < card_num - 1; i++) {
    //要素を1つずつずらす
    card_shoe[i] = card_shoe[i + 1];
  }
  //必要なくなった範囲には-1を代入
  card_shoe[card_num - 1] = -1;
  card_num--; //カード枚数を1減らす

  return top; //先頭にあった要素を返り値とする
}

//プレイヤークラス
class Player {

public:
  vector<int> hand; //手札配列
  int hand_num;     //手札枚数
  int kakekin;//掛け金の変数
  int calc();//playerのスコアをかえす
  void hit(Shoe *shoe);//ヒットの実装
  void DoubleUp(Shoe *shoe);//ダブルアップの実装
  int Dokake(int *ke);//ダブルアップの掛け金の倍率
  void show_hand();//playerのカードを見せる
  bool play(Shoe *shoe);//playerが入力した内容の処理
  //コンストラクタ
  Player() {
    //変数の初期化
    hand_num = 0; //手札枚数を0で初期化
  }
};

//スコアの計算
int Player::calc() {

  //メモリの動的確保
  int *data;                //配列用ポインタ
  data = new int[hand_num]; //手札枚数確保
  //手札配列からデータを取得,1-10の値として格納
  for (int i = 0; i < hand_num; i++) {
    //配列の値が10以下(0-9)であれば1を加えて格納
    if (hand[i] % 13 < 10) {
      // 1-13のカードとして扱うために1加えている
      data[i] = hand[i] % 13 + 1;
      //配列の値が10以上(10-12)であれば10を格納
    } else {
      //ここで10を格納しているのは,11-13のカードのことである
      data[i] = 10;
    }
  }

  //ソート
  sort(data, data + hand_num);
  //返り値用変数(計算結果)
  int score = 0;
  // 2番目から順にスコアを足していく(1番目は判定が必要)
  for (int i = 1; i < hand_num; i++) {
    //スコアにデータを加える
    score += data[i];
  }
  //先頭が1であるか判定
  if (data[0] == 1) {
    // 1をどう扱うか判定する
    if (score + 11 < 22) { // 11として扱ってもバーストしない場合
      // 11を加える
      score += 11;
      // 11として扱うとバーストする場合
    } else {
      //スコアに1を加える
      score += 1;
    }
    //先頭が1でないなら,スコアにデータをそのまま加える
  } else {
    //スコアにデータを加える
    score += data[0];
  }
  if(score == 21)
  {
    score = 21;
  }
  //バーストである場合は0を返す
  if (score > 21) {
    //スコアに0を代入
    score = 0;
  }
  
  delete[] data; //メモリ開放
  return score;   //計算結果を返す
}

//カードの追加
void Player::hit(Shoe *shoe) { // shoeオブジェクトポインタ
  //配列の最後にカードを追加する
  hand.push_back(shoe->take_card());
  hand_num++; //手札枚数を増やす
}

//ダブルアップ関連
//ダブルアップの掛け金の倍率
//doubleupで勝利した場合は
//その勝った枚数の倍率かけるdoubleupの倍率となる
int Player::Dokake(int *ke) {
  *ke = *ke * 4;
  return *ke;
}

//ダブルアップの実装(引き分けになっても掛け金の倍率は実行される)
void Player::DoubleUp(Shoe *shoe) {
  hand.push_back(shoe->take_card());
  hand_num++; //手札枚数を増やす

  Dokake(&kakekin);
}



//手札を表示
void Player::show_hand() {
  //標準出力
  cout << "hand: ";
  //配列の最初から最後までを順に表示
  for (int i = 0; i < hand_num; i++) {
    //カードの種類(スペード,ハート,ダイヤ,クラブ)を探索
    int type = (hand[i] % 52) /
               13; //デッキごとに分けた後,13で割った数(0-3)で4種類を分割
    //分岐条件にtypeをそのまま利用
    switch (type) {
    // 0であればスペードのs
    case 0:
      cout << 's';
      break;
    // 1であればハートのh
    case 1:
      cout << 'h';
      break;
    // 2であればダイヤのd
    case 2:
      cout << 'd';
      break;
    // 3であればクラブのc
    case 3:
      cout << 'c';
      break;
    // 0-3でない場合はエラー処理
    default:
      //標準出力
      cout << "DefTypeError\nPlayer.show_hand" << endl;
      break;
    }
    //標準出力
    cout << hand[i] % 13 + 1 << ' ';
  }
  //改行
  cout << endl;
  cout << "score: " << calc() << endl; //スコアの表示
}

//ゲーム実行
bool Player::play(Shoe *shoe) {
  if(calc() == 21)
  {
    return true;
  }
    //入力
    string str; //文字列型
  //バーストするまでループ処理
  while (calc()) {
   
    cin >> str; //文字列を入力
    // hitが入力された場合
    if (str == "hit") {
      //カードの配布
      hit(shoe);
      //手札の表示
      cout << "====================" << endl;
      cout << "player" << endl;
      show_hand();
      cout << "====================" << endl;
      //BJ(ブラックジャックの判定)
      if(calc() == 21)
      {
        return true;
      }
    }

    // standが入力された場合
    else if (str == "stand") {
      //返り値をtrueとして終了
      return true;
    } 
      
    //ダブルアップの実装
    else if (str == "doubleup") {
        DoubleUp(shoe);
      cout << "====================" << endl;
      cout << "player" << endl;
      show_hand();
      cout << "====================" << endl;
      //BJ(ブラックジャックの判定)
       if(calc() == 21)
      {
        return true;
      }
      return true;
      }
  }
  //バーストしているのでfalseを返して終了
  return false;
}

//ディーラークラス
class Dealer {

private:
  //手札関連はprivateとする
  vector<int> hand; //手札配列
  int hand_num;     //手札枚数

public:
  int calc();//dealerのスコアを返す
  void hit(Shoe *shoe);//dealerのhit
  void show_hand();//dealerのカードを表示
  void play(Shoe *shoe);//dealerのゲーム実行
  //コンストラクタ
  Dealer() {
    //変数の初期化
    hand_num = 0; //手札枚数を0で初期化
  }
};
//スコアの計算
int Dealer::calc() {
  //メモリの動的確保
  int *data;                //配列用ポインタ
  data = new int[hand_num]; //手札枚数確保
  //手札配列からデータを取得,1-10の値として格納
  for (int i = 0; i < hand_num; i++) {
    //配列の値が10以下(0-9)であれば1を加えて格納
    if (hand[i] % 13 < 10) {
      // 1-13のカードとして扱うために1加えている
      data[i] = hand[i] % 13 + 1;
      //配列の値が10以上(10-12)であれば10を格納
    } else {
      //ここで10を格納しているのは,11-13のカードのことである
      data[i] = 10;
    }
  }
  //ソート
  sort(data, data + hand_num);
  //返り値用変数(計算結果)
  int score = 0;
  // 2番目から順にスコアを足していく(1番目は判定が必要)
  for (int i = 1; i < hand_num; i++) {
    //スコアにデータを加える
    score += data[i];
  }
  //先頭が1であるか判定
  if (data[0] == 1) {
    // 1をどう扱うか判定する
    if (score + 11 < 22) { // 11として扱ってもバーストしない場合
      // 11を加える
      score += 11;
      // 11として扱うとバーストする場合
    } else {
      //スコアに1を加える
      score += 1;
    }
    //先頭が1でないなら,スコアにデータをそのまま加える
  } else {
    //スコアにデータを加える
    score += data[0];
  }
  //バーストである場合は0を返す
  if (score > 21) {
    //スコアに0を代入
    score = 0;
  }

  delete[] data; //メモリの解放
  return score;  //計算結果を返す
}

//カードの追加
void Dealer::hit(Shoe *shoe) { // shoeオブジェクトポインタ
  //配列の最後にカードを追加する
  hand.push_back(shoe->take_card());
  hand_num++; //手札枚数を増やす
}

//手札を表示
void Dealer::show_hand() {
  //標準出力
  cout << "hand: ";
  //配列の最初から最後までを順に表示
  for (int i = 0; i < hand_num; i++) {
    //カードの種類(スペード,ハート,ダイヤ,クラブ)を探索
    int type = (hand[i] % 52) /
               13; //デッキごとに分けた後,13で割った数(0-3)で4種類を分割
    //分岐条件にtypeをそのまま利用
    switch (type) {
    // 0であればスペードのs
    case 0:
      cout << 's';
      break;
    // 1であればハートのh
    case 1:
      cout << 'h';
      break;
    // 2であればダイヤのd
    case 2:
      cout << 'd';
      break;
    // 3であればクラブのc
    case 3:
      cout << 'c';
      break;
    // 0-3でない場合はエラー処理
    default:
      //標準出力
      cout << "DefTypeError\nPlayer.show_hand" << std::endl;
      break;
    }
    //標準出力
    cout << hand[i] % 13 + 1 << ' ';
  }
  //改行
  cout << endl;
  cout << "score: " << calc() << endl; //スコアの表示
}
//ゲーム実行
void Dealer::play(Shoe *shoe) {
  //スコアが16以下の場合hitを続ける
  while (calc() < 17 && calc() > 0) {
    //ヒットする
    hit(shoe);
    cout << "hit" << endl;
    //手札の表示
    cout << "====================" << endl;
    cout << "dealer" << endl;
    show_hand();
    cout << "====================" << endl;
  }
}

//ここが勝敗を決めた後の処理
void score(Player player, Dealer dealer, int *zan) {
  //playerの勝利(ブラックジャック)
  if (player.calc() == 21) {
    cout << "Player BJ Win!!" << endl;
    player.kakekin *= 100;//BJの倍率
    cout << player.kakekin << endl;
    *zan += player.kakekin;
    cout << "現在の残高" << *zan << endl;
  }
  //playerの勝利
  else if (player.calc() > dealer.calc()) {
    cout << "Player Win!" << endl;
    //勝ったときの自分の手札の枚数によって倍率が変わる    
    switch (player.hand_num) {
    case 6://シックスアンダーの実装
      player.kakekin *= 3;
      *zan += player.kakekin;
      cout << "現在の残高" << *zan << endl;
      break;
    case 7://セブンアンダーの実装
      player.kakekin *= 5;
      *zan += player.kakekin;
      cout << "現在の残高" << *zan << endl;
      break;

    case 11://イレブンカード
      player.kakekin *= 20;
      *zan += player.kakekin;
      cout << "現在の残高" << *zan << endl;
      break;
    default:
      *zan += player.kakekin * 2;
      cout << "現在の残高" << *zan << endl;
      break;
    }

  } 
  //playerの負け
  else if (player.calc() < dealer.calc()) {
    cout << "Player Lose" << endl;
    cout << "現在の残高" << *zan << endl;
  }
  //引き分け(てもdoubleupしていれば残高に掛け金の倍率が足される)
  else {
    cout << "Push" << endl;
    cout << "今回は引き分けだ!" << endl;
    *zan+=player.kakekin;
    cout << "現在の残高"<< *zan << endl;
  }
}

//メイン関数
int main() {

  //プレイヤーオブジェクトの生成
  Player player;
  //ディーラーオブジェクトの生成
  Dealer dealer;
  //カードシューオブジェクトの生成
  Shoe shoe;

  //残高
  int zandaka = 100;
  
    //掛け金の入力
    cout << "掛け金を入力してください" << endl;
    cin >> player.kakekin;

    //掛け金の下限上限
    while (player.kakekin <= 0 || player.kakekin == 0 || player.kakekin <= 10) {

      cout << "もう一度掛け金を入力してください" << endl;
      cin >> player.kakekin;
    }
    //残高から掛け金を引く処理
    zandaka = zandaka - player.kakekin;
    cout << "Playerの残金::" << zandaka << endl;

    //カードの配布
    cout << "====================" << endl;
    //プレイヤーに1枚目を配布
    player.hit(&shoe);
    //手札の表示
    cout << "====================" << endl;
    cout << "player" << endl;
    player.show_hand();
    cout << "====================" << endl;

    //ディーラーに1枚目を配布
    dealer.hit(&shoe);
    //手札の表示
    cout << "====================" << endl;
    cout << "dealer" << endl;
    dealer.show_hand();
    cout << "====================" << endl;

    //プレイヤーに2枚目を配布
    player.hit(&shoe);
    //手札の表示
    cout << "====================" << endl;
    cout << "player" << endl;
    player.show_hand();
    cout << "====================" << endl;
  
    //BJ(ブラックジャックの判定)
   if(player.calc() == 21)
   {
     cout << "Player BJ Win!!" << endl;
        player.kakekin *= 100;
        cout << player.kakekin << endl;
        zandaka += player.kakekin;
        cout << "現在の残高" << zandaka << endl;
        cout << "====================" << endl;
        cout << "GAME FINISH" << endl;
        return 0;
   }
  
	  //ディーラーに2枚目を配布
    dealer.hit(&shoe);

    //プレイヤーの実行
    //バーストしているかどうか判別
    if (player.play(&shoe)) { //バーストしていない場合
      //BJ(ブラックジャックの判定)
      if(player.calc() == 21)
      {
        cout << "Player BJ Win!!" << endl;
        player.kakekin *= 100;
        cout << player.kakekin << endl;
        zandaka += player.kakekin;
        cout << "現在の残高" << zandaka << endl;
        cout << "====================" << endl;
        cout << "GAME FINISH" << endl;
        return 0;
      }
      else{
      //ディーラーの手札を表示
      cout << "====================" << endl;
      cout << "dealer" << endl;
      dealer.show_hand();
      cout << "====================" << endl;

      
      //ディーラーの自動実行
      dealer.play(&shoe);
      //結果の表示
      score(player,dealer,&zandaka);  
     }
    }

    //残高が0になったらの処理    
    else if(zandaka == 0)
    {
　　　cout << "====================" << endl;
      cout << "GAME FINISH" << endl;
      return 0;
    }

    //バーストしている場合
    else {
      //標準出力
      cout << "Burst Player Lose" << endl;
      zandaka = 0;
      cout << "現在の掛け金は" << zandaka << endl;
      cout << "GAME OVER" << endl;
    }
    cout << "====================" << endl;
    cout << "GAME FINISH" << endl;
  return 0;
}