/*
 * 一、器件：
 *   HC-SR04超声波测距模块--测量身高
 *   HX711模块+5kg压力传感器--测量体重
 *   ESP8266WiFi模块--向onenet上传数据
 *   LED灯*3--显示装置当前状态
 *   按键开关*2--开关装置、上传数据
 *   电阻*5--LED和按键开关的电阻
 * 
 * 二、接线：
 *   超声波测距模块--vcc:5V Trig:11 Echo:12 Gnd
 *   HX711模块--  vcc:5V SCK:4 DATA:5 Gnd
 *   ESP8266-- RX:3 TX:2 电压：3.3V    从有黄色涂层的背面侧看，将WiFi模块竖直放置（黄色涂层向上），一共有两层接口，第一层从左到右记为1-4，第二次从左到右记为5-8----则：位置1:2 位置4：GND 位置5:3.3V 位置7:3.3V 位置8:3
 *   LED--- 蓝灯：13 红灯：10 绿灯：9
 *   按键开关--- 发送按钮：7 总开关：8 
 * 
 * 三、按键开关和灯的具体含义及HX711的注意事项：
 *   按键开关：
 *     1）右边小的按键开关是开关装置的按钮，当装置处于休眠（工作）时长按一秒及以上并松开就会让装置开始工作（休眠）
 *     2）左边大的按键开关是发送信息按钮，长按一秒及以上并松开，ESP8266就会向onenet云平台发送身高、体重、BMI、以及state参数（因为onenet接收的是数据，因此state不能是英文或汉字，只能用1-4来表示）
 *        注意：发送按钮按下之后如果过一会后继续红蓝LED一起亮表明数据发送成功，可以进行下次的数据发送
 *   LED：
 *     1）只亮红灯（只在装置通电初始化的那段时间有可能亮一次，理想状态：只亮一小会）：说明通电后ESP8266WiFi连接过程出故障，只需等待一段时间，程序会循环执行WiFi连接过程直到WiFi连接过程正常，只亮红灯转为只亮蓝灯（进入休眠状态）
 *     2）只亮蓝灯：休眠状态
 *     3）红蓝一起亮：工作状态，开始测量数据
 *     4）只亮绿灯（只在发送按钮按下后才可能亮，理想状态：不亮）：按键开关按下后数据发送过程出现故障，需等待至绿灯熄灭，红蓝灯一起亮才可继续下次的发送数据
 *   HX711：在初始化结束亮蓝灯之前不要在秤盘上放要测量的东西
 *   
 *四、参考链接：
 *  压力传感器：https://blog.csdn.net/Raine_Yang/article/details/121323766
 *  WiFi模块：https://blog.csdn.net/weixin_43242242/article/details/102638671
 *
 *五、使用时自主需要修改的代码：(1--6)我在所需修改部分的该行末尾加上//和修改的序号，共有六处需要修改
 *  1--4:在setup上面部分的代码中的WiFi模块中，即下面四个WiFi名、WiFi密码、APIKEY、设备id     PS.这里的APIKEY和设备id是我建立的，我以后可能会把我onenet里的这个设备删掉，而且如果多个人使用同一个设备id的话你无法知道哪个是自己传的数据，所以最好你自己使用自己的
 *  5：setup的上一行，不同的hx711制作时有误差，因此需要你自己找一个已知重量的物体测一下 并修改gapValue是的hx711显示的质量和物体的实际质量相同
 *  6：loop函数下方的函数中我WiFi部分的函数，即本文档最后一个函数中，也是设备id修改
 *  //关于APIKEY和设备id：微信小程序wx.request请求时的设备id和APIKEY要和这个arduino代码里的保持一致，所以也要修改
 *PS.如果在此代码基础上同时加上OLED显示数据，可能会出问题，在updateBMI里的cmd没有东西，原因是postData里字符串连接过程丢失部分字符串，原因可能是内存不够或者其他原因
 */
 
