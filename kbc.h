#ifndef _KBC_H_
#define _KBC_H_


#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include "macrosKBD.h"

int (read_status_code)(uint8_t* status);

int (read_pressed_key)(int port, uint8_t *output);

int (write_command_byte)(int port, uint8_t command);

#endif

