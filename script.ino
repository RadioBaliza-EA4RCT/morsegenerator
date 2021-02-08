#include <arduino-timer.h>
#define DELAY_RAYA 120
#define DELAY_PUNTO  40
#define DELAY_ESPACIO_LETRAS  120
#define DELAY_ESPACIO_SIMBOLOS  40
#define DELAY_ESPACIO_PALABRAS  360

auto timer = timer_create_default();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT
}

bool apagar(void argument / optional argument given to in/at/every /) {
    digitalWrite(LED_BUILTIN, LOW);
    return false; // to repeat the action - false to stop
}

void raya() {
  digitalWrite(LED_BUILTIN, HIGH);
  timer.in(DELAY_RAYA, apagar);
}

void punto () {
  digitalWrite(LED_BUILTIN, HIGH);
  timer.in(DELAY_PUNTO, apagar);
}

void espacio_simbolos() {
  timer.in(DELAY_ESPACIO_SIMBOLOS, [](void) -> bool { return false; });
}

void espacio_letras() {
  timer.in(DELAY_ESPACIO_LETRAS, [](void) -> bool { return false; });
}

void espacio_palabras() {
  timer.in(DELAY_ESPACIO_PALABRAS, [](void) -> bool { return false; });
}

void loop() {
  timer.tick(); // tick the timer
}
