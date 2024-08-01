/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1827
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"

/*
                         Main application
 */
#define temp_motor 0
#define sens_lum 1

// buffer para receber a mensagem
    uint8_t rx_buffer[3];
    
    //buffer para os dados a serem enviados
    uint8_t tx_buffer[5];
    
 //ativamento do Tx
    bool activate_tx;   
 //variavel de 16 bits para envio de dados H e L
    uint8_t dataL, dataH;
    
void enviar_tx(){
    if(activate_tx){
        for(int i = 0 ; i<5 ; i++){
            if(i >= 1){
            tx_buffer[i] = tx_buffer[i] & 0x7F;
            }
            EUSART_Write(tx_buffer[i]);
        }
    }
}
void main(void)
{
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable();
    TMR1_SetInterruptHandler(enviar_tx);
    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    float gain = 1;
    uint16_t duty_c = 0 ;
    while(1)
    {   
        uint16_t adc_v_temp =  ADC_GetConversion(temp_motor);
        uint16_t adc_v_lum =  ADC_GetConversion(sens_lum);
        uint16_t porcent_temp =(uint16_t)(adc_v_temp*gain);// Converte para 0 a 100 graus celsius
        uint16_t porcent_lum = (uint16_t)(adc_v_lum*gain);   //Converte para 0 a 100 lum %
        if(EUSART_is_rx_ready()){
            for(int i = 0; i<3 ; i++){
                rx_buffer[i] = EUSART_Read(); //armazenando os dados
            }
        }
        if(rx_buffer[0] & 0x80){  //verificando o bit de comando se é 1
            tx_buffer[0] = 0x80; //enviando o bit de comando 7
            //temperatura 00
            dataL = (porcent_temp & 0x7F);
            dataH = (porcent_temp >> 7 );
            tx_buffer[1] = dataH;
            tx_buffer[2] = dataL ;
           //luminosidade
            dataL = (porcent_lum & 0x7F);
            dataH = (porcent_lum >> 7 );
            tx_buffer[3] = dataH;
            tx_buffer[4] = dataL;
            if(rx_buffer[2] & 0x01){ // ativa o envio das medições
                activate_tx = true;
            }else{
                activate_tx = false;
            }
             
            if(rx_buffer[0] & 0x81){
                duty_c = (rx_buffer[1] << 8) + rx_buffer[2];
                EPWM2_LoadDutyValue(duty_c);
            }
            else if(rx_buffer[0] & 0x82){
                gain = (float)rx_buffer[2]/1000.0;
            }
        }  
    }
}
