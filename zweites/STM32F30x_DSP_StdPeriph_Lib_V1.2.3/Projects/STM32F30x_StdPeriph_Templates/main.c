// Folgende Firmware Library Module werden ben�tigt:

// + stm32f30x_rcc
// + stm32f30x_gpio
// + stm32f30x_usart
// + stm32f30x_misc
// + stm32f30x_exti
// + stm32f30x_syscfg

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_conf.h"
#include "inits.h"

int main(void)
{
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    startUARTWithIRQ();
    startSPIWithIRQ();
    // adc aktivieren und dafür brauchen wir Timer 2 also
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE );
    //enableTimer2Sec2();
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
    //startADC();
    //noch den Timer stoppen
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);

    while (1)
        ;
}

void USART2_IRQHandler()
{
    int recvd;

    // Empfangen der Daten
    // setzt auch gleich das RXNE (Interrupt) Flag zurück
    recvd = USART_ReceiveData(USART2);
    SPI_SendData8(SPI1,recvd);

}
void SPI1_IRQHandler()
{
    int r = SPI_ReceiveData8(SPI1);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET)
    {
    };
    USART_SendData(USART2, r);
}

/*void EXTI15_10_IRQHandler()
{

    //Led toggeln
    GPIOA->ODR ^=1<<5;

    //Lesen des ODR
    //Im Folgenden unbedingt immer auf das TXE Flag warten => Datenverlust
    if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5))
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2,'O');
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2,'n');
    }
    else
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2,'O');
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2,'f');
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2,'f');
    }

    //Zurücksetzen des Pending Bits
    EXTI_ClearITPendingBit(EXTI_Line13);
}*/

// ----------------------------------------------------------------------------