//Wifi模块
#include <stdlib.h>
#include <SoftwareSerial.h>
#define SSID "Tenda_DDD858"      //wifi名                    //1
#define PASS "zz393176" //wifi密码                            //2
#define IP "api.heclouds.com" // 连接thingspeak.com服务器
const char OneNetServer[] = "api.heclouds.com";
const char APIKEY[] = "gLvq6SSLYwOA=7E3leJzHhiEe=Y=";    // 使用时请修改为你的API KEY -- Please use your own API KEY   //3
int32_t DeviceId = 952687417;                             // 使用时请修改为你的设备ID -- Please use your own device ID  //4
const char BMI_height[] = "height";                      
const char BMI_weight[] = "weight";         
const char BMI_bmi[] = "bmi";
const char BMI_state[] = "state";         //传给onenet的4个数据的名称 
bool completesend = false;                //判断是否成功发送代码 
bool completeconnect = false;       //判断是否初始化ESP8266正常连接
SoftwareSerial monitor(3, 2); // 定义软串口RX, TX
const int tcpPort = 80;

//led
int Red=10;
int Blue=13;
int Green=9;

//开关,矮的是总开关，高的是发送开关，按下时间超过一秒确保收到信号
int totalswitchPin = 8;//总开关，矮的
int sendswitchPin = 7;//发送开关，高的
bool onoff = false;//初始总开关状态，关闭

//测身高的参数
int trigPin = 11;    //Trig
int echoPin = 12;    //Echo
long duration,cm;

//测体重的参数
extern unsigned long HX711Read(void);
extern long getWeight();
int HX711_SCK = 4;   ///     作为输出口
int HX711_DT= 5;    ///     作为输入口
long HX711Buffer = 0;
long grossWeight = 0;
long netWeight = 0;
int gapValue = 480;       ///该值需校准 每个传感器都有所不同     //5


