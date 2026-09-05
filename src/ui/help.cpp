#include "help.h"
#include "multimeter.h"

static const HelpInfo help_database[] = {
    // 0: Resistor
    {
        "TESTE DE RESISTOR",
        "Conecte o resistor entre quaisquer dois terminais (1, 2 ou 3).",
        "Ponta 1 e Ponta 2 (ou 3). Nao importa a ordem.",
        "Nao segure os terminais com os dedos, pois a resistencia do corpo pode afetar a leitura.",
        "Nao possui polaridade."
    },
    // 1: Capacitor
    {
        "TESTE DE CAPACITOR",
        "Conecte o capacitor entre os terminais 1 e 3.",
        "Vermelho (1) no Positivo, Preto (3) no Negativo (para eletroliticos).",
        "DESCARREGUE o capacitor antes de medir para evitar danos ao equipamento!",
        "Observe a faixa no corpo do capacitor eletrolitico para o lado negativo."
    },
    // 2: Diodo
    {
        "TESTE DE DIODO",
        "Conecte o diodo entre dois terminais.",
        "Vermelho (Anodo) no 1, Preto (Catodo) no 2.",
        "O sistema detecta automaticamente a queda de tensao direta (Uf).",
        "A faixa no corpo indica o Catodo (Negativo)."
    },
    // 3: Transistor
    {
        "ANALISADOR TRANSISTOR",
        "Insira o transistor nos terminais 1, 2 e 3.",
        "O sistema identificara automaticamente os pinos (E, B, C ou G, D, S).",
        "Suporta NPN, PNP, MOSFET (N e P), JFET e IGBT.",
        "Verifique o datasheet se o componente nao for detectado."
    },
    // 4: Indutor
    {
        "TESTE DE INDUTOR",
        "Conecte o indutor entre os terminais 1 e 3.",
        "Ponta 1 e Ponta 3. Ordem indiferente.",
        "Leituras validas para indutancias acima de 10uH.",
        "Nao possui polaridade."
    },
    // 5: Multimetro DC
    {
        "MULTIMETRO DC",
        "Use as pontas de prova externas nos bornes frontais.",
        "Vermelho no V/mA, Preto no COM.",
        "Cuidado para nao ultrapassar o limite de 600V DC.",
        "Inverter as pontas mostrara um valor negativo."
    },
    // 6: Multimetro AC
    {
        "MULTIMETRO AC",
        "Conecte as pontas de prova a fonte de tensao alternada.",
        "Bornes AC exclusivos (se disponivel) ou bornes de multimetro.",
        "Mantenha as maos isoladas. PERIGO: ALTA TENSAO!",
        "Nao possui polaridade (corrente alternada)."
    },
    // 7: Corrente
    {
        "MEDICAO DE CORRENTE",
        "Conecte o multimetro em SERIE com a carga.",
        "Mova a ponta vermelha para o borne de 10A se necessario.",
        "Nunca conecte em paralelo com uma fonte de tensao!",
        "Polaridade determina o sentido da corrente (+/-)."
    },
    // 8: Temperatura
    {
        "TERMOMETRO DIGITAL",
        "Conecte o sensor DS18B20 ao borne de dados.",
        "VCC no 3.3V, GND no GND, DATA no pino de entrada.",
        "O sensor leva cerca de 750ms para completar a conversao.",
        "Pinos do sensor (visto de frente): 1-GND, 2-DATA, 3-VCC."
    },
    // 9: Camera Termica
    {
        "CAMERA TERMICA",
        "Aponte o sensor MLX90640 para o objeto desejado.",
        "Mantenha uma distancia de 10cm a 50cm para melhor nitidez.",
        "Areas brancas/vermelhas indicam maior calor.",
        "Nao aponte para o sol ou lasers de alta potencia."
    },
    // 10: Teste Automatico
    {
        "TESTE AUTOMATICO",
        "Insira qualquer componente passivo ou semicondutor.",
        "Terminais 1, 2 e 3 sao usados para a deteccao.",
        "O sistema tentara identificar o tipo e os valores sozinho.",
        "Se o componente estiver em curto, o sistema avisara."
    },
    // 11: IC / CI
    {
        "ANALISE DE IC / CI",
        "Conecte o CI fora da placa (desacoplado) usando os terminais 1, 2 e 3.",
        "Teste entre pares de pinos para identificar juncoes e comportamento basico.",
        "Nao teste CI energizado e evite pinos com capacitor carregado.",
        "Observe o chanfro/bolha para orientar o pino 1 do encapsulamento."
    },
    // 12: Scanner
    {
        "SCANNER DE COMPONENTES",
        "Conecte o componente e inicie a varredura para identificar tipo/estado.",
        "Use os terminais 1, 2 e 3; o sistema alterna automaticamente os pares.",
        "Para melhor resultado, limpe os terminais e evite contato com os dedos.",
        "Componentes com polaridade devem ser testados nos dois sentidos."
    },
    // 13: Calibracao
    {
        "CALIBRACAO DE PONTAS",
        "Curto-circuite as pontas e inicie o processo de calibracao.",
        "Use as pontas 1 e 2 (ou 1 e 3) conforme solicitado na tela.",
        "Nao mova as pontas durante o processo para evitar offsets incorretos.",
        "A calibracao remove erro de cabo/contato para leituras mais precisas."
    }
};

const HelpInfo* help_get_for_state(AppState state) {
    switch (state) {
        case STATE_MEASURE_RESISTOR:  return &help_database[0];
        case STATE_MEASURE_CAPACITOR: return &help_database[1];
        case STATE_MEASURE_DIODE:     return &help_database[2];
        case STATE_MEASURE_TRANSISTOR: return &help_database[3];
        case STATE_MEASURE_INDUCTOR:  return &help_database[4];
        case STATE_MULTIMETER: {
            MultimeterMode mode = multimeter_get_mode();
            if (mode == MMODE_AC_VOLTAGE) return &help_database[6];
            if (mode == MMODE_DC_CURRENT) return &help_database[7];
            return &help_database[5]; // Default DC Voltage
        }
        case STATE_THERMAL_PROBE:     return &help_database[8];
        case STATE_THERMAL_CAMERA:    return &help_database[9];
        case STATE_MEASURE_GENERIC:   return &help_database[10];
        case STATE_MEASURE_IC:        return &help_database[11];
        case STATE_SCANNER:           return &help_database[12];
        case STATE_CALIBRATION:       return &help_database[13];
        default: return nullptr;
    }
}
