# オセロ(*1)

(*1) オセロはOthello,Co.およびMegaHouseの登録商標です。

## はじめに
オセロゲームです。コンピュータとの対戦が可能です。
なお、このオセロはオーナーがgithubおよびVisual Studioの習熟のため、および最新のAI技術を勉強するために作っています。よって今後はゲームとしての完成度を上げる方向ではなく、実験的な機能が増えていく方向でアップデートしていく予定です。

## 推奨環境
Intel Core i5 2.4GHz程度のCPUであれば十分遊べます。

## 開発環境
Microsoft Visual Studio Community 2019での動作を確認しております。それ以外の環境については未確認ですが、基本的なAPIしか使用してないため、他のバージョンでも動作する可能性は高いと思います。

## History
|Version|Release Date|Note|
|-------|------------|--------|
|1.0|2019/12/15|最初のバージョン。コンピュータとの対戦機能付き。コンピュータの思考ルーチンは古典的なミニマックス法を用いたもの。筆者が1988年頃に作ったものをリメイクした程度のもの。パラメータのチューニングはまだほとんど行っていません。外部思考ルーチンにはまだ未対応です。|
----------------------------
# Othello(*1)
(*1) Othello is the trademark of Othello,Co. and MegaHouse.

## Introduction
This is an Othello game. It is possible to play with computer.
This game is developped for studying github, Visual Studio and recent AI algorithms. Therefore future updates is not for completing as a game application but for supporting experimental codes.

## Machine Specifications for playning
Intel Core i5 2.4GHz CPU is enough for playning.

## Development environment
I confirmed compiling and running under Microsoft Visual Studio Community 2019. I don't confirm with other versions of Visual Studio, but maybe okey because only basic windows APIs are used.

## History
|Version|Release Date|Note|
|-------|------------|--------|
|1.0|2019/12/15|Initial version. Playing with computer is possible. The thinker uses traditional method, i.e. min-max method. This is a remade version which I wrote in 1988. Tuning of parameters is not finished yet. Using separated thinking module is not supported yet.|