void setup() {
  //Serial Port begin
  Serial.begin (115200);
  
  //开关
  pinMode(totalswitchPin,INPUT);
  pinMode(sendswitchPin,INPUT);
  
  //测身高
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //led
  pinMode(Red,OUTPUT);
  pinMode(Blue,OUTPUT);
  pinMode(Green,OUTPUT);
  
  //测体重
  pinMode(HX711_SCK, OUTPUT);  ///SCK 为输出口 ---输出脉冲
  pinMode(HX711_DT, INPUT); ///  DT为输入口  ---读取数据

  digitalWrite(Red,HIGH);//只有红灯亮表明出故障,如果下方代码中ESP866正常连接的话，红灯很快会熄灭，换成只有蓝灯亮

  //Wifi
  monitor.begin(115200);
  monitor.println("AT");//指令测试
  delay(1000);
  if (monitor.find("OK"))  //接收指令正常则返回OK
  {
    Serial.println("Wifi module connection is normal");
    while(!completeconnect){
    connectWiFi();  //第一次执行的连接程序
    }
  }
  else {
    
    digitalWrite(Red,HIGH);//只有红灯亮表明出故障，ESP8266未能正常连接，继续执行连接程序

    Serial.println("Wifi module connection failed");
    while(!monitor.find("OK")){
      Serial.println("Try to reconnect");
      delay(500);
    }
    Serial.println("Wifi module connection is normal");
    while(!completeconnect){
      digitalWrite(Red,HIGH);
    connectWiFi();   //若第一次执行的连接程序未能正常连接，则循环执行连接程序
    }
  }

  //获取毛皮重量，重量传感器初始化，在此之前不要在秤盘上放要称重的东西
  grossWeight = HX711Read(); 
  
  digitalWrite(Red,LOW);
  digitalWrite(Blue,HIGH);// 连接程序成功执行代码才会运行到这里， 蓝亮红不亮 表示休眠
  delay(2000);    ///延时3秒  
}

 
void loop()
{
  //开关，第一次执行loop函数时候此处一定不会运行，只可能执行else（读取总开关的状态赋值给onoff）的函数，
  if(onoff){//进入这个部分后 此后的loop部分会循环执行这个部分而不去else里 直至再次按下总开关
  digitalWrite(Red,HIGH);
  digitalWrite(Blue,HIGH);//红蓝一起亮表示工作
  
    //测身高
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;
  
  //测体重
  float weight = getWeight();  // 计算放在传感器上的重物重量

  //bmi
  float bmi = 10*weight/(cm^2);

  //传给onenet的state以及在串口监视器显示的statement
  String state,statement;
  if(bmi<18.5){
    state = "0";
    statement = "Too light";
  }
  else if(bmi<24){
    state = "1";
    statement = "Normal";
  }
  else if(bmi<28){
    state = "2";
    statement = "Too heavy";
  }
  else{
    state = "3";
    statement = "Fat";
  }

  //串口监视器显示四个数据
  Serial.print(float(weight / 1000),3);  // 串口显示重量，3意为保留三位小数
  Serial.print(" kg\n"); // 显示单位
  Serial.print(cm*0.01);
  Serial.print(" m\n");
  Serial.print(bmi);
  Serial.print(" -- ");
  Serial.println(statement);

  //以上部分为工作状态循环进行的部分，以下部分为按键开关按下后才会执行的发送数据部分，且每按下一次只会发送一组数据
    if(digitalRead(sendswitchPin)){
      Serial.println("Prepare to send");
      while(digitalRead(sendswitchPin)){//类似按键开关去抖，按下一秒及以上肯定可以相当于按下一次
        delay(1000);
      }
      
      digitalWrite(Blue,LOW);
      digitalWrite(Red,LOW);
      digitalWrite(Green,HIGH);//只有绿灯亮表示正在发送信息，如果信息没发过去就会卡在下面一直循环，就是一直只亮绿灯，当绿灯熄灭表示发送成功
      
      Serial.println("Send the follow data");//发送的代码放在这里
      
      char buffer[30];
      String heightStr = dtostrf(cm*0.01, 4, 2, buffer);
      String weightStr = dtostrf(weight/1000, 4, 3, buffer);
      String bmiStr = dtostrf(bmi,4,2,buffer);//onenet云平台只接收字符串类型的数字，因此要浮点型转为字符串

      while(!completesend){ //在上传成功之前会一直上传该数据，上传成功后再updateBMI函数中会completesend=true
        updateBMI(heightStr,weightStr,bmiStr,state);
        delay(3000);
      }
      
      digitalWrite(Green,LOW);
      digitalWrite(Red,HIGH);
      digitalWrite(Blue,HIGH);//发送结束 正常工作
      completesend = false;//再次变回false等待下次上传  
    }
     
    Serial.print('\n');
    if(digitalRead(totalswitchPin)){//看是否有按下总开关，如果在此之前有按下发送开关，只有成功发送之后才能进入睡眠状态
      onoff = false;
      Serial.println("Sleep");  
      while(digitalRead(totalswitchPin)){//类似按键开关去抖，按下一秒及以上肯定可以相当于按下一次
        delay(1000);
      }
      
      digitalWrite(Red,LOW);//只有蓝灯亮，开始休眠
      
    }
  }
  else{
    if(digitalRead(totalswitchPin)){//准备从休眠状态进入工作状态
      Serial.println("Wake");   
      onoff = true;
      while(digitalRead(totalswitchPin)){
        delay(1000);
      }
      
      digitalWrite(Red,HIGH);//红灯也和蓝灯一起亮，开始工作
      
    }
  }
  delay(2000);
}







// 称重函数
// 返回值：测得重量（去皮）
long getWeight() {
 HX711Buffer = HX711Read();    /// 读取此时的传感器输出值
 netWeight = HX711Buffer;     /// 将传感器的输出值储存
 netWeight = netWeight - grossWeight; // 获取实物的AD采样数值。
 netWeight = (long)((float)netWeight/gapValue);    // AD值转换为重量（g）
 return netWeight; 
}


