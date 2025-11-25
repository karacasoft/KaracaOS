#include <dev/keyboard/keyboard.h>
#include <tty/tty.h>

extern SYS_RET arch_keyboard_initialize();

uint8_t keyboard_key_code_to_ascii_map[256] = {
  0x0, 0x0, '1', '2', '3', '4', '5',  '6', '7',  '8', '9', '0',  '-',  '=', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u',  'i', 'o',  'p', '[', ']',  '\n', 0x0, 'a',  's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l',  ';', '\'', '`', 0x0, '\\', 'z',  'x', 'c',  'v',
  'b', 'n', 'm', ',', '.', '/', 0x0,  '*', 0x0,  ' ', 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  '7', '8',  '9', '-', '4',  '5',  '6', '+',  '1',
  '2', '3', '0', '.', 0x0, 0x0, 0x0,  0x0, 0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, '/', '\n', 0x0, 0x0,  0x0, 0x0, 0x0,  0x0,  0x0, 0x0,  0x0
};

SYS_RET keyboard_interrupt_handler(key_code_t keyCode)
{
  const tty_handle_t *def_tty = tty_getdefaulthandle();
  if(keyboard_key_code_to_ascii_map[keyCode] != 0x0) {
    tty_puts(def_tty, &keyboard_key_code_to_ascii_map[keyCode], 1);
  }
  return SYS_RET_NO_ERROR;
}

SYS_RET keyboard_initialize()
{
  return arch_keyboard_initialize();
}
