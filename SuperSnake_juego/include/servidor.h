#ifndef SERVIDOR_H
#define SERVIDOR_H

// Includes del sistema y red
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

// Includes de C++ estándar
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cctype>

// Includes del proyecto
#include "common.h" 
#include "logica.h"      
#include "red_servidor.h"
#include "estado_servidor.h" 

#endif // SERVIDOR_H