// HX711驱动程序
// 返回值：模拟电压值
unsigned long HX711Read(void) {  // 选择芯片工作方式并进行数据读取
  unsigned long count;   /// 储存输出值  
  unsigned char i;     
  digitalWrite(HX711_DT, HIGH);   
  delayMicroseconds(1); 
  digitalWrite(HX711_SCK, LOW);   
  delayMicroseconds(1);   
  count = 0; 
  while(digitalRead(HX711_DT));   // 当DT的值为1时，开始ad转换
  for(i = 0; i < 24; i++) {   /// 24个脉冲，对应读取24位数值
     digitalWrite(HX711_SCK, HIGH);  
                               /// 利用 SCK从0--1 ，发送一次脉冲，读取数值
     delayMicroseconds(1);    
     count = count<<1;    /// 用于移位存储24位二进制数值
     digitalWrite(HX711_SCK, LOW);  
     delayMicroseconds(1);
     if (digitalRead(HX711_DT)) {    /// 若DT值为1，对应count输出值也为1
       count++;
     } 
 }
   
 digitalWrite(HX711_SCK, HIGH);    /// 再来一次上升沿 选择工作方式  128增益
 count ^= 0x800000;   // 按位异或  不同则为1   0^0=0; 1^0=1;
 ///对应二进制  1000 0000 0000 0000 0000 0000  作用为将最高位取反，其他位保留原值
 delayMicroseconds(1);  
 digitalWrite(HX711_SCK, LOW);      /// SCK=0；     
 delayMicroseconds(1);  
 return(count);     /// 返回传感器读取值
}


//Wifi
void updateBMI(String height, String weight, String bmi, String state)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  //sendDebug(cmd);                        //发送指令，链接服务器
  monitor.println(cmd);
  delay(200);
  if (monitor.find("Error"))
  {
    Serial.print("Connection to server failed");
    return;
  }
  cmd = postData(DeviceId, height, weight, bmi, state);
  monitor.print("AT+CIPSEND=");
  monitor.println(cmd.length());
  if (monitor.find(">"))
  {
    Serial.print(">");
    monitor.print(cmd);
    Serial.print(cmd);
  }
  else
  {
    Serial.println("Data transmission failure");
  }
  if (monitor.find("OK"))
  {
    Serial.println("RECEIVED: OK");
    completesend = true;
    monitor.println("AT+CIPCLOSE");
  }
  else
  {
    Serial.println("Data transmission failure");
  }
}

boolean connectWiFi()
{
  //Serial.println("AT+CIPMUX=0");
  monitor.println("AT+CWMODE=1");
  monitor.println("AT+RST");
  delay(500);
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  monitor.println(cmd);
  delay(500);
  if (monitor.find("OK"))
  {
    Serial.println("Wifi connection successful");
    completeconnect = true;
    return true;
  } else
  {
    Serial.println("Wifi connection failed");
    return false;
  }
}

String postData(int dId, String val_height, String val_weight, String val_bmi, String val_state) 
{
    // 创建发送请求的URL -- We now create a URI for the request
    String url = "/devices/952687417";//此处修改为你的设备id              //6
    url += "/datapoints?type=3";           
    String data = "{\"" + String(BMI_height) + "\":" + val_height + ",\"" +
                  String(BMI_weight) + "\":" + val_weight + ",\"" +
                  String(BMI_bmi) + "\":" + val_bmi + ",\"" +
                  String(BMI_state) + "\":" + val_state + "}";
    // 创建发送指令 -- We now combine the request to the server
    String post_data = "POST " + url + " HTTP/1.1\r\n" +
                       "api-key:" + APIKEY + "\r\n" +
                       "Host:" + OneNetServer + "\r\n" +
                       "Content-Length: " + String(data.length()) + "\r\n" +                     //发送数据长度
                       "Connection: close\r\n\r\n" +
                       data;
    // 发送指令 -- This will send the request to server
    return post_data;
}
