#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8254.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t status_byte;
  if(timer_get_conf(timer, &status_byte)!=0){
    return 1;
  }
  if(timer_display_conf(timer , status_byte, field)!=0){
    return 1;
  }
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if(timer_set_frequency(timer, freq)!=0){
    return 1;
  }
  return 0;
}

int(timer_test_int)(uint8_t time) {
}
