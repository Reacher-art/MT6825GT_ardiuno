#include <Arduino.h>
#include <SPI.h>

// 定义MT6826S的SPI引脚
#define MT6826S_SCK 18
#define MT6826S_MISO 19
#define MT6826S_MOSI 23
#define MT6826S_CS 5
#define _2PI 6.28318530719f

// 定义MT6826S的寄存器地址
#define MT6826S_ANGLE_REG_START 0x03
#define MT6826S_ANGLE_REG_END 0x0006

// 定义SPI设置
const int SPI_CLOCK_SPEED = 1000000;  // 降低时钟速度
const int SPI_MODE = SPI_MODE3;  // 使用SPI模式3


void printBinary(uint32_t value) {
  int numBits = 32; // 假设整数为32位
  for (int i = numBits - 1; i >= 0; i--) {
    Serial.print((value >> i) & 0x01); // 打印每一位的二进制值
    if (i % 4 == 0) Serial.print(' '); // 每四位添加一个空格，便于阅读
  }
  Serial.println(); // 打印换行符
}

// 初始化SPI
void initSPI() {
    SPI.begin(MT6826S_SCK, MT6826S_MISO, MT6826S_MOSI, MT6826S_CS);
    SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE));
}

// 发送SPI命令并读取数据
uint8_t spiTransfer(uint8_t command, uint8_t addr) {
    
    uint8_t data = 0;
    digitalWrite(MT6826S_CS, LOW);
    SPI.transfer(command);
    data = SPI.transfer(addr);
    digitalWrite(MT6826S_CS, HIGH);
    
    return data;
}

// 读取MT6826S的角度数据
uint32_t readAngleMT6826S() {
    uint8_t angleData[3];

    // 读取角度寄存器0x03到0x06的数据
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t command = 0x83+i;
        uint8_t addr = (MT6826S_ANGLE_REG_START + i); // 构造读取命令
        //printBinary(command);
        digitalWrite(MT6826S_CS, HIGH);
        angleData[i] = spiTransfer(command, addr);
    }
    // 整合角度数据
    uint32_t rawAngle = 0;
    rawAngle = (angleData[2] >> 4) | ((angleData[1] >> 2) << 4) | (angleData[0] << 10);
    
    
    //printBinary(rawAngle);

    return rawAngle;
}

void setup() {
    Serial.begin(115200);
    pinMode(MT6826S_CS, OUTPUT);
    digitalWrite(MT6826S_CS, HIGH);
    initSPI();
}

void loop() {
    float angle = (readAngleMT6826S()* _2PI) / 262143;
    //Serial.print("Angle: ");
    Serial.println(angle, 2); // 保留一位小数
    delay(100); // 每秒读取一次
}
