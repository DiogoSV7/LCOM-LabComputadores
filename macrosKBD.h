#ifndef _MACROSKBD_H_
#define _MACROSKBD_H_


//Definir a irq line do teclado a 1, pq timer é o 0
#define KBD_IRQ_LINE 1

//Definir hook_id para o keyboard
#define KBD_HOOK_ID BIT(1)

#define KBD_OUT_BUF 0x60
#define KBD_IN_BUF 0x64
#define ESC_KEY 0x81

//Definir macros para os bits mais importantes do status code
#define STATUS_CODE_OBF BIT(0)
#define STATUS_CODE_IBF BIT(1)
#define STATUS_CODE_AUX BIT(5)
#define STATUS_CODE_TIMEOUT BIT(6)
#define STATUS_CODE_PARITY BIT(7)

//Macros para as portas de escrita e leitura do command byte
#define READ_CMD_BYTE_PORT 0x20
#define WRITE_CMD_BYTE_PORT 0x60

#define COOLDOWN 20000
#define TENTATIVAS 5


#endif

