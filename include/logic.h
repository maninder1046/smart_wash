#ifndef LOGIC_H
#define LOGIC_H

enum WasherState { W_IDLE, W_FILLING, W_WASHING, W_DRAINING, W_SPINNING, W_DONE, W_ERROR };
enum DryerState  { D_IDLE, D_RUNNING, D_DONE, D_ERROR };

// Extern variables
extern bool autoMode;
extern bool manualWash;
extern bool manualSpin;
extern bool manualFill;
extern bool manualDrain;
extern bool manualDryer;

extern WasherState washerState;
extern DryerState dryerState;

extern unsigned long stateStartTime;

// Functions
void initLogic();
void runLogic();

const char* getWasherStateString();
const char* getDryerStateString();

#endif
