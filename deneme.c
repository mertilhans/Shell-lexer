#include <pthread.h>
#include <stdio.h>
#include <errno.h> // EBUSY için gerekli

void print_mutex_info(pthread_mutex_t *mutex) {
    printf("Mutex address: %p\n", (void*)mutex);
    
    unsigned char *bytes = (unsigned char *)mutex;
    printf("Mutex bytes: ");
    for (size_t i = 0; i < sizeof(pthread_mutex_t); i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

int main() {
    pthread_mutex_t mutex;
    
    // Mutex henüz başlatılmamış durum
    printf("Uninitialized mutex:\n");
    print_mutex_info(&mutex);
    
    // Mutex'i başlat
    pthread_mutex_init(&mutex, NULL);
    printf("\nInitialized mutex:\n");
    print_mutex_info(&mutex);
    
    // Mutex durumunu kontrol et
    printf("\nChecking mutex state:\n");
    int result = pthread_mutex_trylock(&mutex);
    printf("\nAfter locked mutex:\n");
    print_mutex_info(&mutex);
    pthread_mutex_unlock(&mutex); // Kilidi serbest bırak
    printf("\nAfter unlocked mutex:\n");
    print_mutex_info(&mutex);
    // Temizlik
    pthread_mutex_destroy(&mutex);
    printf("\nAfter destroy mutex:\n");
    print_mutex_info(&mutex);
    return 0;
}