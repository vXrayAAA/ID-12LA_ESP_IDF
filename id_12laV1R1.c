#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_system.h"

 #define UART_NUM UART_NUM_2        //NUNCA DEIXAR EM '0' E EM 'MAX'
#define TXD_PIN GPIO_NUM_17             //Pino TX2  -- D1
#define RXD_PIN GPIO_NUM_16             //Pino RX2  -- D0
#define LED_RED GPIO_NUM_2              //Pino do LED VERMELHO
#define LED_GREEN GPIO_NUM_5            //Pino do LED VERDE
#define BUF_SIZE 128                    // tamanho da memoria

#define cartaoMestre1 {0x01, 0x0B, 0x76, 0x3A, 0xB4}
#define cartaoMestre2 {0x01, 0x0B, 0x76, 0x41, 0xE8}

#define CONFIG_FREERTOS_HZ 100

static void uart_init(void)         //inicializar a UART
{
    printf("Inicializado UART...\n");

    uart_config_t uart_config =         //Config da UART 
    {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    uart_param_config(UART_NUM, &uart_config);      //aplicar configuracao
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);       //definir pinos da UART
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);                             //instalando driver da UART

    printf("UART inicializada.\n");
}

static uint8_t calcular_checksum(const uint8_t *data, size_t len)       //funcao para calcular o checksum
{
    uint8_t checksum = 0;       //Var. para fazer o calculo 
    for (size_t i = 0; i < len; i++)            //Esta parte do código é um loop for que percorre os elementos do array data com base no tamanho len
    {
        checksum ^= data[i];         //Essa operação de XOR é usada para calcular o checksum dos dados.

    }
    
    return checksum;        // é retornado como resultado da função calcular_checksum.

}

int ascii_to_int_hex(char ascii_char)       //conversao de ascii para inteiro
{
    int __numero = -1;        //var para armazenar valores convertidos

    if (ascii_char >= '0' && ascii_char <= '9') //se estiver no intervalo de '0' a '9'
    {
        __numero = ascii_char - 0x30;       //subtrai o valor ASCII de '0' (que é 0x30 em hexadecimal) do valor ASCII do caractere.
    } else if (ascii_char >= 'A' && ascii_char <= 'F') //se estiver no intervalo de 'A' a 'F'
    {
        __numero = 10 + (ascii_char - 0x41);        //valor numérico equivalente é calculado adicionando-se 10 ao resultado da subtração do valor ASCII de 'A' (que é 0x41 em hexadecimal) do valor ASCII do caractere.
    }

    return __numero;
}


// const uint8_t tag_autorizada[][5]=
// {
//     {0x01, 0x0B, 0x75, 0xFC, 0xDD}

// };

// const size_t numero_tags_autorizadas = sizeof(tag_autorizada) / sizeof(tag_autorizada[0]);


// bool TagAuto(const uint8_t *tag)
// {
//     for(size_t i = 0; i < numero_tags_autorizadas; i++)
//     {
//             if (memcmp(tag, tag_autorizada[i], 5) == 0)
//             return true;

//     }
//     return false;

// }

static void rfid_task(void *pvParameters)           // Tarefa para ler dados do RFID
{
    uint8_t data[BUF_SIZE];         //array para armazenar os dados
    printf("iniciando a task RFID...\n");
    while (1) 
    {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);               //ler bytes da UART especificada por UART_NUM. Os bytes lidos são armazenados no array data. O parâmetro BUF_SIZE define o tamanho máximo a ser lido, e o parâmetro 20 / portTICK_RATE_MS especifica o tempo máximo de espera para a leitura. O valor retornado pela função é armazenado na variável len.
        if (len > 0)        //se leu os bytes maior que 0... 
        {
            //   printf("Dados brutos: ");
            //   for (int i = 0; i < len; i++) 
            //   {
            //   printf("%02X ", data[i]);

            //   }
            //     printf("\n");

            data[len] = '\0';      //adiciona um caracter nulo, para transformar os bytes em string                             
            if(data[0] == 0x02 && data[13] == 0x0D)             //Esta linha verifica se os dados lidos começam com o byte de início (STX) representado por 0x02 e terminam com o byte de fim (ETX) representado por 0x03. Essa verificação é feita comparando os valores nos índices 0 e 11 do array data.
            {
                uint8_t ns[5];      //var de numero serial
                uint8_t receber_checksum;
                
                // sscanf((char*)&data[1],"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx", &ns[0], &ns[1], &ns[2], &ns[3], &ns[4], &receber_checksum );              //extração dos valores do array data em formato hexadecimal. Os valores extraídos são armazenados nos elementos do array numero_serial(ns) e na variável receber_checksum. A string de formatação "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx" especifica que serão extraídos 6 valores hexadecimais de 2 caracteres cada.        
                for(int i = 0; i < 6; i++)
                {
                    uint8_t nibble_alto = ascii_to_int_hex(data[1 + i * 2]);      //?:  Ele é usado para avaliar uma expressão e retornar um valor com base em uma condição.          //':':  Ele especifica o que será retornado se a condição não for verdadeira.         //conversao de ASCII para numeros inteiros
                    uint8_t nibble_baixo = ascii_to_int_hex(data[2 + i * 2]);
                    uint8_t valor = (nibble_alto << 4) | nibble_baixo;
                    ns[i] = valor;
                    receber_checksum = ns[5];
                }


                uint8_t calculo_check = calcular_checksum(ns, 5);       //checksum calculado

                 if (receber_checksum == calculo_check)     //verificacao de checksum recebido com o calculado
                 {
                    printf("RFID Data: %02X%02X%02X%02X%02X, Checksum: %02X\n", ns[0], ns[1], ns[2], ns[3], ns[4], receber_checksum);           //Se for igual o valor recebido e calculado...

                    uint8_t masterCard1[] = cartaoMestre1;
                    uint8_t masterCard2[] = cartaoMestre2;
                    if(memcmp(ns, masterCard1, 5) == 0)
                    {  
                        printf("Cartão mestre registrado!\n");
                        gpio_set_direction(LED_RED,GPIO_MODE_OUTPUT);
                        gpio_set_level(LED_RED,1); 

                    }
                    else
                    {
                        gpio_set_direction(LED_RED,GPIO_MODE_OUTPUT);
                        gpio_set_level(LED_RED,0);
                    }

                     if(memcmp(ns, masterCard2 , 5) == 0)
                    {  
                        printf("Cartão mestre negado!\n");
                        gpio_set_direction(LED_GREEN,GPIO_MODE_OUTPUT);
                        gpio_set_level(LED_GREEN,1); 

                    }
                    else
                    {
                        gpio_set_direction(LED_GREEN,GPIO_MODE_OUTPUT);
                        gpio_set_level(LED_GREEN,0);
                    }


                 }
                 else       //se nao, recebe msg de erro informando o valor calculado
                 {
                     printf("Checksum erro: recebido %02X, calculado: %02X\n",receber_checksum, calculo_check);
                 }
                 
                  
            }

            else
            {
                printf("Dados recebidos não correspondem ao formato esperado.\n");
            }
        }
     
        vTaskDelay(100 / portTICK_PERIOD_MS);        // Aguardar 100 ms antes de ler novamente
    }
}


void app_main(void) 
{
    uart_init();        //inicializacao do UART
    xTaskCreate(rfid_task, "rfid_task", 4096, NULL, 5, NULL);       //tarefa do RFID

    
}
