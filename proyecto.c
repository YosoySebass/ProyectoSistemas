#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

// --- CONSTANTES DEL LABORATORIO MATEMÁTICO
// Número de hilos de procesamiento paralelo
#define NUM_HILOS 4          

// Número total de cálculos matemáticos a ejecutar
#define NUM_CALCULOS 20   

// Límite para la simulación de cálculo matemático intensivo
#define LIMITE_CALCULO_MATEMATICO 50000000 

// --- VARIABLES GLOBALES Y SINCRONIZACIÓN 
// Mutex para proteger el resultado global compartido
pthread_mutex_t mut_resultado_global; 

// Resultado global acumulado de todos los cálculos matemáticos
// long long para evitar desbordamientos en cálculos grandes
long long resultado_global_acumulado = 0; 

/*
    Estructura para almacenar los datos de cada cálculo matemático
    Se utiliza para empaquetar los datos necesarios para cada hilo
    y pasar un puntero a través de pthread_create()
*/ 

// Estructura para almacenar los datos de cada cálculo matemático
typedef struct { 
    int id_calculo;               // ID del cálculo (1 a NUM_CALCULOS)
    long long valor_calculado;    // Resultado del cálculo matemático
    double tiempo_ms;             // Tiempo de ejecución en milisegundos
} CalculoMatematico;

// Array que contendrá todos los cálculos matemáticos
// Registra de forma individual los resultados de las 20 tareas
CalculoMatematico listado_calculos[NUM_CALCULOS]; 

// Estructura para pasar argumentos a cada hilo de cálculo
typedef struct { 
    int id_hilo;        // ID del hilo (1 a NUM_HILOS)
    int indice_inicio;  // Índice inicial del rango de cálculos
    int indice_fin;     // Índice final del rango de cálculos
} ArgumentosHilo; 

/*
  time_diff_ms      -> Calcula la diferencia de tiempo entre dos estructuras timespec en milisegundos.
  start             -> Tiempo de inicio de la operación matemática.
  end               -> Tiempo de fin de la operación matemática.
  struct timespec   -> Para mayor precisión en la medición del tiempo computacional.
*/
double time_diff_ms(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0 + 
           (end->tv_nsec - start->tv_nsec) / 1000000.0;
}

/*
    Función que cada hilo ejecutará para procesar su conjunto de cálculos matemáticos.
    Simula operaciones intensivas típicas de un laboratorio matemático.
*/
void *procesar_calculos_paralelo(void *arg) { 
    // Convertir el puntero void* de vuelta a la estructura ArgumentosHilo
    ArgumentosHilo *args = (ArgumentosHilo *)arg;
    int i;
    
    // Mostrar información del hilo y su rango de cálculos
    printf(" Hilo matemático %d iniciado, procesando cálculos desde ID: %d hasta ID: %d \n", 
        args->id_hilo, args->indice_inicio + 1, args->indice_fin + 1);

    // Procesar cada cálculo matemático en el rango asignado
    for (i = args->indice_inicio; i <= args->indice_fin; i++) {
        // Variables para medición precisa del tiempo computacional
        struct timespec inicio_calculo, fin_calculo;
        long long resultado_parcial = 0;
        
        // Inicio del cálculo matemático intensivo
        clock_gettime(CLOCK_MONOTONIC, &inicio_calculo);
        
        // Simula un cálculo matemático intensivo típico de laboratorio
        // Operaciones con funciones trigonométricas y logarítmicas
        double valor = 0.0;
        for (long long j = 1; j <= LIMITE_CALCULO_MATEMATICO; j++) {
            // Cálculo intensivo: combinación de seno, coseno y logaritmo
            valor += sin((double)j) * cos((double)j) + log((double)j); 
        }
        // Convertir resultado a formato entero para acumulación
        resultado_parcial = (long long)valor;

        // Fin del cálculo matemático
        clock_gettime(CLOCK_MONOTONIC, &fin_calculo);

        // Almacenar el resultado y el tiempo del cálculo
        listado_calculos[i].valor_calculado = resultado_parcial;
        listado_calculos[i].tiempo_ms = time_diff_ms(&inicio_calculo, &fin_calculo);

        // --- SINCRONIZACIÓN PARA ACCESO AL RESULTADO GLOBAL
        // Mutex protege el acceso concurrente a 'resultado_global_acumulado'
        pthread_mutex_lock(&mut_resultado_global);
        
        // Actualizar el resultado global compartido del laboratorio
        resultado_global_acumulado += resultado_parcial;

        // Mostrar el resultado del cálculo individual
        printf(" Hilo %d -> Cálculo matemático ID %d completado. Tiempo: %.3f ms\n", 
            args->id_hilo, listado_calculos[i].id_calculo, listado_calculos[i].tiempo_ms);
        
        // Liberar el mutex para permitir acceso a otros hilos
        pthread_mutex_unlock(&mut_resultado_global);
    }
    
    printf("Hilo matemático %d finalizado.\n", args->id_hilo);
    pthread_exit(NULL);
}


