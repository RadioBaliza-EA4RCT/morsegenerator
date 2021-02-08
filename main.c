#include <stdio.h>
#include <ctype.h>

// Prototypes
void str_to_upper(char * input, int lenght);
void text_to_morse(__uint8_t morse_char);

#define BITS_MOVE 5

char input[] = "Hola Caracola";

__uint8_t morse[] = {
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


int main() {

  // Put string in uppercase
  str_to_upper(input, sizeof(input));

  int i = 0;
  for(; i < sizeof(input); i++){

    switch ((int) input[i]) {
      case 32:    // Space
        printf("ESPACIO \n");
        break;
      case 0:
        break;
      default:
        printf("%i\n", (int) input[i] - 'A');
        text_to_morse((int) input[i] - 'A');

    }
  }

  printf("%s", input);
  return 0;
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

void text_to_morse(__uint8_t morse_char){
  int lenght = morse[morse_char] >> BITS_MOVE;
  __uint8_t j;
  for (j = BITS_MOVE; j >= BITS_MOVE - (lenght-1); j--){
    __uint8_t mask = 1 << (j-1) ;
    if(morse[morse_char] & mask ) {
      //RAYA
      printf("-");
    }else{
      //PUNTO
      printf(".");

    }
    //SEPARACION
    printf(" ");

  }
}
