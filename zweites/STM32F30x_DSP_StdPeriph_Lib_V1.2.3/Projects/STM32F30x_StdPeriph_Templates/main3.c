//Folgende Firmware Library Module werden benötigt:

// + stm32f30x_rcc
// + stm32f30x_gpio
// + stm32f30x_usart
// + stm32f30x_misc
// + stm32f30x_exti
// + stm32f30x_syscfg

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_conf.h"

int main(void)
{
    
    // Dieses Mal werden wir die Programmierung in 4 Schritte unterteilen:
    // 1) Konfiguration der LED
    // 2) Konfiguration des Tasters
    // 3) Konfiguration der USART
    // 4) Programmieren der Interrupt Handler
    
    // Zu Beginn die üblichen Initialisierungsstrukturen
    // Dieses mal benötigen wir neben GPIO und NVIC auch USART und EXTI
    USART_InitTypeDef  USART_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
	
	    
    // Wie in Beispiel 2A besprochen, konfigurieren wir gleich zu Beginn auch die Aufteilung von Gruppen- und Subgruppenprioritäten des NVIC
    // Wir teilen die 4 Bit wieder 2-2 auf und können damit sowohl für Gruppe als auch Subgruppe jeweils die Level 0-3 einstellen
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	

    // -------------------------- Schritt 1: LED ----------------------------------
    
    // Hier ist seit dem Beispiel 1C nichts neu, daher ohne Kommentar:
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ----------------------------------------------------------------------------
    
    
    // -------------------------- Schritt 2: Taster -------------------------------
    
    // Jetzt wird es wieder spannender, es kommt etwas Neues: Externe Interrupts, kurz EXTI
    
    // Zu Beginn müssen wir einmal herausfinden, wo und wie der Taster an den STM32 angeschlossen ist
    // Dabei hilft uns das User Manual des Nucleo Boards S. 23 (Kap. 6.5) und der Schaltplan des Nucleo Boards. Das PDF dazu ist über den Link https://www.st.com/resource/en/schematic_pack/nucleo_64pins_sch.zip abrufbar und heißt MB1136.pdf.
    // Auf S. 23 des User Manuals steht, dass der User Button mit Pin C.13 verbunden ist
    // Im Schaltplan (Sheet 2 in der PDF-Datei MB1136) sehen wir, dass der Pin C.13 über einen Pullup-Widerstand dauerhaft gegen Vcc geschalten ist.
    // Wird der Taster betätigt, so wird der Pin C.13 mit Masse verbunden
    // Daraus folgt, dass der Pin dauerhaft auf "high" liegt und nur beim Drücken des Tasters auf "low" wechselt
    // Ein zusätzlicher Pullup/Pulldown Widerstand ist nicht nötig
    
    // Entsprechend der GPIO Beschreibung für externe Interrupts muss der Pin als Eingang konfiguriert werden
    // (PDF, Reference Mannual, Kapitel 9.3.8, S. 144)
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL ;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
    // Der EXTI besitzt für jede Pinnummer (0-15) eine eigene external Interrupt Line (EXTI0-EXTI15)
    // Mit welchem GPIO Port (A-F) die jeweiliege EXTI Line verbunden ist, wird über die Konfiguration des SYSCFG festgelegt
    // (PDF, Reference Manual, Kapitel 10 bzw. Kapitel 12.2.6)
    
    // Da es sich bei dem SYSCFG um eine eigene Peripherie handelt, muss dieser zuerst aktiviert werden (liegt am APB2)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Mittels der Funtion "SYSCFG_EXTILineConfig" können wir nun festlegen, dass EXTI13 mit dem Port C verbunden wird
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    // Die EXTI13 Line ist nun mit Port C verbunden, jetzt können wir die EXTI13 Line konfigurieren
    // Dies geschieht (wie für die Firmware Library üblich) über eine Init-Struktur:
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;               // EXTI Line 13 (die jetzt ja mit Pin C.13 verbunden ist)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;       // wir wollen einen Interrupt erzeugen
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   // auf welche Änderung am Pin soll geachtet werden?
                                                              // Falling Edge tritt auf, wenn Taster gedrückt wird
                                                              // Rising Edge tritt auf, wenn Taster losgelassen wird
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                 // Natürlich aktivieren wir die EXTI Line
   
    // Struktur an EXTI übergeben und damit die Register beschreiben
    EXTI_Init(&EXTI_InitStructure);
  

    // Das war wie üblich bei Interrupts nur die halbe Miete, wir müssen auch noch den zugehörigen Interrupt Channel konfigurieren
    // In diesem Fall ist das "EXTI15_10_IRQn", also ein Channel der die Interrupts von EXTI10 bis EXTI15 zusammenfasst
    // Den Namen finden wir wieder in der Startup Datei "stm32f30x.h"
    // Wir aktivieren den Channel und geben ihm die höchste Priorität
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
    // Struktur übergeben ... fertig, jetzt wird der zugehörige Interrupt Handler aufgerufen, wenn der Taster gedrückt wird :-)
    NVIC_Init(&NVIC_InitStructure);

    // ----------------------------------------------------------------------------

    // -------------------------- Schritt 3: UART ---------------------------------

    // Im einfachen asynchronen Modus ohne Hardware Flow Control benötigen wir für die USART zwei Leitungen: RX und TX (siehe VO)
    // Der STM32F334R8 besitzt drei USART Einheiten (USART1-3), die jeweils mit mehreren Pins verbunden werden können
    // Welche Konfiguration verwenden wir nun? Hier hilft wieder das User Manual des Nucleo Boards (PDF, Kapitel 6.8, S. 25)
    // Der USART Kanal, der über den ST-Link mit dem PC verbunden werden kann, ist von USART2 und hat folgende Pinkonfiguration:
    // Pin A.2 USART2 TX
    // Pin A.3 USART2 RX
    
    // Zu Beginn konfigurieren wir daher die beiden Pins, da wir sowohl Daten empfangen als auch senden wollen
    // Diese müssen wieder als "Alternate Function" konfiguriert werden, um mit der USART verbunden zu werden
    // Wir konfigureren beide Pins genau gleich und können dies in der Init-Struktur auch gleich kombinieren
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3;  // ODER Verknüpfung, um beide Pins auszuwählen
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

    // Struktur an GPIO Port A übergeben und damit beide Pins konfigurieren
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Jetzt müssen wir noch die richtige AF Funktion für beide Pins auswählen, damit sie auch mit der USART2 verbunden werden
    // Im Datasheet (PDF, Kapitel 4, Seite 42) sehen wir, dass Pin A.2 und Pin A.3 als AF7 konfiguriert werden müssen
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

    // Die Pins der USART2 sind nun konfiguriert und mit der USART verbunden
    // Nun muss die USART selbst konfiguriert werden
    // Zu Beginn müssen wir sie wieder aktivieren (liegt am APB1)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // Auch für die USART gibt es wieder eine Initialisierungs-Struktur
    // Gemäß der Angaben auf TUWEL, sollen wir die USART wie folgt konfigurieren:
    USART_InitStructure.USART_BaudRate=9600;                                       // Baudrate = 9600
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;                      // Datenbits = 8 ("Wortlänge")
    USART_InitStructure.USART_StopBits=USART_StopBits_1;                           // Stopbits = 1
    USART_InitStructure.USART_Parity=USART_Parity_No;                              // kein Paritybit
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;  // keine Hardware Flow Control
    USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;                  // Einstellungen gelten für RX und TX
                                                                                   // wir wollen beide Richtungen aktivieren
    
    // Struktur an USART 2 übergeben
	USART_Init(USART2, &USART_InitStructure);
	
    // Ähnlich wie bei dem Timer, muss auch hier die USART aktiviert ("gestartet") werden
	USART_Cmd(USART2, ENABLE);
	
    // Die USART2 ist nun konfiguriert und arbeitet auch schon
    // Jetzt wollen wir noch den Empfang von Daten mit einem Interrupt abfangen
    // So können wir die empfangenen Daten auslesen und gemäß der Aufgabenstellung zurücksenden
    // Die USART bietet eine Vielzahl an Interruptquellen (PDF, Reference Manual, Kapitel 28.7, S. 990)
    // Wir benötigen den "Receive data register not empty" Interrupt (RXNE)...
    // Ein langer Name, aber er sagt nichts anderes als: "Daten sind angekommen und abholbereit"
    // Sobald ein Datenwort vollständig empfangen wurde, wird es in das Receive Data Register (RDR) geschrieben
    // Dadurch wird das RXNE Interrupt Flag gesetzt und der zugehörige Interrupt Handler aufgerufen
    // Lesen wir die Daten aus dem RDR per Software aus, so wird das RXNE Interrupt Flag von der Hardware eigenständig rückgesetzt
    // Erst beim nächsten Datenempfang wird das Interrupt Flag erneut gesetzt und der Interrupt damit wieder ausgelöst
    // Dieses "eigenständige" Rücksetzen des Interurpt Flags wird uns beim Interrupt Handler später noch beschäftigen...
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // Nun fehlt noch die NVIC Konfiguration des Interrupt Channels
    // Der zugehörige Interrupt Channel ist "USART2_IRQn" (siehe Startup Datei "stm32f30x.h")
    // Wir aktivieren den Channel und geben ihm eine niedrigere Priorität als dem EXTI Interrupt (höhere Zahl!)
    NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
		
    // Struktur übergeben ... fertig, jetzt wird der zugehörige Interrupt Handler aufgerufen, wenn Daten empfangen wurden :-)
   	NVIC_Init(&NVIC_InitStructure);

    // ----------------------------------------------------------------------------
    
    // Endlosschleife, damit die CPU aktiv bleibt und auf unsere beiden Interrupts reagieren kann
    while (1);
    
}


