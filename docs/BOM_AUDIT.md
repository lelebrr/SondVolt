# 📋 Auditoria de Materiais (BOM) — Sondvolt v3.2

Esta é a lista exaustiva e detalhada de todos os componentes necessários para construir o Sondvolt v3.2 com o sistema de proteção 220V AC robusto.

## 1. Placas e Módulos Base

| Item | Especificação Exata | Qtd | Notas |
|:---|:---|:---:|:---|
| **Board** | ESP32-2432S028R (Cheap Yellow Display) | 1 | Versão com slot MicroSD |
| **Sensor AC** | Módulo ZMPT101B (Azul) | 1 | Ajustar trimpot para ganho 1:1 |
| **Sensor DC** | Módulo INA219 (I2C) | 1 | Shunt de 0.1Ω (padrão) |
| **Sensor Temp** | Sonda DS18B20 (Waterproof) | 1 | Com cabo de ~1 metro |
| **SD Card** | MicroSD 4GB a 32GB (FAT32) | 1 | Classe 10 recomendada |

## 2. Componentes de Proteção Elétrica (OBRIGATÓRIO)

| Componente | Valor / Part Number | Qtd | Localização |
|:---|:---|:---:|:---|
| **Fusível** | Fusível de Vidro Rápido 5A (5x20mm) | 1 | Entrada Fase (L) |
| **Porta-Fusível** | Suporte de fusível p/ painel ou PCB | 1 | Acomodar o fusível 5A |
| **Varistor** | 14D431K (ou 10D431K) | 1 | Paralelo L/N (Entrada AC) |
| **TVS Diode** | P6KE400A (Unidirecional) | 1 | Paralelo L/N (Pós-Varistor) |
| **Capacitor C1** | 100nF Cerâmico (104) | 1 | Saída OUT do ZMPT101B |
| **Capacitor C2** | 10µF Eletrolítico (16V ou >) | 1 | Alimentação VCC ZMPT101B |
| **Resistor R1** | 10kΩ 1/4W (Marrom-Preto-Laranja) | 1 | Pull-down saída ZMPT101B |
| **Resistor R2** | 4.7kΩ 1/4W (Amarelo-Violeta-Vermelho) | 1 | Pull-up OneWire DS18B20 |

## 3. Conectores e Fiação

| Item | Especificação | Cor Sugerida | Notas |
|:---|:---|:---:|:---|
| **Fio Entrada AC** | Flexível 1.0mm² ou 1.5mm² | Marrom (L) / Azul (N) | Bitola de segurança |
| **Pontas de Prova** | Par de pontas CAT III 600V | Vermelho (+) / Preto (-) | Pino banana ou jacaré |
| **Conector CN1** | JST-PH 2.0mm 4-pin | — | Conexão Analógica |
| **Conector P3/J3** | JST-PH 2.0mm 4-pin | — | Conexão Digital I2C |
| **Jumpers Internos** | Fios 26AWG ou 24AWG | Cores variadas | Conexões internas CYD |

## 4. Miscelânea

| Item | Especificação | Notas |
|:---|:---|:---|
| **Gabinete** | Case Impresso em 3D (PLA ou ABS) | Ver pasta /3d_models |
| **Parafusos** | M2.5 x 6mm (Auto-atarraxante) | Fixação da placa CYD |
| **Isolação** | Tubo Termo-retrátil (2mm e 5mm) | Isolar conexões AC (L/N) |

---
**Status da Auditoria:** ✅ 100% Compatível com Firmware v3.2.0
