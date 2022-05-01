byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin = 2;

float calibrationFactor = 4.5;

volatile byte pulseCount;  

float Debit;
unsigned int flowMilliLitri;
unsigned long totalMilliLitri;

unsigned long Timp_trecut;

void setup()
{
  Serial.begin(9600);
  
  pinMode(sensorPin, INPUT);
  pinMode(8,OUTPUT); //green
  pinMode(9,OUTPUT); //yellow
  pinMode(10,OUTPUT); //red

  pulseCount        = 0;
  Debit             = 0.0;
  flowMilliLitri    = 0;
  totalMilliLitri   = 0;
  Timp_trecut       = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop()
{
   
   if((millis() - Timp_trecut) > 1000)    
  { 
    detachInterrupt(sensorInterrupt);
    Debit = ((1000.0 / (millis() - Timp_trecut)) * pulseCount) / calibrationFactor;
    Timp_trecut= millis();
    flowMilliLitri = (Debit / 60) * 1000;
    totalMilliLitri += flowMilliLitri;
    
    Serial.print("Debit: ");
    Serial.print(int(Debit));  
    Serial.print(" L/min");
    Serial.print("\t");     
    Serial.print("Total mililitri apa: ");        
    Serial.print(totalMilliLitri);
    Serial.println("mL"); 
    Serial.print("\n"); 
    
    pulseCount = 0;
    
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    if(Debit<10) //situatie buna
    {
      digitalWrite(8,HIGH);
      digitalWrite(9,LOW);
      digitalWrite(10,LOW);
    }
    else if(Debit > 10 && Debit<30)// situatie acceptabila
    {
      digitalWrite(9,HIGH);
      digitalWrite(8,LOW);
      digitalWrite(10,LOW);
    } 
    else if(Debit>30)// situatie critica
    {
      digitalWrite(10,HIGH);
      digitalWrite(8,LOW);
      digitalWrite(9,LOW);
    }
  }
  
}
void pulseCounter()
{
  pulseCount++;
}
