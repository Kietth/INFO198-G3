# INFO198-G3 SISTEMA

**Propósito de la aplicación:**

Este es un sistema que reune múltiples utilidades en un único punto de acceso. 

**Ejecución del Programa:**

Para iniciar el sistema, sigue estos pasos:

1.Descomprimir el archivo: Extrae el contenido del archivo comprimido en una carpeta de tu preferencia.

2.Compilar el código: Abre una terminal en la carpeta principal del proyecto y ejecuta el Makefile con el comando: make menu.

3.Iniciar el programa: Una vez compilado, puedes iniciar la aplicación principal usando el siguiente comando, donde debes reemplazar [usuario] y [contraseña] con tus credenciales:

Bash
./menu_principal -u [usuario] -p [contraseña]

Este comando iniciará el menú principal desde donde podrás acceder a las distintas aplicaciones.

**Módulos y Funcionalidades:**

-ADMINISTRADOR DE USUARIOS: Permite crear, eliminar y listar usuarios con diferentes perfiles y permisos. <br/>
-MULTIPLICADOR DE MATRICES NXN: Una herramienta para realizar la multiplicación de matrices de NXN. <br/>
-JUEGO: (en construcción) <br/>
-ES PALINDROMO?: Una utilidad para verificar si una cadena de texto es un palíndromo. <br/>
-CALCULAR UNA FUNCION: Un módulo para la evaluación de una funcion matematica predefinida. <br/>
-CONTEO DE PALABRAS: Una herramienta que cuenta la frecuencia de las palabras en una cadena de texto.

**Variables de Entorno:**

USER_FILE: Especifica la ruta del archivo donde se almacenan los usuarios. <br/>
PROFILE_FILE: Especifica la ruta del archivo donde se almacenan los perfiles y permisos.
