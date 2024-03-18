#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int hook_id=0;
int counter=0;


// timer_set_frequency eu tirei muita inspiração do fábio sá porque não sabia mesmo por onde começar sequer

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(freq > TIMER_FREQ | freq <=19){
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

  if(sys_outb(TIMER_CTRL, control_word) != 0) return 1;
  if(sys_outb(selected_timer,LSB) != 0) return 1;
  if(sys_outb(selected_timer, MSB)!= 0) return 1;
  
  return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
        return 1;

    *bit_no = BIT(hook_id);
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0){
        return 1; 
    }
    return 0; 
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id) != 0){
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(st==NULL || timer > 2 || timer < 0){     // vai estar sempre acima de 0, entao aquela condição é desnecessária
    return 1;
  } 
  uint8_t Read_back_command = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)); // Utilizar estas macros seria a mesma coisa que utilizar BIT(7) | BIT(6)... só que utilizamos macros
  
  if(sys_outb(TIMER_CTRL, Read_back_command)!= 0){   // Verificar se o read_back_command não é horrível
    return 1;
  }                                         // só procuramos saber o timer pq os bits 7,6 já s\ao predefinidos a 1 e o COUNT é inicializado a 1 e STATUS a 0
  if(util_sys_inb(TIMER_0 + timer, st)){
    return 1;
  }                                           // TIMER_0 começa no aderaço 40 então é só aumentar com o timer
  return 0;
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
        break;
      }else if(st==2){
        conf.in_mode = MSB_only;
        break;
      }else if(st == 3){
        conf.in_mode =MSB_after_LSB;
        break;
      }else{
        conf.in_mode = INVAL_val;
        break;
      }
    case tsf_mode:
      st = st >> 1;
      st = st & 0x07;
      conf.count_mode=st;
      break;
    case tsf_base:
      conf.bcd = st & 0x01;
      break;
  }
  if(timer_print_config(timer, field, conf) !=0){
    return 1;
  }
  return 0;
}
