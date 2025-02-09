# Instruções de uso

Para uso do software, siga os seguintes passos:

- **1°:** clone o repositório para o seu computador.

    - Ao abrir o projeto com o **VSCode**, a extensão do **CMake** irá criar a pasta ``build`` para você com os arquivos de compilação.

    - Caso não seja gerada a pasta, crie uma pasta com nome `build` e execute o seguinte comando dentro da pasta criada:
        
        ``cmake ..``

        O comando acima irá criar os arquivos de compilação.
        
        > Você pode também seguir para o passo 2 sem executar o passo anterior

- **2°:** execute a compilação do firmware usando a extensão do ***Raspberry Pi Pico*** do ***VSCode***.

A partir daqui, seu firmware já está compilado e pronto para uso, e só depende de onde será usado.

## Execução na *BitDogLab*

Siga os passos a seguir para poder executar seu firmware na placa *BitDogLab*:

- **1°:** coloque o seu ***Raspberry*** em modo de ***bootsel***, clicando no botão branco na placa e reiniciando a mesma.

- **2°:** copie o arquivo `.uf2` que foi gerado na pasta `build` para o seu ***Raspberry*** (ele aparecerá como um armazenamento externo, um Pen-Drive com nome de RPI-RP2).

    - Após isso, o seu código já vai está rodando na sua plaquinha ***BitDogLab***.

- **3°:** Está pronto, dê os comandos para executar as funcionalidades clicando nas teclas do seu teclado.

## Instruções para uso do firmware

O firmware que o código gera, faz uso dos botões A e B da placa de desenvolvimento, presentes nos pinos 5 e 6 do **Raspberry Pi Pico W**, além da matriz de LEDs 5x5 e do LED RGB, presentes no pinos 7, 11, 12 e 13, respectivamente, e também do display **SSD1306** que faz uso do canal de comunicação I2C, presente nos pinos 14 (*SDA*) e 15 (*SCL*).

Esses botões fazem uso do recurso de *interrupção* da placa, fazendo com que o LED RGB acenda ou apague a **cor verde**, no caso de pressionar o **botão A**, ou acenda ou apague a **cor azul** no caso de pressionar o **botão B**.

Já a matriz de LEDS e display são controladas via UART, a partir de um monitor serial. Para usar, envie caracteres pelo monitor serial. Caracteres que são letras e números serão apresentados no display. Caso sejam enviados números, o caractere é mostrado na matriz de LEDs.

## Vídeo Ensaio

Clique em ***[link do video](https://drive.google.com/file/d/13nEDxBb7f5qj4OJI7Dda6RjyALVxjWtO/view?usp=sharing)*** para visualizar o vídeo ensaio do projeto.
