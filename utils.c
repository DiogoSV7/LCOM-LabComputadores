#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3
uint32_t counter_KBC = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL){
    return 1;
  }
  *lsb = val & 0xFF;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL){
    return 1;
  }
  *msb = (val & 0xFF00) >>8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) { // Esta função é utilizada para inserir valores de 8 bits, ao invés de 32 bits
  if(value==NULL){                             // sys_inb é utilizado para ler alguma coisa, sys_outb é utilizado se o controlador quiser escrever alguma coisa
    return 1;
  }
  uint32_t num;
  int ret = sys_inb(port, &num);
  #ifdef LAB3
    extern uint32_t counter;
    counter++;
    #endif
  *value = num;
  *value = *value & 0xFF;
  return ret;
}