// --------------------- Schritt 4: Interrupt Handler -----------------------------


// Die Funktionsnamen der Interrupt Handler finden wir wieder in der "startup_stm32f334x8.s" Datei
// In unserem Fall also "EXTI15_10_IRQHandler" und "USART2_IRQHandler"
// ERINNERUNG: Die Funktionen müssen GENAU DIESEN Namen haben (case-sensitiv)

// Fangen wir mit dem USART2 Interrupt an
void USART2_IRQHandler()
{
    // Zur Erinnerung: Wir haben den RXNE Interrupt aktiviert, der Handler wird also aufgerufen, wenn ein Datenwort empfangen wurde
	
    // Wir werden eine Variable benötigen, in der wir das empfangene Datenwort zwischenspeichern
    int recvd;
    
    // Wir lesen die empfangenen Daten aus dem RDR aus und speichern sie in unserer Variable
    // Genau jetzt setzt die Hardware das RXNE Interrupt Flag eingeständig zurück und die USART ist bereit neue Daten zu empfangen
    // (PDF, Reference Manual, Kapitel 28.5.3, S. 957)
    // Daher ist es auch ratsam, die empfangenden Daten in einer Variable zu speichern und ab dann nur mehr diese zu verwenden
    // Schließlich können ab jetzt neue Daten empfangen werden, die von der Hardware ins RDR Register geschrieben werden
    // Wenn wir zu lange "herumrechnen" und nochmals aus dem RDR lesen, stehen dort also vielleicht schon die neueren Daten
    recvd=USART_ReceiveData(USART2);


    //  Wir prüfen, ob es sich bei dem empfangenen Datenwort um einen Großbuchstaben handelt
    if ((recvd>='A')&&(recvd<='Z')){
		
        // Haben wir einen Großbuchstaben empfangen, wollen wir den selben Buchstaben in klein zurücksenden
        // Bevor wir jetzt einfach Daten in das Senderegister (Transmit Data Register, TDR) schreiben, müssen wir erst prüfen,
        // ob die USART nicht gerade damit beschäftigt ist, andere Daten zu senden (z.B. von unserem Taster)
        // Dazu nützen wir das TXE Flag der USART (PDF, Reference Manual, Kapitel 28.5.2, S. 955)
	// Es wird von der Hardware gesetzt, wenn das TDR leer ist und mit neuen Daten zum Senden gefüllt werden kann
        // Wir prüfen also das TXE Flag und warten solange, bis es gesetzt ist
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
		
        // Jetzt ist die USART sicher bereit ...
        // Wir schreiben also den entsprechenden Kleinbuchstaben in das TDR und die USART versendet die Daten für uns
        // Im ASCII Code ist der Kleinbuchstabe genau 32 Zeichen über dem zugehörigen Großbuchstaben -> "revcd+32"
        USART_SendData(USART2, recvd+32);
    }

    // Jetzt das gleiche für Kleinbuchstaben ...
    if ((recvd>='a')&&(recvd<='z')){
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
        USART_SendData(USART2, recvd-32);
    }
    
    // Hier folgt normalerweise noch das Rücksetzen des Pending Bits (in diesem Fall das RXNE Interrupt Flag)
    // Das ist dieses Mal aber nicht nötig, denn das hat ja bereits die Hardware für uns erledigt!
    // Das Rücksetzen ist genau genommen nicht nur "nicht NÖTIG", es ist gar NICHT MÖGLICH und streng genommen gar NICHT ERLAUBT
    // Schauen wir uns die USART Register etwas genauer an (PDF, Reference Manual, Kapitel 28.8, S. 992-1014), so fällt auf:
    // 1) Im Interrupt and Status Register (ISR) können wir uns den aktuellen Zustand des RXNE Flags zwar anschauen
    // 2) Im Interrupt Flag Clear Register (ICR) können wir das RXNE Flag aber gar nicht rücksetzen, das Bit darf gar nicht beschrieben werden
    // Dieses Flag kann also nur die Hardware rücksetzen
    // Und das macht sie jedes Mal, wenn wir mit unserer Software lesend auf das RDR Register zugreifen
    // USART_ClearITPendingBit(USART2, USART_IT_RXNE) ... brauchen wir also nicht, das macht die Hardware :-)
}


// Und zu guter Letzt noch der Interrupt Handler des Tasters (EXTI)
void EXTI15_10_IRQHandler()
{
    // Zur Erinnerung: Wir haben den Interrupt so konfiguriert, dass er beim Drücken des Tasters auslöst (Falling Edge)
    
    // Zunächst wollen wir die LED toggeln, den Befehl kennen wir schon:
    GPIOA->ODR ^=1<<5;
    
    // Wir wollen den aktuellen Zustand der LED aber auch noch an den PC senden
    // Mit der Funktion "GPIO_ReadOutputDataBit" können wir aus dem ODR Register auslesen, ob unser Pin A.5 gesetzt ist
    // Ist er das, dann senden wir "On" an den PC, ansonsten "Off"
    // Hier ist es wieder wichtig, dass wir nach jedem Datenwort (jedem Buchstaben) warten, bis die USART wieder "sendebereit" ist
    // Warten wir nicht, so können einzelne Datenworte verloren gehen
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
    
    // Aufgepasst, hier müssen wir das Pending Bit wieder selber zurücksetzen
    EXTI_ClearITPendingBit(EXTI_Line13);
}

// ----------------------------------------------------------------------------

