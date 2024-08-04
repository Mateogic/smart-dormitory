/*
  引脚接线图
  SEP8266       FRC522
  ---------------------------
    D1            RST
    D2            SDA
    D5            SCK
    D6            MSOI
    D7            MOSI
    GND           GND
    3.3V          3.3V
                  RQ (不接)
  控制开门的电机接D4（GPIO2）引脚
  控制开关灯的电机接D3（GPIO0）引脚
  蜂鸣器接入D8（GPIO15）引脚
*/

//NFC读卡器  声明及宏定义
//相关库的导入
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         5           // 配置针脚
#define SS_PIN          4
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <Servo.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 1, /* data=*/ 3, /* reset=*/ U8X8_PIN_NONE);  //OLED显示屏定义 使用GPIO1和GPIO3进行I2C通信

Servo myservo1;//开门的电机
Servo myservo2;//开关灯的电机

MFRC522 mfrc522(SS_PIN, RST_PIN);   // 创建新的RFID实例
MFRC522::MIFARE_Key key;


/*************************IO配置**************************/
int NfcNum[] = {298, 478};               //存储可以被识别卡的值 //刷卡后oled屏幕上会显示当前卡值，将显示卡值加入此数组下次刷卡即可被识别 需要同时改变Num也即数组中元素的个数
int NumBer ;                             //定义全局变量，将存储卡的总值
int Num = 2 ;                            //当前NfcNum[]中储存的卡值个数
int Buzzer = 15;                         //GPIO15蜂鸣器，提示音
int counter = 0;
int timer = 1800;                        //多长时间重启一次 单位秒
long int ww = 0;                         //定时重启计数值

//记录卡值：例：285：xxx校园卡；
//可识别校园卡，手机nfc，ic卡等多种类型卡

/********************************点灯科技远程开门部分***********************************/
//BLINKER网络开关  宏定义
#define BLINKER_MIOT_LIGHT
#define BLINKER_WIFI
#define BLINKER_WITHOUT_SSL

//BLINKER网络开关 声明
#include <Blinker.h>

char auth[] = "ff858f09c875"; //点灯key
char ssid[] = "liujice";      //wifi账号
char pswd[] = "liujice123";     //wifi密码

// 新建组件对象
BlinkerButton ButtonOn1("door");//按键的名称 开门
BlinkerButton ButtonOn2("light");//按键的名称 开灯



/********************************开门函数***********************************/
void button1_callback(const String & state) {
  if (state == "tap") {
    Di(2);        //蜂鸣器响两声
    OLED_NET();   //OLED显示远程开门字样
    Servo1_SW();  //电机开门子函数
    delay(1000);
    OLED_NOM();   //OLED默认显示
  }

  if (state == "off") {
  }
  Blinker.vibrate();
}

/********************************开关灯函数***********************************/
void button2_callback(const String & state) {
  if (state == "on") {
    Di(1);        //蜂鸣器响一声
    Servo2_SW();         //电灯开灯子函数
  }
  if (state == "press") {
    Di(1);        //蜂鸣器响一声
    Servo3_SW();         //电灯关灯子函数
  }
  Blinker.vibrate();
}

/**************如果未绑定的组件被触发，则会执行其中内容****************************/
void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  counter++;
}

void miotPowerState(const String & state)
{
  BLINKER_LOG("need set power state: ", state);

  if (state == BLINKER_CMD_OFF) {//如果语音接收到是关闭灯就设置GPIO口为高电平
    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();
  }
  else if (state == BLINKER_CMD_ON) {

  }
}
/********************************OLED显示远程开门函数***********************************/
void OLED_NET()
{
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("   Welcome   ");
  u8g2.setCursor(0, 35);
  u8g2.print("远程开门中.....");
  u8g2.setCursor(0, 55);
  u8g2.print("    请推门    ");
  u8g2.sendBuffer();
}
/********************************OLED显示刷卡开门函数***********************************/
void OLED_NFC()
{
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("   Welcome   ");
  u8g2.setCursor(0, 35);
  u8g2.print("身份验证成功...");
  u8g2.setCursor(0, 55);
  u8g2.print("      请推门  ");
  u8g2.setCursor(10, 55);
  u8g2.print(NumBer);
  u8g2.sendBuffer();
}
/********************************OLED显示卡值错误***********************************/
void OLED_ERROR()
{
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("   Welcome   ");
  u8g2.setCursor(0, 35);
  u8g2.print("身份验证失败...");
  u8g2.setCursor(0, 55);
  u8g2.print("      请重刷  ");
  u8g2.setCursor(10, 55);
  u8g2.print(NumBer);
  u8g2.sendBuffer();
}
/********************************OLED显示默认值***********************************/
void OLED_NOM()
{
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("    Welcome   ");
  u8g2.setCursor(0, 35);
  u8g2.print("     请刷卡    ");
  u8g2.setCursor(0, 55);
  u8g2.print("当前门已关闭...");
  u8g2.sendBuffer();
}

