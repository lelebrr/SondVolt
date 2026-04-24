import csv

def generate_e96_values():
    e96 = [10.0, 10.2, 10.5, 10.7, 11.0, 11.3, 11.5, 11.8, 12.1, 12.4, 12.7, 13.0, 13.3, 13.7, 14.0, 14.3, 14.7, 15.0, 15.4, 15.8, 16.2, 16.5, 16.9, 17.4, 17.8, 18.2, 18.7, 19.1, 19.6, 20.0, 20.5, 21.0, 21.5, 22.1, 22.6, 23.2, 23.7, 24.3, 24.9, 25.5, 26.1, 26.7, 27.4, 28.0, 28.7, 29.4, 30.1, 30.9, 31.6, 32.4, 33.2, 34.0, 34.8, 35.7, 36.5, 37.4, 38.3, 39.2, 40.2, 41.2, 42.2, 43.2, 44.2, 45.3, 46.4, 47.5, 48.7, 49.9, 51.1, 52.3, 53.6, 54.9, 56.2, 57.6, 59.0, 60.4, 61.9, 63.4, 64.9, 66.5, 68.1, 69.8, 71.5, 73.2, 75.0, 76.8, 78.7, 80.6, 82.5, 84.5, 86.6, 88.7, 90.9, 93.1, 95.3, 97.6]
    results = []
    for exp in range(-1, 6): # 1 Ohm to 10M
        for v in e96:
            val = v * (10**exp)
            if val >= 1 and val <= 10000000:
                results.append(val)
    return results

def main():
    components = []
    
    # Real World NPNs
    npns = [
        ("BC547", 300, 700), ("BC548", 300, 700), ("BC549", 500, 700), ("2N2222", 200, 700),
        ("2N3904", 200, 700), ("BC337", 250, 700), ("TIP120", 2500, 1400), ("TIP31C", 50, 700),
        ("BD139", 60, 700), ("BC546", 300, 700), ("MPSA42", 100, 700), ("SS8050", 200, 700)
    ]
    for name, hfe, vf in npns:
        components.append([name, 1, hfe, int(hfe*0.5), int(hfe*1.5), vf, 1, 2, 3, f"BJT NPN {name}", "General Purpose", 0])

    # Real World PNPs
    pnps = [
        ("BC557", 300, 700), ("BC558", 300, 700), ("BC559", 500, 700), ("2N3906", 200, 700),
        ("BC327", 250, 700), ("TIP125", 2500, 1400), ("TIP32C", 50, 700), ("BD140", 60, 700),
        ("MPSA92", 100, 700), ("SS8550", 200, 700)
    ]
    for name, hfe, vf in pnps:
        components.append([name, 2, hfe, int(hfe*0.5), int(hfe*1.5), vf, 1, 2, 3, f"BJT PNP {name}", "General Purpose", 0])

    # Resistors E96 (CAT 10)
    for v in generate_e96_values():
        name = f"R {v:.1f}" if v < 100 else f"R {int(v)}"
        if v >= 1000000: name = f"R {v/1000000:.2f}M"
        elif v >= 1000: name = f"R {v/1000:.2f}k"
        components.append([name, 10, int(v), int(v*0.99), int(v*1.01), 0, 1, 2, 0, f"Resistor {name} 1%", "Precision", 0])

    # Capacitors (CAT 9)
    for exp in range(-2, 4): # 100pF to 1000uF
        for v in [10, 22, 33, 47, 100]:
            val_nf = v * (10**exp)
            name = f"C {val_nf:.1f}nF"
            if val_nf < 1: name = f"C {val_nf*1000:.0f}pF"
            elif val_nf >= 1000: name = f"C {val_nf/1000:.1f}uF"
            components.append([name, 9, int(val_nf) if val_nf >= 1 else 1, int(val_nf*0.8), int(val_nf*1.2), 0, 1, 2, 0, f"Capacitor {name}", "Filter", 0])

    # Diodes (CAT 5)
    for v in range(500, 850, 2):
        name = f"D-Vf{v}"
        components.append([name, 5, v, v-5, v+5, 0, 1, 2, 0, f"Diode Standard Vf={v}mV", "Rectifier", 0])

    # Extended 2N Series (CAT 1 & 2)
    for i in range(1000, 5000):
        hfe = 100 + (i % 400)
        vf = 600 + (i % 150)
        name = f"2N{i}"
        cat = 1 if (i % 2 == 0) else 2
        desc = "NPN" if cat == 1 else "PNP"
        components.append([name, cat, hfe, int(hfe*0.7), int(hfe*1.3), vf, 1, 2, 3, f"{desc} Transistor {name}", "General Purpose", 0])

    # Extended BC Series
    for i in range(100, 900):
        hfe = 150 + (i % 500)
        vf = 650 + (i % 100)
        name = f"BC{i}"
        cat = 1 if (i < 500) else 2
        desc = "NPN" if cat == 1 else "PNP"
        components.append([name, cat, hfe, int(hfe*0.8), int(hfe*1.2), vf, 1, 2, 3, f"{desc} Transistor {name}", "Small Signal", 0])
    zeners = [2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1, 10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30]
    for z in zeners:
        v = int(z * 1000)
        name = f"Zener {z}V"
        components.append([name, 6, v, int(v*0.95), int(v*1.05), 0, 1, 2, 0, f"Zener Diode {z}V", "Voltage Reg", 0])

    with open('sd_files/COMPBD.CSV', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(components)
    
    print(f"Total components generated: {len(components)}")

if __name__ == "__main__":
    main()
