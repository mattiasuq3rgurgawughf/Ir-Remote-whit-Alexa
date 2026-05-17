# Sistema de Control IR Inteligente con ESP32 y Alexa:
 
Proyecto universitario desarrollado como implementación práctica de automatización IoT utilizando ESP32, Alexa y Sinric Pro para el control remoto de dispositivos electrónicos mediante señales infrarrojas (IR) y conexión Wi-Fi.


# Descripción:

El proyecto permite controlar dispositivos como televisores, parlantes y aire acondicionado mediante comandos de voz utilizando Alexa.

El sistema emplea un microcontrolador ESP32 conectado a la plataforma Sinric Pro para recibir instrucciones desde internet y posteriormente transmitir códigos infrarrojos a los dispositivos electrónicos.
Además, se incorpora un sensor receptor TSOP1838 para capturar y decodificar señales IR provenientes de controles remotos reales, permitiendo almacenar y reutilizar códigos infrarrojos.
Para mejorar la potencia y alcance de transmisión, se utiliza un transistor 2N2222 como etapa de amplificación para el LED infrarrojo emisor.


# Componentes utilizados:

- ESP32
- Sensor infrarrojo TSOP1838
- LED emisor IR
- Transistor 2N2222
- Resistencias 1k,110 ohms
- Conexión Wi-Fi
- Amazon Alexa
- Sinric Pro


 # Funcionalidades:

* Control de dispositivos mediante comandos de voz  
* Captura y decodificación de señales infrarrojas  
* Transmisión de códigos IR  
* Integración IoT mediante Wi-Fi  
* Control de TV, parlante y aire acondicionado  
* Amplificación de señal IR usando transistor 2N2222  


# Protocolos IR implementados:

- NEC
- Panasonic
- COOLIX



# Tabla de códigos IR capturados:

Dispositivo  |  Función  | Protocolo  |  Código HEX   |    Bits     |

TV LG           Power        NEC         0x20DF10EF       32 Bits

TV LG            Vol +       NEC         0x20DF40BF       32 Bits

TV LG            Vol -       NEC         0x20DFC03F       32 Bits

TV LG            Mute        NEC         0x20DF906F       32 Bits

Parlante        Power      Panasonic   0x40040538BC81     48 Bits

Parlante         Vol +     Panasonic   0x400405000401     48 Bits

Parlante         Vol -     Panasonic   0x400405008481     48 Bits

Parlante        Mute       Panasonic   0x400405004C49     48 Bits

Parlante        Next       Panasonic   0x40040538615C     48 Bits

Parlante      Previous     Panasonic   0x40040538E1DC    48 Bits


#  Funcionamiento del sistema:

1. Alexa recibe el comando de voz del usuario.  
2. Sinric Pro envía la instrucción al ESP32 mediante internet.  
3. El ESP32 procesa la orden recibida.  
4. El transistor 2N2222 amplifica la señal infrarroja.  
5. El LED IR transmite el código correspondiente al dispositivo.  
6. El equipo responde como si se utilizara el control remoto original.  


#  Tecnologías utilizadas:

- Arduino IDE
- ESP32
- KiCad
- GitHub
- Sinric Pro
- IRremoteESP8266

# Conclusion:

Proyecto desarrollado por estudiante universitaria como aplicación práctica de automatización IoT y control inteligente mediante señales infrarrojas.
