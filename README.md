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

**Variables de Entorno:**
Para Administrador de Usuarios:
    USER_FILE: Especifica la ruta del archivo donde se almacenan los usuarios. <br/>
    PROFILE_FILE: Especifica la ruta del archivo donde se almacenan los perfiles y permisos. <br/>

Para Menú Principal:
    ADMIN_SYS: Ruta de la aplicación de administración de usuarios. <br/>
    MULTI_M: Ruta de la aplicación de multiplicación de matrices. <br/>
    CREATE_INDEX: Ruta de la aplicación para crear el índice invertido. <br/>
    CONTEO_TEXTO: Ruta de la aplicación de conteo de texto. <br/>
    INDICE_INVET_PARALELO: Ruta de la aplicación creación de un índice invertido paralelo. <br/>

Para Juego:
    SERVER_IP: IP del servidor al que el cliente ha de conectarse para jugar.
    SERVER_PORT: Número de puerto en el que el servidor está escuchando y aceptando conexiones.

**Juego**

**Compilar el Juego:**
Libreria necesaria: libsfml-dev.
Abre una terminal en la carpeta raíz del proyecto (donde está el `Makefile`) y ejecuta:

make all

**Ejecución Juego**

El juego utiliza una arquitectura cliente-servidor. Para jugar, primero debes compilar ambos programas y luego iniciar el servidor y los clientes.

Se puede jugar modo solo(todos contra todos) o modo duo (equipos de dos jugadores)

**Controles**
Se usan las flechas direcionales UP, DOWN, RIGHT, LEFT para el movimiento y ademas al mantener presionado espacio (SPACE) se acelera el movimiento del jugador.
