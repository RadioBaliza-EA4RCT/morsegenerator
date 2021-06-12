/*
 * MIT License
 *
 * Copyright (c) 2018-2021 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * \brief Serial Terminal getting started example
 * \details
 *      Source:         https://github.com/Erriez/ErriezSerialTerminal
 *      Documentation:  https://erriez.github.io/ErriezSerialTerminal
 */

#include <ErriezSerialTerminal.h>
#include <arduino-timer.h>

#define ADF4351_CS 3
#define LOCK 2
#define BITS_MOVE 5
#define DOT_TIME 250     // In milliseconds
#define SPACE_BTW_WORDS 1750    // 7*DOT_TIME
#define SPACE_BTW_CHARACTERS 750 // 3*DOT_TIME
#define SPACE_BTW_PHRASES 3000

// LED pin
#define LED_PIN     LED_BUILTIN

// Structures
typedef enum {
  IDLE,
  PUNTO,
  RAYA,
  SEPARADOR_ELEMENTOS
} MORSE_STATES;

// Newline character '\r' or '\n'
char newlineChar = '\n';
// Separator character between commands and arguments
char delimiterChar = ' ';

// Create serial terminal object
SerialTerminal term(newlineChar, delimiterChar);

// Function prototypes
void unknownCommand(const char *command);
void cmdHelp();
void cmdGetText();
void cmdSetText();
void cmdOn();
void cmdOff();
void cmdStatus();
void str_to_upper(char * input, int lenght);
void text_to_morse(uint8_t morse_char);
void ptt_on();
void ptt_on();

String msg = "EA4RCT EIT";
boolean transmitiendo = false;    // Status, true if tx, false if not
boolean text_changed = false;
auto timer = timer_create_default();
MORSE_STATES estado = IDLE;
uint8_t count = 0; 
uint16_t indice_string = 0;
uint8_t mask;
uint8_t j;
int length;
unsigned long time;

uint32_t registers_ON[6] = {0x338088, 0x80080C9, 0x4E42, 0x4B3, 0x9C8024, 0x580005};
uint32_t registers_OFF[6] = {0x338088, 0x80080C9, 0x4E42, 0x4B3, 0x9C8004, 0x580005};

uint8_t morse[] = {         // First 3 bits represents duration, then 0 dot and 1 dash.
    0b01001000, // A (2 .-   )
    0b10010000, // B (4 -... )
    0b10010100, // C (4 .. )
    0b01110000, // D (3 _..  )
    0b00100000, // E (1 .    )
    0b10000100, // F (4 ..-. )
    0b01111000, // G (3 //.  )
    0b10000000, // H (4 .... )
    0b01000000, // I (2 ..   )
    0b10001110, // J (4 .//- )
    0b01110100, // K (3 -.-  )
    0b10001000, // L (4 .-.. )
    0b01011000, // M (2 //   )
    0b01010000, // N (2 -.   )
    0b01111100, // O (3 //-  )
    0b10001100, // P (4 .//. )
    0b10011010, // Q (4 //.- )
    0b01101000, // R (3 .-.  )
    0b01100000, // S (3 ...  )
    0b00110000, // T (1 -    )
    0b01100100, // U (3 ..-  )
    0b10000010, // V (4 ...- )
    0b01101100, // W (3 .--  )
    0b10010010, // X (4 -..- )
    0b10010110, // Y (4 -.-- )
    0b10011000, // Z (4 --.. )
    0b10101111, // 1 (5 .----)
    0b10100111, // 2 (5 ..---)
    0b10100011, // 3 (5 ...--)
    0b10100001, // 4 (5 ....-)
    0b10100000, // 5 (5 .....)
    0b10110000, // 6 (5 -....)
    0b10111000, // 7 (5 --...)
    0b10111100, // 8 (5 ---..)
    0b10111110, // 9 (5 ----.)
    0b10111111  // 0 (5 -----)
};

