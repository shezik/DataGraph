#pragma once

// Customizable
#define U8G2_DEVICE_TYPE U8G2_SSD1306_128X64_NONAME_F_SW_I2C
#define U8G2_USER_FONT u8g2_font_6x13B_mr
#define U8G2_USER_FONT_WIDTH 6
#define U8G2_USER_FONT_HEIGHT 13

#include <Arduino.h>
#include <U8g2lib.h>

enum CursorMode {
    HIDDEN = 0, SIMPLE, DETAILED
};

enum GridMode {
    NONE = 0, DOT, LINE
};

class DataGraph {
    private:
        U8G2_DEVICE_TYPE u8g2;
        uint16_t graphLength, graphHeight, ringBufferLength;
        uint16_t rightBoundary;  // Initialized with ringBufferLength - 1
        uint16_t cursorPos = 0;  // Initialized with ringBufferLength - 1
        uint8_t xDistance = 0;
        double *dataRingBuffer = nullptr;
        double peakValue = 1.0;
        double bottomValue = 0.0;
        bool autoScroll = true;
        bool autoScaling = true;
        enum CursorMode cursorMode = HIDDEN;
        enum GridMode gridMode = NONE;

        void drawCursor(uint16_t pos);
        
    public:
        DataGraph(uint16_t _graphLength, uint16_t _graphHeight, uint16_t _ringBufferLength, U8G2_DEVICE_TYPE _u8g2);
        ~DataGraph();

        bool init();  // Allocate memory for the buffer
        void draw();  // Please call u8g2.sendBuffer() manually
        void appendValue(double val);  // !! POSITIVE VALUE ONLY!?
        void setAutoScroll(bool enabled);  // Move screen AND cursor to right boundary **on data addition**
        void setAutoScaling(bool enabled);  // Fit waveform to screen dynamically
        void setXDistance(uint8_t dist);  // Set spacing between each data point
        void setPeakValue(double val);    // Disables auto-scaling
        void setBottomValue(double val);  // Disables auto-scaling
        void setGridMode(enum GridMode mode);  // Grids!
        void setRightBoundary(int32_t n);  // Move right boundary to nth point, detects out-of-bound jumps
        double getValueAt(uint16_t n);

        uint16_t getCursorPos() {return cursorPos;}
        void setCursorPos(int32_t pos);  // Detects out-of-bound jumps
        void moveCursor(int32_t step);
        void setCursorMode(enum CursorMode mode);

        uint16_t getLength() {return graphLength;}
        uint16_t getHeight() {return graphHeight;}
        uint16_t getBufferLength() {return ringBufferLength;}
};
