#include"SparkFun_MiniGen.h"
#include"Adafruit_MCP3008.h"
#include<SPI.h>
#include<LiquidCrystal.h>

MiniGen sig_gen;
Adafruit_MCP3008 adc;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const float MAX_FREQ = 140000;
const float MIN_FREQ = 110000;
const float ADC_SAMPLES = 10;
uint32_t currentFreq = MIN_FREQ;
uint32_t peakFreq = 0;
unsigned long startTime = 0;
uint16_t adcMax = 0;

void setup() {
  Serial.begin(230400);
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(8, HIGH);
  pinMode(10, HIGH);

  lcd.begin(20, 4);
  lcd.print("COIL TUNER");

  sig_gen.reset();
  sig_gen.setMode(MiniGen::SINE);
  sig_gen.selectFreqReg(MiniGen::FREQ0);
  uint32_t freqRegVal = calculateFrequency(currentFreq);
  sig_gen.adjustFreq(MiniGen::FREQ0, MiniGen::FULL, freqRegVal);

  adc.begin(8);
  startTime = millis();
}

void loop() {
  increaseFreq();
  uint16_t adcCurrent = sampleAdc(ADC_SAMPLES);
  if (adcCurrent > adcMax){
    adcMax = adcCurrent;
    peakFreq = currentFreq;
  }
  if(currentFreq == MAX_FREQ){
    currentFreq = MIN_FREQ;
    if(peakFreq > 110100){
      printSerialOutput();
      lcdOutput();
    }
    else{
      lcdClearOutput();
    }
    
    startTime = millis();
    peakFreq = 0;
    adcMax = 0;
  }
}

void increaseFreq(){
  currentFreq += 10;
  uint32_t freqRegVal = calculateFrequency(currentFreq);
  SPI.setDataMode(SPI_MODE2);  // Clock idle high, data capture on falling edge
  sig_gen.adjustFreq(MiniGen::FREQ0, MiniGen::FULL, freqRegVal);
}

void printSerialOutput(){
  Serial.print(peakFreq);
  Serial.print(" ");
  Serial.print(adcMax);
  Serial.print(" ");
  Serial.println((int)(millis() - startTime));
}

void lcdClearOutput(){
  lcd.setCursor(0, 2);
  lcd.print("                    ");
}

void lcdOutput(){
  lcd.setCursor(0, 2);
  lcd.print((peakFreq / 1000.0), 1);
  lcd.print("kHz");
}

// Function from SparkFun_MiniGen - taken to change param and return types
// Used to calculate the integer value to be written to a freq register for
// a desired output frequency.
// The output frequency is fclk/2^28 * FREQREG. For us, fclk is 16MHz. We can
// save processor time by specifying a constant for fclk/2^28- .0596. That is,
// in Hz, the smallest step size for adjusting the output frequency.
uint32_t calculateFrequency(uint32_t desiredFrequency){
  return (uint32_t) (desiredFrequency/.0596);
}

//Sample the ADC 'samples' times
uint16_t sampleAdc(uint8_t samples){
  uint16_t adcCurrent = 0;
  for(int i = 0; i < samples; i++){
    adcCurrent += adc.readADC(0);
  }
  return adcCurrent /= 10;
}