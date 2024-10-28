// #include <MUIU8g2.h>
#include <U8g2lib.h>
// #include <U8x8lib.h>
#include <Arduino.h>
#include <stdio.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
// 引入 wifi 模块，并实例化，不同的芯片这里的依赖可能不同
#include <ESP8266WiFi.h>
static WiFiClient espClient;

// 引入阿里云 IoT SDK
#include <AliyunIoTSDK.h>

// 设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY "k049itIrqXT"
#define DEVICE_NAME "J7r6x59B7aZGRpRiykKi"
#define DEVICE_SECRET "4dc96b97d961de9fb45ed8c7ccade09e"
#define REGION_ID "cn-shanghai"

// 设置 wifi 信息
#define WIFI_SSID "wantopen"
#define WIFI_PASSWD "zhenghaoyue201532"
int pwmPin = 16;
int PowerSwitch = 90;
char str1[20];
char str2[20];
char str3[20];
char str4[20];
void setup()
{
  u8g2.begin();
  sprintf(str1,"LIGHT ON!");
  sprintf(str2,"LIGHT OFF!");
  sprintf(str3,"WELCOME");
  sprintf(str4,"INITIALIZING...");
  // u8g2.drawStr(20,20,str4);
  // pinMode(pwmPin, OUTPUT);
  analogWriteFreq(50);
  analogWriteRange(200);
  Serial.begin(115200);

  // 初始化 wifi
  wifiInit(WIFI_SSID, WIFI_PASSWD);

  // 初始化 iot，需传入 wifi 的 client，和设备产品信息
  AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  // 绑定一个设备属性回调，当远程修改此属性，会触发 powerCallback
  // PowerSwitch 是在设备产品中定义的物联网模型的 id
  AliyunIoTSDK::bindData("PowerSwitch", powerCallback);

  // 发送一个数据到云平台，LightLuminance 是在设备产品中定义的物联网模型的 id
  AliyunIoTSDK::send("LightLuminance", 100);
}
void pwm_server(int timePwm)
{
  // int i, a;
  //  for (a = 0; a < 100; a++)
  //  {
  //    for (i = 0; i < 5 * (50 + timePwm); i++)
  //    {
  //      digitalWrite(pwmPin, HIGH);
  //    }
  //    for (i = 0; i < 5 * 2000; i++)
  //    {
  //      digitalWrite(pwmPin, LOW);
  //    }
  //  }
  analogWrite(pwmPin, (int)(5 + (((float)timePwm / 180) * 20)));
}
void loop()
{
  AliyunIoTSDK::loop();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  // sprintf(str1,"Angle:%4d",PowerSwitch);

  if(PowerSwitch < 90){
    u8g2.drawStr(30,20,str1);	// write something to the internal memory
  }
  else if(PowerSwitch == 90){
    u8g2.drawStr(30,20,str3);	// write something to the internal memory
  }
  else{
    u8g2.drawStr(30,20,str2);	// write something to the internal memory
  }
 
  u8g2.sendBuffer();
}

// 初始化 wifi 连接
void wifiInit(const char *ssid, const char *passphrase)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passphrase);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("WiFi not Connect");
  }
  Serial.println("Connected to AP");
}

// 电源属性修改的回调函数
void powerCallback(JsonVariant p)
{
  PowerSwitch = p["PowerSwitch"];
  pwm_server(PowerSwitch);
  
  
  // if (PowerSwitch == 1) {
  //   Serial.println("111111111");
  //   // 启动设备
  // }

  // Serial.println("111111111");
}