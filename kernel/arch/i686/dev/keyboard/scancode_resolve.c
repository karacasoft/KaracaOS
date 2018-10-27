#include "scancode_resolve.h"

#define SCANCODE_COUNT 109

const static uint8_t scancode_set_1[][SCANCODE_COUNT] = {
  { 0x29, 0x00, KEYBOARD_KEYCODE_BACKTICK },
  { 0x02, 0x00, KEYBOARD_KEYCODE_1 },
  { 0x03, 0x00, KEYBOARD_KEYCODE_2 },
  { 0x04, 0x00, KEYBOARD_KEYCODE_3 },
  { 0x05, 0x00, KEYBOARD_KEYCODE_4 },
  { 0x06, 0x00, KEYBOARD_KEYCODE_5 },
  { 0x07, 0x00, KEYBOARD_KEYCODE_6 },
  { 0x08, 0x00, KEYBOARD_KEYCODE_7 },
  { 0x09, 0x00, KEYBOARD_KEYCODE_8 },
  { 0x0A, 0x00, KEYBOARD_KEYCODE_9 },
  { 0x0B, 0x00, KEYBOARD_KEYCODE_0 },
  { 0x0C, 0x00, KEYBOARD_KEYCODE_MINUS },
  { 0x0D, 0x00, KEYBOARD_KEYCODE_EQUAL },
  { 0x0E, 0x00, KEYBOARD_KEYCODE_BACKSPACE },
  { 0x0F, 0x00, KEYBOARD_KEYCODE_TAB },
  { 0x10, 0x00, KEYBOARD_KEYCODE_Q },
  { 0x11, 0x00, KEYBOARD_KEYCODE_W },
  { 0x12, 0x00, KEYBOARD_KEYCODE_E },
  { 0x13, 0x00, KEYBOARD_KEYCODE_R },
  { 0x14, 0x00, KEYBOARD_KEYCODE_T },
  { 0x15, 0x00, KEYBOARD_KEYCODE_Y },
  { 0x16, 0x00, KEYBOARD_KEYCODE_U },
  { 0x17, 0x00, KEYBOARD_KEYCODE_I },
  { 0x18, 0x00, KEYBOARD_KEYCODE_O },
  { 0x19, 0x00, KEYBOARD_KEYCODE_P },
  { 0x1A, 0x00, KEYBOARD_KEYCODE_BRACKET_OPEN },
  { 0x1B, 0x00, KEYBOARD_KEYCODE_BRACKET_CLOSE },
  { 0x2B, 0x00, KEYBOARD_KEYCODE_BACKSLASH },
  { 0x3A, 0x00, KEYBOARD_KEYCODE_CAPS_LOCK },
  { 0x1E, 0x00, KEYBOARD_KEYCODE_A },
  { 0x1F, 0x00, KEYBOARD_KEYCODE_S },
  { 0x20, 0x00, KEYBOARD_KEYCODE_D },
  { 0x21, 0x00, KEYBOARD_KEYCODE_F },
  { 0x22, 0x00, KEYBOARD_KEYCODE_G },
  { 0x23, 0x00, KEYBOARD_KEYCODE_H },
  { 0x24, 0x00, KEYBOARD_KEYCODE_J },
  { 0x25, 0x00, KEYBOARD_KEYCODE_K },
  { 0x26, 0x00, KEYBOARD_KEYCODE_L },
  { 0x27, 0x00, KEYBOARD_KEYCODE_SEMICOLON },
  { 0x28, 0x00, KEYBOARD_KEYCODE_QUOTE },
  { 0x1C, 0x00, KEYBOARD_KEYCODE_ENTER },
  { 0x2A, 0x00, KEYBOARD_KEYCODE_LEFTSHIFT },
  { 0x2C, 0x00, KEYBOARD_KEYCODE_Z },
  { 0x2D, 0x00, KEYBOARD_KEYCODE_X },
  { 0x2E, 0x00, KEYBOARD_KEYCODE_C },
  { 0x2F, 0x00, KEYBOARD_KEYCODE_V },
  { 0x30, 0x00, KEYBOARD_KEYCODE_B },
  { 0x31, 0x00, KEYBOARD_KEYCODE_N },
  { 0x32, 0x00, KEYBOARD_KEYCODE_M },
  { 0x33, 0x00, KEYBOARD_KEYCODE_COMMA },
  { 0x34, 0x00, KEYBOARD_KEYCODE_DOT },
  { 0x35, 0x00, KEYBOARD_KEYCODE_SLASH },
  { 0x36, 0x00, KEYBOARD_KEYCODE_RIGHT_SHIFT },
  { 0x1D, 0x00, KEYBOARD_KEYCODE_LEFT_CTRL },
  { 0x38, 0x00, KEYBOARD_KEYCODE_LEFT_ALT },
  { 0x39, 0x00, KEYBOARD_KEYCODE_SPACE },
  { 0xE0, 0x38, 0x00, KEYBOARD_KEYCODE_RIGHT_ALT },
  { 0xE0, 0x1D, 0x00, KEYBOARD_KEYCODE_RIGHT_CTRL },
  { 0xE0, 0x52, 0x00, KEYBOARD_KEYCODE_INSERT },
  { 0xE0, 0x53, 0x00, KEYBOARD_KEYCODE_DELETE },
  { 0xE0, 0x47, 0x00, KEYBOARD_KEYCODE_HOME },
  { 0xE0, 0x4F, 0x00, KEYBOARD_KEYCODE_END },
  { 0xE0, 0x49, 0x00, KEYBOARD_KEYCODE_PAGE_UP },
  { 0xE0, 0x51, 0x00, KEYBOARD_KEYCODE_PAGE_DOWN },
  { 0xE0, 0x4B, 0x00, KEYBOARD_KEYCODE_LEFT },
  { 0xE0, 0x48, 0x00, KEYBOARD_KEYCODE_UP },
  { 0xE0, 0x50, 0x00, KEYBOARD_KEYCODE_DOWN },
  { 0xE0, 0x4D, 0x00, KEYBOARD_KEYCODE_RIGHT },
  { 0x45, 0x00, KEYBOARD_KEYCODE_NUM_LOCK },
  { 0x47, 0x00, KEYBOARD_KEYCODE_KEYPAD_7 },
  { 0x4B, 0x00, KEYBOARD_KEYCODE_KEYPAD_4 },
  { 0x4F, 0x00, KEYBOARD_KEYCODE_KEYPAD_1 },
  { 0xE0, 0x35, 0x00, KEYBOARD_KEYCODE_NUMPAD_SLASH },
  { 0x48, 0x00, KEYBOARD_KEYCODE_KEYPAD_8 },
  { 0x4C, 0x00, KEYBOARD_KEYCODE_KEYPAD_5 },
  { 0x50, 0x00, KEYBOARD_KEYCODE_KEYPAD_2 },
  { 0x52, 0x00, KEYBOARD_KEYCODE_KEYPAD_0 },
  { 0x37, 0x00, KEYBOARD_KEYCODE_KEYPAD_TIMES },
  { 0x49, 0x00, KEYBOARD_KEYCODE_KEYPAD_9 },
  { 0x4D, 0x00, KEYBOARD_KEYCODE_KEYPAD_6 },
  { 0x51, 0x00, KEYBOARD_KEYCODE_KEYPAD_3 },
  { 0x53, 0x00, KEYBOARD_KEYCODE_KEYPAD_DOT },
  { 0x4A, 0x00, KEYBOARD_KEYCODE_KEYPAD_MINUS },
  { 0x4E, 0x00, KEYBOARD_KEYCODE_KEYPAD_PLUS },
  { 0xE0, 0x1C, 0x00, KEYBOARD_KEYCODE_NUMPAD_ENTER },
  { 0x01, 0x00, KEYBOARD_KEYCODE_ESC },
  { 0x3B, 0x00, KEYBOARD_KEYCODE_F1 },
  { 0x3C, 0x00, KEYBOARD_KEYCODE_F2 },
  { 0x3D, 0x00, KEYBOARD_KEYCODE_F3 },
  { 0x3E, 0x00, KEYBOARD_KEYCODE_F4 },
  { 0x3F, 0x00, KEYBOARD_KEYCODE_F5 },
  { 0x40, 0x00, KEYBOARD_KEYCODE_F6 },
  { 0x41, 0x00, KEYBOARD_KEYCODE_F7 },
  { 0x42, 0x00, KEYBOARD_KEYCODE_F8 },
  { 0x43, 0x00, KEYBOARD_KEYCODE_F9 },
  { 0x44, 0x00, KEYBOARD_KEYCODE_F10 },
  { 0x57, 0x00, KEYBOARD_KEYCODE_F11 },
  { 0x58, 0x00, KEYBOARD_KEYCODE_F12 },
  { 0xE0, 0x37, KEYBOARD_KEYCODE_PRINT_SCREEN },
  { 0x54, 0x00, KEYBOARD_KEYCODE_ALT_SYSRQ },
  { 0x46, 0x00, KEYBOARD_KEYCODE_SCROLL_LOCK },
  { 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5, 0x00, KEYBOARD_KEYCODE_PAUSE },
  { 0xE0, 0x46, 0x00, KEYBOARD_KEYCODE_CTRL_BREAK },
  { 0xE0, 0x5B, 0x00, KEYBOARD_KEYCODE_LEFT_META },
  { 0xE0, 0x5C, 0x00, KEYBOARD_KEYCODE_RIGHT_META },
  { 0xE0, 0x5D, 0x00, KEYBOARD_KEYCODE_MENU },
  { 0xE0, 0x5F, 0x00, KEYBOARD_KEYCODE_SLEEP },
  { 0xE0, 0x5E, 0x00, KEYBOARD_KEYCODE_POWER },
  { 0xE0, 0x63, 0x00, KEYBOARD_KEYCODE_WAKE },
};

