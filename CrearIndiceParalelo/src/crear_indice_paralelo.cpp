#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <cctype>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;
namespace fs = filesystem;

// Mutex para proteger estructuras compartidas
mutex indiceMutex;
mutex logMutex;
mutex contadorLibroMutex;

// Variables globales
int N_THREADS = 4;
int N_LOTE = 10;
int contadorLibroGlobal = 1;

// Mapa de libros: id -> nombre
map<int, string> mapaLibros;

// Estructura para el log
struct LogEntry {
    int threadId;
    int libroId;
    int cantidadPalabras;
    string timestampInicio;
    string timestampFin;
};

vector<LogEntry> logEntries;

// Función para normalizar palabras
string normalizar(const string& palabra) {
    string out;
    for (char c : palabra) {
        if (isalpha((unsigned char)c)) {
            out.push_back(tolower(c));
        }
    }
    return out;
}

// Función para obtener timestamp
string obtenerTimestamp() {
    auto now = chrono::system_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = chrono::system_clock::to_time_t(now);
    tm bt{};
    #ifdef _WIN32
    localtime_s(&bt, &timer);
    #else
    localtime_r(&timer, &bt);
    #endif
    
    ostringstream oss;
    oss << put_time(&bt, "%H:%M:%S");
    oss << '.' << setfill('0') << setw(3) << ms.count();
    return oss.str();
}

// Función para asignar ID a un libro
int asignarIdLibro(const string& nombreArchivo) {
    lock_guard<mutex> lock(contadorLibroMutex);
    int id = contadorLibroGlobal++;
    mapaLibros[id] = nombreArchivo;
    return id;
}

