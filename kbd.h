#ifndef _KBD_H_
#define _KBD_H_

#include "macrosKBD.h"
#include "i8254.h"
#include "kbc.h"


int (kbd_subscribe_interrupts)(uint8_t *bit_no);

int (kbd_unsubscribe_interrupts)();

void (kbc_ih)();

int (keyboard_restore)();

#endif

