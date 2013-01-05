================================================================
 Hyper Estraier: a full-text search system for communities
 Copyright (C) 2004-2006 Mikio Hirabayashi
================================================================


これはHyper EstraierのWin32版バイナリパッケージです。
このパッケージには、Hyper EstraierのコアAPIとユーティリティコマンドが
含まれています。また、幾つかの他の製品のライブラリやコマンドも同梱さ
れています。

  estraier.dll   : Hyper EstraierのDLL
  libestraier.dll.a : Hyper Estraierのインポートライブラリ

  qdbm.dll       : QDBM（データベースライブラリ）のDLL
  mgwz.dll       : zlib（データ圧縮ライブラリ）のDLL
  libiconv-2.dll : iconv（文字コード変換ライブラリ）のDLL
  regex.dll      : regex（正規表現ライブラリ）のDLL
  pthreadGC2.dll : Pthreads（POSIXスレッドライブラリ）のDLL
  libpthreadGC2.a : Pthreadsのインポートライブラリ

  estraier.h     : Hyper Estraierのヘッダファイル
  depot.h        : QDBMのヘッダファイル
  curia.h        : QDBMのヘッダファイル
  cabin.h        : QDBMのヘッダファイル
  villa.h        : QDBMのヘッダファイル
  pthread.h      : Pthreadsのヘッダファイル
  sched.h        : Pthreadsのヘッダファイル
  semaphore.h    : Pthreadsのヘッダファイル

  estcmd.exe     : ユーティリティコマンド
  estmtcmd.exe   : 動作確認用コマンド
  estmaster.exe  : ノードマスタコマンド
  estmaster-sv.exe : ノードマスタのサービス版
  estbutler.exe  : ノードマスタのヘルパコマンド
  estcall.exe    : ノードクライアントコマンド
  estwaver.exe   : Webクローラのコマンド
  estload.exe    : 負荷テスト用コマンド
  scmutil.exe    : サービス設定用コマンド
  estcheck.bat   : 動作確認用コマンド
  estxfilt.bat   : テキスト抽出フィルタのラッパー
  estresult.dtd  : estcmdのXML出力のDTD
  estseek.cgi    : 検索用CGIスクリプト
  estseek.conf   : 検索用CGIスクリプトの設定ファイル
  estseek.tmpl   : 検索用CGIスクリプトの設定ファイル
  estseek.top    : 検索用CGIスクリプトの設定ファイル
  estseek.help   : 検索用CGIスクリプトの設定ファイル
  estfraud.cgi   : 疑似ノードサーバ用CGIスクリプト
  estfraud.conf  : 疑似ノードサーバ用CGIスクリプトの設定ファイル
  estproxy.cgi   : ハイライト表示用CGIスクリプト
  estproxy.conf  : ハイライト表示用CGIスクリプトの設定ファイル
  estscout.cgi   : ローカルメタ検索用CGIスクリプト
  estscout.conf  : ローカルメタ検索用CGIスクリプトの設定ファイル
  estsupt.cgi    : リモートメタ検索用CGIスクリプト
  estsupt.conf   : リモートメタ検索用CGIスクリプトの設定ファイル
  locale/        : 地域化用設定ファイル
  increm/        : インクリメンタル検索用のファイル
  doc/           : 説明書などの文書

  xdoc2txt.exe   : テキスト抽出フィルタの本体
  xdoc2txt.txt   : xdoc2txtの説明書
  zlib.dll       : xdoc2txtが利用するzlib

  estraiernative.jar : JavaバインディングのJAR
  jestraier.dll      : JavaバインディングのDLL
  estraierpure.jar   : ピュアJavaインターフェイスのJAR

  estraierpure.rb    : ピュアRubyインターフェイスのモジュール
  estcall.rb         : ピュアRubyインターフェイスのコマンド

コマンド（*.exe、*.bat）とDLL（*.dll）はコマンド検索パスの通った場所に
置いてください。CGIスクリプトと各種設定ファイルは、Webサーバに依存した
適当な位置に置いてください。

CGIスクリプトのコマンド検索パスはWebサーバの実行ユーザによって異なるこ
とに注意してください。CGIスクリプトと同じ場所にDLLも置いておけば確実に
パスを通すことができます。


Hyper EstraierはGNU Lesser General Public Licenseのもとにリリースされ
ています。詳しくは「COPYING.txt」をご覧ください。

Hyper Estraierは平林幹雄が作成しました。作者に連絡を取るには、
「mikio@fallabs.com」まで電子メールを送ってください。


このパッケージに同梱されているHyper Estraier本体以外のプログラム群の
権利は、各製品の著作者に帰属します。詳細な使用法やライセンスについて
は各製品の資料をご覧ください。

  QDBMは平林幹雄が開発しました。
  詳しくは http://fallabs.com/qdbm/ をご覧ください。

  zlibはJean-loup Gailly氏らが開発しました。
  詳しくは http://www.gzip.org/zlib/ をご覧ください。

  iconvはUlrich Drepper氏が開発しました。
  詳しくは http://www.gnu.org/software/libiconv/ をご覧ください。

  regexはHenry Spencer氏が開発しました。
  詳しくは ftp://ftp.zoo.toronto.edu/pub/ をご覧ください。

  Pthreads for Win32はBen Elliston氏らが開発しました。
  詳しくは http://sources.redhat.com/pthreads-win32/ をご覧ください。

  xdoc2txtはhishida氏が開発しました。
  詳しくは xdoc2txt.txt ファイルをご覧ください。

  MD5の実装はL. Peter Deutsch氏が開発しました。


Hyper Estraierをお選びいただきありがとうございます。



== END OF FILE ==