// Función que ejecuta cada thread procesando en lotes
void procesarLibrosEnLotes(int threadId, 
                           vector<fs::path> archivos,
                           map<string, map<int, int>>& indiceGlobal) {
    
    // Procesar en lotes de N_LOTE libros
    for (size_t i = 0; i < archivos.size(); i += N_LOTE) {
        size_t fin = min(i + N_LOTE, archivos.size());
        
        // Procesar lote actual
        for (size_t j = i; j < fin; j++) {
            const auto& archivo = archivos[j];
            string nombreArchivo = archivo.filename().string();
            
            // Asignar ID al libro
            int libroId = asignarIdLibro(nombreArchivo);
            
            string timestampInicio = obtenerTimestamp();
            
            ifstream in(archivo);
            if (!in.is_open()) {
                cerr << "⚠ Thread " << threadId << " no pudo abrir: " << nombreArchivo << endl;
                continue;
            }

            // Índice local del lote
            map<string, int> indiceLocal;
            int totalPalabras = 0;
            
            string palabra;
            while (in >> palabra) {
                string limpia = normalizar(palabra);
                if (!limpia.empty()) {
                    indiceLocal[limpia]++;
                    totalPalabras++;
                }
            }
            in.close();

            string timestampFin = obtenerTimestamp();

            // Actualizar el índice global (sección crítica)
            {
                lock_guard<mutex> lock(indiceMutex);
                for (auto& [palabra, cant] : indiceLocal) {
                    indiceGlobal[palabra][libroId] += cant;
                }
            }

            // Registrar en el log
            {
                lock_guard<mutex> lock(logMutex);
                logEntries.push_back({threadId, libroId, totalPalabras, 
                                     timestampInicio, timestampFin});
            }

            cout << "Thread " << threadId << " procesó: [ID:" << libroId << "] " 
                 << nombreArchivo << " (" << totalPalabras << " palabras)" << endl;
        }
        
        // Liberar memoria del lote procesado (los maps locales se destruyen automáticamente)
        cout << "Thread " << threadId << " completó lote de " << (fin - i) << " libros" << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "=== Crear Índice Invertido PARALELO === PID: " << obtenerPID() << endl;

    if (argc < 5) {
        cerr << "Uso: " << argv[0] << " <archivo.idx> <carpeta_libros> <N-THREADS> <N-LOTE>" << endl;
        cerr << "Ejemplo: " << argv[0] << " indice.idx ./libros 4 10" << endl;
        return 1;
    }

    string archivoIdx = argv[1];
    string carpeta = argv[2];
    N_THREADS = stoi(argv[3]);
    N_LOTE = stoi(argv[4]);

    if (N_THREADS <= 0) {
        cerr << "❌ N-THREADS debe ser mayor que 0" << endl;
        return 1;
    }

    if (N_LOTE <= 0) {
        cerr << "❌ N-LOTE debe ser mayor que 0" << endl;
        return 1;
    }

    // Validar extensión .idx
    if (archivoIdx.size() < 4 || archivoIdx.substr(archivoIdx.size() - 4) != ".idx") {
        cerr << "❌ El archivo debe terminar en .idx" << endl;
        return 1;
    }

    // Validar carpeta
    if (!fs::exists(carpeta) || !fs::is_directory(carpeta)) {
        cerr << "❌ La carpeta no existe o no es válida: " << carpeta << endl;
        return 1;
    }

    // Recolectar todos los archivos
    vector<fs::path> archivos;
    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            archivos.push_back(entry.path());
        }
    }

    if (archivos.empty()) {
        cerr << "❌ No hay archivos en la carpeta" << endl;
        return 1;
    }

    cout << "📚 Total de libros: " << archivos.size() << endl;
    cout << "🧵 N-THREADS: " << N_THREADS << endl;
    cout << "📦 N-LOTE: " << N_LOTE << " libros por lote" << endl;

    // Dividir archivos entre threads
    vector<vector<fs::path>> archivosPorThread(N_THREADS);
    for (size_t i = 0; i < archivos.size(); i++) {
        archivosPorThread[i % N_THREADS].push_back(archivos[i]);
    }

    // Índice global compartido: palabra -> (libroId -> cantidad)
    map<string, map<int, int>> indiceGlobal;

    // Crear y lanzar threads
    vector<thread> threads;
    auto tiempoInicio = chrono::high_resolution_clock::now();

    for (int i = 0; i < N_THREADS; i++) {
        if (!archivosPorThread[i].empty()) {
            threads.emplace_back(procesarLibrosEnLotes, i, archivosPorThread[i], ref(indiceGlobal));
        }
    }

    // Esperar a que terminen todos los threads
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    auto tiempoFin = chrono::high_resolution_clock::now();
    auto duracion = chrono::duration_cast<chrono::milliseconds>(tiempoFin - tiempoInicio);

    // Guardar MAPA-LIBROS
    ofstream mapaFile("MAPA-LIBROS.txt");
    if (mapaFile.is_open()) {
        for (auto& [id, nombre] : mapaLibros) {
            mapaFile << id << ";" << nombre << "\n";
        }
        mapaFile.close();
        cout << "📋 Mapa de libros guardado en: MAPA-LIBROS.txt" << endl;
    }

    // Guardar índice invertido con IDs
    ofstream out(archivoIdx);
    if (!out.is_open()) {
        cerr << "❌ No se pudo crear el archivo de índice: " << archivoIdx << endl;
        return 1;
    }

    for (auto& [palabra, docs] : indiceGlobal) {
        out << palabra;
        for (auto& [libroId, cant] : docs) {
            out << ";(" << libroId << "," << cant << ")";
        }
        out << "\n";
    }
    out.close();

    // Guardar log
    ofstream logFile("log_proceso_paralelo.txt");
    if (logFile.is_open()) {
        for (const auto& entry : logEntries) {
            logFile << entry.threadId << "," 
                    << entry.libroId << "," 
                    << entry.cantidadPalabras << "," 
                    << entry.timestampInicio << "," 
                    << entry.timestampFin << "\n";
        }
        logFile.close();
        cout << "📝 Log guardado en: log_proceso_paralelo.txt" << endl;
    }

    cout << "\n✅ Índice invertido paralelo creado en: " << archivoIdx << endl;
    cout << "⏱️  Tiempo total: " << duracion.count() << " ms" << endl;
    cout << "📊 Total de palabras únicas: " << indiceGlobal.size() << endl;
    cout << "📚 Total de libros procesados: " << mapaLibros.size() << endl;

    return 0;
}