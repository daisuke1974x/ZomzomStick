===================================================================
 ZomzomStick   by tech.Daisuke
   Ver1.0  2021.01.26
===================================================================

参考にしたサイト

　ESP32でキーボードショートカットを作ってしまおう
　httpsqiita.comporurubaitemseff3fedb1d4a63cbe08d

  画像データ変換
  httpslang-ship.comtoolsimage2data


  M5StickC の無印,Plusの種類に応じてコメントアウトしてください。
#include M5StickC.h
int M5StickSystem = 0; 0M5Stick  1M5Stick Plus
#include M5StickCPlus.h
int M5StickSystem = 1; 0M5Stick  1M5Stick Plus

#include BLEDevice.h
#include BLEUtils.h
#include BLEServer.h
#include BLE2902.h
#include BLEHIDDevice.h
#include HIDTypes.h
#include HIDKeyboardTypes.h

アイコンの画像ファイルの指定。
ZomzomStick.ino と同じフォルダに置いてください。
#include icon.h

ペアリング用PINコード
#define BLE_PASSKEY 1234

bool flag_mic_on = false;   マイクの表示
bool flag_video_on = false; ビデオの表示
bool flag_BtnA = false;     [A]ボタンの長押し検知兼リピート防止フラグ
bool flag_BtnB = false;     [B]ボタンの長押し検知兼リピート防止フラグ
int flag_os = 0;            起動時のOS選択　 0Windows  1mac  ※[B]ボタン長押しで切り替えできます。
bool flag_pairing = false;   ペアリングが完了しているかどうか

-------------------------------------------------------------------
 画面表示
-------------------------------------------------------------------
void indicate(){

  ペアリング未完了の場合は、何も表示しない。
  if (flag_pairing == false){
    return;
  }

  M5Stick無印の表示範囲の確認用（デバッグ）
  M5.Lcd.fillRect(0,0,160,80,YELLOW);

  ------------------------
   M5Stick 無印 のとき
  ------------------------
  if (M5StickSystem == 0){

    アイコンサイズの指定
    uint16_t imgWidth = 32;
    uint16_t imgHeight = 32;
    
    タイトルと状態の表示
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    M5.Lcd.println([Zomzom Stick]);
    M5.Lcd.println(Connected);

    対象システムとアプリアイコン
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    M5.Lcd.setCursor(0, 32);
    if (flag_os == 0 ){
      M5.Lcd.println(-Windows-);
    }
    if (flag_os == 1 ){
      M5.Lcd.println(-  mac  -);
    }
    M5.Lcd.pushImage(8, 48, imgWidth, imgHeight, img_app_32);
  
    マイク表示
    if ( flag_mic_on == true){
      M5.Lcd.pushImage(48, 48, imgWidth, imgHeight, img_mic_on_32);
    } else {
      M5.Lcd.pushImage(48, 48, imgWidth, imgHeight, img_mic_mute_32);
    }
  
    ビデオ表示
    if ( flag_video_on == true){
      M5.Lcd.pushImage(80, 48, imgWidth, imgHeight, img_video_on_32);
    } else {
      M5.Lcd.pushImage(80, 48, imgWidth, imgHeight, img_video_mute_32);
    }
  }

  ------------------------
   M5Stick Plus のとき
  ------------------------
  if (M5StickSystem == 1){

    アイコンサイズの指定
    uint16_t imgWidth = 64;
    uint16_t imgHeight = 64;

    タイトルと状態の表示
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    M5.Lcd.println( [ Zomzom Stick ]);
    M5.Lcd.println( StatusConnected);

    対象システムとアプリアイコン
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    M5.Lcd.setCursor(0, 32);
    if (flag_os == 0 ){
      M5.Lcd.println( -Windows-);
    }
    if (flag_os == 1 ){
      M5.Lcd.println( -  mac  -);
    }
    M5.Lcd.pushImage(16, 48, imgWidth, imgHeight, img_app_64);
  
    マイク表示
    if ( flag_mic_on == true){
      M5.Lcd.pushImage(96, 48, imgWidth, imgHeight, img_mic_on_64);
    } else {
      M5.Lcd.pushImage(96, 48, imgWidth, imgHeight, img_mic_mute_64);
    }
  
    ビデオ表示
    if ( flag_video_on == true){
      M5.Lcd.pushImage(160, 48, imgWidth, imgHeight, img_video_on_64);
    } else {
      M5.Lcd.pushImage(160, 48, imgWidth, imgHeight, img_video_mute_64);
    }
  
    使い方説明
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    M5.Lcd.setCursor(0, 112);
    M5.Lcd.println(      Btn[A] --  [Press] [Long Press]);
    M5.Lcd.println(      Btn[B] --  Rst LCD    Cng OS);
  }

}

-------------------------------------------------------------------
 BLEデバイス処理
-------------------------------------------------------------------
「ESP32でキーボードショートカットを作ってしまおう」をそのまま使用。
httpsqiita.comporurubaitemseff3fedb1d4a63cbe08d

BLEHIDDevice hid;
BLECharacteristic input;
BLECharacteristic output;

bool connected = false;

