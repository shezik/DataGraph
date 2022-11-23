#include "DataGraph.hpp"
#include <Arduino.h>
#include <U8g2lib.h>
#include <SlowSoftI2CMaster.h>
#include <INA226.h>

U8G2_DEVICE_TYPE u8g2(U8G2_R0, /* clock=*/5, /* data=*/17 /*, reset=*/);
SlowSoftI2CMaster arciic = SlowSoftI2CMaster(19, 18);
DataGraph voltageGraph(128, 64, 256, u8g2);
DataGraph currentGraph(128, 64, 256, u8g2);
DataGraph powerGraph(128, 64, 256, u8g2);

int whichGraph  = 0;  // 0: voltageGraph
                      // 1: currentGraph
                      // 2: powerGraph

int whichChart  = 0;  // 0: Voltage chart
                      // 1: Current chart
                      // 2: Power chart

int displayMode = 0;  // 0: Graph view
                      // 1: Chart view

int voltage, current, power;

void readINA226() {
    if (!arciic.i2c_start((0x81) | I2C_WRITE)) {
        Serial.println("I2C device busy!\n");
        return;
    }
    voltage = INA226_read(0x02) * 1.25;
    current = INA226_read(0x04) * 0.32;
    power   = INA226_read(0x03) * (0.32 * 25);
}

bool checkKey(int pin) {
    return !digitalRead(pin);  // Active low
}

