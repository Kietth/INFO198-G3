# INFO198-G3 SISTEMA

Propósito de la aplicación:

Este es un sistema que reune múltiples utilidades en un único punto de acceso. 

Ejecución del Programa:

Para iniciar el sistema, sigue estos pasos:
Descomprimir el archivo: Extrae el contenido del archivo comprimido en una carpeta de tu preferencia.
Compilar el código: Abre una terminal en la carpeta principal del proyecto y ejecuta el Makefile con el comando: make.
Iniciar el programa: Una vez compilado, puedes iniciar la aplicación principal usando el siguiente comando, donde debes reemplazar [usuario] y [contraseña] con tus credenciales:
Bash
./menu_principal -u [usuario] -p [contraseña]

Este comando iniciará el menú principal desde donde podrás acceder a las distintas aplicaciones.

Módulos y Funcionalidades:

ADMINISTRADOR_DE_USUARIOS: Permite crear, eliminar y listar usuarios con diferentes perfiles y permisos.
MULTIPLICADOR_DE_MATRICES_NXN: Una herramienta para realizar la multiplicación de matrices de NXN.
JUEGO: (en construcción)
ES_PALINDROMO?: Una utilidad para verificar si una cadena de texto es un palíndromo.
CALCULAR_UNA_FUNCION: Un módulo para la evaluación de una funcion matematica predefinida.
CONTEO_DE_PALABRAS: Una herramienta que cuenta la frecuencia de las palabras en una cadena de texto.

Variables de Entorno

USER_FILE: Especifica la ruta del archivo donde se almacenan los usuarios.
PROFILE_FILE: Especifica la ruta del archivo donde se almacenan los perfiles y permisos.
