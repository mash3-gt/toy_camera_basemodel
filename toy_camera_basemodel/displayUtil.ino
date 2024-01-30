// 液晶ディスプレイの下部に文字列を表示する
void putStringOnLcd(String str, int color) {
  int len = str.length();
  // ディスプレイの下に文字表示用の黒い領域を描く
  display.fillRect(
    0,               // 左上_x
    CLIP_HEIGHT,     // 左上_y
    DISPLAY_WIDTH,   // 右下_x
    DISPLAY_HEIGHT,  // 右下_y
    ILI9341_BLACK    // 色
  );
  display.setTextSize(2);
  int sx = CLIP_WIDTH - len / 2 * 12;
  if (sx < 0) sx = 0;
  display.setCursor(sx, 225);
  display.setTextColor(color);
  display.println(str);
}

// 中央の画像を左へシフト
void slideImageLeft(uint16_t* imgBuf) {
  uint16_t pxData;

  // 元画像imgBufの左1/4から3/4に対して処理を行う
  for (int y = 0; y < 0 + DISPLAY_HEIGHT; ++y) {
    for (int x = DISPLAY_WIDTH / 4; x < DISPLAY_WIDTH / 4 + DISPLAY_WIDTH / 2; ++x) {
      // カラービットデータを取得
      pxData = imgBuf[y * DISPLAY_WIDTH + x];

      // 左半分へシフトして書き込み
      int pointerOffset = DISPLAY_WIDTH * y + x - DISPLAY_WIDTH / 4;
      *(imgBuf + pointerOffset) = pxData;
    }
  }
}

// 液晶ディスプレイにLINE_THICKNESSの太さの四角形を描画する
void drawBox(
  uint16_t* imgBuf,  // 画像アドレス
  int offset_x,      // 枠の左上_X
  int offset_y,      // 枠の左上_Y
  int width,         // dx
  int height,        // dy
  int thickness,     // 枠の太さ
  int color          // 枠の色
) {
  for (int x = offset_x; x < offset_x + width; ++x) {
    for (int n = 0; n < thickness; ++n) {
      *(imgBuf + DISPLAY_WIDTH * (offset_y + n) + x) = color;
      *(imgBuf + DISPLAY_WIDTH * (offset_y + height - 1 - n) + x) = color;
    }
  }
  for (int y = offset_y; y < offset_y + height; ++y) {
    for (int n = 0; n < thickness; ++n) {
      *(imgBuf + DISPLAY_WIDTH * y + offset_x + n) = color;
      *(imgBuf + DISPLAY_WIDTH * y + offset_x + width - 1 - n) = color;
    }
  }
}

// データを加工して右側に表示する
void drawOutputImg(
  uint16_t* imgBuf,
  int offset_x,   // 枠の左上_X
  int offset_y,   // 枠の左上_Y
  int width,      // dx
  int height,     // dy
  int effectType  // 数値に応じてエフェクトを切り替える
) {
  // 変数
  uint16_t pxData;  // 画素データ(RGB565)
  uint16_t r;       // red value
  uint16_t g;       // green value
  uint16_t b;       // blue value
  uint16_t m;
  uint16_t mg;
  int pointerOffset;
  int pictureData;

  // 元画像imgBufの左半分に対して処理を行う
  for (int y = 0; y < 0 + DISPLAY_HEIGHT; ++y) {
    for (int x = 0; x < 0 + DISPLAY_WIDTH / 2; ++x) {
      // 16ビット数でカラービットデータを取得
      uint16_t pxData = imgBuf[y * DISPLAY_WIDTH + x];

      // 加工（条件に応じて変更）
      switch (effectType) {
        case 1:  // モノクロ処理
          // red valueの取り出し
          r = pxData & 63488;  // 1111 1000 0000 0000b(=63488)でr以外をマスク
          r = r >> 11;         // rの値を得るためビットシフト
          // green valueの取り出し
          g = pxData & 2016;  // 0000 0111 1110 0000b(=2016)でg以外をマスク
          g = g >> 5;         // gの値を得るためビットシフト
          g /= 2;             // gは6bit、rとbは5bitなので2倍のダイナミックレンジがある. /2でレンジを揃える
          // blue valueの取り出し
          b = pxData & 31;  // 0000 0000 0001 1111b(=31)でb以外をマスク. ビットシフトは不要
          // グレー画像への変換
          m = 0.299 * r + 0.587 * g + 0.114 * b;  // グレースケールの値を計算
          mg = m * 2;                             // gは6bitで２倍のダイナミックレンジがあるため、２倍の値を用意
          // グレー画像をRGB565形式に変換
          pxData = 0;                   // 初期化
          pxData = m;                   // blue valueを格納
          pxData = pxData | (mg << 5);  // green value を格納
          pxData = pxData | (m << 11);  // red valueを格納
          // 右半分に移動
          pointerOffset = DISPLAY_WIDTH * y + (x + DISPLAY_WIDTH / 2);
          break;
        case 2:  // 左右反転
          pointerOffset = DISPLAY_WIDTH * y + (DISPLAY_WIDTH - x);
          break;
        case 3:                                       // image.hの画像を重ねる
          pictureData = bitmap_picture[y * 160 + x];  //画像データの値を取得
          pxData = blend(pxData, pictureData, 0.2);
          pointerOffset = DISPLAY_WIDTH * y + (x + DISPLAY_WIDTH / 2);
          break;
        default:  // エフェクトなし
          // エフェクトなし、右半分に移動
          pointerOffset = DISPLAY_WIDTH * y + (x + DISPLAY_WIDTH / 2);
          break;
      }

      // 画像バッファへの書き込み
      *(imgBuf + pointerOffset) = pxData;
    }
  }

  // 変換画像と枠を表示
  drawBox(imgBuf, offset_x, offset_y, width, height, 2, ILI9341_GREEN);
}

// ディスプレイに映す映像を作る
void makeDisplayImg(uint16_t* imgBuf, int effectType) {
  // 加工１：ディスプレイの中央部分を左に80pxスライド
  slideImageLeft(imgBuf);
  // 加工２：オリジナル画像を左半分に表示し、枠をつける
  drawBox(imgBuf, 0, 0, CLIP_WIDTH, CLIP_HEIGHT, 2, ILI9341_BLUE);
  // 加工３：加工画像を右半分に表示し、枠をつける
  drawOutputImg(imgBuf, OUTPUT_OFFSET_X, OUTPUT_OFFSET_Y, CLIP_WIDTH, CLIP_HEIGHT, effectType);
}
