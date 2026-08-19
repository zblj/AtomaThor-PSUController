#ifndef BOARDCOMMANDS_H
#define BOARDCOMMANDS_H

#include <stdint.h>

#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 64
#define MAX_PARAMS 8

typedef void (*CommandFunc)(const char *params[MAX_PARAMS], int countOfParams);

typedef struct {
    CommandFunc cmd;
    const char *name;
} Command;

void USART_IRQ_Hnadler(void);

int splitCommand(char *str, char *cmd, char *params[MAX_PARAMS]);
int findNextCommandParameter(const char *str);

void processCommand(void);

void readAdc(const char *params[MAX_PARAMS], int countOfParams);
void setDac(const char *params[MAX_PARAMS], int countOfParams);

#endif