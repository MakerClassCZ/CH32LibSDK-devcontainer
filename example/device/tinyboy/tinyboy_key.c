
// ****************************************************************************
//
//                             TinyBoy - Keyboard
//
// ****************************************************************************

#include "../../include.h"

#if USE_KEY // 1=use keyboard support

// keyboard buffer
Bool KeyInitOk = False; // keyboard initialized
u32 KeyLastPress[KEY_NUM];
u32 KeyLastRelease[KEY_NUM];
volatile Bool KeyPressMap[KEY_NUM]; // keys are currently pressed (index =
                                    // button code - 1)
u8 KeyBuf[KEYBUF_SIZE];             // keyboard buffer
volatile u8 KeyWriteOff = 0;        // write offset to keyboard buffer
volatile u8 KeyReadOff = 0;         // read offset from keyboard buffer

// #define KEYCNT_REL	50	// keyboard counter - release interval in [ms]
// #define KEYCNT_PRESS	400	// keyboard counter - first repeat in [ms]
// #define KEYCNT_REPEAT	100	// keyboard counter - next repeat in
// [ms]

#define PIN_ACT PA2 // pin connected to fire button
#define PIN_PAD PC4 // pin conected to direction buttons

#define JOY_N 199   // joypad UP
#define JOY_NE 260 // joypad UP + RIGHT
#define JOY_NW 567 // JOYPAD UP + LEFT
#define JOY_S 346  // joypad DOWN
#define JOY_SE 388 // joypad DOWN + RIGHT
#define JOY_SW 616 // joypad DOWN + LEFT
#define JOY_E 90   // joypad RIGHT
#define JOY_W 511  // joypad LEFT
#define JOY_DEV 10  // deviation

// write key to keyboard buffer
void KeyWriteKey(u8 key) {
  u8 w = KeyWriteOff;
  u8 w2 = w + 1;
  if (w2 >= KEYBUF_SIZE)
    w2 = 0;
  if (w2 != KeyReadOff) {
    KeyBuf[w] = key;
    cb();
    KeyWriteOff = w2;
  }
}

INLINE u8 CheckKey(const int i) {
  const u16 val = ADC1_GetSingle(2);
  switch (i) {
  case 0:
    // RIGHT
    if (((val > JOY_E - JOY_DEV) && (val < JOY_E + JOY_DEV)) |
        ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV)) |
        ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV))) {
      return 0;
    }
    break;
  case 1:
    // UP
    if (((val > JOY_N - JOY_DEV) && (val < JOY_N + JOY_DEV)) |
        ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV)) |
        ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV))) {
      return 0;
    }
    break;
  case 2:
    // LEFT
    if (((val > JOY_W - JOY_DEV) && (val < JOY_W + JOY_DEV)) |
        ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV)) |
        ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV))) {
      return 0;
    }
    break;
  case 3:
    // DOWN
    if (((val > JOY_S - JOY_DEV) && (val < JOY_S + JOY_DEV)) |
        ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV)) |
        ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV))) {
      return 0;
    }
    break;
  case 4:
    // A
    return GPIO_In(PIN_ACT);
  default:
    return 1;
  }

  return 1;
}

// scan keys
void KeyScan() {
  if (!KeyInitOk)
    return; // keyboard not initialized

  int i;
  u32 t = Time(); // time in ticks
  for (i = 0; i < KEY_NUM; i++) {
    // check if button is pressed
    if (CheckKey(i) == 0) {
      // button is pressed for the first time
      if (!KeyPressMap[i]) {
        KeyLastPress[i] =
            t + (KEYCNT_PRESS - KEYCNT_REPEAT) * 1000 * HCLK_PER_US;
        KeyPressMap[i] = True;
        KeyWriteKey(i + 1);
      }

      // button is already pressed - check repeat interval
      else {
        if ((s32)(t - KeyLastPress[i]) >=
            (s32)(KEYCNT_REPEAT * 1000 * HCLK_PER_US)) {
          KeyLastPress[i] = t;
          KeyWriteKey(i + 1);
        }
      }
      KeyLastRelease[i] = t;
    }

    // button is release - check stop of press
    else {
      if (KeyPressMap[i]) {
        if ((s32)(t - KeyLastRelease[i]) >=
            (s32)(KEYCNT_REL * 1000 * HCLK_PER_US)) {
          KeyPressMap[i] = False;
        }
      }
    }
  }
}

// get button from keyboard buffer KEY_* (returns NOKEY if no key)
u8 KeyGet() {
  // check if keyboard buffer is empty
  u8 r = KeyReadOff;
  cb();
  if (r == KeyWriteOff)
    return NOKEY;

  // get key from keyboard buffer
  u8 ch = KeyBuf[r];

  // write new read offset
  r++;
  if (r >= KEYBUF_SIZE)
    r = 0;
  KeyReadOff = r;

  return ch;
}

// key flush
void KeyFlush() {
  while (KeyGet() != NOKEY) {
  }
}

// check no pressed key
Bool KeyNoPressed() {
  int i;
  for (i = 0; i < KEY_NUM; i++)
    if (KeyPressMap[i])
      return False;
  return True;
}

// wait for no key pressed
void KeyWaitNoPressed() {
  while (!KeyNoPressed()) {
  }
}

// Initialize keyboard service (should be called before display init)
void KeyInit() {
  KeyWriteOff = 0;
  KeyReadOff = 0;

  // initialize key ports
  RCC_PCClkEnable();
  RCC_PAClkEnable();
  GPIO_Mode(PIN_PAD, GPIO_MODE_AIN);
  GPIO_Mode(PIN_ACT, GPIO_MODE_IN_PU);
  RCC_ADC1ClkEnable();
  ADC1_InitSingle();

  for (int i = 0; i < KEY_NUM; i++) {
    KeyPressMap[i] = False;
  }

  KeyInitOk = True; // keyboard initialized
}

// terminate keyboard
void KeyTerm() {
  KeyInitOk = False; // keyboard not initialized
  cb();
  GPIO_PinReset(PIN_ACT);
  GPIO_PinReset(PIN_PAD);
}

#endif // USE_KEY
