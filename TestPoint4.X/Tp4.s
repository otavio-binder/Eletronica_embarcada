#include <xc.inc>
CONFIG FOSC = INTRC_NOCLKOUT
CONFIG WDTE = ON
CONFIG MCLRE = ON
CONFIG LVP = OFF
    
PSECT udata_shr
 counter:
    DS 1
led_green_blink MACRO
    BANKSEL PORTA 
    bcf PORTA, 0
    call delay_500ms
    bsf	PORTA, 0
ENDM
led_red_blink MACRO
    BANKSEL PORTA 
    bcf PORTA, 1
    call delay_500ms
    bsf	PORTA, 1
ENDM
    
psect code,class=CODE,abs,delta=2
ORG 0x000
    PAGESEL iniciar
    goto iniciar
    
psect code,class=CODE,delta=2
iniciar:
    BANKSEL ANSEL ; definindo todos as portas como digitais
    clrf ANSEL
    clrf ANSELH
    BANKSEL WDTCON
    movlw   0x1F ; Configurando WDT para prescaler de 1:32768 (aproximadamente 1 segundo)
    movwf   WDTCON
    BANKSEL TRISA ; Indo ao banco dos tris para definir como entradas e saidas
    bcf TRISA, 0 
    bcf TRISA, 1 ;dois leds como saida
    banksel OPTION_REG
    movlw  0b00000011 ; Prescaler de 1:16 para Timer0 (ja estamos no banco 1) 
    movwf  OPTION_REG
    banksel TMR0
    movlw 0xff
    movwf PORTA
    clrf TMR0	;zerando o contador
    bcf	    INTCON,2
    bsf	    INTCON,5
  
led_loop:
    clrwdt ;limpando o cão de guarda
    led_red_blink
    call low_power
    led_green_blink
    call low_power
    goto led_loop
 
 delay_500ms:
    movlw 152 ; 
    movwf counter; armazenar em counter

delay_loop:
    movlw 55 ; Valor inicial do TMR0 para overflow 
    clrwdt
    movwf TMR0 ; carregar o valor inicial
    bcf INTCON, 2 ; limpar a flag do TMR0 (T0IF)

wait_for_overflow:
    clrwdt
    btfss INTCON, 2 ; verificar a flag do TMR0 (T0IF)
    goto wait_for_overflow ; esperar o overflow
    clrwdt
    decfsz counter, f ; decrementar counter_125ms
    goto delay_loop ; repetir se não chegou a 0
    return
    
low_power:
    clrwdt ; limpando o cão de guarda
    BANKSEL PORTA
    bcf	    INTCON,5  ; desabilitando o interrupt do TMR0 e habilitando do wdt
    banksel OPTION_REG
    movlw 0b00001101 ;setando o prescaler do WDT para 1:32
    movwf OPTION_REG
    sleep
    clrwdt
    bcf	    INTCON,2 ;limpaando a flag do TMR0
    bsf	    INTCON,5 ; habilitando o vetor de interrupcao
    movlw   0b00000011  ; setando prescaler para 1:16
    movwf   OPTION_REG
    return
    
 