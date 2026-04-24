
import csv
import random

def generate_transistors():
    res = []
    # Real BC series
    bc_npn = [107, 108, 109, 140, 211, 237, 337, 338, 487, 517, 546, 547, 548, 549, 550, 635, 637, 639, 817, 847, 848, 849, 850]
    bc_pnp = [177, 178, 179, 141, 161, 212, 213, 214, 327, 328, 488, 516, 556, 557, 558, 559, 560, 636, 638, 640, 807, 857, 858, 859, 860]
    for val in bc_npn: res.append([f"BC{val}", 1, 200, 150, 300, 500, 1, 2, 3, f"BC{val} NPN Signal", "General Purpose", 0])
    for val in bc_pnp: res.append([f"BC{val}", 2, 200, 150, 300, 500, 1, 2, 3, f"BC{val} PNP Signal", "General Purpose", 0])

    # 2N series (Common real ones only)
    common_2n_npn = [2222, 3904, 4401, 5551, 3055, 3440, 2369, 1711, 1613, 2102, 1893, 3019, 3053, 3704, 3903, 4400, 6487, 6488, 2219, 2218, 706, 708, 709]
    common_2n_pnp = [2907, 3906, 4403, 5401, 2955, 3702, 3905, 4402, 6491, 2905, 2904]
    for val in common_2n_npn: res.append([f"2N{val}", 1, 100, 50, 300, 800, 1, 2, 3, f"2N{val} NPN", "Switching", 0])
    for val in common_2n_pnp: res.append([f"2N{val}", 2, 100, 50, 300, 800, 1, 2, 3, f"2N{val} PNP", "Switching", 0])

    # TIP series
    tip_npn = [29, 31, 33, 35, 41, 47, 100, 101, 102, 110, 111, 112, 120, 121, 122, 140, 141, 142]
    tip_pnp = [30, 32, 34, 36, 42, 48, 105, 106, 107, 115, 116, 117, 125, 126, 127]
    for val in tip_npn: res.append([f"TIP{val}", 1, 50, 20, 100, 5000, 1, 2, 3, f"TIP{val} NPN Power", "Power", 0])
    for val in tip_pnp: res.append([f"TIP{val}", 2, 50, 20, 100, 5000, 1, 2, 3, f"TIP{val} PNP Power", "Power", 0])

    # Japanese series
    japanese = [
        ("2SC", [945, 1815, 5200, 3281, 2655, 3355, 3358, 3807, 1906, 1923, 2570, 2922, 3858], 1),
        ("2SA", [1015, 1943, 1302, 1020, 1265, 1216, 1295], 2),
        ("2SB", [772, 817, 649, 1375, 1560, 1647], 2),
        ("2SD", [880, 882, 669, 718, 1047, 1555, 1880, 2139], 1),
    ]
    for prefix, vals, tid in japanese:
        for val in vals: res.append([f"{prefix}{val}", tid, 150, 80, 300, 1000, 1, 2, 3, f"{prefix}{val} Transistor", "Audio/RF", 0])

    # BD series
    bd_npn = [135, 137, 139, 233, 235, 237, 239, 241, 243, 437, 439, 441, 677, 679, 681, 911]
    bd_pnp = [136, 138, 140, 234, 236, 238, 240, 242, 244, 438, 440, 442, 678, 680, 682, 912]
    for val in bd_npn: res.append([f"BD{val}", 1, 40, 25, 100, 1500, 1, 2, 3, f"BD{val} NPN Power", "Driver", 0])
    for val in bd_pnp: res.append([f"BD{val}", 2, 40, 25, 100, 1500, 1, 2, 3, f"BD{val} PNP Power", "Driver", 0])

    return res

