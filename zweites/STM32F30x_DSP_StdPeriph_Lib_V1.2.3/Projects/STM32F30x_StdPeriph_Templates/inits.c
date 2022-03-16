#include "stm32f30x_conf.h"
#include "inits.h"


void startExti(void) 
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL ;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);

 	EXTI_InitStructure.EXTI_Line = EXTI_Line13;               
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;       
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   
                                                              
                                                              
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                 
   
   EXTI_Init(&EXTI_InitStructure);
  
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
    NVIC_Init(&NVIC_InitStructure);
}

void startLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void startUARTWithIRQ(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

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
}
// enables the usart interrupt channel and configures the Nvic accordingly
void startSPIWithIRQ(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);

	SPI_InitTypeDef SPI_InitStructure;

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
	SPI_Cmd(SPI1, ENABLE);

	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	// Interrupt configuruieren

	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * enables Timer 2 but does not start it
 * feuert alle 2 Sekunden
 */
void enableTimer2Sec2(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7999; // 8 MHz Takt dividiert durch 8000 -> 1kHz Zählfrequenz
	TIM_TimeBaseInitStructure.TIM_Period = 1999;	// Von 0 bis 1999 zählen -> 2 Sekunden
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}
/*
 * verwendet GPIOA Pin 0 weil da der eingang vom potentiometer hängt
 * Timer 2 muss vorher aktiviert werden
 */
void startADC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	// hier nicht push pull setzen, da es ja analog und nicht digital ist
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PLL für den ADC ausschalten
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_OFF);

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	// defaultwerte
	ADC_CommonStructInit(&ADC_CommonInitStructure);

	// verwenden von dem AHB Bus Takt
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_SynClkModeDiv1;

	// initialisieren der commonstructure
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

	// defaultwerte
	ADC_StructInit(&ADC_InitStructure);

	// ADC soll durchgehend messen und nicht nur einmal
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;

	// nachdem wir immer den aktuellen Wert im Register haben wollen
	//überschreiben wir ihn einfach
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Enable;

	// initialisieren der normalen structure
	ADC_Init(ADC1, &ADC_InitStructure);

	// 1) Welchen ADC wollen wir konfigurieren   --> ADC1
	// 2) Welchen Channel wollen wir in die "Channel Sequence" Liste einfügen  --> ADC_Channel_16
	// 3) An welcher Stelle in der "Channel Sequence" Liste soll der Channel stehen --> 1
	// 4) Welche Sample Time soll für den Channel eingestellt werden --> 19.5 Cycles weils nicht so genau sein muss
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_19Cycles5);

	// anzahl der channels in der channel sequence liste
	ADC_RegularChannelSequencerLengthConfig(ADC1, 1);

	// voltage regulator starten
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);

	// An dieser Stelle müssen wir laut Reference Manual aber mindestens 10us warten,  bevor wir den ADC kalibrieren oder starten können
	// Da es hier leider kein Flag gibt, auf das wir warten könnten, müssen wir wohl oder übel einen Timer heranziehen

	// Er ist so konfiguriert, dass der Update Interrupt aktivert ist aber der zugehörige Interrupt Channel noch nicht
	// Das Update Interrupt Flag wird also gesetzt, aber kein Interrupt Channel aufgerufen
	// Das nützen wir jetzt für uns:
	// Wir starten den Timer und setzen das Update Interrupt Flag zurück (es wird beim Start nämlich "sofort" einmal gesetzt)
	TIM_Cmd(TIM2, ENABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	// Jetzt warten wir, bis das Flag erneut gesetzt wird
	// Dann sind 2 Sekunden vorbei, mehr als genug Zeit für den ADC Voltage Regulator :-)
	// Wir stoppen den Timer und setzen sicherheitshalber das Update Interrupt Flag und das Interrupt Channel Flag zurück
	// (Sonst wird der Interrupt sofort aufgerufen, wenn wir dann später den Interrupt Channel aktivieren)

	// warten bis das Flag gesetzt wird, dann wissen wir, dass 2 Sekunden vorbei sind
	// noch das Interruptflag zurücksetzten (safe programming) und den Timer ausschalten
	while (TIM_GetFlagStatus(TIM2, TIM_IT_Update) == 0)
	{
	};
	TIM_Cmd(TIM2, DISABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	NVIC_ClearPendingIRQ(TIM2_IRQn);

	// einfache Kalibrierung für den adc da wir nur einen einzigen verwenden
	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);

	// warten bis die Kalibrierung fertig ist
	while (ADC_GetCalibrationStatus(ADC1) != RESET)
		;

	// hoffen dass wir 4 Taktzyklen verbraten, da wir 4 Takte warten müssen
	// bis das ADCAL Flag zurückgesetzt ist
	uint32_t calibration_value = ADC_GetCalibrationValue(ADC1);

	// starten des ADC
	ADC_Cmd(ADC1, ENABLE);

	// warten bis der Startvorgang abgeschlossen ist
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY))
		;

	// Messvorgang starten
	ADC_StartConversion(ADC1);
}
