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

## The PC´s Keyboard - LAB3

### Mini resumo do lab3

  1. Escrever funções *int kbd_test_scan()* e *int kbd_test_poll()*
  2. Requerem que programemos o controlador do teclado
  3. ESTAS FUNÇÕES NÃO SÃO REUTILIZÁVEIS NO PROJETO

### Input de dados do PC Keyboard

  - O teclado tem o seu próprio chip controlador ( C@KBD )
 
  - Quando uma tecla é pressionada, o chip do teclado gera um scancode, coloca -o num buffer e envia para o PC ( scancode é, normalmente, 1 byte)
 
  - Acontece a mesma coisa quando uma tecla é libertada ( breakcode - é o makecode com o MSB a 1)
  
  - A comunicação entre o PC e o C@KBD é feito através de uma serial line
 
  - No lado da comunicação do PC, esta comunicação é controlada pelo keyboard controller (KBC)

  - Quando *OUT_BUF* (@ port Ox60) está vazia:
      1. KBC sinaliza que *OUT_BUF* está vazia pela serial bus
      2. O C@KBC envia o byte que está na head do seu buffer para o KBC
      3. O KBC coloca esse byte no *OUT_BUF*
      4. O KBC gera um interrup levantando IRQ1;
   
### Os registos KBC

O KBC tem 2 registos na porta Ox60:

  1. IN_BUF -> utilizado para enviar comandos para o teclado (KBD commands) (não usamos em lcom)
  2. OUT_BUF -> utilizado para receber scancodes

O KBC tem mais 2 registos na porta Ox64:

  1. Status Register -> para ler o estado do KBC
  2. Not named -> para escrever KBC commands (não é fundamentalmente diferente do KBD)

### Status Register

Tanto o *OUT_BUF* como o *IN_BUF* implicam ler o status register

<img width="575" alt="Captura de ecrã 2024-03-01, às 15 33 05" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/1588debd-65ba-446a-9b66-72169c513ae2">

!!!!Bits 6 e 7 sinalizam um erro na serial communication entre o teclado e KBC
    - Temos de verifica-lo no Interrupt Handler
    - Devemos também ler sempre o *OUT_BUF* mas descartá-lo em caso de erro

!!!! SE IBF FOR 1, IBF está set, mas não escreve no IN_BUF para ambas as portas Ox60 e Ox64
     
### kbd_test_scan() LAB 3

Esta função dá print tanto do makecode como do breakcode.

  1. Deve terminar quando lê o breakcode da ESC key: Ox81
  
  2. O primeiro byte de ambos os scancodes é normalmente OxEO
  
*COMO PODEMOS FAZER ESTA FUNÇÃO*

  1. Temos de subscrever os interrupts do KBC
       - Quando recebermos um interrupt, temos de ler o scancide de *OUT_BUF*
       - Não precisamos de configurar o KBC pq já está inicializado no MINIX

  Problema!!! - Minix já tem um Interrupt handler instalado
       - Este IH tem de ser desativado para previnir que este leia o OUT_BUF antes do handler

  Solução!!! - Utilizar o IRQ_REENABLE e também o IRQ_EXCLUSIVE policy na sys_irqpolicy(), ou seja, usar como argumentos IRQ_REENABLE | IRQ_EXCLUSIVE

  KBC interrupt subscription encontra-se agora em mode exclusivo!

  *driver_receive()* loop funciona da mesma forma que no lab2.

  - Interrupt handler lê os bytes do KBC´s *OUT_BUF*
  - Deve ler apenas um byte por interrupt (comms entre teclado e KBC é muito lenta)
  - Tem de ler sempre o status register para verificar se existiu um erro
  - Não pode printar os scancodes (pq não são reutilizáveis)

### Contar o número de sys_inb() calls

O objetivo de contar o número de sys_inb() calls é comparar I/O interrupt-driven e um I/O poll-based

!!! Não queremos este feature no projeto

Solução - utilizar #ifdef antes de cada *sys_inb()* ou *util_sys_inb()*
        - Esta chamada tem um incrementador de um contador
  
        - Utilizar uma wrapper function util_sys_inb(), porque já a chamamos, ao invés da sys_inb() e assim só é preciso aumentar o counter
        
### Commandos do KBC para PC-AT/PS2

* Estes comandos devem ser escritos utilizando aderessos, assim como os argumentos se forem necessários. 
* Para além disso os valores do return deverão ser passados no OUT_BUF.

<img width="623" alt="Captura de ecrã 2024-03-01, às 15 53 10" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/8b80d7cf-9fff-4d07-94d0-5a09180f183b">

- KBD Interface - serial interface entre teclado e KBC
                - Para desativar o KBD Interface temos de deixar a clock line baixa


### KBC "Command Byte"

<img width="595" alt="Captura de ecrã 2024-03-01, às 15 55 26" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/91827be9-8730-4e1c-a8cc-928b67856777">

* DIS2 : se for 1 desativa o mouse interface
* DIS : se for 1 desativa o keyboard interface
* INT2 : se for 1 ativa interrupt no OUT_BUF, a partir do rato
* INT : se for 1 ativa o interrupt no OUT_BUT, a partir do teclado
* BITS 2,3,6,7 não são relevantes para o lab

- READ : Utilizar o comando Ox20, que deve ser escrito para Ox64, mas o valor do "command byte" deve ser lido a partir da Ox60
- WRITE : Utilizar o comando Ox60, que deve ser escrito para Ox64, mas o novo valor do "command byte" tem de ser escrito para Ox60

### Notas importantes sobre registos KBC