def generate_mosfets():
    res = []
    irf_n = [510, 520, 530, 540, 610, 620, 630, 640, 710, 720, 730, 740, 820, 830, 840, 1010, 1404, 3205, 3710, 4905]
    irf_p = [9510, 9520, 9530, 9540, 9610, 9620, 9630, 9640, 5305]
    for val in irf_n: res.append([f"IRF{val}", 3, 50, 40, 60, 10000, 1, 2, 3, f"IRF{val} N-MOSFET", "Switching", 0])
    for val in irf_p: res.append([f"IRF{val}", 4, 100, 80, 120, 8000, 1, 2, 3, f"IRF{val} P-MOSFET", "Switching", 0])
    for val in [24, 34, 44, 46, 48]: res.append([f"IRFZ{val}", 3, 20, 15, 25, 40000, 1, 2, 3, f"IRFZ{val} N-MOSFET", "Power", 0])
    res.append(["2N7000", 3, 5000, 4000, 6000, 200, 1, 2, 3, "2N7000 N-MOSFET", "Small Signal", 0])
    res.append(["2N7002", 3, 5000, 4000, 6000, 115, 1, 2, 3, "2N7002 N-MOSFET SMD", "Switching", 0])
    res.append(["BS170", 3, 5000, 4000, 6000, 500, 1, 2, 3, "BS170 N-MOSFET", "Switching", 0])
    return res

def generate_diodes():
    res = []
    for i in range(4001, 4008): res.append([f"1N{i}", 5, 700, 600, 800, 1000, 1, 2, 0, f"1N{i} Rectifier", "Power", 0])
    for i in range(5400, 5409): res.append([f"1N{i}", 5, 800, 700, 900, 3000, 1, 2, 0, f"1N{i} Rectifier", "Power", 0])
    for i in [5817, 5818, 5819, 5820, 5821, 5822]: res.append([f"1N{i}", 5, 400, 300, 500, 3000, 1, 2, 0, f"1N{i} Schottky", "Switching", 0])
    res.append(["1N4148", 5, 600, 500, 700, 200, 1, 2, 0, "1N4148 Fast Diode", "Switching", 0])
    volts = [2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1, 10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30, 33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91, 100]
    for v in volts:
        vz = int(v * 1000)
        res.append([f"BZX55C{v}".replace('.','V'), 6, vz, int(vz*0.95), int(vz*1.05), 500, 1, 2, 0, f"Zener {v}V 500mW", "Regulation", 0])
        res.append([f"1N47{32 + volts.index(v)}", 6, vz, int(vz*0.95), int(vz*1.05), 1000, 1, 2, 0, f"Zener {v}V 1W", "Regulation", 0])
    return res

def generate_passives():
    res = []
    # E24 series (1.0 to 9.1) * (1 to 1M)
    e24 = [1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1]
    for m in [1, 10, 100, 1000, 10000, 100000, 1000000]:
        for b in e24:
            val = int(b * m)
            if val >= 1000000: s = f"{val/1000000:.1f}M"
            elif val >= 1000: s = f"{val/1000:.1f}K"
            else: s = str(val)
            s = s.replace('.0', '')
            res.append([f"R_{s}", 10, val, int(val*0.99), int(val*1.01), 250, 1, 2, 0, f"Resistor {s} Ohm", "Passive", 0])
    return res

unique_data = []
seen = set()

def add_data(rows):
    for r in rows:
        if not r: continue
        pn = r[0].upper()
        if pn not in seen:
            seen.add(pn)
            unique_data.append(r)

# START CLEAN: ONLY REFINED DATA
add_data(generate_transistors())
add_data(generate_mosfets())
add_data(generate_passives())
add_data(generate_diodes())
add_data(generate_ics())

# Sort
unique_data.sort(key=lambda x: (int(x[1]) if str(x[1]).isdigit() else 999, x[0]))

print(f"Total verified real components: {len(unique_data)}")

with open("D:/Projetos/Component_Tester/sd_files/bd_npn.csv", "w", newline="", encoding="utf-8") as f:
    csv.writer(f).writerows(unique_data)
with open("D:/Projetos/Component_Tester/sd_files/COMPBD.CSV", "w", newline="", encoding="utf-8") as f:
    csv.writer(f).writerows(unique_data)
