#ifndef HELP_H
#define HELP_H

#include "globals.h"

struct HelpInfo {
    const char* title;
    const char* connection;
    const char* probes;
    const char* tips;
    const char* polarity;
};

// Interface do Sistema de Ajuda
const HelpInfo* help_get_for_state(AppState state);

#endif