* Comandos para o KBC -> acessados através do aderesso Ox64;
* Argumentos dos comandos do KBC -> acessados através do aderesso Ox60;
* OUT_BUF -> aderesso Ox60, utilizado para ler
* IN_BUF -> aderesso Ox64 ou Ox60 -> utilizado para escrever (argumentos ou comandos)
 
### kbd_test_poll() LAB3

* Objetivo da função é ler os scancodes através de polling

*COMO FAZER ISSO*
  - Estar sempre a fazer polling do status register(Ox64) e, se OBF (BIT(0) a 1) estiver set e AUX (BIT(5) a 1) está cleared, ler OB.
  
  - A função *lcf_start()* vai desativar os interrupts do teclado pelo KBC (também previne o IH do MINIX roubar os scancodes)
  - Antes de sair da função deve ativar os interrupts através do command byte

### kbd_test_timed_scan(uint8_t idle)

Similar ao kbd_test_scan() exceto quando o processo deve terminar:
  - Ou libertar a ESC key
  - Ou dps de "idle" seconds, durante os quais nenhum scancode foi recebido

*COMO PODEMOS FAZER ISSO*
  1. Temos de subscrever tanto os interrupts do keyboard como do timer/counter
       - *msg.m_notify.interrupts & timer0_int_bit)* 
       - *msg.m_notify.interrupts & kbd_int_bit)*
  2. Estes 2 interrupts devem ser lidados em "driver_receive()" loop


## Computer Labs: The PS/2 Mouse (Lab 4)

 * Devemos utilizar o KBC controller (i8042) para interagir com o rato
 
 * Processar inputs do rato
 
 * Utilizando state machines

### PS/2 Operação do rato

 * O rato também tem o seu chip controlador como o teclado.
 
 * O seu chip deteta eventos no rato

     - Pressionar/Largar butões do rato
     - Deslocar o rato no plano ( 2 9-bit (2´s complement) counters, um por direção)
     - Estes recebem um reset cada vez que o controlador retorna o seu valor

  * Rato reporta estes eventos ao KBC, enviando 3-byte packet através da serial line

### PS/2 Data Packet do rato

<img width="677" alt="Captura de ecrã 2024-03-08, às 18 55 04" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/62ec523a-84d8-4136-b7f6-fe728d6ddeaf">


* Um **scaling parameter** no controlador do rato afeta o valor dos contadores reportados pelo rato:
    - 1:1, neste caso os valores reportados são os dos counters
    - 2:1, neste caso os valores reportados são uma função do
counter conforme determinado por uma tabela

### Modos de Operação do rato

**STREAM MODE** - O rato envia o data packet num maximum fixed rate para o KBC, este rate é programável.

**REMOTE MODE** - O rato apenas envia data packets a pedido do KBC

* Independentemente do modo, cada um dos bytes do mouse data packet são colocados no output buffer do KBC

* O IH do rato deve ler um byte de cada vez, no remote mode é mais fácil não utilizar interrupts


### LAB4 : *mouse_test_packet()*

**O QUE A FUNÇÃO FAZ:** 

  * Printa os packets recebidos pelo rato em **STREAM MODE**
  
    - Deve terminar depois de receber n nº de packets
    - Deve dar display do conteúdo dos packets numa maneira humana

**COMO FAZER ESTA FUNÇÃO:**

  * Temos de subscrever as interrupções do rato, ou seja, a cada interrupt devemos ler o *OUT_BUF*

  * Não precisamos de configurar o mouse pq já está inicializado pelo MINIX

  * Porém, precisamos de ativar o stream mode, pq o minix desativa-o em escrita de texto
  
  * Temos de desabilitar o IH já instalado no MINIX, através do **IRQ_EXCLUSIVE** policy 

  * IH tem de ler os bytes através do *OUT_BUF* do teclado

      - **packet[]** para guardar os packet bytes
      - **counter** para saber qual é o byte number

**PROBLEMAS NA SINCRONIZAÇÃO:** 

  * O problema é que os bytes num packet não têm id
  * A solução é que o bit 3 do primeiro byte de um packet está sempre set

### PS/2 Mouse-Related KBC Commands

 <img width="676" alt="Captura de ecrã 2024-03-08, às 19 10 47" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/72fef180-f042-4c9e-b750-6aa6d34d7c05">

### Status Register bit 5 do AUX

* Bit 5 , AUX, indica se os dados no OUT_BUF vêm do rato ou do teclado

* Não escrever no **IN_BUF** ou no **CTRL_REG** se bit for 1

 ### Comandos do Rato

 <img width="672" alt="Captura de ecrã 2024-03-08, às 19 13 45" src="https://github.com/DiogoSV7/ResumosLCOM/assets/145665382/7b380657-0df1-4f8f-8104-de8b14c13537">

* Cada um destes comandos é enviado para o rato e não é interpretado pelo KBC

* Em resposta a todos os bytes que recebe, o controlador do rato envia um acknowledgment byte:

    - **ACK** (OxFA) - caso esteja tudo OK
    - **NACK** (OxFE) - se tiver um byte inválido
    - **ERROR** (OxFC) - caso seja o 2º consecutivo invalid byte
 
* O acknowledgment byte para cada argumento escrito deve, também, ser colocado no **OUT_BUF** do KBC

**!!!! IMPORTANTE - O ACKNOWLEDGMENT BYTE NÃO É UMA RESPOSTA AO COMANDO**

* O **mouse_test_async() também deve subscrever o Timer 0 tal como o kdb_test_timed_scan()


  
