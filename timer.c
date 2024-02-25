#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int identifier=0;
int counter=0;


// timer_set_frequency eu tirei muita inspiração do fábio sá porque não sabia mesmo por onde começar sequer

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(freq > TIMER_FREQ | freq <=0){
    return 1;
  }
  uint32_t division_value = TIMER_FREQ/freq;
  if(division_value > UINT16_MAX){
    return 1;
  }
  uint8_t control_word;
  uint8_t MSB, LSB;
  util_get_MSB(division_value, &MSB);
  util_get_LSB(division_value, &LSB);
  uint8_t selected_timer;
  if(timer_get_conf(timer, &control_word) != 0){
    return 1;
  }
  switch(timer){
    case 0:
      control_word = TIMER_SEL0;
      selected_timer= TIMER_0;
      break;
    case 1:
      control_word = TIMER_SEL1;
      selected_timer = TIMER_1;
      break;
    case 2:
      control_word = TIMER_SEL2;
      selected_timer = TIMER_2;
      break;
  }

  // Pq é que inserimos o valor do lsb antes do msb no contador?????

  if(sys_outb(selected_timer,LSB) != 0) return 1;
  if(sys_outb(selected_timer, MSB)!= 0) return 1;
  return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
        return 1;

    *bit_no = BIT(identifier);
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &identifier) != 0){
        return 1; 
    }
    return 0; 
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&identifier) != 0){
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(st==NULL || timer > 2 || timer < 0){
    return 1;
  }
  int timer_port = 0;
  if(timer==0){
    timer_port = TIMER_0;
  }else if(timer==1){
    timer_port = TIMER_1;
  }else if(timer==2){
    timer_port = TIMER_2;
  }
  return util_sys_inb(timer_port, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val conf;
  
  switch(field){
    case tsf_all:
      conf.byte = st;
      break;
    case tsf_initial:
      st = st & 0x30;
      st = st >> 4;
      if(st==1){
        conf.in_mode = LSB_only;
      }else if(st==2){
        conf.in_mode = MSB_only;
      }else if(st == 3){
        conf.in_mode =MSB_after_LSB;
      }
      break;
    case tsf_mode:
      st = st >> 1;
      st = st & 0x07;
      conf.count_mode=st;
      break;
    case tsf_base:
      conf.bcd = st & TIMER_BCD;
      break;
  }
  if(timer_print_config(timer, field, conf) !=0){
    return 1;
  }
  return 0;
}