// --- FUNCIÓN PRINCIPAL DEL LABORATORIO
int main() {
    
    pthread_t hilos_calculo[NUM_HILOS];
    ArgumentosHilo args_calculo[NUM_HILOS]; 
    struct timespec inicio_procesamiento_total, fin_procesamiento_total;
    double tiempo_total_paralelo;
    int i;

    printf("--- Simulador de Laboratorio Matemático - Procesamiento Paralelo ---\n");
    printf(" Cálculos Matemáticos Totales: %d | Hilos de Procesamiento: %d\n", NUM_CALCULOS, NUM_HILOS);
    
    // Inicializar array de cálculos matemáticos
    for (i = 0; i < NUM_CALCULOS; i++) {
        listado_calculos[i].id_calculo = i + 1;
        listado_calculos[i].valor_calculado = 0;
        listado_calculos[i].tiempo_ms = 0.0;
    }

    // Inicializar Mutex para sincronización
    if (pthread_mutex_init(&mut_resultado_global, NULL) != 0) {
        perror(" ---> Error al inicializar mutex de sincronización");
        return 1;
    }

    // Inicio del procesamiento paralelo total
    clock_gettime(CLOCK_MONOTONIC, &inicio_procesamiento_total);

    // Creación y distribución de hilos de cálculo
    int calculos_por_hilo = NUM_CALCULOS / NUM_HILOS;
    int calculos_sobrantes = NUM_CALCULOS % NUM_HILOS;
    int indice_actual = 0;

    for (i = 0; i < NUM_HILOS; i++) {
        // Calcular la carga computacional para este hilo
        // Distribución equitativa de cálculos sobrantes
        int carga_computacional = calculos_por_hilo + (i < calculos_sobrantes ? 1 : 0);
        
        args_calculo[i].id_hilo = i + 1; // ID identificador del hilo
        
        // Índice inicial en el array de cálculos
        args_calculo[i].indice_inicio = indice_actual; 

        // Índice final en el array de cálculos
        args_calculo[i].indice_fin = indice_actual + carga_computacional - 1; 

        // Crear hilo para procesamiento paralelo de cálculos
        /*
            hilos_calculo[i]           -> almacenará el identificador del hilo
            NULL                       -> atributos por defecto del hilo
            procesar_calculos_paralelo -> función de cálculo matemático intensivo
            &args_calculo[i]           -> argumentos con el rango de cálculos a procesar
        */
        if (pthread_create(&hilos_calculo[i], NULL, procesar_calculos_paralelo, &args_calculo[i]) != 0) {
            perror(" ---> Error al crear hilo de cálculo");
            return 1;
        }

        // Actualizar índice para el siguiente hilo
        indice_actual += carga_computacional;
    }

    // Esperar a que todos los hilos matemáticos terminen su procesamiento
    for (i = 0; i < NUM_HILOS; i++) {
        if (pthread_join(hilos_calculo[i], NULL) != 0) {
            perror(" ---> Error al esperar finalización de hilo");
            return 1;
        }
    }
    
    // Fin del procesamiento paralelo total
    clock_gettime(CLOCK_MONOTONIC, &fin_procesamiento_total);

    // Calcular tiempo total de ejecución en paralelo
    tiempo_total_paralelo = time_diff_ms(&inicio_procesamiento_total, &fin_procesamiento_total);

    printf("\n--- RESULTADOS DEL LABORATORIO MATEMÁTICO ---\n");
    printf("Tiempo total de procesamiento paralelo: %.3f ms\n", tiempo_total_paralelo);
    printf("Resultado global acumulado: %lld\n", resultado_global_acumulado);

    // Análisis de Rendimiento Computacional
    printf("\n ------------------------------------------------------\n");
    printf("|    Análisis de Tiempos de Ejecución de Cálculos      |\n");
    printf(" ------------------------------------------------------ \n");
    
    // Variables para análisis de rendimiento
    double tiempo_total_secuencial_teorico = 0.0;
    double tiempo_maximo_calculo = 0.0;
    
    for (i = 0; i < NUM_CALCULOS; i++) {
        /*
        Recorre el array de cálculos para analizar tiempos individuales
        y calcular el tiempo secuencial teórico para comparación
        */ 
        tiempo_total_secuencial_teorico += listado_calculos[i].tiempo_ms;
        if (listado_calculos[i].tiempo_ms > tiempo_maximo_calculo) {
            tiempo_maximo_calculo = listado_calculos[i].tiempo_ms;
        }
    }
    
    printf("\n ------------------------------------------------------\n");
    printf("| Tiempo secuencial teórico: %.3f ms\n", tiempo_total_secuencial_teorico);
    printf("| Tiempo promedio por cálculo: %.3f ms\n", tiempo_total_secuencial_teorico / NUM_CALCULOS);
    printf("| Tiempo máximo de un cálculo: %.3f ms\n", tiempo_maximo_calculo);
    printf(" ------------------------------------------------------\n");

    // Liberar recursos de sincronización
    pthread_mutex_destroy(&mut_resultado_global);

    return 0;
}
