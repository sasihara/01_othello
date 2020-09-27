# オセロ(*1)

(*1) オセロ・Othelloは登録商標です。TM＆© Othello,Co. and MegaHouse

## はじめに
オセロゲームです。コンピュータとの対戦が可能です。
なお、このオセロはオーナーがgithubおよびVisual Studioの習熟のため、および最新のAI技術を勉強するために作っています。よって今後はゲームとしての完成度を上げる方向ではなく、実験的な機能が増えていく方向でアップデートしていく予定です。

## 制限事項
週末の趣味程度で開発しているものなので、完成度は期待しないで下さい。

## 推奨環境
Intel Core i5 2.4GHz程度のCPUであれば十分遊べます。

## 開発環境
Microsoft Visual Studio Community 2019での動作を確認しております。それ以外の環境については未確認ですが、基本的なAPIしか使用してないため、他のバージョンでも動作する可能性は高いと思います。

## History
|Version|Release Date|Note|
|-------|------------|--------|
|1.0|2019/12/15|最初のバージョン。コンピュータとの対戦機能付き。コンピュータの思考ルーチンは古典的なミニマックス法を用いたもの。筆者が1988年頃に作ったものをリメイクした程度のもの。パラメータのチューニングはまだほとんど行っていません。外部思考ルーチンにはまだ未対応です。|
|2.0|2020/9/27|異なるバージョン間の思考ルーチン同士で対戦ができるようにするため、思考ルーチン部分だけを別モジュールとした外部思考ルーチンに対応しました。今回サンプルとして格納した外部思考ルーチンのプロジェクトファイルをexternalThinker_v1.slnとして格納しています。このプロジェクトを開いてコンパイルし実行させて下さい。実行させると、外部思考ルーチンはデフォルトでUDPのポート番号60001でメッセージ受信を待機します。オセロ本体と同じマシンで外部思考ルーチンを動作させた場合は、オセロ本体の設定で"Computer(External)"のホスト名に"localhost"、ポート番号に"60001"とセットすると、この外部思考ルーチンを使って思考させることができます。もちろん外部思考ルーチンをネットワークにつながった別のマシン上で動作させても良いです。その場合はホスト名にそのマシンのホスト名を入れれば良いはずです。|
----------------------------

# Othello(*1)
(*1) TM＆© Othello,Co. and MegaHouse

## Introduction
This is an Othello game. It is possible to play with computer.
This game is developed for studying github, Visual Studio and recent AI algorithms. Therefore future updates is not for completing as a game application but for supporting experimental codes.

## Restrictions
This is developed in weekend as my hobby, the qualiy is not guaranteed.

## Machine Specifications required for playning
Intel Core i5 2.4GHz CPU is enough for playning.

## Development environment
I confirmed compiling and running under Microsoft Visual Studio Community 2019. I don't confirm with other versions of Visual Studio, but maybe okey because only basic windows APIs are used.

## History
|Version|Release Date|Note|
|-------|------------|--------|
|1.0|2019/12/15|Initial version. Playing with computer is possible. The thinker uses traditional method, i.e. min-max method. This is a remade version which I wrote in 1988. Tuning of parameters is not finished yet. Using external thinking module is not supported yet.|
