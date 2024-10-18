#include "bsp.h"
#include "modelos.h"  

bool sistema_encendido = false;  // Estado del sistema
float lectura_adc[3];  // Almacena las lecturas de los 3 sensores (en voltios)
bool sensores_conectados[3] = {false, false, false};  // Estado de los sensores
const int boton_pin = 23;  
const int led_pin = 2;  
const int sensores_pines[3] = {32, 33, 34};  // Pines de los sensores
int contador_presiones = 0;  // Contador de presiones del botón

const float V_REF = 3.3;  // Referencia de voltaje ADC de la ESP32
const float ESCALA_VOLT = 5.0 / V_REF;  // Escala para convertir de 3.3V a 5V
const float UMBRAL_SENSOR_CONECTADO = 0.65;  // Umbral para determinar si un sensor está conectado

TaskHandle_t sensor1Handle = NULL;
TaskHandle_t sensor2Handle = NULL;
TaskHandle_t sensor3Handle = NULL;
TaskHandle_t impresionHandle = NULL;

// Prototipos de las tareas
void tareaBoton(void* param);
void tareaSensor1(void* param);
void tareaSensor2(void* param);
void tareaSensor3(void* param);
void tareaImpresion(void* param);

void setup() {
    #ifdef KNN
    knn_init();
   #endif

    modoPin(boton_pin, INPUT_PULLUP);
    modoPin(led_pin, OUTPUT);
    for (int i = 0; i < 3; i++) {
        modoPin(sensores_pines[i], INPUT);
    }

    initcom(9600);

    // Crear tarea del botón
    xTaskCreate(tareaBoton, "Boton Task", 2048, NULL, 1, NULL);

    // Crear tareas de los sensores e impresión (inicialmente suspendidas)
    xTaskCreate(tareaSensor1, "Sensor1 Task", 2048, NULL, 1, &sensor1Handle);
    vTaskSuspend(sensor1Handle);

    xTaskCreate(tareaSensor2, "Sensor2 Task", 2048, NULL, 1, &sensor2Handle);
    vTaskSuspend(sensor2Handle);

    xTaskCreate(tareaSensor3, "Sensor3 Task", 2048, NULL, 1, &sensor3Handle);
    vTaskSuspend(sensor3Handle);

    xTaskCreate(tareaImpresion, "Impresion Task", 4096, NULL, 1, &impresionHandle);
    vTaskSuspend(impresionHandle);
}

void loop() {
    
}
char input;
// Tarea que maneja el botón 
void tareaBoton(void* param) {
    bool estado_boton_anterior = HIGH;
     bool sist = false;
    while (1) {
        bool estado_boton_actual = readpin(boton_pin);  // Leer estado actual del botón
        char buffer[50];
        if (estado_boton_anterior == HIGH && estado_boton_actual == LOW) {
            contador_presiones++;  // Incrementar el contador de presiones
            sistema_encendido = (contador_presiones % 2 != 0);  // Encender en presiones impares, apagar en pares
            writepin(led_pin, sistema_encendido);  // Actualizar LED

            if (sistema_encendido) {
                // Reanudar tareas de sensores e impresión
                vTaskResume(sensor1Handle);
                vTaskResume(sensor2Handle);
                vTaskResume(sensor3Handle);
                vTaskResume(impresionHandle);
                sist = true;
            } else {
                // Suspender tareas de sensores e impresión
                vTaskSuspend(sensor1Handle);
                vTaskSuspend(sensor2Handle);
                vTaskSuspend(sensor3Handle);
                vTaskSuspend(impresionHandle);
                sist = false;
            }
        }

        estado_boton_anterior = estado_boton_actual;
        if(!sist){
            if(Serial.available()){
                input = Serial.read();
                 sprintf(buffer, "0.0,0.0,0.0,0,");
                 imprimir(buffer);  // Imprime los valores de los sensores, separados por comas
            }    
        } 
        delaybsp(pdMS_TO_TICKS(100));
    }
}

// Tarea para el Sensor 1
void tareaSensor1(void* param) {
    while (1) {
        int16 valor_adc = getadc(sensores_pines[0]);
        float voltaje = (valor_adc * V_REF / 4095.0) * ESCALA_VOLT;
        if (voltaje >= UMBRAL_SENSOR_CONECTADO) {
            sensores_conectados[0] = true;
            lectura_adc[0] = 5 + ((voltaje - 0) / 5.0) * (40 - 5);  // Convertir voltaje a grados
        } else {
            sensores_conectados[0] = false;
            lectura_adc[0] = 0;  // Si no está conectado, se pone a 0
        }
        delaybsp(pdMS_TO_TICKS(1000));  
    }
}

// Tarea para el Sensor 2
void tareaSensor2(void* param) {
    while (1) {
        int16 valor_adc = getadc(sensores_pines[1]);
        float voltaje = (valor_adc * V_REF / 4095.0) * ESCALA_VOLT;
        if (voltaje >= UMBRAL_SENSOR_CONECTADO) {
            sensores_conectados[1] = true;
            lectura_adc[1] = 5 + ((voltaje - 0) / 5.0) * (40 - 5);  // Convertir voltaje a grados
        } else {
            sensores_conectados[1] = false;
            lectura_adc[1] = 0;  // Si no está conectado, se pone a 0
        }
        delaybsp(pdMS_TO_TICKS(1000));  
    }
}

// Tarea para el Sensor 3
void tareaSensor3(void* param) {
    while (1) {
        int16 valor_adc = getadc(sensores_pines[2]);
        float voltaje = (valor_adc * V_REF / 4095.0) * ESCALA_VOLT;
        if (voltaje >= UMBRAL_SENSOR_CONECTADO) {
            sensores_conectados[2] = true;
            lectura_adc[2] = 5 + ((voltaje - 0) / 5.0) * (40 - 5);  // Convertir voltaje a grados
        } else {
            sensores_conectados[2] = false;
            lectura_adc[2] = 0;  // Si no está conectado, se pone a 0
        }
        delaybsp(pdMS_TO_TICKS(1000));  
    }
}

// Tarea que imprime el estado de los sensores en grados y determina la estación
void tareaImpresion(void* param) {

        char buffer[100];
        
        // Determinar la estación basándose en las lecturas del ADC usando km
        int estacion; 
 
        while (1) {
        if(Serial.available()){
            estacion = determinar_estacion(lectura_adc);
            input = Serial.read();
            char buffer[50];
            sprintf(buffer, "%.1f,%.1f,%.1f,1,%d,", lectura_adc[0], lectura_adc[1], lectura_adc[2], estacion);  
            imprimir(buffer);  // Imprime los valores de los sensores, separados por comas
        }
    delaybsp(pdMS_TO_TICKS(100));
    }
}
