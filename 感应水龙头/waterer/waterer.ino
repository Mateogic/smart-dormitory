int RelayPin = 13; //控制继电器的引脚
int BUTTON = 8; //红外避障模块输入信号的引脚
int val;//红外避障模块测得信号值，当val==0表示检测到人手，val==1表示没有检测到人手
void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON, INPUT); //定义BUTTON 接口为输入接口
  pinMode(RelayPin, OUTPUT); //定义RelayPin 接口为输出接口
}
void loop()
{
  Serial.begin(9600);
  val = digitalRead(BUTTON);
  Serial.println(val);
  if (val == 0) //如果监测到障碍物
  {
    digitalWrite(RelayPin, HIGH); //驱动继电器闭合导通
    delay(100);//表现为延迟的高低，设置较小一些使得水龙头随人手打开/关门更快捷的效果
  }
  else
  {
    digitalWrite(RelayPin, LOW); //驱动继电器断开
    delay(100);
  }
}
