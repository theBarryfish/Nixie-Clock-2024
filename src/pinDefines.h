
#define LED_pin 2  // conencts to a chain of RGB leds on the front panel board and the analog board

#define UART2_RX 16  // from front panel USB via a DC isolated signal path
#define UART2_TX 17  // to front panel USB via a DC isolated signal path



/*
ESP32 GPIO port allocation information    GPIOs with - not readily available

0	– Pulled HIGH. Connected to BOOT Button

1   TX0 typ. connected to debug / uart

2   I/O Typ. Onboard LED

3   RX0 typ. connected to debug / uart

4   I/O

5   I/O  typ used for VSPI CS

6	–	Connected to SPI Flash IC
7   –	Connected to SPI Flash IC
8	–	Connected to SPI Flash IC
9   –	Connected to SPI Flash IC
10	–	Connected to SPI Flash IC
11  –	Connected to SPI Flash IC

12	HSPI MISO
13  HSPI MOSI
14  HSPI CLK

15  I/O

16	typ. and default UART2 RX
17  typ. and default UART2 TX

18	VSPI CLK
19  VSPI MISO

21	I2C SDA
22  I2C SCL

23  VSPI MOSI

25  I/O             //def. CANBUS to ESP

26  I/O             //def. CANBUS from ESP

27  I/O

32  I/O

33  I/O

34  Input

35  Input

36  Input

39  Input

*/