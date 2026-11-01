#include "boardCommands.h"
#include "main.h"
#include "stm32g051xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stm32g0xx_hal.h"
#include "supportingFunctions.h"
#include "analogFunctions.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Internal temperature sensor calibration parameters
#define TS_CAL1_TEMP    30.0f      // °C
#define TS_CAL2_TEMP    130.0f     // °C
#define TS_CAL1_ADDR    ((uint16_t *)0x1FFF75A8)
#define TS_CAL2_ADDR    ((uint16_t *)0x1FFF75CA)

char tx_buff[TX_BUFFER_SIZE];
int txidx, txcnt;
uint8_t TxCmdBusy;

char rx_buff[RX_BUFFER_SIZE];
int rxidx, rxcnt;
uint8_t commandReceived;


void fcn1(const char *params)
{
    strncpy(tx_buff, "fcn1", TX_BUFFER_SIZE);
    txcnt = rxcnt;
    transmitMessage();
}

void fcn2(const char *params)
{
    int nextParam;
    nextParam = findNextCommandParameter(params);
    int num = atoi(params+nextParam);
    //strncpy(tx_buff, "fcn1", TX_BUFFER_SIZE);
    txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "fcn2, %i\n", num);
    transmitMessage();
}

static const Command commands[] = {
    { fcn1, "fn1" },
    { fcn2, "fn2" },
    { readAdc, "readAdc"},
    { checkPower, "pwr"},
    { readSensors, "sns"},
    /* ... */
};

static const int command_count = sizeof(commands) / sizeof(commands[0]);

void USART_IRQ_Hnadler(void){
    uint32_t isrflags   = USART1->ISR;

    uint32_t errorflags;

    uint8_t data;

    /* If no error occurs */
    errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));
    if (errorflags){
        // manage errors
    }
    
    // Rx Flag & Rx interrupts enabled
    if ((USART1->ISR & USART_ISR_RXNE_RXFNE) && (USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE)){
        data = USART1->RDR;
        // USART1->TDR = data; // Loopback
        rx_buff[rxidx] = data;
        if (data == '\n'){
            rx_buff[rxidx] = 0;
            commandReceived = 1;
        }
        rxidx++;
        rxcnt++;
        if (rxidx >= RX_BUFFER_SIZE){
            // TODO Handle buffer overflow
            commandReceived = 1;
        }
    }

    // Tx flag & Tx interrupts enabled
    if ((USART1->ISR & USART_ISR_TXE_TXFNF) && (USART1->CR1 & USART_CR1_TXEIE_TXFNFIE)){
        // Copy data from Tx buffer into Tx register until all data is transferred, then disable Tx interrupts
        USART1->TDR = tx_buff[txidx];
        if (tx_buff[txidx] == 0){
            txidx--;
            txidx++;
        }
        txidx++;
        // Stop when there is no more data to transmit or when the enxt character is \0
        if ((txidx >= txcnt) || (tx_buff[txidx] == 0)){
            USART1->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;    // Disable Tx interrupts
        }

    }

    USART1->ICR = -1; // Clear all interrupt flags, unsafe, but gets the job done
}

int findNextCommandParameter(const char *str){
    // Assumes null termination, will work weird otherwise
    char *separators = " \t,;";

    // If the first character is a whitespace, the code must skip over to the first non-white space character
    uint8_t startedOnWhitespace = 0;

    // int lastSeparator = 0;
    // Go through maximum command size of characters (but stop if 0 is reached)
    for(int i=0; i<RX_BUFFER_SIZE; i++){
        if (str[i] == 0){
            return 0;
        }
        // if the current character is whitespace, return that index unless the code stareted on whitespace
        for(int j=0; j<(sizeof(separators)/sizeof(separators[0])-1); j++){  //-1 in count calculation to ellimiante \0
            if (str[i] == separators[j]){
                // lastSeparator = i;
                // break;
                if (i==0){
                    startedOnWhitespace = 1;    // Although it might be more efficient to do this check in advance, this way we avoid code duplication
                }
                if (startedOnWhitespace){
                    break;
                }
                return i;
            }
        }
        if (startedOnWhitespace && (i != 0)){
            return i;
        }
        // return i;
    }
    return 0;
}