class MyCallbacks  public BLEServerCallbacks {
  void onConnect(BLEServer pServer){
    connected = true;
    BLE2902 desc = (BLE2902)input-getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc-setNotifications(true);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    if (M5StickSystem == 0){
      M5.Lcd.println([ZomzomStick]);
      M5.Lcd.println(Connected);
    }
    if (M5StickSystem == 1){
      M5.Lcd.println( [ Zomzom Stick ]);
      M5.Lcd.println( StatusConnected);
    }
    flag_pairing = true;
  }

  void onDisconnect(BLEServer pServer){
    connected = false;
    BLE2902 desc = (BLE2902)input-getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc-setNotifications(false);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE,BLACK); 
    if (M5StickSystem == 0){
      M5.Lcd.println([ZomzomStick]);
      M5.Lcd.println(Disonnected);
    }
    if (M5StickSystem == 1){
      M5.Lcd.println( [ Zomzom Stick ]);
      M5.Lcd.println( StatusDisonnected);
    }
    flag_pairing = false;
  }
};

 ペアリング処理用
class MySecurity  public BLESecurityCallbacks {
  bool onConfirmPIN(uint32_t pin){
    return false;
  }

  uint32_t onPassKeyRequest(){
    Serial.println(ONPassKeyRequest);
    return BLE_PASSKEY;
  }

  void onPassKeyNotify(uint32_t pass_key){
     ペアリング時のPINの表示
    Serial.println(onPassKeyNotify number);
    Serial.println(pass_key);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println(PIN);
    M5.Lcd.println(pass_key);
    flag_pairing = false;
  }

  bool onSecurityRequest(){
    Serial.println(onSecurityRequest);
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
    Serial.println(onAuthenticationComplete);
    if(cmpl.success){
       ペアリング完了
      Serial.println(auth success);
      flag_pairing = true;
    }else{
       ペアリング失敗
      Serial.println(auth failed);
      flag_pairing = false;
    }
  }
};

-------------------------------------------------------------------
 BLEデバイスの起動
-------------------------------------------------------------------
void taskServer(void){
  BLEDeviceinit(Zomzom Stick);

  BLEDevicesetEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
  BLEDevicesetSecurityCallbacks(new MySecurity());  

  BLEServer pServer = BLEDevicecreateServer();
  pServer-setCallbacks(new MyCallbacks());

  hid = new BLEHIDDevice(pServer);
  input = hid-inputReport(1);  -- input REPORTID from report map
  output = hid-outputReport(1);  -- output REPORTID from report map

  stdstring name = Poruruba;
  hid-manufacturer()-setValue(name);

  hid-pnp(0x02, 0xe502, 0xa111, 0x0210);
  hid-hidInfo(0x00,0x02);

  BLESecurity pSecurity = new BLESecurity();
  pSecurity-setKeySize(16);

  pSecurity-setAuthenticationMode(ESP_LE_AUTH_BOND);
  pSecurity-setCapability(ESP_IO_CAP_OUT);
  pSecurity-setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK  ESP_BLE_ID_KEY_MASK);

  uint32_t passkey = BLE_PASSKEY;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

    const uint8_t report[] = {
      USAGE_PAGE(1),      0x01,        Generic Desktop Ctrls
      USAGE(1),           0x06,        Keyboard
      COLLECTION(1),      0x01,        Application
      REPORT_ID(1),       0x01,          Report ID (1)
      USAGE_PAGE(1),      0x07,          KbrdKeypad
      USAGE_MINIMUM(1),   0xE0,
      USAGE_MAXIMUM(1),   0xE7,
      LOGICAL_MINIMUM(1), 0x00,
      LOGICAL_MAXIMUM(1), 0x01,
      REPORT_SIZE(1),     0x01,          1 byte (Modifier)
      REPORT_COUNT(1),    0x08,
      HIDINPUT(1),        0x02,          Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x01,          1 byte (Reserved)
      REPORT_SIZE(1),     0x08,
      HIDINPUT(1),        0x01,          Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x06,          6 bytes (Keys)
      REPORT_SIZE(1),     0x08,
      LOGICAL_MINIMUM(1), 0x00,
      LOGICAL_MAXIMUM(1), 0x65,          101 keys
      USAGE_MINIMUM(1),   0x00,
      USAGE_MAXIMUM(1),   0x65,
      HIDINPUT(1),        0x00,          Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x05,          5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
      REPORT_SIZE(1),     0x01,
      USAGE_PAGE(1),      0x08,          LEDs
      USAGE_MINIMUM(1),   0x01,          Num Lock
      USAGE_MAXIMUM(1),   0x05,          Kana
      HIDOUTPUT(1),       0x02,          Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
      REPORT_COUNT(1),    0x01,          3 bits (Padding)
      REPORT_SIZE(1),     0x03,
      HIDOUTPUT(1),       0x01,          Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
      END_COLLECTION(0)
    };

  hid-reportMap((uint8_t)report, sizeof(report));
  hid-startServices();

  BLEAdvertising pAdvertising = pServer-getAdvertising();
  pAdvertising-setAppearance(HID_KEYBOARD);
  pAdvertising-addServiceUUID(hid-hidService()-getUUID());
  pAdvertising-start();
  hid-setBatteryLevel(7);

  Serial.println(Advertising started!);
  delay(portMAX_DELAY);
};

