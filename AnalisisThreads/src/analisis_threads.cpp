#include "env_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <fstream>
#include <utility>  // std::pair
#include <cstdlib>

using namespace std;

int main() {
    cout << "=== Análisis de rendimiento del índice invertido paralelo ===" << endl;

    // 1) Cargar variables de entorno (.env)
    map<string, string> env = cargarEnv(".env");

    auto it = env.find("INDICE_INVET_PARALELO");
    if (it == env.end() || it->second.empty()) {
        cerr << " La variable INDICE_INVET_PARALELO no está definida o está vacía en .env" << endl;
        return 1;
    }
    string progIndice = it->second;

    // 2) Pedir cuántas configuraciones de threads se van a probar
    int nConfigs = 0;
    cout << "¿Cuántas configuraciones distintas de threads quieres probar? ";
    if (!(cin >> nConfigs) || nConfigs <= 0) {
        cerr << " Debes ingresar un número entero mayor que 0." << endl;
        return 1;
    }

    // 3) Leer cada valor de CANT_THREADS[i]
    vector<int> cantThreads;
    cantThreads.reserve(nConfigs);

    for (int i = 0; i < nConfigs; ++i) {
        int t;
        cout << "CANT_THREADS[" << i << "] = ";
        if (!(cin >> t) || t <= 0) {
            cerr << " Cada cantidad de threads debe ser un entero mayor que 0." << endl;
            return 1;
        }
        cantThreads.push_back(t);
    }

    // 4) Carpeta con libros
    string carpetaLibros;
    cout << "Ingrese la carpeta con los libros (por ejemplo, ./LIBROS): ";
    cin >> carpetaLibros;

    // 5) Tamaño de lote
    int nLote = 0;
    cout << "Ingrese el tamaño del lote (N-LOTE): ";
    if (!(cin >> nLote) || nLote <= 0) {
        cerr << " N-LOTE debe ser un entero mayor que 0." << endl;
        return 1;
    }

    // 6) Nombre base para los archivos índice
    string baseNombreIdx;
    cout << "Ingrese el nombre base para los archivos índice (sin extensión): ";
    cin >> baseNombreIdx;

    // 7) Carpetas:
    //    - outputDir: SOLO la imagen del gráfico
    //    - dataDir:   todos los demás archivos (csv, idx, logs, mapas)
    const string outputDir = "AnalisisThreads/output";
    const string dataDir   = "AnalisisThreads/data";

    system(("mkdir -p " + outputDir).c_str());
    system(("mkdir -p " + dataDir).c_str());

    // 8) Log de rendimiento (CSV) dentro de dataDir
    const string logPath = dataDir + "/rendimiento_threads.csv";
    ofstream logFile(logPath);
    if (!logFile.is_open()) {
        cerr << " No se pudo crear el log de rendimiento en: " << logPath << endl;
        return 1;
    }
    logFile << "threads,ms\n";

    vector<pair<int, long long>> resultados;

    // 9) Loop sobre cada cantidad de threads
    for (int t : cantThreads) {
        // Índice se guarda en dataDir
        string nombreIdx = dataDir + "/" + baseNombreIdx + "_t" + to_string(t) + ".idx";

        // Comando para llamar al ejecutable paralelo
        string comando = progIndice + string(" ") +
                         nombreIdx + string(" ") +
                         carpetaLibros + string(" ") +
                         to_string(t) + string(" ") +
                         to_string(nLote);

        cout << "\n--- Ejecutando con " << t << " threads ---" << endl;
        cout << "Comando: " << comando << endl;

        auto inicio = chrono::high_resolution_clock::now();
        int ret = system(comando.c_str());
        auto fin = chrono::high_resolution_clock::now();

        // Mover MAPA-LIBROS.txt y log_proceso_paralelo.txt, si existen, a dataDir
        string moveMapa = "mv MAPA-LIBROS.txt " + dataDir +
                          "/MAPA-LIBROS_t" + to_string(t) + ".txt 2>/dev/null";
        string moveLog = "mv log_proceso_paralelo.txt " + dataDir +
                         "/log_proceso_paralelo_t" + to_string(t) + ".txt 2>/dev/null";

        system(moveMapa.c_str());
        system(moveLog.c_str());

        if (ret != 0) {
            cerr << "  El programa crear_indice_paralelo retornó código "
                 << ret << " para " << t << " threads." << endl;
        }

        auto dur = chrono::duration_cast<chrono::milliseconds>(fin - inicio).count();
        resultados.push_back({t, dur});
        logFile << t << "," << dur << "\n";

        cout << "Tiempo total para " << t << " threads: " << dur << " ms" << endl;
    }

    logFile.close();
    cout << "\n Log de rendimiento guardado en: " << logPath << endl;

    // 10) Llamar al script de Python para generar el gráfico
    const string imgPath = outputDir + "/grafico_rendimiento_threads.png";
    string cmdPython = "python3 ./AnalisisThreads/graficar_rendimiento_threads.py " +
                       logPath + " " + imgPath;

    cout << "\nGenerando gráfico de rendimiento..." << endl;
    int retPy = system(cmdPython.c_str());
    if (retPy != 0) {
        cerr << "  Hubo un problema al ejecutar el script de Python (código "
             << retPy << ")." << endl;
    } else {
        cout << " Gráfico generado en: " << imgPath << endl;
    }

    cout << "\nProceso de análisis de rendimiento finalizado." << endl;
    return 0;
}
