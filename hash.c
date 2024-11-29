#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 10

typedef struct MemoryBlock {
    char id[20];
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;

MemoryBlock* hashTable[TABLE_SIZE];

unsigned int hashFunction(const char* id) {
    unsigned int hash = 0;
    while (*id) {
        hash += *id++;
    }
    return hash % TABLE_SIZE;
}

void addBlock(const char* id, size_t size) {
    unsigned int index = hashFunction(id);
    MemoryBlock* newBlock = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!newBlock) {
        printf("Error: no se pudo asignar memoria.\n");
        return;
    }
    strcpy(newBlock->id, id);
    newBlock->size = size;
    newBlock->next = hashTable[index];
    hashTable[index] = newBlock;
    printf("Bloque '%s' de tamaño %zu agregado.\n", id, size);
}

MemoryBlock* findBlock(const char* id) {
    unsigned int index = hashFunction(id);
    MemoryBlock* current = hashTable[index];
    while (current) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void freeBlock(const char* id) {
    unsigned int index = hashFunction(id);
    MemoryBlock* current = hashTable[index];
    MemoryBlock* prev = NULL;

    while (current) {
        if (strcmp(current->id, id) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                hashTable[index] = current->next;
            }
            free(current);
            printf("Bloque '%s' liberado.\n", id);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Bloque '%s' no encontrado.\n", id);
}

void displayTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("[%d]: ", i);
        MemoryBlock* current = hashTable[i];
        while (current) {
            printf("-> {ID: %s, Size: %zu} ", current->id, current->size);
            current = current->next;
        }
        printf("\n");
    }
}

void measureTime(void (*func)(const char*, size_t), const char* id, size_t size) {
    clock_t start, end;
    start = clock();
    func(id, size);
    end = clock();
    printf("Tiempo de ejecución: %.6f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
}

void measureTimeFind(MemoryBlock* (*func)(const char*), const char* id) {
    clock_t start, end;
    start = clock();
    MemoryBlock* result = func(id);
    end = clock();
    if (result) {
        printf("Bloque encontrado: ID='%s', Tamaño=%zu\n", result->id, result->size);
    } else {
        printf("Bloque '%s' no encontrado.\n", id);
    }
    printf("Tiempo de ejecución: %.6f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
}

int main() {
    int choice;
    char id[20];
    size_t size;

    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    do {
        printf("\nSimulación de Tabla Hash - Administración de Memoria\n");
        printf("1. Agregar bloque\n");
        printf("2. Buscar bloque\n");
        printf("3. Liberar bloque\n");
        printf("4. Mostrar tabla\n");
        printf("5. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Ingrese ID del bloque: ");
                scanf("%s", id);
                printf("Ingrese tamaño del bloque: ");
                scanf("%zu", &size);
                measureTime(addBlock, id, size);
                break;
            case 2:
                printf("Ingrese ID del bloque: ");
                scanf("%s", id);
                measureTimeFind(findBlock, id);
                break;
            case 3:
                printf("Ingrese ID del bloque: ");
                scanf("%s", id);
                measureTime((void (*)(const char*, size_t))freeBlock, id, 0);
                break;
            case 4:
                displayTable();
                break;
            case 5:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción inválida.\n");
                break;
        }
    } while (choice != 5);

    return 0;
}
