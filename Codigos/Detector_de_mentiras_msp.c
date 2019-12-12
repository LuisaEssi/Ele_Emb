#include <msp430.h> 
#include <legacymsp430.h>

# define LEDRED BIT3 // P1.3
# define LEDGREEN BIT4 // P1.4
# define LEDBLUE BIT0 // P1.0
# define N 20
//LCD
#define BTN BIT3
#define LCD_OUT P2OUT
#define LCD_DIR P2DIR
#define D4 BIT0
#define D5 BIT1
#define D6 BIT2
#define D7 BIT3
#define RS BIT4
#define E  BIT5
#define DADOS 1
#define COMANDO 0
#define CMND_DLY 1000
#define DATA_DLY 1000
#define BIG_DLY  20000
#define CLR_DISPLAY  Send_Byte(1, COMANDO, BIG_DLY)
#define POS0_DISPLAY Send_Byte(2, COMANDO, BIG_DLY)

// AD
#define CANAIS_ADC 2 // vetor com A0 e A1
#define IN_AD BIT1 // P1.1 potenciometro
#define IN_AD1 BIT2 // P1.2 sensor


int resis[CANAIS_ADC];

int sensor, poten ; // estara incluso em resis

int band = 230; //sensibilidade

int i,media = 0;

void Atraso_us(volatile unsigned int us);
void Send_Nibble(volatile unsigned char nibble, volatile unsigned char dados, volatile unsigned int microsegs);
void Send_Byte(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs);
void Send_Data(volatile unsigned char byte);
void Send_String(char str[]);
void Send_Int(int n);
void InitLCD(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= LEDRED;
    P1OUT = LEDRED;

    P1DIR |= LEDGREEN;
    P1OUT = LEDGREEN;

    P1DIR |= LEDBLUE;
    P1OUT = LEDBLUE;

    ADC10CTL0 = MSC + SREF_0 + ADC10SHT_0 + ADC10ON + ADC10IE;
    ADC10CTL1 |= INCH_2 + ADC10SSEL_3 + CONSEQ_3; //multiplas conversões
    ADC10AE0 |= IN_AD + IN_AD1; // bits correspondentes a cada pino da msp
    ADC10DTC1 = CANAIS_ADC;
    ADC10CTL0 |= ENC + ADC10SC;
    InitLCD();
    _BIS_SR(GIE);

   while (1){


    poten = resis[0];
    for (i = 0; i < N-1 ; i++) {

    media+=resis[1]/N;  // faz uma media dos valores obtidos
    }
    sensor = media;
    media = 0;

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

    CLR_DISPLAY;
    POS0_DISPLAY;

      if ( sensor > poten + band){
        Send_String("    Mentira");
        P1OUT = LEDRED;
        //  ativar buzzer em assembly
        buzzer();

      }
      else if (sensor < poten - band) {
        Send_String("    Ajustar");
        P1OUT = LEDBLUE;

      }
      else {

        Send_String("    Verdade");
        P1OUT = LEDGREEN;

      }
      __bic_SR_register(GIE);
     }

void Atraso_us(volatile unsigned int us)
{
    TA1CCR0 = us-1;
    TA1CTL = TASSEL_2 + ID_0 + MC_1 + TAIE;
    while((TA1CTL & TAIFG)==0);
    TA1CTL = TACLR;
    TA1CTL = 0;
}

void Send_Nibble(volatile unsigned char nibble, volatile unsigned char dados, volatile unsigned int microsegs)
{
    LCD_OUT |= E;
    LCD_OUT &= ~(RS + D4 + D5 + D6 + D7);
    LCD_OUT |= RS*(dados==DADOS) +
        D4*((nibble & BIT0)>0) +
        D5*((nibble & BIT1)>0) +
        D6*((nibble & BIT2)>0) +
        D7*((nibble & BIT3)>0);
    LCD_OUT &= ~E;
    Atraso_us(microsegs);
}

void Send_Byte(volatile unsigned char byte, volatile unsigned char dados, volatile unsigned int microsegs)
{
    Send_Nibble(byte >> 4, dados, microsegs/2);
    Send_Nibble(byte & 0xF, dados, microsegs/2);
}

void Send_Data(volatile unsigned char byte)
{
    Send_Byte(byte, DADOS, DATA_DLY);
}

void Send_String(char str[])
{
    while((*str)!='\0')
    {
        Send_Data(*(str++));
    }
}

void Send_Int(int n)
{
    int casa, dig;
    if(n==0)
    {
        Send_Data('0');
        return;
    }
    if(n<0)
    {
        Send_Data('-');
        n = -n;
    }
    for(casa = 10000; casa>n; casa /= 10);
    while(casa>0)
    {
        dig = (n/casa);
        Send_Data(dig+'0');
        n -= dig*casa;
        casa /= 10;
    }
}

void InitLCD(void)
{
    unsigned char CMNDS[] = {0x20, 0x14, 0xC, 0x6};
    unsigned int i;
    // Atraso de 10ms para o LCD fazer o boot
    Atraso_us(10000);
    LCD_DIR |= D4+D5+D6+D7+RS+E;
    Send_Nibble(0x2, COMANDO, CMND_DLY);
    for(i=0; i<4; i++)
        Send_Byte(CMNDS[i], COMANDO, CMND_DLY);
    CLR_DISPLAY;
    POS0_DISPLAY;
}
