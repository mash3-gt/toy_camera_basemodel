// 二つのピクセルを合成する
uint16_t blend(uint16_t color1,  // ベース画像のpixel
               uint16_t color2,  // 重ねる画像のpixel
               float alpha       // color2の透明度:0~1
) {
  // RGB565から個々のR, G, B成分を抽出
  uint8_t r1 = (color1 >> 11) & 0x1F;
  uint8_t g1 = (color1 >> 5) & 0x3F;
  uint8_t b1 = color1 & 0x1F;

  uint8_t r2 = (color2 >> 11) & 0x1F;
  uint8_t g2 = (color2 >> 5) & 0x3F;
  uint8_t b2 = color2 & 0x1F;

  // 新しいR, G, B成分を計算
  uint8_t r = (uint8_t)((r1 * (1 - alpha)) + (r2 * alpha));
  uint8_t g = (uint8_t)((g1 * (1 - alpha)) + (g2 * alpha));
  uint8_t b = (uint8_t)((b1 * (1 - alpha)) + (b2 * alpha));

  // 新しい色をRGB565フォーマットに変換
  return (r << 11) | (g << 5) | b;
}

// BMPファイルヘッダを生成する
void createBmpHeader(uint8_t* header, uint32_t width, uint32_t height) {
  height = CLIP_HEIGHT;                           // 文字領域は除いて高さを設定
  uint32_t rowSize = (24 * width + 31) / 32 * 4;  // 各行は4バイトの倍数
  uint32_t imageSize = rowSize * height;
  uint32_t fileSize = 54 + imageSize;  // 54バイトのヘッダ + ピクセルデータ

  // BMPファイルヘッダ (14 bytes)
  header[0] = 'B';
  header[1] = 'M';  // ファイルタイプ
  header[2] = fileSize;
  header[3] = fileSize >> 8;
  header[4] = fileSize >> 16;
  header[5] = fileSize >> 24;  // ファイルサイズ
  header[6] = 0;
  header[7] = 0;  // 予約済み
  header[8] = 0;
  header[9] = 0;  // 予約済み
  header[10] = 54;
  header[11] = 0;
  header[12] = 0;
  header[13] = 0;  // ピクセルデータまでのオフセット

  // DIBヘッダ (40 bytes)
  header[14] = 40;
  header[15] = 0;
  header[16] = 0;
  header[17] = 0;  // DIBヘッダのサイズ
  header[18] = width;
  header[19] = width >> 8;
  header[20] = width >> 16;
  header[21] = width >> 24;  // 画像の幅
  header[22] = height;
  header[23] = height >> 8;
  header[24] = height >> 16;
  header[25] = height >> 24;  // 画像の高さ
  header[26] = 1;
  header[27] = 0;  // プレーン数
  header[28] = 24;
  header[29] = 0;  // ピクセルあたりのビット数
  header[30] = 0;
  header[31] = 0;
  header[32] = 0;
  header[33] = 0;  // 圧縮タイプ
  header[34] = imageSize;
  header[35] = imageSize >> 8;
  header[36] = imageSize >> 16;
  header[37] = imageSize >> 24;  // 画像サイズ (バイト)
  header[38] = 0;
  header[39] = 0;
  header[40] = 0;
  header[41] = 0;  // 水平解像度 (ピクセル/メートル)
  header[42] = 0;
  header[43] = 0;
  header[44] = 0;
  header[45] = 0;  // 垂直解像度 (ピクセル/メートル)
  header[46] = 0;
  header[47] = 0;
  header[48] = 0;
  header[49] = 0;  // カラーパレット数
  header[50] = 0;
  header[51] = 0;
  header[52] = 0;
  header[53] = 0;
}