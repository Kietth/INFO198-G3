# INFO198-G3 SISTEMA

**Propósito de la aplicación:**

Este es un sistema que reune múltiples utilidades en un único punto de acceso. 

**Ejecución del Programa:**

Para iniciar el sistema, sigue estos pasos:

1. Descomprimir el archivo: Extrae el contenido del archivo comprimido en una carpeta de tu preferencia.

2. Compilar el código: Abre una terminal en la carpeta principal del proyecto y ejecuta el Makefile con el comando: make menu.

3. Iniciar el programa: Una vez compilado, puedes iniciar la aplicación principal usando el siguiente comando, donde debes reemplazar [usuario] y [contraseña] con tus credenciales y además añadir la ruta de un archivo de texto a ser leído para hacer un conteo de letras y palabras.

Bash
./menu_principal -u [usuario] -p [contraseña] -f [ruta-archivo-libro]

Este comando iniciará el menú principal desde donde podrás acceder a las distintas aplicaciones.

**Módulos y Funcionalidades:**

-ADMINISTRADOR DE USUARIOS: Permite crear, eliminar y listar usuarios con diferentes perfiles y permisos. <br/>
-MULTIPLICADOR DE MATRICES NXN: Una herramienta para realizar la multiplicación de matrices de NxN. <br/>
-JUEGO: Modulo para jugar "Super Snake" con 1 o más jugadores. Requiere la inicialización de un servidor al cual los jugadores en cuestión deberán conectarse al momento de iniciar el cliente. <br/>
-ES PALINDROMO?: Una utilidad para verificar si una cadena de texto es un palíndromo. <br/>
-CALCULAR UNA FUNCION: Un módulo para la evaluación de una funcion matematica predefinida. <br/>
-CONTEO DE PALABRAS: Una herramienta que cuenta la frecuencia de letras y palabras en un archivo de texto. <br/>
-INDICE INVERTIDO: Mapea palabras o términos de los libros disponibles para hacer una búsqueda rápida. <br/>
-INDICE PARALELO: Crea un índice invertido de forma paralela procesando múltiples archivos de texto simultáneamente usando hilos (threads). <br/>
-BUSCADOR: Sistema diseñado para búsquedas eficientes en grandes volúmenes de documentos con gestión inteligente de caché. <br/>

**Variables de Entorno:** 
Para Administrador de Usuarios: <br/>
    USER_FILE: Especifica la ruta del archivo donde se almacenan los usuarios. <br/>
    PROFILE_FILE: Especifica la ruta del archivo donde se almacenan los perfiles y permisos. <br/>

Para Menú Principal: <br/>
    ADMIN_SYS: Ruta de la aplicación de administración de usuarios. <br/>
    MULTI_M: Ruta de la aplicación de multiplicación de matrices. <br/>
    CREATE_INDEX: Ruta de la aplicación para crear el índice invertido. <br/>
    CONTEO_TEXTO: Ruta de la aplicación de conteo de texto. <br/>
    INDICE_INVET_PARALELO: Ruta de la aplicación creación de un índice invertido paralelo. <br/>
    BUSCADOR: Ruta de la interfaz principal del usuario del buscador <br/>
    JUEGO_DIR: PATH juego.
    JUEGO_SERVER_BIN: Ruta servidor juego.
    JUEGO_CLIENT_BIN: Ruta cliente juego.
    JUEGO_SCRIPT: Ruta script para generar graficos juego.
    
Para Buscador: <br/>
    CACHE: Ruta para el sistema de cache intermedio <br/>
    MOTOR_BUSQUEDA: Ruta para el motor principal de busqueda <br/>
    CACHE_SIZE: Tamaño maximo del cache <br/>
    TOPK: Limite de resultados por consulta <br/>

Para Juego: <br/>
    SERVER_IP: IP del servidor al que el cliente ha de conectarse para jugar. <br/>
    SERVER_PORT: Número de puerto en el que el servidor está escuchando y aceptando conexiones. <br/>
    STATS_OUTPUT_DIR: PATH graficos. <br/>

**Juego**

# Super Snake

## Descripción General
Super Snake es una implementación multijugador del clásico juego Snake, desarrollada bajo una arquitectura cliente-servidor en C++. El sistema integra un módulo de análisis de datos post-partida implementado en Python, diseñado para la evaluación de rendimiento y comportamiento de los jugadores.

## Mecánicas de Juego
El objetivo principal consiste en controlar una entidad (serpiente) dentro de una cuadrícula delimitada, consumiendo objetos (comida) para incrementar su tamaño. La condición de derrota se activa si la cabeza de la serpiente colisiona con:
1. Los límites del mapa (paredes).
2. Su propio cuerpo.
3. El cuerpo de otro jugador (salvo excepciones en modo de equipos).

La partida concluye cuando queda un único jugador o equipo activo.

## Modos de Juego
El servidor permite la configuración de dos modalidades distintas:

### SOLO (Free-for-All)
Modalidad de todos contra todos. Cada jugador es asignado a un equipo individual. Cualquier colisión con otra entidad resulta en la eliminación inmediata del jugador.

### DUO (Equipos)
Los jugadores son agrupados en pares. La lógica de colisiones se modifica para permitir que los miembros de un mismo equipo se atraviesen entre sí sin penalización, fomentando la cooperación estratégica.

## Configuración del Entorno
El despliegue del proyecto requiere la configuración de variables de entorno para la gestión de red y rutas de salida de datos. Estas deben definirse en un archivo `.env` ubicado en la raíz del ejecutable.

### Variables Requeridas

```env
# Configuración de Red
IP=127.0.0.1
PORT=8080

# Configuración de Análisis de Datos
STATS_OUTPUT_DIR=./estadisticas_finales
```
# Módulo de Análisis de Datos y Métricas - Super Snake

## Descripción
Este módulo es un componente independiente desarrollado en Python, diseñado para procesar los registros de actividad (logs) generados por el servidor de juego Super Snake. Su función principal es transformar los datos crudos de las partidas en visualizaciones estadísticas que permitan evaluar el balance del juego, el comportamiento de los jugadores y el rendimiento de las diferentes modalidades.

## Requisitos del Sistema

El script de análisis requiere un entorno de Python 3.x con las siguientes librerías instaladas:

* **pandas**: Para la manipulación y limpieza de estructuras de datos.
* **matplotlib**: Para la generación de gráficos base.
* **seaborn**: Para la visualización estadística avanzada.
* **python-dotenv**: Para la gestión de configuración mediante variables de entorno.

### Instalación de Dependencias
Puede instalar todas las dependencias necesarias ejecutando el siguiente comando:

```bash
pip install pandas matplotlib seaborn python-dotenv
```
