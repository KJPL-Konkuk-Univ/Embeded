/*
 * protocols.c
 *
 *  Created on: 2022. 9. 5.
 *      Author: Trollonion03
 */


#include "driverlib.h"
#include "device.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint16_t RxReadyFlag = 0;
uint16_t RxCopyCount = 16; //default: 16
uint16_t receivedChar[16];

void sendDataSCI(uint32_t SelSCI, uint16_t * TrsData, SCI_TxFIFOLevel size) {
    //uint16_t rACK[16];

    SCI_writeCharArray(SelSCI, TrsData, 32);
    NOP;

    //ADD check ACK
    //TODO : refactor interrupt setup code
#if 0
    SCI_resetChannels(SelSCI);
    SCI_disableInterrupt(SelSCI, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_clearInterruptStatus(SelSCI, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SelSCI);
    SCI_enableModule(SelSCI);
    SCI_performSoftwareReset(SelSCI);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0 , SCI_FIFO_RX16);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
#endif

//    SCI_resetChannels(SelSCI);
//    SCI_clearInterruptStatus(SelSCI, SCI_INT_TXFF | SCI_INT_RXFF);
//    SCI_enableFIFO(SelSCI);
//    SCI_enableModule(SelSCI);
//    SCI_performSoftwareReset(SelSCI);
//
//    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX16);
//    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);

//    while(RxReadyFlag == 0);
//    memcpy(rACK, receivedChar, SCI_FIFO_RX16*2);
//    if(rACK[1] != 8) {
//        ESTOP0;
//    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

void rcvCmdData(uint32_t SelSCI, uint16_t * RcvData, SCI_RxFIFOLevel size) {
    RxReadyFlag = 0;
    RxCopyCount = size;

#if 0
    SCI_resetChannels(SelSCI);
    SCI_disableInterrupt(SelSCI, SCI_INT_TXRDY | SCI_INT_RXFF);
    SCI_clearInterruptStatus(SelSCI, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableFIFO(SelSCI);
    SCI_enableModule(SelSCI);
    SCI_performSoftwareReset(SelSCI);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, size);
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXFF);
#endif

//    SCI_resetChannels(SelSCI);
//        SCI_clearInterruptStatus(SelSCI, SCI_INT_TXFF | SCI_INT_RXFF);
//        SCI_enableFIFO(SelSCI);
//        SCI_enableModule(SelSCI);
//        SCI_performSoftwareReset(SelSCI);
//
//        SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX16);
//        SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
//
//    while(RxReadyFlag == 0);


    memcpy(RcvData, receivedChar, size*2);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

/*******************************************************************
* parseMsgSCI - Parsing received data
*
* DataFrame[16]---------------
* ID 1(0)
* Target 2(1)
* Length 3(2)       - MAX:13
* Data 4~16(3 ~ 15)
*
* CmdData[16]-----------------
* ID 1(0)
* Target 2(1)
* Data 3~16(2~15)
*
*******************************************************************/
void parseMsgSCI(uint16_t* DataFrame, uint16_t* CmdData) {
    uint16_t i;

    if(DataFrame[0] > 0) {
        CmdData[0] = 4;
        CmdData[1] = DataFrame[1];
        if(DataFrame[2] <= 13) {
            for (i=0;i<DataFrame[2];i++) {
                CmdData[i+2] = DataFrame[i*2+3] & 0xFF;
                CmdData[i+2] |= (DataFrame[i*2+4] & 0xFF) << 8;
            }
        }
    }
}

/*******************************************************************
* makePacketSCI
*
* packet_data[16]---------------
* ID 1(0)
* Target 2(1)
* Length 3(2)       - MAX:6
* Data 4~16(3 ~ 15)
*
*******************************************************************/
void makePacketSCI(uint16_t* dataFrame, uint16_t* packet_data, uint16_t ID, uint16_t len, uint16_t target) {
    unsigned char temp[16];
    uint16_t i;
    temp[0] = ID;
    temp[1] = target;
    if (len < 6)
        for (i=0;i<len;i++) {
            temp[i*2+2] = dataFrame[i] & 0xFF;
            temp[i*2+3] = (dataFrame[i] & 0xFF) >> 8;
        }
    memcpy(packet_data, temp, sizeof(packet_data));
}


#ifdef __cplusplus
}
#endif
