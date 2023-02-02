# gamelinux


昔なつかしのGAME言語のインタプリタをCで書いてみました。

資料が少ないもので、オリジナルとは少し変わっていると思いますが、

一応Linux上での実装ということで、ご勘弁にいただきたいと思います。

GAME言語は今は廃れてしまいましたが、

1980年前後にマイコンで手軽に使える処理系として一世を風靡しました。

僕もゲーム言語を参考に独自のインタプリタ・コンパイラを作って遊んでいたものです。

このプログラムはまだ未完成な所があり、エラーチェックなどが十分ではないです。

実用とは程遠いものですが、まあ、遊びだと思って下さい。一応制御構文などは全部動きます。

エディタはついてないです。行末が0xa（LF)で終わるテキストを読み込めます。

カレントディレクトリにgamelinux.cとconio.hを置き、

$ cc gamelinux.c -o gamelinux

としてコンパイルして下さい。

実行は、

$ ./gamelinux [file.gm]

として動かして下さい。

このプログラムは引数なしで実行させると、プロンプトがでて、キーボードから入力を取り、

コマンドを実行できます。

ゲーム言語のファイル拡張子はgmであるらしいので、ここではそれに則ります。

サンプルプログラムは、インタプリタ起動時に引数で指定すると読み込まれます。

gamelinuxでは、シバン（shebang;スクリプトの先頭にある#!で始まる文字列）

を付けることができます。gamelinuxのスクリプトに、シバンを付けて実行権を付けると、

Linuxのシェルからコマンドのように実行できます。

/usr/bin/にgamelinuxをコピーしておくと良いでしょう。

バイナリをインストールすると、/usr/bin/に配置されます。

gamelinuxのコマンドプロンプトからは*ld file.gmとして読み込んで下さい。#=1でrunです。

# Optional Command

*がついているコマンドはオプショナルコマンドで、ユーザーが追加できるようにしてあります。

（と言ってもインタプリタのソースを書き換える必要がありますが)


このプログラムのオプショナルコマンドは次のとおりです。

*TN Trace on

*TF Trace off

*LD load game program

*QU quit

*SH invoke shell

   
# FILES

conio.h                 ヘッダファイル

test.gm                 サンプルプログラム/テスト

fibonacci.gm            サンプルプログラム/Fibonacci数を求めます。

sieve.gm                サンプルプログラム/エラトステネスのふるいにより、素数を求めます。

gamelinux_0.9-1_amd64.deb  実行ファイル(バイナリ)

# Sites

ゲーム言語についてはこのサイトが詳しいようです。http://www43.tok2.com/home/cmpslv/Manual/GAME%20(J)(1979)(Ascii).pdf"

# 可搬性

一文字キー入力のgetch()関数だけがLinux固有のものです。
getch()関数はヘッダファイルのconio.hにあります。
getch()問題さえクリアできれば、ターミナルだったらどんな機種でも動くと思います。

# 履歴

2023年1月31日 16ビット配列の取得の所にバグがあったので訂正。

2023年2月1日　マイナーチェンジ＆名前をgamelinuxにする。

2023年2月2日　バグ修正 ver 0.9

