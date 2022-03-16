#include "stm32f30x_conf.h"



int recv_cnt = 0;

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // Enable pins 
    // pa5 - clk
    // pa6 - miso
    // pa7 - mosi

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);

USART_InitTypeDef USART_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // ODER Verkn�pfung, um beide Pins auszuw�hlen
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

	USART_InitStructure.USART_BaudRate = 9600;					// Baudrate = 9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			// Datenbits = 8 ("Wortl�nge")
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// Stopbits = 1
	USART_InitStructure.USART_Parity = USART_Parity_No;				// kein Paritybit
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // keine Hardware Flow Control
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;			// Einstellungen gelten f�r RX und TX
											// wir wollen beide Richtungen aktivieren

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStructure);
	SPI_InitTypeDef  SPI_InitStructure;

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    // Interrupt configuruieren

    int addr = 2;
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
    recv_cnt = 0;
    SPI_SendData8(SPI1, (addr << 2 | 00));
    SPI_SendData8(SPI1, (addr << 2 | 11));
    SPI_SendData8(SPI1, (addr << 2 | 11));
    SPI_SendData8(SPI1, (addr << 2 | 11));

    while(recv_cnt != 4);
    // done;

    //SPI_I2S_IT_RXNE
    //SPI_ReceiveData8(SPI1);

    //xxxxxxbb xxxxxxbb xxxxxxbb xxxxxxbb
    //xxxxxx01 xxxxxx01 xxxxxx01 xxxxxx01
    while(1);
    return 0;

}



void SPI1_IRQHandler(){
   // uart_send_string("r");
    uint8_t r = SPI_ReceiveData8(SPI1);
  //  if(r & (0b10))
    //    push(checkcollisionlist, r>>6, 3-receivecounter)    uart_send(r);
    recv_cnt ++;
    if(recv_cnt == 4){
        // strobe = 1;
        // timer
        // start strobe counter        uart_send(0xFF);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET)
    {
    };
    USART_SendData(USART2, r);
}


//push
//pop