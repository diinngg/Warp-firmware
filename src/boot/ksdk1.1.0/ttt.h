#include "fsl_gpio_driver.h"


void tttMain(void);

bool keypadGetKey(char *key);

bool move(const bool me, const char key, const bool learn, const bool draw);
bool checkWin(const uint8_t box1, const uint8_t box2, const uint8_t box3, const bool cross);
void resetGame(void);

void clearBoard(void);
void drawBoard(void);
void printBoard(void);

void drawGrid();
void drawCross(const uint8_t box, const bool mine);
void drawNought(const uint8_t box, const bool mine);
void drawWin(const uint8_t box1, const uint8_t box2, const bool mine);
void drawError(const uint8_t box);

uint8_t pickBox(void);
uint8_t getMinBoardID(uint32_t *boardID);
uint32_t getBoardID(const uint8_t *board, const uint8_t *permute);

void learnFrom(const bool won);


const gpio_output_pin_user_config_t kpOutputPins[] = {
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOA, 6),
        .config.outputLogic = 1,
        .config.slewRate = kPortSlowSlewRate,
        .config.driveStrength = kPortLowDriveStrength,
    },
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOA, 7),
        .config.outputLogic = 1,
        .config.slewRate = kPortSlowSlewRate,
        .config.driveStrength = kPortLowDriveStrength,
    },
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOA, 12),
        .config.outputLogic = 1,
        .config.slewRate = kPortSlowSlewRate,
        .config.driveStrength = kPortLowDriveStrength,
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};

const gpio_input_pin_user_config_t kpInputPins[] = {
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOA, 5),
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = true,
        .config.interrupt = kPortIntDisabled,
    },
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOB, 10),
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = true,
        .config.interrupt = kPortIntDisabled,
    },
    {
        .pinName = GPIO_MAKE_PIN(HW_GPIOB, 11),
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = true,
        .config.interrupt = kPortIntDisabled,
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};