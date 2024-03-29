#define MAX_BITS 100            // max number of bits
#define WEIGAND_WAIT_TIME 3000  // time to wait for another weigand pulse

unsigned char databits[MAX_BITS];// stores all of the data bits
unsigned char bitCount;          // number of bits currently captured
unsigned char flagDone;          // goes low when data is currently being captured
unsigned int weigand_counter;    // countdown until we assume there are no more bits

unsigned long facilityCode = 0;  // decode facility code
unsigned long cardCode = 0;      // decode card code


// interrupt when INT0 goes low(0 bit)
void ISR_INT0()
{
  //Serial.print("0");  // you can uncomment this line if you want to print binary
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;
}


// interrupt when INT1 goes low (1 bit)
void ISR_INT1()
{
    //Serial.print("1"); // you can uncomment this line if you want to print binary
    databits[bitCount] = 1;
    bitCount++;
    flagDone = 0;
    weigand_counter = WEIGAND_WAIT_TIME;  
}

void setup()
{
  pinMode(13,OUTPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);

  Serial.begin(9600);
  Serial.println("HID Reader for 35 bit HID Corporate");

  
  // binds the ISR functions to the falling edge of INT0 and INT1 
  attachInterrupt(0, ISR_INT0, FALLING);
  attachInterrupt(1, ISR_INT1, FALLING);

  weigand_counter = WEIGAND_WAIT_TIME;
}

void loop()
{
  if(!flagDone){
    if(--weigand_counter == 0)
    flagDone = 1;
  }

  if(bitCount>0 && flagDone){
    unsigned char i;

    Serial.print("Read ");
    Serial.print(bitCount);
    Serial.println(" bits.");

        if (bitCount == 35)
    {
      // 35 bit HID corporate 1000 format
      // facility code = bits 2 to 14
      for (i=2; i<14; i++)
      {
        facilityCode <<=1;
        facilityCode |= databits[i];
      }

      // card code = bits 15 to 34
      for (i=14; i<34; i++)
      {
        cardCode <<=1;
        cardCode |= databits[i];
      }
      printBits();
    }

    // cleanup and get ready for next card
    bitCount = 0;
    facilityCode = 0;
    cardCode = 0;
    for (i=0; i<MAX_BITS; i++)
    {
      databits[i]=0;
    }
  }
}

void printBits()
{
  Serial.print("Facility Code : ");
  Serial.println(facilityCode);
  Serial.print("Card Code : ");
  Serial.println(cardCode);
}
