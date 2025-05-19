#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <string>

class Keyboard{
private:

static SwkbdState swkbd;
static char mybuf[60];
static SwkbdStatusData swkbdStatus;
static SwkbdLearningData swkbdLearning;
SwkbdButton button = SWKBD_BUTTON_NONE;
bool didit = false;

public:
std::string openKeyboard(const char* hintText);

};