#include "buttons.h"

// Definição dos objetos Bounce para cada botão
Bounce2 btnUp = Bounce2();
Bounce2 btnDown = Bounce2();
Bounce2 btnLeft = Bounce2();
Bounce2 btnRight = Bounce2();
Bounce2 btnOk = Bounce2();
Bounce2 btnBack = Bounce2();

// Inicializa os botões
void buttons_init() {
  btnUp.attach(BTN_UP_PIN, INPUT_PULLUP);
  btnUp.interval(50); // Debounce de 50ms
  btnDown.attach(BTN_DOWN_PIN, INPUT_PULLUP);
  btnDown.interval(50);
  btnLeft.attach(BTN_LEFT_PIN, INPUT_PULLUP);
  btnLeft.interval(50);
  btnRight.attach(BTN_RIGHT_PIN, INPUT_PULLUP);
  btnRight.interval(50);
  btnOk.attach(BTN_OK_PIN, INPUT_PULLUP);
  btnOk.interval(50);
  btnBack.attach(BTN_BACK_PIN, INPUT_PULLUP);
  btnBack.interval(50);
}

// Funções avançadas de debounce com tempo de repetição
bool isUpPressed() {
  static unsigned long lastPressTime = 0;
  if (btnUp.fell()) {
    if (currentMillis - lastPressTime > 200) { // Debounce adicional
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

bool isDownPressed() {
  static unsigned long lastPressTime = 0;
  if (btnDown.fell()) {
    if (currentMillis - lastPressTime > 200) {
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

bool isLeftPressed() {
  static unsigned long lastPressTime = 0;
  if (btnLeft.fell()) {
    if (currentMillis - lastPressTime > 200) {
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

bool isRightPressed() {
  static unsigned long lastPressTime = 0;
  if (btnRight.fell()) {
    if (currentMillis - lastPressTime > 200) {
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

bool isOkPressed() {
  static unsigned long lastPressTime = 0;
  if (btnOk.fell()) {
    if (currentMillis - lastPressTime > 200) {
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

bool isBackPressed() {
  static unsigned long lastPressTime = 0;
  if (btnBack.fell()) {
    if (currentMillis - lastPressTime > 200) {
      lastPressTime = currentMillis;
      return true;
    }
  }
  return false;
}

// Atualiza o estado de todos os botões
void buttons_update() {
  btnUp.update();
  btnDown.update();
  btnLeft.update();
  btnRight.update();
  btnOk.update();
  btnBack.update();
}

// Verifica se o botão Cima foi pressionado
bool isUpPressed() { return btnUp.fell(); }

// Verifica se o botão Baixo foi pressionado
bool isDownPressed() { return btnDown.fell(); }

// Verifica se o botão Esquerda foi pressionado
bool isLeftPressed() { return btnLeft.fell(); }

// Verifica se o botão Direita foi pressionado
bool isRightPressed() { return btnRight.fell(); }

// Verifica se o botão OK foi pressionado
bool isOkPressed() { return btnOk.fell(); }

// Verifica se o botão Voltar foi pressionado
bool isBackPressed() { return btnBack.fell(); }
