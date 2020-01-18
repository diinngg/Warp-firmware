void tttMain(void);

bool keypadGetKey(uint8_t *key);

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
uint32_t getMinBoardID(void);
uint32_t getBoardID(const uint8_t *board, const uint8_t *permute);

void learnFrom(const bool won);