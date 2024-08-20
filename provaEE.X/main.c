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

uint8_t distanceL, distanceH, tempH, tempL = 0;
uint16_t distance, temp = 0;
uint16_t air_flux = 0;

void send_data(){
    temp = ADC_GetConversion(0); //pegando a temperatura
    temp = (temp * 4.88); //ganho do FVR esta em 2.048, entao essa é a temp em unidade reais 
    //1000/2048 = 4.88      
    distanceL = distance & 0xFF;
    EUSART_Write(distanceL);
    distanceH = (distance >> 8) & 0xFF;
    EUSART_Write(distanceH);
    tempL = temp & 0xFF;
    EUSART_Write(tempL);
    tempH =  (temp >> 8) & 0xFF;
    EUSART_Write(tempH);
}

void Trigger_and_Echo(){
    LATAbits.LATA4 = 1; //lançando o sinal Trigger
    __delay_us(10);
    LATAbits.LATA4 = 0;
    while(!LATAbits.LATA3); //espera até Echo ficar alto
    TMR1_WriteTimer(0); //voltando a 0
    TMR1_StartTimer();
    while(LATAbits.LATA3); //espera até Echo ficar baixo
    TMR1_StopTimer();
    uint16_t tempo_voo = TMR1_ReadTimer(); //o valor do timer agora corresponde ao tempo de voo (em microsegundos)
    distance = (float)(tempo_voo * 340.0/2.0); //calcula a distância (considerando a velocidade do som = 340 m/s)
    //dist em mm
}
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts*
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR0_SetInterruptHandler(send_data);  //interrupcao para mandar os dados
    while (1)
    {
        if(LATAbits.LATA1 == 0) air_flux += 10; //Se pressionar o aumento de fluxo de ar, aumenta
        if(LATAbits.LATA2 == 0) air_flux -= 10;//Se pressionar a dimunuicao de fluxo de ar, diminui    
        if(air_flux >= 1023) air_flux = 1023; //defininido os limites
        if (0 >= air_flux ) air_flux = 0; //defininido os limites
        EPWM1_LoadDutyValue(air_flux);
        Trigger_and_Echo();
    }
}
/**
 End of File
*/