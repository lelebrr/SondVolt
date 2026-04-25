
import sys
import os
from PIL import Image

def convert_to_rgb565(image):
    pixels = list(image.getdata())
    data = []
    for p in pixels:
        # Handle transparency
        if len(p) == 4 and p[3] < 128:
            r, g, b = 0, 0, 0 # Black for splash
        else:
            r, g, b = p[0], p[1], p[2]
        
        # RGB888 to RGB565
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        # Swap bytes for ESP32 (Little Endian to Big Endian for pushImage)
        # Actually pushImage expects 16-bit values in big-endian order (high byte first)
        data.append(f"0x{((rgb565 >> 8) | (rgb565 << 8)) & 0xFFFF:04X}")
    return data

def process_logo(input_path, output_h):
    if not os.path.exists(input_path):
        print(f"Error: {input_path} not found")
        return

    img = Image.open(input_path).convert("RGBA")
    orig_w, orig_h = img.size
    
    # 1. Full screen version (fit to 320 width)
    w_full = 320
    h_full = int(orig_h * (w_full / orig_w))
    img_full = img.resize((w_full, h_full), Image.Resampling.LANCZOS)
    data_full = convert_to_rgb565(img_full)
    
    # 2. Small version (fit to 80 width)
    w_small = 80
    h_small = int(orig_h * (w_small / orig_w))
    img_small = img.resize((w_small, h_small), Image.Resampling.LANCZOS)
    data_small = convert_to_rgb565(img_small)
    
    with open(output_h, "w") as f:
        f.write("#ifndef LOGO_BITMAP_H\n#define LOGO_BITMAP_H\n\n#include <Arduino.h>\n\n")
        
        f.write(f"// Logo Full Screen: {w_full}x{h_full}\n")
        f.write(f"const uint16_t logo_full_width = {w_full};\n")
        f.write(f"const uint16_t logo_full_height = {h_full};\n")
        f.write(f"const uint16_t logo_full_bitmap[{len(data_full)}] PROGMEM = {{\n    ")
        f.write(", ".join(data_full))
        f.write("\n};\n\n")
        
        f.write(f"// Logo Small: {w_small}x{h_small}\n")
        f.write(f"const uint16_t logo_small_width = {w_small};\n")
        f.write(f"const uint16_t logo_small_height = {h_small};\n")
        f.write(f"const uint16_t logo_small_bitmap[{len(data_small)}] PROGMEM = {{\n    ")
        f.write(", ".join(data_small))
        f.write("\n};\n\n")
        
        f.write("#endif\n")

if __name__ == "__main__":
    logo_path = r"C:\Users\leleb\Desktop\img\Nova pasta\logo.png"
    output_path = r"d:\Projetos\Component_Tester\src\logo_bitmap.h"
    process_logo(logo_path, output_path)
    print("Logo conversion complete.")
