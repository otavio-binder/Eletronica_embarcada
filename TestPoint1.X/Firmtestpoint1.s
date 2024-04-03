 #include<xc.inc>
    
 psect resetVec, class =CODE,delta=2
    resetVec:
	PageSEL iniciar
	goto iniciar
	
psect code, class = CODE, delta = 2
 ;Inicialização do código0
iniciar:
    BANKSEL TRISB ; Acessando o banco de reg do TRISB
    movlw 0xFF ; Setando todos os bits como 1
    movwf TRISB ; Colocando todos os bits para 1 para serem entradas
    movwf TRISC ; COnfigurando todos os TRISC para serem entradas
    clrf TRISA ; deixando todos os pinos A como saida
    bsf TRISA, 5 ; deixando apenas o pino 5 como entrada
    BANKSEL ANSEL
    clrf ANSELH ; Setando todos os bits do ANSELH para 0
    clrf ANSEL ; Setando todos os bits do ANSEL para 0, para que sejam digitais
     
leituraRA5:
    BANKSEL PORTA
    btfsc   PORTA, 5 ; verificando se ele está limpo, se estiver ele pula uma linha
    goto    adicao
    goto    subtracao
    
adicao:
    movf PORTB, W; Armazenando o valor de B para W
    andlw 0x0F  ;pegando apenas os ultimos 4 bits com o nibble
    movwf 0x20  ; movendo para o reg de ID 0x20 (v1)

    movf PORTC, W ; Armazenando o valor de C para W
    andlw 0x0F  ;pegando apenas os ultimos 4 bits com o nibble
    movwf 0x21 ; movendo para o reg de ID 0x21 (v2) 
    
    ;Adcionando os valores
    addwf 0x20, W   ; Add o PORT B e o PORT C
    movwf 0x23	;Armazenando o resultado no reg 0x23
    
    ;Exibição do resultado
    andlw 0x1F; Mostrando apenas os 5 primeiros bits
    movwf PORTA	    ;movendo para a saída A
    goto leituraRA5
    
subtracao:
    movf PORTB, W; Armazenando o valor de B para W
    andlw 0x0F  ;pegando apenas os ultimos 4 bits com o nibble
    movwf 0x20 ; movendo para o reg de ID 0x20 (v1)
    
    movf PORTC, W ; Armazenando o valor de C para W
    andlw 0x0F ;pegando apenas os ultimos 4 bits com o nibble
    movwf 0x21 ; movendo para o reg de ID 0x21 (v2)
    
    ;Subtraindo os valores
    subwf 0x20, W   ; sub o PORTB - PORT C e movendo pra W
    movwf 0x23	;Armazenando o resultado no reg 0x23
    
    ;Exibição do resultado
    andlw 0x1F  ; Mostrando apenas os 5 primeiros bits
    movwf PORTA	    ;movendo para a saída A
    
    goto leituraRA5