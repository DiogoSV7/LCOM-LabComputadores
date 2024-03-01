# Resumos de Laboratório de Computadores

## Lab 2 : The i8254 Timer/Counter

  Funções como : *int timer_test_read_config(uint8_T timer, enum timer_status_field field)* requerem programar o PC´s Timer/Counter

### The i8254 

  O i8254 é um timer/counter programável. 
      - Tem 3 16-bit counters
      - Cada um pode contar tanto em binário como em BCD
      - Tem 6 counting modes, que determinam oq acontece qd counting value chega a 0 ou o out pin changes

![image](https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/fb1d490c-2195-43b8-8c8a-01cbf153fa1d)

### i8254 Counting Modes (4 de 6)

  Mode 0 -> Interrupt on terminal count ( contar eventos )
            - *OUT* goes high e permanece high quando count chega a 0

  Mode 1 -> Hardware retriggerable one-shot 
            - *OUT* goes low and remains low until count chega a 0, counter é reloaded na rising edge do *ENABLE* input

  Mode 2 -> Rate Generator ( divide-por-N counter )
            - *OUT* goes low por um clock cycle when counter reaches 0, o counter é reloaded com o seu ccount inicial

  Mode 3 -> Square Wave Generator ( !!!!!! FOR LAB 2 )
            - Similar ao modo 2, exceto pelo seu duty-cycle
            - *OUT* vai ser high por metade do cíclo e low pelo resto do cíclo

  !!!! Em todos os modos, counters fazem count down a partir de um valor inicial programável

  ### I8254 Block Diagram

  ![image](https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/a478d081-5561-4516-a845-387aa9cd3e56) 

  *3 counters de 16 bits independentes:*
  
    $ Portas 0x40, 0x41, Ox42
    $ MSB e LSB são adressados separadamente
    $ Counting modes são todos independentes
    $ Valores de counting iniciais independentes

  *1 control register de 8 bits:*

    $ Porta 0x43
    $ Cada counter é programado independentemente

  ### i8254 Control World

    * Control word tem de ser escrita no control register (Ox43) 

    * BIT 0 (BCD)  - Se for 1 -> BCD
                   - Se for 0 -> Binário

    * BIT 1,2,3 - o número em binário corresponde ao counting mode

    * BIT 4,5  (Counter inicialization) - 01 -> LSB (least significant byte)
                                        - 10 -> MSB (most significant byte)
                                        - 11 -> LSB seguido de MSB

    * BIT 7,6 (Counter selection) - 00 -> counter 0
                                  - 01 -> counter 1
                                  - 10 -> counter 2

### i8254: Read-back Command

<img width="883" alt="Captura de ecrã 2024-03-01, às 15 16 26" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/e4cd8e29-2377-461c-9601-4d2f42d96eea">



    * O comando permite retornar a configuração programada e o valor atual do counter.
    * As barras em cima de *COUNT* e *STATUS* significam que estes bits estão ativos em 0.
    * O comando é escrito no control register (0x43).

    *LEITURA DO STATUS/COUNT*
    
    * A configuração (status) é lida a partir do timer´s data register
              
### Utiliziçaõ de i8254 no PC

  - Timer 0: Utilizado para fornecer uma base temporal ( Por isso é que é o timer a ser subscrito na implementação do timer)
  - Timer 1: Utilizado para dar refresh a DMA
  - Timer 2: Utilizado para gerar tone

  - É necessário utilizar as funções SYS_DEVIO kernel call para I/O.
      *int sys_inb(int port, u32_t *byte);
      *int sys_outb(int port, u32_t byte);

  - Não há necessidade de utilizar control register antes de acessar qualquer 1 dos timers.

### C Enumerated Types

  - enum timer_status_ field {
      - tsf_all,               (configuração em hexadecimal)
      - tsf_initial,           (modo de inicialização do timer)
      - tsf_mode,              (timer counting mode)
      - tsf_base               (timer counting base)
- };

- O prefixo "tsf_" é utilizado para evitar colisões

### C Unions

  - union timer_status_field_val{
        - uint8_t      byte;
        - enum timer_init       in_mode;
        - uint8_t      count_mode;
        - bool         bcd;
  -};

### Generalizações de C

  *Bitwise operators:*

    $ & bitwise AND
    $ | bitwise inclusive OR
    $ ^ bitwise exclusive OR
    $ ~ one´s complement

  *Logical operator*

    $ && logical AND
    $ || logical OR
    $ ! negation

  *Masks*

    $ uchar mask = Ox80;  -> 10000000 binary

  *Shift Operators* 

    $ << left shift (RHS) -- Right hand side tem de ser um nº não negativo
    $ >> right shift (LHS) -- Left hand side pode ser qualquer tipo de inteiro

  *Ports*

    $ port - 0x5a (8-bit) ou Ox0000005a (32-bit)
    $ ~port - 0xa5 (8-bit) ou Oxffffffa5 (32-bit)

  *Como definir uma macro*

    $ #define MACRO Ox41 - Atribuir uma string a um valor inteiro