bool checkKey(int pin, int delayms) {
    if (!digitalRead(pin)) {  // Active low
        delay(delayms);
        return !digitalRead(pin);
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    u8g2.begin();
    u8g2.setFont(U8G2_USER_FONT);
    u8g2.setFontMode(1);   // Transparent

    if (!arciic.i2c_init()) Serial.println("Failed to init SlowSoftI2C!\n");

    // INA226 初始化配置
    INA226_Write(0x00, 0x484f);  // 0x4527: 16次平均，1.1ms，1.1ms，连续测量分流电压和总线电压；
                                 // 0x484f: 设置转换时间204us，求平均值次数128，采样时间为204*128，设置模式为分流和总线连续模式
    INA226_Write(0x05, 0x0640);  // 0100

    // LED
    pinMode(12, OUTPUT);

    // 按键 I/O
    pinMode(2,  INPUT_PULLUP);  // Up
    pinMode(14, INPUT_PULLUP);  // Down
    pinMode(27, INPUT_PULLUP);  // Left
    pinMode(15, INPUT_PULLUP);  // Right

    // 五向摇杆 I/O
    pinMode(23,  INPUT_PULLUP);  // Up
    pinMode(25,  INPUT_PULLUP);  // Down
    pinMode(26,  INPUT_PULLUP);  // Left
    pinMode(33,  INPUT_PULLUP);  // Right
    pinMode(32,  INPUT_PULLUP);  // OK

    // USB 数据线
    pinMode(34, INPUT);  // USB D+
    pinMode(35, INPUT);  // USB D-

    // 屏幕 I/O
    pinMode(5,  OUTPUT);
    pinMode(17, OUTPUT);

    // 分配内存
    voltageGraph.init();
    currentGraph.init();
    powerGraph.init();

    // DataGraph 配置
    voltageGraph.setXDistance(0);
    currentGraph.setXDistance(0);
    powerGraph.setXDistance(0);
    voltageGraph.setCursorMode(DETAILED);
    currentGraph.setCursorMode(DETAILED);
    powerGraph.setCursorMode(DETAILED);
}

void loop() {
    readINA226();
    voltageGraph.appendValue(voltage);
    currentGraph.appendValue(current);
    powerGraph.appendValue(power);

    // These four keys are equipped with hardware debouncing
    if (checkKey(2)) {  // Up
        displayMode = displayMode ? 0 : 1;
    } else if (checkKey(14)) {  // Down
        displayMode = displayMode ? 0 : 1;
    } else if (checkKey(27)) {  // Left
        if (displayMode) {  // Chart
            whichChart = (whichChart == 0 ? 2 : whichChart - 1);
        } else {  // Graph
            whichGraph = (whichGraph == 0 ? 2 : whichGraph - 1);
        }
    } else if (checkKey(15)) {  // Right
        if (displayMode) {  // Chart
            whichChart = (whichChart == 2 ? 0 : whichChart + 1);
        } else {  // Graph
            whichGraph = (whichGraph == 2 ? 0 : whichGraph + 1);
        }
    }

    if (displayMode == 0) {
        if (checkKey(32, 10)) {  // OK
            switch (whichGraph) {
                case 0:
                    voltageGraph.setAutoScroll(true);
                    break;
                case 1:
                    currentGraph.setAutoScroll(true);
                    break;
                case 2:
                    powerGraph.setAutoScroll(true);
                    break;
            }
        } else if (checkKey(23, 10)) {  // Up
            switch (whichGraph) {
                case 0:
                    voltageGraph.moveCursor(-10);
                    voltageGraph.setAutoScroll(false);
                    break;
                case 1:
                    currentGraph.moveCursor(-10);
                    currentGraph.setAutoScroll(false);
                    break;
                case 2:
                    powerGraph.moveCursor(-10);
                    powerGraph.setAutoScroll(false);
                    break;
            }
        } else if (checkKey(25, 10)) {  // Down
            switch (whichGraph) {
                case 0:
                    voltageGraph.moveCursor(10);
                    voltageGraph.setAutoScroll(false);
                    break;
                case 1:
                    currentGraph.moveCursor(10);
                    currentGraph.setAutoScroll(false);
                    break;
                case 2:
                    powerGraph.moveCursor(10);
                    powerGraph.setAutoScroll(false);
                    break;
            }
        } else if (checkKey(26, 10)) {  // Left
            switch (whichGraph) {
                case 0:
                    voltageGraph.moveCursor(-1);
                    voltageGraph.setAutoScroll(false);
                    break;
                case 1:
                    currentGraph.moveCursor(-1);
                    currentGraph.setAutoScroll(false);
                    break;
                case 2:
                    powerGraph.moveCursor(-1);
                    powerGraph.setAutoScroll(false);
                    break;
            }
        } else if (checkKey(33, 10)) {  // Right
            switch (whichGraph) {
                case 0:
                    voltageGraph.moveCursor(1);
                    voltageGraph.setAutoScroll(false);
                    break;
                case 1:
                    currentGraph.moveCursor(1);
                    currentGraph.setAutoScroll(false);
                    break;
                case 2:
                    powerGraph.moveCursor(1);
                    powerGraph.setAutoScroll(false);
                    break;
            }
        }
    }

    u8g2.clearBuffer();
    if (displayMode) {
        switch (whichChart) {
            case 0:
                // draw voltage chart here
                break;
            case 1:
                // draw current chart here
                break;
            case 2:
                // draw power chart here
                break;
        }
    } else {
        switch (whichGraph) {
            case 0:
                voltageGraph.draw();
                u8g2.setDrawColor(2);
                u8g2.drawStr(0, 8, "mV");
                break;
            case 1:
                currentGraph.draw();
                u8g2.setDrawColor(2);
                u8g2.drawStr(0, 8, "mA");
                break;
            case 2:
                powerGraph.draw();
                u8g2.setDrawColor(2);
                u8g2.drawStr(0, 8, "mW");
                break;
        }
        // We assume that DrawColor is already set to 2
        static char charBuf[16];
        itoa(voltage, charBuf, 10);
        u8g2.drawStr(5, 64, charBuf);
        itoa(current, charBuf, 10);
        u8g2.drawStr(40, 64, charBuf);
        itoa(power, charBuf, 10);
        u8g2.drawStr(85, 64, charBuf);
    }
    u8g2.sendBuffer();
}