/********************************开门 子函数**************************************/
void Servo1_SW()
{
  myservo1.write(0);
  delay(2000);
  myservo1.write(90);   //写入旋转角度（可适当修改）
}
/********************************开/关灯 子函数***********************************/
void Servo2_SW()
{
  myservo2.write(150);
  delay(1000);
  myservo2.write(90);
}

void Servo3_SW()
{
  myservo2.write(30);
  delay(1000);
  myservo2.write(90);
}

/********************************蜂鸣器音效***********************************/
void Di(int a)
{
  for (int i = 0; i < a; i++)
  {
    digitalWrite(Buzzer, 1);
    delay(250);
    digitalWrite(Buzzer, 0);
    delay(50);
  }
}

/********************************初始化部分***********************************/
void setup()
{
  // 初始化串口
  Serial.begin(9600);
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();
  // 初始化blinker
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  ButtonOn1.attach(button1_callback);
  ButtonOn2.attach(button2_callback);
  BlinkerMIOT.attachPowerState(miotPowerState);

  /*以下是nfc初始化*/
  Serial.begin(115200); // 设置串口波特率为115200
  SPI.begin();        // SPI开始
  mfrc522.PCD_Init(); // Init MFRC522 card

  /*以下是OLED屏初始化*/
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_unifont_t_chinese3);
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 35);
  u8g2.print("初始化中......");
  delay(1000);
  u8g2.setCursor(0, 35);
  u8g2.print("初始化成功....");
  u8g2.sendBuffer();

  /*以下是舵机初始化*/
  myservo1.attach(2);//开门舵机
  myservo1.write(90);
  myservo2.attach(0);//开灯舵机
  myservo2.write(90);

  /*  蜂鸣器初始化  */
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, 0);
  //Serial.println();
  Serial.println("初始化完成");
  OLED_NOM();
}


/********************************循环部分***********************************/
void loop()
{
  Blinker.run();  //BLINKER网络开关 loop
  ww = millis();  //给开机时间赋值
  if (ww >= timer * 1000) //定时重启函数
  {
    Serial.println("重启中...");
    ESP.reset();
    return;
  }
  /********************************NFC部分***********************************/
  // 寻找新卡
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    //Serial.println("没有找到卡");
    return;
  }

  // 选择一张卡
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    Serial.println("没有卡可选");
    return;
  }


  // 显示卡片的详细信息
  Serial.print(F("卡片 UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("卡片类型: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  MFRC522::StatusCode status;
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("身份验证失败？或者是卡链接失败"));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //停止 PICC
  mfrc522.PICC_HaltA();
  //停止加密PCD
  mfrc522.PCD_StopCrypto1();

  return;
}

/********************************NFC卡值转换部分***********************************/
//将字节数组转储为串行的十六进制值
void dump_byte_array(byte *buffer, byte bufferSize)
{

  //  byte num;
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    NumBer += buffer[i];              //将指针记录的值汇总成一个总值
  }
  Serial.println();
  Serial.print("NumBer全值:");    //待上个循环完成后，打印NumBer的值
  Serial.println(NumBer);

  NFC_judge();
  NumBer = 0;              //完成后将NumBer的值清零，不然影响下次刷卡的计算
}
//对NumBer和NfcNum[]数组的值进行对比，相同则开门，不同则报错
void NFC_judge()
{
  unsigned char h = 0;
  unsigned char NFC_OK = 0;

  for ( h = 0; h < Num; h++)
  {
    if (NumBer == NfcNum[h])
    {
      NFC_OK = 1;
      break;
    }
    else
    {
      NFC_OK = 0;
    }
  }
  {
    if (NumBer == NfcNum[h])
    {
      Serial.println("刷卡正确;开门");
      Di(2);
      OLED_NFC();
      Servo1_SW();
      OLED_NOM();
      Serial.println("关门");
    }
    else
    {
      Serial.println("卡不对，请重试");
      OLED_ERROR();
      Di(4);                             //蜂鸣器响四声
      delay(1000);
      OLED_NOM();
    }
  }
}
