//If you are viewing this and you don't know romanian, the comments are in that language, sorry:((
#define DECIMAL 10

unsigned char litera;
unsigned int aux;
float temp;
volatile int numar_can;

void adc_init() //functia de initializare CAN 
{
ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); //set division factor between system clock frequency and the input clock to the ADC-128
ADMUX |= (1<<REFS0); //Referinta 5v 
ADCSRA |= (1<<ADEN); //enable ADC
ADCSRA |=(1<<ADIE); //enable interrupts
}

void init_serial()
{
  UCSR0B=(1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0); //transmitere + receptie + tratarea intreruperilor pentru receptie activate 
  UCSR0C=(1<<UCSZ01)|(1<<UCSZ00); //un byte de date
  UBRR0=103; //9600 biti (freqMC/16*BAUD) - 1
}

void init_port()
{
  //initializare porturi
  DDRD=0xF8;
  DDRB=0x3F;
  DDRC=0X00;
}

void init_timer0()
{
  //Timer 0 - 7 segmente catod comun
  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0A |= (1 << WGM01); //CTC
  TCCR0B |= 1 << CS02 | 1 << CS00; //prescaler 1024 timer 0
  TIMSK0 |= 1 << OCIE0A; //intreruperi regim CTC
  OCR0A = 124;  //numar 125 de fronturi crescatoare pt a avea 125 de intreruperi pe sec
}

void init_timer1()
{
  //Timer 1 - Led 13 si senzor 
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 1 << CS12 | 1 << CS10; //prescaler 1024 timer 1
  TIMSK1 |= 1 << TOIE1; //intreruperi regim normal
  TCNT1 = 49910;
}

void init_timer2()
{
  //Timer 2 - PWM led verde folosit pt a utiliza pinul 11 pwm
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= 1 << COM2A1;    // non - inverting
  TCCR2A |= (1 << WGM20) | (1 << WGM21); //FAST PWM
  TCCR2B |= (1 << CS20); //no prescaler (PWM freq = 62,5khz)
}

int main()
{ 
  init_port();
  init_timer0();
  init_timer1();
  init_timer2();
  init_serial();
  adc_init();
  
  sei(); 


  while(1)
 {
    for(int i=0; i<256; i++)
    {
         OCR2A = i;
         _delay_ms(3.90625); //o secunda impartita la nr de valori
    }
    _delay_ms(1000);
    for(int i=255; i>=0; i--)
    {
         OCR2A = i;
         _delay_ms(3.90625);
    }
    _delay_ms(1000);  
 }
}
  
// Timer 1 se ocupa cu led-ul 13 si de citirea senzorului analogic

ISR(TIMER1_OVF_vect)
{
  PORTB ^= 1 << 5;
  ADCSRA |= (1<<ADSC); //conversie la fiecare secunda
  TCNT1 = 49910;
}

// 7 segmente config:
// D: 0b FGEDC(DP)00
// B: 0b 000000AB

void reset()
{
  PORTD = 0;
  PORTB &=~(0x03); //setez pe 0 doar pinii conectati la afisajul 7 segmente
}

void litera_R()
{
  reset();
  //R - mare 
  PORTD |= 0xE8;
  PORTB |= 0x03;
  //r - mic
  //PORTD |= 0xA0;
  //PORTB |= 0x02;
}

void litera_I()
{
  reset();
  PORTD |=0xA0;
}

// Timer 0 se ocupa cu afisajul 7 segmente si cu comunicarea seriala ISR este apelat de 125 de ori pe sec

ISR(TIMER0_COMPA_vect)
{
  aux++; // retine numarul de intreruperi
  
  if(aux==125) //un interval de 1 sec la 125 de intreruperi
    litera_R();
    
  if(aux==250) //interval de 2 sec
  {
    litera_I();
    aux = 0;
  } 
}

ISR(USART_RX_vect) //tratarea intreruperii pentru receptie
{
  litera=(char)UDR0;
  if(litera=='A' || litera=='a')
      PORTB |= 1 << 4;           //led rosu pin 12
  if(litera=='S' || litera=='s')
      PORTB &= ~(1 << 4);
}

void trimite(unsigned char var)
{
  while(!(UCSR0A&(1<<UDRE0))); //asteapta finalizarea transmiterii
  UDR0=var;
}

ISR(ADC_vect)
{
  char vect_parte_intreaga[10];
  char vect_parte_fractionara[10];
  
  numar_can = ADC;
  temp = (numar_can * float(5.0/1023)) * 100;    //formula de determinare a temperaturii
  
   if(temp <= 34.5) //histereza de 0.5 pe 35 grade Celsius
   { 
    PORTB |= 0x04; //led galben pin 10
   }
   else if(temp >= 35.5)
   {
     PORTB &= ~(0x04);
   }

  //aflu partea intreaga / partea fractionara a conversiei
  int parte_intreaga = (int)(temp * 100) / 100;
  int parte_fractionara = (int)(abs(temp) * 100) % 100;

  //functie de conversie din int in array
  itoa(parte_intreaga, vect_parte_intreaga, DECIMAL);
  itoa(parte_fractionara, vect_parte_fractionara, DECIMAL);

  //concatenez totul in sirul de caractere vector parte intreaga
  strcat(vect_parte_intreaga, ".");
  strcat(vect_parte_intreaga, vect_parte_fractionara);
  strcat(vect_parte_intreaga, "\n");

  //afisez fiecare octet prin functia de afisare
  for(int i = 0; i < strlen(vect_parte_intreaga); i++)
      trimite(vect_parte_intreaga[i]);
}
