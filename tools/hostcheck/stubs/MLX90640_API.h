#ifndef STUB_MLX_API_H
#define STUB_MLX_API_H
#include <Arduino.h>
typedef struct {
  int16_t kVdd; int16_t vdd25; float KvPTAT; float KtPTAT; uint16_t vPTAT25;
  float alphaPTAT; int16_t gainEE; float tgc; float cpKv; float cpKta;
  uint8_t resolutionEE; uint8_t calibrationModeEE; float KsTa; float ksTo[5];
  int16_t ct[5]; uint16_t alpha[768]; uint8_t alphaScale; int16_t offset[768];
  int8_t kta[768]; uint8_t ktaScale; int8_t kv[768]; uint8_t kvScale;
  float cpAlpha[2]; int16_t cpOffset[2]; float ilChessC[3]; uint16_t brokenPixels[5];
  uint16_t outlierPixels[5];
} paramsMLX90640;
int   MLX90640_DumpEE(uint8_t, uint16_t*);
int   MLX90640_ExtractParameters(uint16_t*, paramsMLX90640*);
int   MLX90640_GetFrameData(uint8_t, uint16_t*);
float MLX90640_GetTa(uint16_t*, const paramsMLX90640*);
void  MLX90640_CalculateTo(uint16_t*, const paramsMLX90640*, float, float, float*);
int   MLX90640_SetRefreshRate(uint8_t, uint8_t);
int   MLX90640_SetResolution(uint8_t, uint8_t);
int   MLX90640_SetChessMode(uint8_t);
#endif
