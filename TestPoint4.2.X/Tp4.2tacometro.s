CONFIG FOSC = INTRC_NOCLKOUT    
CONFIG WDTE = ON
CONFIG MCLRE = ON
CONFIG LVP = OFF
    
#include <xc.inc>

global contador
global estado_motor
global select
global status_aux
global temporario
PSECT udata_shr
  
    temporario:
	DS 1
    status_aux:
	DS 1
    contador:
	DS 1
    select:
	DS 1
psect code,class=CODE,abs,delta=2
org 0x0000
resetar:
    PAGESEL iniciar
    goto    iniciar
ORG 0x0004
    movwf   temporario ;armazenando o W atual antes de ir para interrupcao
    swapf   STATUS, w
    movwf   status_aux ;armazenando o status atual antes de ir para interrupca
    btfss INTCON, 2
    goto displays
    goto motor
org 0x0400
iniciar:
    BANKSEL ANSEL ; definindo todos as portas como digitais
    clrf ANSEL
    clrf ANSELH
    BANKSEL TRISA
    clrf TRISC
    movlw 0b11001101 ;Configurando os pinos B
    movwf TRISB
    bsf TRISA, 4
    BANKSEL OPTION_REG
    bcf	 OSCCON,6  ;clock do pic 31k
    bcf	 OSCCON,5	
    movlw 0b01100000
    movwf OPTION_REG
    banksel PORTB
    movlw   0b11100000 ;recebendo o sinal do tacometro, e comecar a contar
    movwf   INTCON  ;ativando as interrupcoes globais 
    movlw   240 ;calibraçao do tmr0 para poder contar 200 voltas
    movwf   TMR0  ;carregando o TMR0
    banksel PIE1
    bsf	    PIE1, 1   ;habilita tmr2
    movlw   71
    movwf   PR2   ;70 pra comparar e ficar 50hz
    BANKSEL PORTA
    bsf	    T2CON,2   ;tmr2 contando
    
   void_loop:
    clrwdt
    movf contador, w
    subwf 0x00, w ;contador- 0
    btfss STATUS, 2 ; pula se for maior 
    goto contador_carregado
    bcf PORTB, 1
    goto void_loop
    contador_carregado:
    bsf PORTB, 1
    movf contador, w
    subwf 0x00, w ;contador- 0
    btfsc STATUS, 2 ; pula se for maior 
    goto void_loop  
    goto contador_carregado
    
    motor:
    bcf INTCON,2
    bsf PORTB, 1
    movlw 240 ;recalibrando
    movwf TMR0 ;recarregando o TMR0 para novas interrupcoes sempre aproximadamente 3 pulsos
    movlw 0xFF
    andlw contador
    btfss STATUS, 2
    goto decrementa_contador ;decrementando o contador, por conta que atingiu 200 pulsos
    motor_off:
    bcf PORTB, 1
    clrf contador ;certificando que está limpo
    goto preservacao_status
    decrementa_contador:
    decf contador
    goto preservacao_status
    
    displays:
	banksel PIR1
	bcf PIR1, 1
	btfsc select, 0
	goto display_MS
	movlw high(driver_display)
	movwf PCLATH
	bcf PORTB, 4 ; desligando o display MS
	movf contador, w
	call driver_display
	movwf PORTC
	bsf PORTB, 5 ;ligando o outro display
	bsf select, 0
	goto preservacao_status
	
	
    display_MS:
	bcf select, 0
	bcf PORTB, 5
	movlw high(driver_display)
	movwf PCLATH
	movf contador, w ;colocando o contador 
	swapf contador, w ; trocando os nibble 
	call driver_display
	movwf PORTC
	bsf PORTB, 4
	goto preservacao_status
	
driver_display:
    andlw   0x0F
    addwf   PCL,f
    retlw   00111111B;0
    retlw   00110000B;1
    retlw   01101101B;2
    retlw   01111001B;3
    retlw   01110010B;4
    retlw   01011011B;5
    retlw   01011111B;6
    retlw   00110001B;7
    retlw   01111111B;8
    retlw   01111011B;9
    retlw   01110111B;a
    retlw   01011110B;b
    retlw   01001100B;c
    retlw   01111100B;d
    retlw   01001111B;e
    retlw   01000111B;f

    preservacao_status:
    swapf  status_aux, w
    movwf   STATUS
    swapf   temporario, f
    swapf   temporario, w
    retfie
	
	