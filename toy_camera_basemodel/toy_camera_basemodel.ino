/*
 *  toy_camera_basemodel.ino - The base model of a toy camera using Spresense.
 *  Copyright 2024 mash3-gt.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Camera.h>
#include <Adafruit_ILI9341.h>
#include <SDHCI.h>
#include "image.h"

// ディスプレイの接続ポート設定
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);

// ディスプレイの縦横の大きさ
#define DISPLAY_WIDTH (320)
#define DISPLAY_HEIGHT (240)

// クリップする領域サイズ
#define CLIP_WIDTH (DISPLAY_WIDTH / 2)
#define CLIP_HEIGHT (DISPLAY_HEIGHT - 16)  // 文字領域を下に確保する

// 加工済みデータの表示位置（右半分）
#define OUTPUT_OFFSET_X (DISPLAY_WIDTH * 0.5)  // 右半分へシフト
#define OUTPUT_OFFSET_Y (0)                    // 上から開始

// ボタン用ピンの定義
#define BUTTON_SHUTTER 0
#define BUTTON_EFFECT_RESET 4
#define BUTTON_EFFECT_1 5
#define BUTTON_EFFECT_2 6
#define BUTTON_EFFECT_3 7

// 撮影機能
bool shutterPressed = false;
// 撮影ボタンが押されたら、shutterPressed をtrueにする
void changeStateShutter() {
  shutterPressed = true;
}

// エフェクト機能
bool effectPressed = false;
int buttonNumber = 0;  // 直近で押されたボタン番号
// 押されたボタンに応じて番号を変更
void changeStateEffectReset() {
  effectPressed = true;
  buttonNumber = 0;
}
void changeStateEffect1() {
  effectPressed = true;
  buttonNumber = 1;
}
void changeStateEffect2() {
  effectPressed = true;
  buttonNumber = 2;
}
void changeStateEffect3() {
  effectPressed = true;
  buttonNumber = 3;
}

SDClass theSD;
int take_picture_count = 0;

void setup() {
  Serial.begin(115200);
  display.begin();  // 表示開始
  theCamera.begin();
  display.setRotation(3);  // ディスプレイの向きを設定

  CamErr err = theCamera.setStillPictureImageFormat(
    CAM_IMGSIZE_QUADVGA_H,
    CAM_IMGSIZE_QUADVGA_V,
    CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS) {
    printError(err);
  }

  // カメラのストリーミング開始
  Serial.println("Start streaming");
  err = theCamera.startStreaming(
    true,  // ストリームの開始/停止
    CamCB  // Video画像を取得するためのコールバック関数
  );
  if (err != CAM_ERR_SUCCESS) {
    Serial.println("Streaming error.");
    printError(err);
  }

  while (!theSD.begin()) {
    putStringOnLcd("Insert SD card", ILI9341_RED);
  }
  putStringOnLcd("ready to start", ILI9341_GREEN);

  // ボタンが押された時の動作
  attachInterrupt(digitalPinToInterrupt(BUTTON_SHUTTER), changeStateShutter, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_EFFECT_RESET), changeStateEffectReset, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_EFFECT_1), changeStateEffect1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_EFFECT_2), changeStateEffect2, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_EFFECT_3), changeStateEffect3, FALLING);
}

void loop() {
}
