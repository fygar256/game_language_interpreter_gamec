昔懐かしのGAME言語のインタプリタをCで書いてみました。

資料が少ないもので、オリジナルとは少し変わっていると思いますが、一応Linux上での実装ということで、ご勘弁いただきたいと思います。

GAME言語は今は廃れてしまいましたが、1980年前後にマイコンで手軽に使える処理系として一世を風靡しました。僕も当時GAME言語を参考に独自のインタプリタ・コンパイラを作って遊んでいたものです。

このプログラムはエラーチェックなどが十分ではないです。

コンパイル、インストール、実行

カレントディレクトリにgamec.cを置き、コンソールで、cc gamec.c -o gamecとしてコンパイルして下さい。

インストールは、sudo cp gamec /usr/bin/として、/usr/bin/にgamecを配置するだけです。

実行は、gamec [file.gm]として動かして下さい。

gamec では、シバンをつけることができます。シバンを付けて実行権を付けると、Linuxのシェルからスクリプトをコマンドのように実行できます。

シバンのサンプル

#!/usr/bin/gamec

使い方

GAME言語のファイル拡張子はgmであるらしいので、ここではそれに則ります。

サンプルプログラムは、インタプリタ起動時に引数で指定すると読み込まれます。

コマンドプロンプトからは*ld filename.gmとして読み込んで下さい。#=1でrunです。

*がついているコマンドはオプショナルコマンドで、ユーザーが追加できるようにしてあります。（と言ってもインタプリタのソースを書き換える必要がありますが)

エディタはついてないです。行末が0xa（LF)で終わるテキストを読み込めます。

文法

syntax.docにあります。

履歴

・2023年1月31日 16ビット配列の取得の所にバグがあったので訂正。

・2023年2月1日　マイナーチェンジ＆名前をgamelinuxにする。

・2023年2月2日　バグ修正 ver 0.9

・2023年2月3日　getch()関数を内包 ver 0.9.1

・2023年2月4日　for文修正 ver 0.9.2

・2023年2月8日　for文の普通の言語モードで動作するのを　*FM 1に、GAME言語モードで動作するのを*FM 0にする。 ver 0.9.3

・2024年7月24日 数のサポートをushortからshortにする。 ver 1.0.0

・2024年8月24日 1文字入力の呼び出しをgetch()からgetchar()にする。これにより一般のC言語に対応。名前をgamecとする。ver 1.0.1
