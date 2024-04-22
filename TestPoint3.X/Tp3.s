#include <xc.inc>
    
global valorDenominador
global valorNumerador
global resto
global quociente
global cont
    
PSECT udata_shr
valorDenominador:
    DS 1
valorNumerador:
    DS 1
resto:
    DS 1
quociente:
    DS 1
cont:
    DS 1 
 led_on MACRO
    bsf PORTE, 1 ; 
ENDM
psect code,class=CODE,abs,delta=2
ORG 0x000
    PAGESEL iniciar
    goto iniciar
    
psect code,class=CODE,delta=2
iniciar:
    CONFIG FOSC = INTRC_NOCLKOUT 
    BANKSEL ANSEL ; definindo todos as portas como digitais
    clrf ANSEL
    clrf ANSELH
    BANKSEL TRISA ; Indo ao banco dos tris para definir como entradas e saidas
    clrf TRISA ;SAIDA
    clrf TRISC ;SAIDA
    clrf TRISE ;Saida
    movlw 0xFF ; colocando todos os bits de W em 1
    movwf TRISB ;ENTRADA
    movwf TRISD ;ENTRADA
    bsf	  TRISE , 0 ;apenas a porta E0, para 1, para ser entrada
    bsf OPTION_REG, 7  ; Ativa o pull-up para todos os pinos de E/S configurados como entrada
    
verifica_d:;codigo para verificar qual Div vai ser feita
    BANKSEL PORTA
    clrwdt ; limpando o cao de guarda
    btfsc PORTE, 0 ; verificando se ele está limpo, se estiver ele pula uma linha 
    call shift_d
    btfss PORTE, 0;verificando se está setado, se estiver setado, pula uma linha
    call subtracao_d
    call armazena_reg	
    goto verifica_d
    
 subtracao_d: ;codigo da divisao por subtração
    movf PORTB , W
    movwf valorNumerador
    movf PORTD, W
    movwf valorDenominador
    xorwf 0x00 ;verificando se o Denominador é zero
    btfsc STATUS, 2
    goto operation_0
    movf valorNumerador, W ;movendo o numerador para W
    movwf resto ; setando o resto igual o numerador
    movf valorDenominador, W ; colocando denominador em W
    operation_start:
    subwf resto, F ;resto - denominador(valor do numerador está no resto) 
    btfsc STATUS, 2 ;se o Z estiver limpo, pula
    goto end_op ; rotina para incf o quociente e retornar
    btfss STATUS, 0; verificando o Carry, se for 1 pula
    goto set_resto ; setando o resto
    incf quociente
    goto operation_start
    
    set_resto:
	addwf resto, F
	return
    
    end_op:
	incf quociente
	return
    operation_0:
	clrw
	movwf quociente
	movwf resto
	led_on 
	return
	

shift_d:
    movf PORTB , W ; movendo o valor para B
    movwf valorNumerador
    movf PORTD, W ; movendo o valor para D
    movwf valorDenominador
    xorwf 0x00 ;verificando se o Denominador é zero
    btfsc STATUS, 2
    goto operation_0
    movlw 0x08 ; contador de 8 bits
    movwf cont
    
 loop_shift:
   rlf valorNumerador, F ; desloca o numerador para esquerad
   rlf resto, F ;deslocar o resto para a esquerda
   movf valorDenominador, W 
   subwf resto, W ; W= Resto-Denominador
   btfsc STATUS, 0
   movwf resto ;se o resultado for negativo, armazena em resto
   rlf quociente,F ;deslocando o quociente para a direita para que se pegue o carry da operação passada
   decfsz cont
   goto loop_shift
   return
    
armazena_reg:
    clrwdt
    banksel PORTA ;selecionando o banco de regs 0
    movlw high(Tabela_Ascii)		     
    movwf PCLATH
    movf quociente, W
    andlw 0x0F ;fazendo o nibble menos significativo permanecer
    call Tabela_Ascii
    movwf 0x21 ; movendo para 0x21 o nibble incial
    swapf quociente, W ; pegando agr o Nibble mais significativo e colocando em W
    andlw 0x0F ;fazendo o nibble menos significativo permanecer
    call Tabela_Ascii
    movwf 0x20; movendo para o prox endereço de memoria
    movf resto, W
    andlw 0x0F ;fazendo o nibble menos significativo permanecer
    call Tabela_Ascii
    movwf 0x23; movendo para 0x23 o nibble incial
    swapf resto, W ; pegando agr o Nibble mais significativo
    andlw 0x0F ;fazendo o nibble menos significativo permanecer
    call Tabela_Ascii
    movwf 0x22 ; armazenando no reg final(seguindo de acordo com o que foi pedido no TP)
    ;printando o resultado
    movf quociente, W
    movwf PORTA
    movf resto, W
    movwf PORTC
    clrf quociente
    clrf resto
    return
 
ORG 0X300
Tabela_Ascii:
   addwf PCL, F
   retlw 0x30 ;0
   retlw 0x31;1
   retlw 0x32 ;2
   retlw 0x33 ;3
   retlw 0x34;4
   retlw 0x35;5;
   retlw 0x36;6;
   retlw 0x37;7
   retlw 0x38;8
   retlw 0x39;9
   retlw 0x41;A
   retlw 0x42;B
   retlw 0x43;C
   retlw 0x44;D
   retlw 0x45;E
   retlw 0x46;F