import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys
from dotenv import load_dotenv 

# --- CARGAR CONFIGURACIÓN ---
load_dotenv()

output_dir = os.getenv('STATS_OUTPUT_DIR', 'graficos_por_defecto')

# Crear la carpeta si no existe
if not os.path.exists(output_dir):
    try:
        os.makedirs(output_dir)
        print(f" Carpeta creada exitosamente: {output_dir}")
    except OSError as e:
        print(f" Error al crear directorio {output_dir}: {e}")
        sys.exit(1)
else:
    print(f"ℹ Usando carpeta existente: {output_dir}")

csv_file = 'game_stats.csv'
if not os.path.exists(csv_file):
    print(f"Error: No existe el archivo {csv_file}. ¡Ejecuta el servidor y juega primero!")
    sys.exit(1)

print("Procesando datos...")

# --- CARGA Y LIMPIEZA DE DATOS ---
try:
    # Leer CSV ignorando líneas corruptas por cierres forzados del servidor
    df = pd.read_csv(csv_file, on_bad_lines='skip')
    
    # Validar que existan las columnas necesarias 
    required_cols = ['PARTIDA_ID', 'PLAYER_ID', 'EQUIPO_ID', 'POS_X', 'POS_Y', 'SIZE', 'MODO_JUEGO']
    
    # Verificar si falta alguna columna crítica
    missing_cols = [c for c in required_cols if c not in df.columns]
    if missing_cols:
        print(f"⚠️ El CSV está incompleto (faltan: {missing_cols}). Juega una partida nueva con el código actualizado.")
        sys.exit(1)

    # Convertir columnas numéricas y limpiar errores
    cols_num = ['PARTIDA_ID', 'PLAYER_ID', 'EQUIPO_ID', 'POS_X', 'POS_Y', 'SIZE']
    for col in cols_num:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    
    df.dropna(subset=required_cols, inplace=True)
    
    if df.empty:
        print("⚠️ El archivo existe pero no tiene datos válidos para graficar.")
        sys.exit(0)
        
except Exception as e:
    print(f"Error crítico al leer CSV: {e}")
    sys.exit(1)

# Configurar estilo visual de los gráficos
sns.set_theme(style="darkgrid")

print(f"Generando 4 gráficos en: {output_dir} ...")

# --- GRÁFICO 1: Popularidad por Modo de Juego ---
try:
    plt.figure(figsize=(8, 6))
    
    # Contar jugadores únicos por modo
    players_per_mode = df.groupby('MODO_JUEGO')['PLAYER_ID'].nunique().reset_index()
    
    sns.barplot(data=players_per_mode, x='MODO_JUEGO', y='PLAYER_ID', palette='viridis')
    plt.title('Cantidad de Jugadores Únicos por Modo')
    plt.ylabel('Cantidad de Jugadores')
    plt.xlabel('Modo de Juego')
    
    save_path = os.path.join(output_dir, '1_popularidad_modos.png')
    plt.savefig(save_path)
    plt.close()
    print("-> Gráfico 1 generado")
except Exception as e:
    print(f"Error G1: {e}")

# --- GRÁFICO 2: Mapa de Calor de Actividad ---
try:
    plt.figure(figsize=(8, 8))
    # Usamos histograma 2D para ver densidad de movimiento
    plt.hist2d(df['POS_X'], df['POS_Y'], bins=50, cmap='magma', cmin=1)
    plt.colorbar(label='Frecuencia de paso')
    plt.title('Mapa de Calor: Zonas más transitadas')
    plt.xlabel('Posición X')
    plt.ylabel('Posición Y')
    plt.gca().invert_yaxis() # Invertir eje Y (coordenadas de pantalla)
    
    save_path = os.path.join(output_dir, '2_mapa_calor.png')
    plt.savefig(save_path)
    plt.close()
    print("-> Gráfico 2 generado")
except Exception as e:
    print(f"Error G2: {e}")

# --- GRÁFICO 3: Distribución de Tamaños (Habilidad) ---
try:
    plt.figure(figsize=(10, 6))
    # Tamaño máximo alcanzado por cada jugador en cada partida
    max_sizes = df.groupby(['PARTIDA_ID', 'PLAYER_ID'])['SIZE'].max()
    
    sns.histplot(max_sizes, bins=15, kde=True, color='green')
    plt.title('Distribución de Tamaños Máximos alcanzados (Habilidad)')
    plt.xlabel('Tamaño (Segmentos)')
    plt.ylabel('Frecuencia')
    
    save_path = os.path.join(output_dir, '3_distribucion_tamano.png')
    plt.savefig(save_path)
    plt.close()
    print("-> Gráfico 3 generado")
except Exception as e:
    print(f"Error G3: {e}")

# --- GRÁFICO 4: Tiempo Promedio de Supervivencia (Comparativa) ---
try:
    plt.figure(figsize=(8, 6))
    
    # Calcular segundos vivos por jugador por partida
    # Asumiendo 1 tick = 0.1 segundos (GAME_TICK_RATE = 100ms en servidor)
    survival = df.groupby(['PARTIDA_ID', 'PLAYER_ID', 'MODO_JUEGO']).size().reset_index(name='TICKS')
    survival['SEGUNDOS'] = survival['TICKS'] * 0.1
    
    # Filtramos partidas glitch/instantáneas (< 1 segundo)
    survival = survival[survival['SEGUNDOS'] > 1] 

    # Gráfico de barras con promedio y barra de error (desviación estándar)
    sns.barplot(data=survival, x='MODO_JUEGO', y='SEGUNDOS', estimator='mean', errorbar='sd', palette='Set2')
    
    plt.title('Tiempo Promedio de Supervivencia: SOLO vs DUO')
    plt.ylabel('Segundos (Media)')
    plt.xlabel('Modo de Juego')
    
    # Nota explicativa en el gráfico sobre la barra de error
    plt.figtext(0.5, 0.01, "La línea negra indica la variabilidad (Desv. Estándar)", ha="center", fontsize=9)
    
    save_path = os.path.join(output_dir, '4_supervivencia_comparativa.png')
    plt.savefig(save_path)
    plt.close()
    print("->  Gráfico 4 generado")
except Exception as e:
    print(f"Error G4: {e}")

print("\n Proceso finalizado exitosamente. Revisa la carpeta de salida.")