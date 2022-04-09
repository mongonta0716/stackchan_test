# stackchan_test
Stackchanライブラリ化の実験用


# 概要
espnow用からJSONを受け取ったり、サーボ制御の部分をライブラリ化する前段階の実験公開版です。

# ビルド
Platformio用です。

ArduinoIDEで利用する場合は、srcフォルダとmain.cppの名前を揃えて変更してください。

## 例、
src -> stackchan_test
main.cpp -> stackchan_test.ino

# 必要なライブラリ
platformio.iniを参照してください。

## 2022/4時点の注意事項
 m5stack-avatarライブラリはM5Unified対応が必要です。公開されているライブラリではなく、[Github](https://github.com/meganetaaan/m5stack-avatar)から最新版を取得してください。

# Author
[Takao Akaki](https://github.com/mongonta0716)

# License
[MIT](https://github.com/mongonta0716/stackchan_test/blob/main/LICENSE)