bool morse_write(uint8_t morse_char) {
  switch(estado){
    case IDLE:
      break;
    case PUNTO:
      //Serial.println("Punto");
      ptt_on();
      estado = SEPARADOR_ELEMENTOS;
      break;
    case RAYA:
      //Serial.println("RAYA");
      ptt_on();
      if (count == 2) {
        estado = SEPARADOR_ELEMENTOS;
      }
      count++;
      break;
    case SEPARADOR_ELEMENTOS:  // Puede ser separador elemento, de caracter o de palabra, dependiendo de donde estemos tiraremos para uno y otro
      //Serial.println("--");
      ptt_off();
      count = 0;
      
      if(j < BITS_MOVE - (length-1)){
        estado = IDLE;
        //Serial.println("Saliendo");
        return false;
      }

      mask = 1 << (j-1) ;
      if(morse[morse_char] & mask ) {
        estado = RAYA;
      }else{
        estado = PUNTO;
      }

      j--;
      
      break;
  }
  return true; // keep timer active? true
}

void setup(){
    // Startup delay to initialize serial port
    delay(500);

    // Initialize serial port
    Serial.begin(115200);
    Serial.println(F("\nEA4RCT E.I.T Baliza Software Serial"));
    Serial.println(F("Type 'help' to display usage."));

    // Initialize the built-in LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Set default handler for unknown commands
    term.setDefaultHandler(unknownCommand);

    // Add command callback handlers
    term.addCommand("?", cmdHelp);
    term.addCommand("help", cmdHelp);
    term.addCommand("get-text", cmdGetText);    // Get the current text
    term.addCommand("set-text", cmdSetText);    // Set a new text
    term.addCommand("on", cmdOn);            // Start trasmitting
    term.addCommand("off", cmdOff);          // Stop transmitting
    term.addCommand("status", cmdStatus);       // Get Status
}

void loop(){
    // put your main code here, to run repeatedly:
    int i = 0;
    text_changed = false;

    time = millis();
  
    while(millis() < time + SPACE_BTW_PHRASES){
      timer.tick();
      term.readSerial();
    }
    
    for(; i < msg.length() && !text_changed; i++){
      Serial.println(msg[i]);
      switch ((int) msg[i]) {
        case 32:    // Space
          Serial.println("ESPACIO \n");

          time = millis();
  
          while(millis() < time + SPACE_BTW_WORDS){
            timer.tick();
            term.readSerial();
          }
          
          break;
        case 0:
          break;
        default:
          j = BITS_MOVE - 1;
          uint8_t morse_char = (int) msg[i] - 'A';
          length = morse[morse_char] >> BITS_MOVE;
          if(morse[morse_char] & 16 ) {   // Mask to the first bit
            //RAYA
            estado = RAYA;
          }else{
            //PUNTO
            estado = PUNTO;
          }
          
          timer.every(DOT_TIME, morse_write, morse_char);
          while(estado != IDLE){
            timer.tick();
            term.readSerial();
          }
  
          time = millis();
  
          while(millis() < time + SPACE_BTW_CHARACTERS){
            timer.tick();
            term.readSerial();
          }
          //delay(SPACE_BTW_CHARACTERS);  // Separación entre caracteres
          
      }
    }
}

void unknownCommand(const char *command)
{
    // Print unknown command
    Serial.print(F("Unknown command: "));
    Serial.println(command);
    Serial.println(F("Type help or ? for command usage"));
}

void cmdHelp()
{
    // Print usage
    Serial.println(F("Serial terminal usage:"));
    Serial.println(F("  help or ?          Print this usage"));
    Serial.println(F("  get-text           Get the current msg"));
    Serial.println(F("  set-text <string>  Set a new msg"));
    Serial.println(F("  on                 Set on status"));
    Serial.println(F("  off                Set off status"));
    Serial.println(F("  status             Get current status"));
}

void cmdGetText(){
  // Print current text
  Serial.println(msg);
}

void cmdSetText(){
  char* input = term.getRemaining();
  str_to_upper(input, strlen(input));
  msg = input;

  text_changed = true;
}

void cmdOn(){
  transmitiendo = true;
  
}

void cmdOff(){
  transmitiendo = false;
}

void cmdStatus(){
  Serial.println(transmitiendo);
}

void str_to_upper(char * input, int lenght){
  char * original = input;

  while(lenght){
    *input = toupper((unsigned char) *input);
    lenght--;
    input++;
  }

  input = original;
}

void ptt_on(){
  digitalWrite(LED_BUILTIN, true); // toggle the LED
}
void ptt_off(){
  digitalWrite(LED_BUILTIN, false); // toggle the LED
}