key_code_t scancode_resolve_getKeyCode_set1();
key_code_t scancode_resolve_getKeyCode_set2();
key_code_t scancode_resolve_getKeyCode_set3();


key_code_t scancode_resolve_getKeyCode_set1() {
  uint8_t i = 0;
  uint8_t signalLength = 1;
  uint8_t scanCode = inb(0x60);
  uint8_t codeArr[10] = {0};
  codeArr[0] = scanCode;

  for(i = 0; i < SCANCODE_COUNT; i++) {
    uint8_t j = 0;
    BOOLEAN release = FALSE;
    while(scancode_set_1[i][j] == codeArr[j] || (scancode_set_1[i][j] | 0x80) == codeArr[j]) {
      if(codeArr[j] == (scancode_set_1[i][j] | 0x80)) {
        release = TRUE;
      }
      j++;
      if(scancode_set_1[i][j] == 0) {
        return (release) ? (scancode_set_1[i][j+1] | 0x80) : scancode_set_1[i][j+1];
      } else {
        codeArr[signalLength++] = inb(0x60);
      }
    }
  }
  // TODO unknown key pressed
  return KEYBOARD_KEYCODE_NONE;
}

key_code_t scancode_resolve_getKeyCode_set2() {

}

key_code_t scancode_resolve_getKeyCode_set3() {

}

key_code_t scancode_resolve_getKeyCode(uint8_t set)
{
  if(set == 1) {
    return scancode_resolve_getKeyCode_set1();
  } else if(set == 2) {
    return scancode_resolve_getKeyCode_set2();
  } else if(set == 3) {
    return scancode_resolve_getKeyCode_set3();
  }

}
