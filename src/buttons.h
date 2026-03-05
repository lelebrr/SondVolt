#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"
#include <Arduino.h>
#include <Bounce2.h>

// Declaração dos objetos Bounce para cada botão
extern Bounce btnUp;
extern Bounce btnDown;
extern Bounce btnLeft;
extern Bounce btnRight;
extern Bounce btnOk;
extern Bounce btnBack;

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
