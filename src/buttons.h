#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <Bounce2.h>
#include "config.h"

// Declaração dos objetos Bounce para cada botão
extern Bounce2 btnUp;
extern Bounce2 btnDown;
extern Bounce2 btnLeft;
extern Bounce2 btnRight;
extern Bounce2 btnOk;
extern Bounce2 btnBack;

// Funções para manipulação dos botões
void buttons_init();
void buttons_update();
bool isUpPressed();
bool isDownPressed();
bool isLeftPressed();
bool isRightPressed();
bool isOkPressed();
bool isBackPressed();

#endif // BUTTONS_H
