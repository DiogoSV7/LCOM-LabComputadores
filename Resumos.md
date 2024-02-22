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

  3 counters de 16 bits independentes:
  
    $ Portas 0x40, 0x41, Ox42
    $ MSB e LSB são adressados separadamente
    $ Counting modes são todos independentes
    $ Valores de counting iniciais independentes

    
