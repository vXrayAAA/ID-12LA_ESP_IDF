# ID-12LA


#RFID Reader Application#

Este é um codigo simples escrito em C que interage com um leitor RFID usando a interface UART em um microcontrolador ESP32.

Ele lê os dados do cartão RFID, calcula as somas de verificação e controla os LEDs com base nos dados específicos do cartão.


#Características#

*Inicializa a comunicação UART com o leitor RFID.

*Lê os dados do cartão RFID do UART.

*Verifica a soma de verificação dos dados recebidos.

*Controla os LEDs com base nos dados do cartão reconhecido.


#Configuração de hardware

*Conecte o pino TX2 (Transmitir) do ESP32 ao pino RX do leitor RFID.

*Conecte o pino RX2 (Receive) do ESP32 ao pino TX do leitor RFID.

*Conecte os pinos LED_RED e LED_GREEN aos LEDs correspondentes.

#Como funciona
*O aplicativo inicializa a comunicação UART com o leitor RFID usando UART2.

*A tarefa rfid_task é criada, que lê continuamente os dados do UART e os processa.

*Quando dados válidos são recebidos, o aplicativo extrai o número do cartão RFID e a soma de verificação dos dados.

*A soma de verificação calculada é comparada com a soma de verificação recebida para verificar a integridade dos dados.

*Se os dados e a soma de verificação coincidirem, o aplicativo verifica se o cartão é mestre e controla os LEDs de acordo.

*O codigo espera por um curto período e depois repete o processo.

#Executando o aplicativo
*Configure as conexões de hardware conforme descrito na seção "Configuração de hardware".

*Atualize o microcontrolador ESP32 com este aplicativo.

*Abra um terminal serial (por exemplo, PuTTY ou Tera Term) com uma taxa de transmissão de 9600 para monitorar a saída do aplicativo.

#Notas

*Este aplicativo demonstra a funcionalidade básica de leitura e processamento de dados de cartão RFID usando UART em um microcontrolador ESP32.

*Você pode estender o aplicativo para incluir recursos mais avançados, como armazenar dados de cartões autorizados em um banco de dados e implementar controle de acesso com base em diferentes tipos de cartões.
Certifique-se de ter a estrutura de desenvolvimento ESP-IDF instalada para compilar e atualizar este aplicativo no seu ESP32.
