#include <xc.inc>
    
global valorD
global valorB
global MSB_result
global LSB_result
global cont
global id
PSECT udata_shr
valorD:
    DS 1
valorB:
    DS 1
MSB_result:
    DS 1
LSB_result:
    DS 1
cont:
    DS 1
id:
    DS 1
    
m_inicial equ 0x20 ; inicial
m_final	  equ 0x6F ;final
	    
psect resetVec,class=CODE,delta=2
resetVec:
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
    movlw 0xFF ; colocando todos os bits de W em 1
    movwf TRISB ;ENTRADA
    movwf TRISD ;ENTRADA
    bsf	  TRISE , 0 ;apenas a porta E0, para 1, para ser entrada
    BANKSEL PORTA ;selecionando o banco dos PORT
    clrf PORTA
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    movlw m_inicial
    movwf id ; definindo o primeiro endereço de memoria
    
verifica_m:;codigo para verificar qual Mult vai ser feita
    BANKSEL PORTA
    clrwdt ; limpando o cao de guarda
    btfsc   PORTE, 0 ; verificando se ele está limpo, se estiver ele pula uma linha 
    goto shift_m
    
soma_m: ;codigo para multiplicação por soma
    clrw
    movf PORTB , W
    movwf valorB
    movf PORTD, W
    movwf valorD
    sublw valorB ;subtraindo PORTB - PORTD
    btfsc STATUS, 0 ;se o carry tiver acertado, pula a proxima instrução
    goto valBMAIORvalD
    	    
    valBMENORvalD:
    movf valorD, W
    addwf LSB_result, F
    btfss STATUS, 0 ; verificando carry, se tiver pula a instrução
    goto semcarry1
    incf MSB_result, F ;add o carry
    
	semcarry1:
	    decfsz valorB ;decrementando o multiplicador
	    goto valBMENORvalD
	    goto armazena_reg
	    
    valBMAIORvalD:
    movf valorB, W
    addwf LSB_result, F
    btfss STATUS, 0 ; verificando carry, se tiver pula a instrução
    goto semcarry2
    incf MSB_result, F ;add o carry
    
	semcarry2:
	    decfsz valorD ;decrementando o multiplicador
	    goto valBMAIORvalD
	    goto armazena_reg    
shift_m:
    movf PORTB , W ; movendo o valor para B
    movwf valorB
    movf PORTD, W ; movendo o valor para D
    movwf valorD
    movlw 0x08 ; contador de 8 bits
    movwf cont
    
    loop_shift:
	rrf valorB ; deslocando para a direita
	btfss STATUS, 0  ; se o carry tiver 1, pula
	goto desloca
	movf valorD, W ; move o valor de D para W
	addwf MSB_result, F ; adciona com o MSB
	desloca:
	    bcf STATUS, 0 ;limpa o carry
	    rrf MSB_result ; desloca tudo a direita
	    rrf LSB_result
	    decfsz cont, F ; desconta do contador
	    goto loop_shift
	    
armazena_reg:
   bcf STATUS, 7 ;decidindo se vai ser banco 0 ou 1
   bcf FSR, 7 ; escolhendo o banco 0
   movf id, W ; id foi inicializado no começo do codigo como 0x20
   movwf FSR ; id indo para o registrador
   movf PORTB, W
   movwf INDF ; movendo o valor do portb para o id
   bcf STATUS, 7 ;decidindo se vai ser banco 0 ou 1
   bsf FSR, 7 ; escolhendo o banco 1
   movf PORTD, W
   movwf INDF ;  movendo o valor do portD para o id
   bsf STATUS, 7 ;decidindo se vai ser banco 2 ou 3
   bcf FSR, 7 ; escolhendo o banco 2
   movf MSB_result, W
   movwf INDF ; movendo o valor do LSB para l
   bsf STATUS, 7 ;decidindo se vai ser banco 2 ou 3
   bsf FSR, 7 ; escolhendo o banco 3
   movf LSB_result, W
   movwf INDF
   bcf STATUS, 2 ; limpando o Z para certificar que está desativado
   movlw m_final ;  registrado de fim o qual eh 0x6F
   subwf id  , W
   btfsc STATUS, 2
   goto reseta_reg
   incf id, F ; incrementando o ID
   
print_resultado:
    movf MSB_result, W
    movwf PORTC
    movf LSB_result, W
    movwf PORTA
    clrf LSB_result
    clrf MSB_result
    clrf valorB
    clrf valorD
    goto verifica_m
    
 reseta_reg:
    movlw m_inicial
    movwf id
    goto print_resultado
    