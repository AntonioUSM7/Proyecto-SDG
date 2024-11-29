#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MEMORIA_TOTAL (1024 * 1024)  // 1 MB
#define MAXIMO_REQUEST 4096  // Tamaño máximo para solicitudes
#define HASH_SIZE 10

typedef struct T_hash {
    char* llave;
    size_t size;
    void* memory_block;
    struct T_hash* siguiente;
} T_hash;

typedef struct Bloque {
    size_t size;
    bool libre;
    struct Bloque* siguiente;
} Bloque;

size_t desperdicio_fijas = 0;
size_t desperdicio_first_fit = 0;
size_t desperdicio_buddy = 0;
double tiempo_fijas = 0;
double tiempo_first_fit = 0;
double tiempo_buddy = 0;
T_hash* Tabla_Hash[HASH_SIZE];
Bloque* lista_libre;
char memoria[MEMORIA_TOTAL];

double medir_tiempo(void (*func)(size_t), size_t parametro) {
    clock_t inicio = clock();
    func(parametro);
    clock_t fin = clock();
    return (double)(fin - inicio) / CLOCKS_PER_SEC;
}

void iniciar_tabla() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Tabla_Hash[i] = NULL;
    }
}

unsigned int funcion_hash(const char* llave) {
    unsigned int hash = 0;
    for (int i = 0; llave[i] != '\0'; i++) {
        hash = (hash + llave[i]) % HASH_SIZE;
    }
    return hash;
}

void asignar_tabla(const char* llave, size_t size) {
    unsigned int index = funcion_hash(llave);
    T_hash* nodo = (T_hash*)malloc(sizeof(T_hash));

    if (!nodo) {
        printf("Error: No se pudo asignar memoria.\n");
        return;
    }

    nodo->llave = strdup(llave);
    nodo->size = size;
    nodo->memory_block = malloc(size);

    if (!nodo->memory_block) {
        printf("Error: No se pudo asignar bloque de memoria.\n");
        free(nodo->llave);
        free(nodo);
        return;
    }

    nodo->siguiente = Tabla_Hash[index];
    Tabla_Hash[index] = nodo;
    printf("Memoria asignada con llave '%s' de %zu bytes.\n", llave, size);
}

void liberar_tabla(const char* llave) {
    unsigned int index = funcion_hash(llave);
    T_hash* actual = Tabla_Hash[index];
    T_hash* previo = NULL;

    while (actual) {
        if (strcmp(actual->llave, llave) == 0) {
            if (previo) {
                previo->siguiente = actual->siguiente;
            } else {
                Tabla_Hash[index] = actual->siguiente;
            }
            free(actual->memory_block);
            free(actual->llave);
            free(actual);
            printf("Memoria liberada con llave '%s'.\n", llave);
            return;
        }
        previo = actual;
        actual = actual->siguiente;
    }
    printf("Llave '%s' no encontrada.\n", llave);
}

void particiones_fijas(size_t block_size) {
    size_t bloques_totales = MEMORIA_TOTAL / block_size;
    size_t memoria_usada = 0, desperdicio = 0;

    for (size_t i = 0; i < bloques_totales; i++) {
        size_t solicitud = (rand() % (block_size)) + 1;
        if (solicitud > 0) {
            memoria_usada += solicitud;
            desperdicio += block_size - solicitud;
        }
    }

    desperdicio_fijas = desperdicio;
    printf("Particiones Fijas:\n");
    printf("Memoria usada: %zu bytes\n", memoria_usada);
    printf("Memoria perdida: %zu bytes\n", desperdicio);
}

void inicializar_lista_libre() {
    lista_libre = (Bloque*)malloc(sizeof(Bloque));
    lista_libre->size = MEMORIA_TOTAL;
    lista_libre->libre = true;
    lista_libre->siguiente = NULL;
}

void first_fit(size_t solicitud) {
    size_t desperdicio = 0;
    size_t bloques_totales = MEMORIA_TOTAL / MAXIMO_REQUEST;

    for (size_t i = 0; i < bloques_totales; i++) {
        if (rand() % 2 == 0) { // Simula asignaciones aleatorias
            if (MAXIMO_REQUEST >= solicitud) {
                desperdicio += MAXIMO_REQUEST - solicitud;
            }
        }
    }

    desperdicio_first_fit = desperdicio;
    printf("First Fit:\n");
    printf("Memoria perdida: %zu bytes\n", desperdicio);
}

void buddy_system(size_t solicitud) {
    size_t desperdicio = 0;
    size_t potencia = 1;
    while (potencia < solicitud) {
        potencia *= 2;
    }
    desperdicio = potencia - solicitud;
    desperdicio_buddy += desperdicio;

    printf("Buddy System:\n");
    printf("Memoria perdida: %zu bytes\n", desperdicio);
}

void comparar_resultados() {
    printf("\n--- Comparación de Métodos ---\n");

    printf("\n** Particiones Fijas **\n");
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo_fijas);
    printf("Memoria perdida: %zu bytes\n", desperdicio_fijas);

    printf("\n** First Fit **\n");
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo_first_fit);
    printf("Memoria perdida: %zu bytes\n", desperdicio_first_fit);

    printf("\n** Buddy System **\n");
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo_buddy);
    printf("Memoria perdida: %zu bytes\n", desperdicio_buddy);

    printf("\n--- Resultados Finales ---\n");
    printf("Método más rápido: ");
    if (tiempo_fijas <= tiempo_first_fit && tiempo_fijas <= tiempo_buddy) {
        printf("Particiones Fijas\n");
    } else if (tiempo_first_fit <= tiempo_buddy) {
        printf("First Fit\n");
    } else {
        printf("Buddy System\n");
    }

    printf("Método con menor desperdicio de memoria: ");
    if (desperdicio_fijas <= desperdicio_first_fit && desperdicio_fijas <= desperdicio_buddy) {
        printf("Particiones Fijas\n");
    } else if (desperdicio_first_fit <= desperdicio_buddy) {
        printf("First Fit\n");
    } else {
        printf("Buddy System\n");
    }
}

void menu() {
    int opcion;
    size_t size;

    do {
        printf("\n--- Menú de Gestión de Memoria ---\n");
        printf("1. Usar Particiones Fijas\n");
        printf("2. Usar First Fit\n");
        printf("3. Usar Buddy System\n");
        printf("4. Comparar Resultados\n");
        printf("5. Salir\n");
        printf("Ingrese una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Ingrese el tamaño de bloque para particiones fijas (bytes): ");
                scanf("%zu", &size);
                tiempo_fijas = medir_tiempo(particiones_fijas, size);
                break;
            case 2:
                printf("Ingrese el tamaño de memoria a asignar (bytes): ");
                scanf("%zu", &size);
                tiempo_first_fit = medir_tiempo(first_fit, size);
                break;
            case 3:
                printf("Ingrese el tamaño de memoria a asignar (bytes): ");
                scanf("%zu", &size);
                tiempo_buddy = medir_tiempo(buddy_system, size);
                break;
            case 4:
                comparar_resultados();
                break;
            case 5:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 5);
}

int main() {
    srand(time(NULL));
    menu();
    return 0;
}
