#!/usr/bin/env python3
import sys
import csv
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) != 3:
        print("Uso: graficar_rendimiento_threads.py <log_csv> <imagen_salida>")
        sys.exit(1)

    log_path = sys.argv[1]
    img_path = sys.argv[2]

    threads = []
    tiempos = []

    try:
        with open(log_path, 'r', newline='') as f:
            reader = csv.DictReader(f)
            for row in reader:
                try:
                    threads.append(int(row['threads']))
                    tiempos.append(float(row['ms']))
                except (KeyError, ValueError):
                    # Saltar líneas mal formateadas
                    continue
    except FileNotFoundError:
        print(f"No se encontró el archivo de log: {log_path}")
        sys.exit(1)

    if not threads:
        print("El log no contiene datos válidos para graficar.")
        sys.exit(1)

    plt.figure()
    plt.plot(threads, tiempos, marker='o')
    plt.xlabel("Cantidad de threads")
    plt.ylabel("Tiempo total (ms)")
    plt.title("Rendimiento índice invertido paralelo")
    plt.grid(True)

    # Guardar sin mostrar en pantalla
    plt.savefig(img_path, bbox_inches='tight')

if __name__ == "__main__":
    main()
