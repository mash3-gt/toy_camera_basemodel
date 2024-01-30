// カメラのストリーミング画像を受け取った時のコールバック関数
void CamCB(CamImage img) {
  if (!img.isAvailable()) {
    Serial.println("Image is not available. Try again");
    return;
  }

  // エフェクトボタンが押された時：文字を表示
  if (effectPressed) {
    Serial.println("Effect Pressed");
    String effectName;
    switch (buttonNumber) {
      case 1:
        effectName = "1:gray";
        break;
      case 2:
        effectName = "2:reverse";
        break;
      case 3:
        effectName = "3:blend";
        break;
      default:
        effectName = "0:original";
    }
    putStringOnLcd("Effect " + effectName, ILI9341_GREEN);  // 押したボタンに応じたエフェクト名を表示
    effectPressed = false;
  }

  // RGB565に変換
  img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);

  // 画像の加工
  makeDisplayImg(
    (uint16_t*)img.getImgBuff(),  // ストリーミング画像のポインタ
    buttonNumber                  // エフェクト処理番号
  );
  uint16_t* imgBuf = (uint16_t*)img.getImgBuff();

  // 加工後の画像を参照し、ディスプレイに表示
  display.drawRGBBitmap(0, 0, (uint16_t*)img.getImgBuff(), DISPLAY_WIDTH, CLIP_HEIGHT);

  // 撮影処理
  if (shutterPressed) {
    Serial.println("Shutter button is true.");
    shutterPressed = false;

    // BMPヘッダを生成
    uint8_t bmpHeader[54];
    createBmpHeader(bmpHeader, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // ファイル名を決める
    char filename[16] = { 0 };
    sprintf(filename, "PICT%03d.BMP", take_picture_count);

    // SDの書き込み先ファイルを作成
    File myFile = theSD.open(filename, FILE_WRITE);
    if (myFile) {
      myFile.write(bmpHeader, 54);  // BMPヘッダを書き込み

      // BMPフォーマットはピクセルデータをボトムアップで格納
      for (int y = CLIP_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
          uint16_t pixel = imgBuf[y * DISPLAY_WIDTH + x];
          uint8_t r = ((pixel >> 11) & 0x1F) << 3;
          uint8_t g = ((pixel >> 5) & 0x3F) << 2;
          uint8_t b = (pixel & 0x1F) << 3;

          // BMPはBGRフォーマットを使用
          myFile.write(b);
          myFile.write(g);
          myFile.write(r);
        }
        // パディング（行のバイト数が4の倍数になるように）
        int padding = (4 - (DISPLAY_WIDTH * 3) % 4) % 4;
        for (int p = 0; p < padding; p++) {
          myFile.write((uint8_t)0);
        }
      }
      myFile.close();

      // ファイル保存成功のメッセージ
      putStringOnLcd("Saved as " + String(filename), ILI9341_GREEN);

      // カウント更新
      take_picture_count += 1;
    } else {
      Serial.println("Failed to open file for writing.");
    }
  }
}

void printError(enum CamErr err) {
  Serial.print("Error: ");
  switch (err) {
    case CAM_ERR_NO_DEVICE:
      Serial.println("No Device");
      break;
    case CAM_ERR_ILLEGAL_DEVERR:
      Serial.println("Illegal device error");
      break;
    case CAM_ERR_ALREADY_INITIALIZED:
      Serial.println("Already initialized");
      break;
    case CAM_ERR_NOT_INITIALIZED:
      Serial.println("Not initialized");
      break;
    case CAM_ERR_NOT_STILL_INITIALIZED:
      Serial.println("Still picture not initialized");
      break;
    case CAM_ERR_CANT_CREATE_THREAD:
      Serial.println("Failed to create thread");
      break;
    case CAM_ERR_INVALID_PARAM:
      Serial.println("Invalid parameter");
      break;
    case CAM_ERR_NO_MEMORY:
      Serial.println("No memory");
      break;
    case CAM_ERR_USR_INUSED:
      Serial.println("Buffer already in use");
      break;
    case CAM_ERR_NOT_PERMITTED:
      Serial.println("Operation not permitted");
      break;
    default:
      break;
  }
}