-------------------------------------------------------------------
 キー操作関連
-------------------------------------------------------------------
enum KEY_MODIFIER_MASK {
  KEY_MASK_CTRL = 0x01,
  KEY_MASK_SHIFT = 0x02,
  KEY_MASK_ALT = 0x04,
  KEY_MASK_WIN = 0x08
};

void sendKeys(uint8_t mod, uint8_t keys, uint8_t num_key = 1){
  uint8_t msg[] = {mod, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  for( int i = 0 ; i  num_key ; i++ )
    msg[2 + i] = keys[i];

  input-setValue(msg, sizeof(msg));
  input-notify();

  uint8_t msg1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  input-setValue(msg1, sizeof(msg1));
  input-notify();
}

-------------------------------------------------------------------
 起動時処理
-------------------------------------------------------------------
void setup() {
  M5.begin();
  M5.IMU.Init();

  M5.Axp.ScreenBreath(9);
  M5.Lcd.setRotation(1); 画面に対して[A]ボタンが・・・1右側、3左側
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);

  M5.Lcd.println([M5StickC]);
  M5.Lcd.println([ Zomzom Stick ]);
  delay(1000);
  M5.Lcd.println(start Serial);

  Serial.begin(9600);
  Serial.println(Starting ZomzomStick!);
  M5.Lcd.println(start BLE);

   BLEデバイスの起動処理の開始
  xTaskCreate(taskServer, server, 20000, NULL, 5, NULL);
}

-------------------------------------------------------------------
 メインループ
-------------------------------------------------------------------
void loop() {
  M5.update();M5Stickのボタン等の状態を取得

  -----------------  
   [A]ボタンの処理
  -----------------  
  [A]ボタン長押し（ビデオの切り替え）
  if (M5.BtnA.pressedFor(500)) {
    if(connected){
      if(flag_BtnA == false){
        
        キーコード送信
        Serial.println(BtnA pressedFor);
        if (flag_os == 0){
           Windowsのとき Alt+Vでビデオの切り替え
          uint8_t keys[] = { 0x19 };V
          sendKeys(KEY_MASK_ALT, keys, 1);
        }
        if (flag_os == 1){
           macのとき Command(⌘)+Shift+Vでビデオを開始停止する。 Commandキーのコードは、WINキーと同じらしい。
          uint8_t keys[] = { 0x19 };V
          sendKeys(KEY_MASK_WIN +KEY_MASK_SHIFT , keys, 1);
        }

        表示状態フラグの更新
        if (flag_video_on == true){
          flag_video_on = false;
        }else{
          flag_video_on = true;
        }

        リピート操作防止フラグ
        flag_BtnA = true;
      }
    }
  }else{
    [A]ボタンを離した瞬間の処理（短押し・・・音声の切り替え）
    if( M5.BtnA.wasReleased() ){
      if(connected){
        if(flag_BtnA == false){長押し操作のときは除く

          キーコード送信
          Serial.println(BtnA released);
          if (flag_os == 0){
             Windowsのとき Alt+Aでオーディオをミュートミュート解除する。
            uint8_t keys[] = { 0x04 };A
            sendKeys(KEY_MASK_ALT, keys, 1);
          }
          if (flag_os == 1){
             macのとき Command(⌘)+Shift+A：オーディオをミュートミュート解除する。 Commandキーのコードは、WINキーと同じらしい。
            uint8_t keys[] = { 0x04 };A
            sendKeys(KEY_MASK_WIN +KEY_MASK_SHIFT, keys, 1);
          }

          表示状態フラグの更新
          if (flag_mic_on == true){
            flag_mic_on = false;
          }else{
            flag_mic_on = true;
          }
        }
      }
    }else{
      ボタンを離した後に、フラグをクリア
      flag_BtnA = false;
    }
  }

  -----------------  
   [B]ボタンの処理
  -----------------  
  [B]ボタンの長押し（OSの切り替え）
  if (M5.BtnB.pressedFor(500)) {
    if(flag_BtnB == false){
      OSフラグの更新
      if (flag_os == 0){
        flag_os = 1;
      }else if (flag_os == 1){
        flag_os = 0;
      }
      リピート操作防止フラグ
      flag_BtnB = true;
    }
  }else{
    [B]ボタンを離した瞬間の処理（短押し・・・状態表示フラグのリセット）
    if( M5.BtnB.wasReleased() ){
      if(flag_BtnB == false){長押し操作のときは除く
        状態表示フラグをリセットする
        flag_mic_on = false;
        flag_video_on = false;
      }
    }else{
      ボタンを離した後に、フラグをクリア
      flag_BtnB = false;
    }
  }

  ----------------  
  画面表示実施
  ----------------  
  indicate();

  -------------------------------
  画面表示した後に挿入するdelay
  -------------------------------
  100ミリ以下だと負荷が高すぎてフリーズする模様。
  delay(100);

}