## I/O and Interrupts

### I/O Operations

  * I/O devices normalmente são muito mais lentos que o processador, então este tem de esperar que os mesmos completem a sua operação atual

Como é que o processador sabe de um I/O evento?

1. POLLING - o processador polls o I/O device e lê o status register para descobrir
      - Polling apresenta um tempo de resposta *ALTAMENTE VARIÁVEL*
      - Bandwidth pode ser alta se interface bus for rápido e processador polls frequentemente.

2. INTERRUPTS - o I/O device notifica o processador através de interrupts
      - Interrupts apresentam um tempo de resposta *NORMALMENTE RESPONSIVOS*
      - Bandwidth media para pequena, depende na quantidade de dados disponíveis para serem transferidos
  
### PC Interrup Hardware: Priority Interrupt Controller(PIC)

  ![image](https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/35873fd2-5b4c-46b6-9cbb-b2b2d9db4c7c)


### IRQ lines and Vectors

  IRQ line -> determinada pelo HW designer
  Vector -> Também especificado pelo HW designer, porém pode ser configurado no boot time

### Interrupt Handlers (IH)

  - Interrupt handlers são executados pelo hardware ao chegar a um interrupt
  - No MINIX 3, device drivers sao implementados as user-level processes, ao invés de kernel-level
  - Então como é que fazemos interrupt handling em MINIX 3?

    *SOLUÇÃO*
    1. Executar o mínimo no kernel, isso é feito pelo generic interrupt handle (GIH)
    2. Operações específicas dos devices são executadas pelas device drivers no user-level
   
### Generic Interrupt Handle (GIH)

  Quando é apresentado com um interrupt, the GIH does:
  
    1. Masks, in the PIC, a respetica IRQ line.
    2. Notifica todas as device drivers interessadas naquele interrupt.
    3. Se possível, unmasks, no PIC, a respetiva IRQ line.
    4. Reconhece o interrupt ao utilizar o EOI command para o PIC.
    5. Envia a IRETD instruction.

### Como é que o GIH sabe quais device drivers estão interessadas num interrupt?

  RESPOSTA: As device drivers dizem, utilizando a kernel call:
    *int sys_irqsetpolicy(int irq_line, int policy, int *hook_id)*

  !!! policy uses *IRQ_REENABLE* para informar o GIH que pode fazer unmask da IRQ line na PIC

  !!! hook_id funciona tanto como:
    - input : um id utilizado pelo kernel na notificação de um interrupt
    - output : um id utilizado pelas device drivers em outras chamadas ao kernel neste interrupt

  Podemos olhar para *sys_irqsetpolicy()* como uma subscrição de notificação de interrupts

### MINIX 3 : Outras kernel calss relacionadas com interrupts

*sys_irqrmpolicy(int *hook_id)** - Cancela a subscrição de notificações de interrupts anterior, ao especificar um pointer para o hook_id.

*sys_irqenable(int *hook_id)** - Unsmaks no PIC uma interrupt line associada com uma notificação de interrupção previamente subscrita.

*sys_irqdisable(int *hook_id)** - Masks no PIC uma linterrupt line associada com uma notificação de interrupção previamente subscrita.

### Como é que o GIH notifica os device drivers da ocorrência de um interrupt?

  RESPOSTA : Utiliza o standard interprocess communication (IPC) mechanism.
              Mais especificamente utiliza NOTIFICAÇÕES

  MINIX 3 IPC:
  
     - Mecanismo baseado em messagens
     - Processos enviam e recebem mensagens para comunicar entre eles e com o kernel
     - Uma notificação é um tipo especial de mensagem, utilizado pelo kernel para uma comunicação não solicitada com um user-level process

### Como é que os device drivers recebem a notificação do GIH?

  1. Utiliza o IPC mechanism
  2. Utiliza some library calls fornecida por *libdrivers* library

  Utilizando *msg.m_notify.interrupts* conseguimos notificar interrupts, uma vez que Interrupt handlers não recebem argumentos e não retornam valores

  *irq_set* é utilizado como uma mask para testar quais interrupts estão pendentes

### O que acontece se o GIH não der unmask à IRQ line no PIC?

  Isto pode acontecer se um device driver não der set à IRQ_REENABLE policy na sua interrupt subscription request.

  RESPOSTA : Se tal acontecer, as device drivers terão de o fazer o mais rápido possível.

### Como é que um device driver pode dar unmask a uma IRQ line no PIC?

  Ao chamar *sys_irqenable(int *hook_id)**, com esta chamada o kernel vai dar unmask à IRQ line a pedido dos device drivers.

### Interrupt Sharing no MINIX 3

  - Minix 3 já inclui o seu próprio TIMER 0 interrupt handler
  - Ao subscrever interrupts na IRQ line 0, o interrupt handler do nossa driver não vai substituir o IH do kernel
  - Deste modo, teremos de partilhar interrupt lines entre dispositivos
  - UTILIZAR 2 IH PARA O MESMO DISPOSITIVO NÃO É OQ QUEREMOS FAZER!!! 








  
 
