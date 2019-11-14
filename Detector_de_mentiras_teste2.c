#include <msp430.h> 
#include <legacymsp430.h>

# define LEDRED BIT3 // P1.3
# define LEDGREEN BIT4 // P1.4
# define LEDBLUE BIT0 // P1.0

// AD
#define CANAIS_ADC 2 // vetor com A0 e A1

#define IN_AD BIT1 // P1.1 potenciometro

#define IN_AD1 BIT2 // P1.2 sensor


int resis[CANAIS_ADC];

int sensor, poten ; // estara incluso em resis

int band = 250; //sensibilidade


int main(void)


{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    P1DIR |= LEDRED;
    P1OUT = LEDRED;

    P1DIR |= LEDGREEN;
    P1OUT = LEDGREEN;

    P1DIR |= LEDBLUE;
    P1OUT = LEDBLUE;

    ADC10CTL0 = MSC + SREF_0 + ADC10SHT_0 + ADC10ON + ADC10IE;
    ADC10CTL1 |= INCH_2 + ADC10SSEL_3 + CONSEQ_3;
    ADC10AE0 |= IN_AD + IN_AD1; // bits correspondentes a cada pino da msp
    ADC10DTC1 = CANAIS_ADC;
    ADC10CTL0 |= ENC + ADC10SC;

    _BIS_SR(GIE);

   while (1){

    poten = resis[0]; //MODIFIQUEI
    sensor = resis[1]; //carrega as resistencias para as variaveis correspondentes

    ADC10CTL0 &= ~ENC; //encerra a conversão AD

    while (ADC10CTL1 & BUSY); // garantir que o conversor AD não esteja ocupado para iniciar novas conversões

    ADC10SA = (unsigned int)resis;// endereço do registrador que está guardando as conversões;

    ADC10CTL0 |= ENC+ADC10SC ;  // inicia a conversão AD
    _BIS_SR(GIE);

          }

}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
     {

      if ( sensor > poten + band){

        P1OUT = LEDRED;

        //  ativar buzzer em assembly

      }
      else if (sensor < poten - band) {
          P1OUT = LEDBLUE;
      }
      else {
          P1OUT = LEDGREEN;
      }
      __bic_SR_register(GIE);
     }



