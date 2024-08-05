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
#define RxEnd 0x0D         //fim do quadro
#define RxList 0x4C       // 4C = L em ascii, exibir lista
#define RxAgend 0x41       //41 = A em ascii, agendar
#define RxProx 0x50       // 50 = P proxima da lista
#define RxErase 0x52       //52 = R apagar a lista
#define VAZIO 0x20         //20 = espaço no ascii

const char msg0[] = "L-Exibe lista, A-Agenda, P-Proximo, R-Apaga lista";
const char msg1[] = "Lista de agendamentos:"; //Comando L
const char msg2[] = "Lista de agendamentos vazia"; //Comando L,P
const char msg3[] = "Proximo:"; //Comando P
const char msg4[] = "Digite o nome:"; //Comando A
const char msg5[] = "Nao foi possivel agendar (nome vazio)"; //Comando A
const char msg6[] = "Todos os agendamentos foram atendidos"; //Comando P
const char msg7[] = "Nao disponemos de mais agendamentos"; //Comando A
const char msg8[] = "O agendamento foi realizado"; //Comando A

uint8_t rx_count =0;
uint8_t command = 0;
uint8_t ang_count = 0;
uint8_t atend_count = 0;
uint8_t receive;
char buffer_rx[21]; //Recebimento para o nome de cadastro tem que ser do mesmo tamanho

typedef struct {
	char estado;
	char nome[21]; //maximo de 20 caracteres + caracter de fim
}en_eepr;
__eeprom en_eepr name_list[10];

void send_Msg(const char *msg){
    uint8_t i=0;
    while(msg[i]>0){            
        EUSART_Write(msg[i]);   
        i++;
    }
    EUSART_Write(RxEnd); //mandando o final para saber que acabou
}
void send_Name(char *name){
    uint8_t i=0;
    while(name[i]!= 0){           
        EUSART_Write(name[i]);  
        i++;
    }
    EUSART_Write(RxEnd); //mandando o final para saber que acabou
}

void print_list(){
    if(ang_count == 0){ //nenhum agendamento
            send_Msg(msg2);
        }else{
            send_Msg(msg1);
            uint8_t k = 0;
            while(k<ang_count){
                EUSART_Write(VAZIO);
                EUSART_Write(name_list[k].estado);
                send_Name(name_list[k].nome);
                k++;
            }
        }
}

void Agend(){
     if(ang_count - atend_count == 10){ // agenda lotada
            send_Msg(msg7);
        }else{
            send_Msg(msg4);
            receive = EUSART_Read();
            if(receive == VAZIO || receive == RxEnd)send_Msg(msg5);
            else{
                for(uint8_t i = 0; i<=20 && receive != RxEnd; i++){
                    buffer_rx[i] = receive;
                    buffer_rx[i+1] = 0;
                    receive = EUSART_Read();
                }
                uint8_t j =0;
                while(buffer_rx[j] != 0){
                    name_list[ang_count].nome[j] = buffer_rx[j];
                    j++;
                }
                ang_count++;
            }
        }
}

void Next(){
        if (ang_count- atend_count <= 0){
            send_Msg(msg2);
        }
        else{
            send_Msg(msg3);
            name_list[atend_count].estado = 'X';
            EUSART_Write(VAZIO);
            EUSART_Write(name_list[atend_count].estado);
            send_Name(name_list[atend_count].nome);
            atend_count++;
        }
}

void Erase(){
        for(int i = 0; i<atend_count ; i++){
            name_list[i].estado = 0; //apagando os estados
        }
        atend_count = 0;
    }

void main(void)

{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    while (1)
    {
        if(EUSART_is_rx_ready()){
            command = EUSART_Read();
            if(command == RxList) print_list();
            else if(command == RxAgend) Agend();
            else if(command == RxProx) Next();
            else if(command == RxErase) Erase();
            else send_Msg(msg0);  //enviando os comandos novamente pois ele digitou nenhum comando
        }
    }
}
/**
 End of File
*/