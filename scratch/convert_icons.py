import os
from PIL import Image

icon_dir = r"C:\Users\leleb\Desktop\img\Nova pasta"
output_file = r"d:\Projetos\Component_Tester\src\icons_bitmap.h"

def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

icons = {
    "RESISTOR.png": "icon_resistor",
    "ajustes.png": "icon_settings",
    "capacitor.png": "icon_capacitor",
    "diodo.png": "icon_diode",
    "historico.png": "icon_history",
    "indutor.png": "icon_inductor",
    "led.png": "icon_led",
    "multimetro.png": "icon_multimeter",
    "sobre.png": "icon_about",
    "termometro.png": "icon_temp",
    "transistor.png": "icon_transistor"
}

with open(output_file, "w") as f:
    f.write("#ifndef ICONS_BITMAP_H\n")
    f.write("#define ICONS_BITMAP_H\n\n")
    f.write("#include <Arduino.h>\n\n")

    for filename, var_name in icons.items():
        path = os.path.join(icon_dir, filename)
        if not os.path.exists(path):
            print(f"Warning: {path} not found")
            continue
            
        img = Image.open(path).convert("RGBA")
        img = img.resize((32, 32), Image.Resampling.LANCZOS)
        
        f.write(f"const uint16_t {var_name}_32x32[] PROGMEM = {{\n")
        
        pixels = []
        for y in range(32):
            row = []
            for x in range(32):
                r, g, b, a = img.getpixel((x, y))
                # Se for transparente, usa preto ou cor de fundo (0x1082)
                # Mas para pushImage o ideal é ter o fundo correto ou usar sprite.
                # Vamos assumir que se alpha < 128, é fundo (0x1082)
                if a < 128:
                    val = 0x1082 
                else:
                    val = rgb888_to_rgb565(r, g, b)
                # Swap bytes for ESP32 pushImage
                val = ((val >> 8) | (val << 8)) & 0xFFFF
                row.append(f"0x{val:04X}")
            pixels.append(", ".join(row))
        
        f.write("    " + ",\n    ".join(pixels))
        f.write("\n};\n\n")

    f.write("#endif\n")

print("Done! Header generated at", output_file)