void processCommand(void){
    int endOfCommand;
    int comparison;
    if (commandReceived){
        for (int i = 0; i < command_count; i++) {
            endOfCommand = findNextCommandParameter(rx_buff);
            if (endOfCommand){
                comparison = strncmp(commands[i].name, rx_buff, endOfCommand);
            }
            else{
                comparison = strcmp(commands[i].name, rx_buff);
            }
            if (comparison == 0) {
                commands[i].cmd(rx_buff + endOfCommand);
                return;
            }
        }
        // If no command match was found, do at least clear the buffer
        rxcnt = 0;
        rxidx = 0;
        commandReceived = 0;
    }
}

void transmitMessage(void){
    // The user is expected to fill the tx_buff and set the number of Tx characters before calling this function.
    // Example:
    //      txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "Hello World!);
    txidx = 0;
    rxcnt = 0;
    rxidx = 0;
    commandReceived = 0;
    USART1->CR1 |= USART_CR1_TXEIE_TXFNFIE;  // Enable Tx interrupts; must be done after setting  indexes, otherwise buggy operation ensues.
}
void readAdc(const char *params){
    const int decimals = 3;
    int nextParam;
    nextParam = findNextCommandParameter(params);
    char channel = params[nextParam];

    // Make sure that channel is valid
    if ((channel-'0') <= 7){
        float Vsns = getVoltage(channel-'0');
        txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "V%i = %i.%0*i V\n", (int)channel-'0', (int)Vsns, decimals, (int)getDecimals(Vsns, decimals));
    }
    else if ((channel == 't') || (channel == 'T')){
        float temperature = getTemperature(ADCmeasurements[6]);
        txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "T = %i.%0*i C\n", (int)temperature, 2, (int)getDecimals(temperature, 2));
    }
    else{
        txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "Chennel should be '0'-'7' or 'T' (is '%c')\n", channel);
    }
    
    transmitMessage();
}

void checkPower(const char *params){
    float Vsns1, Vsns2;

    Vsns1 = getVoltage(0) *11;
    Vsns2 = getVoltage(1) *11;
    
    txcnt = snprintf(tx_buff, TX_BUFFER_SIZE, "V1 = %i.%0*i V, V2 = %i.%0*i V\n", (int)Vsns1, 2, (int)getDecimals(Vsns1, 2), (int)Vsns2, 2, (int)getDecimals(Vsns2, 2));
    
    transmitMessage();
}

void readSensors(const char *params){
    float Vsns1, Vsns2, id1, id2;

    float readings[4];
    readPirani(readings);

    Vsns1 = readings[0];
    Vsns2 = readings[1];
    id1 = readings[2];
    id2 = readings[3];

    txcnt = snprintf(tx_buff, TX_BUFFER_SIZE,  "id1 = %i.%0*ik, V1 = %i.%0*i V, id2 = %i.%0*ik, V2 = %i.%0*i V\n", 
                (int)id1, 2, (int)getDecimals(id1, 2),
                (int)Vsns1, 2, (int)getDecimals(Vsns1, 2),
                (int)id2, 2, (int)getDecimals(id2, 2),
                (int)Vsns2, 2, (int)getDecimals(Vsns2, 2));
    
    transmitMessage();
}

int readPirani(float readings[4]){
    readings[0] = getVoltage(2)*4;  // Vsns1
    readings[1] = getVoltage(4)*4;  // Vsns2
    readings[2] = readResistance(getVoltage(3));    // id1 in kOhm
    readings[3] = readResistance(getVoltage(5));    // id2 in kOhm
    return 0;
}

float readResistance(float voltage){
    const float Vp = 3.3f;  // pullup voltage
    const float Rr = 10.f; // reference resisstor in kOhm
    const float Rp = 1.f;  // protection resistor in kOhm
    float resistance;

    resistance = Rr*voltage / (Vp-voltage) - Rp;
    return resistance;
}