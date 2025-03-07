#include "MatrixScanner.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include  "esp_attr.h" 

static const char *TAG = "MatrixScanner";

MatrixScanner* MatrixScanner::instance = nullptr;

MatrixScanner::MatrixScanner(RingBuffer &rb, const uint8_t* rows, uint8_t rCount, const uint8_t* cols, uint8_t cCount)
    : ringBuffer(rb), rowPins(rows), rowCount(rCount), colPins(cols), colCount(cCount) {
    instance = this;
}

void MatrixScanner::begin() {
    // Configure row pins as inputs with pull-ups and attach interrupts.
    for (uint8_t i = 0; i < rowCount; i++) {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << rowPins[i]);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);
    }
    gpio_install_isr_service(0);
    for (uint8_t i = 0; i < rowCount; i++) {
        gpio_isr_handler_add((gpio_num_t)rowPins[i], isrHandler, NULL);
    }
    // Configure column pins as outputs.
    for (uint8_t i = 0; i < colCount; i++) {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL << colPins[i]);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&io_conf);
        gpio_set_level((gpio_num_t)colPins[i], 0);
    }
}

uint8_t MatrixScanner::getLastRowFromMask(uint8_t mask) {
    uint8_t last = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            last = i;
        }
    }
    return last;
}

void MatrixScanner::updateOutputs(const KeyEvent &event, int triggeredRow) {
    if (event.rowMask & (1 << triggeredRow)) {
        for (uint8_t col = 0; col < colCount; col++) {
            gpio_set_level((gpio_num_t)colPins[col], (event.colData & (1 << col)) ? 1 : 0);
        }
    } else {
        for (uint8_t col = 0; col < colCount; col++) {
            gpio_set_level((gpio_num_t)colPins[col], 0);
        }
    }
}

void MatrixScanner::handleInterrupt() {
    int triggeredRow = -1;
    for (uint8_t i = 0; i < rowCount; i++) {
        if (gpio_get_level((gpio_num_t)rowPins[i]) == 0) {
            triggeredRow = i;
            break;
        }
    }
    if (triggeredRow < 0) return;
    
    KeyEvent event;
    if (!ringBuffer.peek(event)) {
        for (uint8_t col = 0; col < colCount; col++) {
            gpio_set_level((gpio_num_t)colPins[col], 0);
        }
        return;
    }
    updateOutputs(event, triggeredRow);
    if (triggeredRow == getLastRowFromMask(event.rowMask)) {
        ringBuffer.pop();
    }
}

void IRAM_ATTR MatrixScanner::isrHandler(void* arg) {
    if (instance != nullptr) {
        instance->handleInterrupt();
    }
}
