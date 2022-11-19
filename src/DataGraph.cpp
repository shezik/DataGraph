#include "DataGraph.hpp"

DataGraph::DataGraph(uint16_t _graphLength, uint16_t _graphHeight, uint16_t _ringBufferLength, U8G2_DEVICE_TYPE _u8g2)
    : graphLength(_graphLength)
    , graphHeight(_graphHeight)
    , ringBufferLength(_ringBufferLength)
    , u8g2(_u8g2)
    , rightBoundary(_ringBufferLength - 1)
    , cursorPos(_ringBufferLength - 1)
{
    // Do nothing
}

DataGraph::~DataGraph() {
    free(dataRingBuffer);
    dataRingBuffer = nullptr;  // Excessive?
}

bool DataGraph::init() {
    dataRingBuffer = (double *) malloc(ringBufferLength * sizeof(double));
    if (dataRingBuffer) {
        for (uint16_t i = 0; i < ringBufferLength; i++) {
            dataRingBuffer[i] = 0.0;
        }
        return true;
    } else return false;
}

void DataGraph::appendValue(double val) {
    for (uint16_t i = 0; i < ringBufferLength - 1; i++) {  // Left shift data
        dataRingBuffer[i] = dataRingBuffer[i + 1];
    }
    dataRingBuffer[ringBufferLength - 1] = val;
    if (autoScroll) rightBoundary = ringBufferLength - 1;
}

void DataGraph::drawCursor(uint16_t pos) {
    // *CALL AFTER SETTING PROPER PEAK AND BOTTOM VALUES!!*

    static char str[64];
    uint8_t charCount = sprintf(str, "%.0f", dataRingBuffer[pos]);  // 0 decimal places, customizable
    int32_t curX = (graphLength - 1) - (1 + xDistance) * (rightBoundary - pos);
    int32_t curY = graphHeight - 1 - round((dataRingBuffer[pos] - bottomValue) / (peakValue - bottomValue) * (graphHeight - 1));
    int32_t strX, strY;
    if (curX - (U8G2_USER_FONT_WIDTH * charCount + 1) > 0)  strX = curX - (U8G2_USER_FONT_WIDTH * charCount + 1); else strX = curX + 1;
    if (curY + U8G2_USER_FONT_HEIGHT + 1 < graphHeight - 1) strY = curY + U8G2_USER_FONT_HEIGHT + 1;              else strY = curY - 1;
    // printf("%s, charCount = %d, curX = %d, curY = %d\n", str, charCount, curX, curY);  // DEBUG

    switch (cursorMode) {
        case DETAILED:
            u8g2.setFont(U8G2_USER_FONT);  // DEBUG
            u8g2.drawStr(strX, strY, str);
            // No break here!
        case SIMPLE:
            u8g2.drawVLine(curX, curY - 2 >= 0 ? curY - 2 : 0, curY - 2 >= 0 ? 5 : 5 + (curY - 2));  // 5 pixels tall
            if (curX - 1 >= 0) u8g2.drawVLine(curX - 1, curY - 2 >= 0 ? curY - 2 : 0, curY - 2 >= 0 ? 5 : 5 + (curY - 2));  // THICKER
            break;
        default:
            break;
    }
}

void DataGraph::draw() {
    if (autoScaling) {
        // Find peak value in window
        bottomValue = peakValue = dataRingBuffer[rightBoundary];  // Initialize with a feasible value
        for (int32_t i = rightBoundary; i >= 0; i--) {  // In case that the buffer is narrower than graphLength
                // OVERFLOW IF USE uint16_t
            // printf("%d, dataRingBuffer[i] = %f, peakValue = %f, bottomValue = %f\n", rightBoundary - i, dataRingBuffer[i], peakValue, bottomValue);  // DEBUG
            if (rightBoundary - i > floor((graphLength + xDistance) / (xDistance + 1)) - 1) {
                // printf("Break\n");  // DEBUG
                break;
            } else {
                if (dataRingBuffer[i] > peakValue) {
                    peakValue = dataRingBuffer[i];
                }
                if (dataRingBuffer[i] < bottomValue) {
                    bottomValue = dataRingBuffer[i];
                }
            }
        }
        if (bottomValue == peakValue) peakValue += 1.0;  // Bad things are gonna happen if they are equal
    }

    if (xDistance == 0) {
        for (int32_t i = rightBoundary; i >= 0; i--) {
            if (rightBoundary - i > graphLength - 1) break;
            u8g2.drawPixel((graphLength - 1) - (rightBoundary - i), graphHeight - 1 - round((dataRingBuffer[i] - bottomValue) / (peakValue - bottomValue) * (graphHeight - 1)));
            if (i == cursorPos) drawCursor(i);
        }
    } else {  // such complication
        for (uint16_t i = rightBoundary; i >= 1; i--) {
            if (rightBoundary - (i - 1) > floor((graphLength + xDistance) / (xDistance + 1)) - 1) {  // l = n + d(n - 1)
                if (cursorPos == i) drawCursor(i);
                break;
            }
            u8g2.drawLine((graphLength - 1) - (1 + xDistance) * (rightBoundary - i), \
                          graphHeight - 1 - round((dataRingBuffer[i] - bottomValue) / (peakValue - bottomValue) * (graphHeight - 1)), \
                          (graphLength - 1) - (1 + xDistance) * (rightBoundary - (i - 1)), \
                          graphHeight - 1 - round((dataRingBuffer[i - 1] - bottomValue) / (peakValue - bottomValue) * (graphHeight - 1)));
            if (i == cursorPos || (i == 1 && cursorPos == 0)) drawCursor(cursorPos);
        }
    }
}

void DataGraph::setAutoScroll(bool enabled) {
    autoScroll = enabled;
}

void DataGraph::setAutoScaling(bool enabled) {
    autoScaling = enabled;
}

void DataGraph::setXDistance(uint8_t dist) {
    xDistance = dist;
}

void DataGraph::setPeakValue(double val) {
    setAutoScaling(false);
    peakValue = val;
}

void DataGraph::setBottomValue(double val) {
    setAutoScaling(false);
    bottomValue = val;
}

void DataGraph::setGridMode(enum GridMode mode) {
    gridMode = mode;
}

void DataGraph::jumpTo(uint16_t n) {
    rightBoundary = (n > ringBufferLength - 1 ? ringBufferLength - 1 : n);
}

double DataGraph::getValueAt(uint16_t n) {
    return (n > ringBufferLength - 1 ? dataRingBuffer[ringBufferLength - 1] : dataRingBuffer[n]);
}

void DataGraph::setCursorPos(uint16_t pos) {
    cursorPos = pos;
}

void DataGraph::setCursorMode(enum CursorMode mode) {
    cursorMode = mode;
}
