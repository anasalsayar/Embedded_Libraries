
# **nRF24L01+ Communication using SPI on STM32 (F411 Transmitter, F303 Receiver)**

This project demonstrates how to use **SPI communication** with **nRF24L01+ modules** between two STM32 boards:  
- **STM32F411 Discovery** as the **transmitter**  
- **Nucleo-F303** as the **receiver**

The **receiver** prints the received data using `printf` through **SWV (Serial Wire Viewer)** in STM32CubeIDE. Both boards communicate using SPI to transmit and receive messages.

---

## **Project Overview**

This project involves:

- **SPI** communication between two STM32 microcontrollers using nRF24L01+ modules.
- Printing **received data** on the SWV console in **STM32CubeIDE** using `printf`.
- Toggling an LED on the transmitter to confirm successful transmission.
- Using `printf` output redirected to **ITM_SendChar()** for SWV trace visualization on the **receiver**.

---

## **Hardware Requirements**

- **STM32F411 Discovery Board** (transmitter)
- **Nucleo-F303 Board** (receiver)
- **2 x nRF24L01+ Modules** (one for each STM32 board)
- Jumper wires for SPI and power connections

---

## **Connections for SPI and nRF24L01+ Modules**

| **nRF24L01+ Pin** | **STM32F411 (Transmitter)** | **STM32F303 (Receiver)** |
|-------------------|-----------------------------|--------------------------|
| **VCC**           | 3.3V                        | 3.3V                     |
| **GND**           | GND                         | GND                      |
| **CE**            | PA3                         | PA4                      |
| **CSN**           | PA4                         | PA5                      |
| **SCK**           | PA5                         | PB13                     |
| **MOSI**          | PA7                         | PB15                     |
| **MISO**          | PA6                         | PB14                     |

---

## **Software Setup**

### 1. **Enable SPI and SWV in STM32CubeMX**

1. Enable **SPI1** on both boards:
   - **SCK**: PA5 (on both)
   - **MOSI**: PA7 (on both)
   - **MISO**: PA6 (on both)
   - **NSS (CSN)**: PA4 (controlled in software)

2. Enable **SWD** on both boards for debugging.  
   - Route **SWO** to **PB3** on both STM32F411 and Nucleo-F303.

3. Configure **USART2** on both boards if needed for backup UART communication.

---

### 4. **`_write` Function for `printf` Redirection**

To use **`printf`** output with SWV (Serial Wire Viewer), implement the following function in **`main.c`** of the **receiver**:

int _write(int file, char *ptr, int len)
{
    (void)file;  // Ignore the 'file' argument
    for (int i = 0; i < len; i++)
    {
        ITM_SendChar(*ptr++);  // Send each character over SWV
    }
    return len;
}



## **Configuring SWV in STM32CubeIDE**

1. **Open Debug Configuration**:
   - Go to **Run > Debug Configurations**.
   - Select your debug profile (e.g., `ST-Link (ST-LINK GDB server)`).

2. **Enable SWV**:
   - In the **Debugger** tab, scroll to **SWO Configuration**.
   - Set the **Core Clock** (e.g., **72 MHz for F303** or **100 MHz for F411**).
   - Set **Baud Rate** to **115200**.

3. **Open the SWV Console**:
   - Go to **Window > Show View > SWV > SWV ITM Data Console**.
   - In the ITM console, click **Configure Trace** (wrench icon).
   - Enable **Port 0** for printf output.



## **Testing the Setup**

1. **Compile and Flash** the code to both the transmitter and receiver boards.
2. **Start Debugging** using STM32CubeIDE.
3. Open the **SWV ITM Data Console** to view the output.


## **Troubleshooting**

1. **No Output in SWV Console**:
   - Ensure **SWO** pin is configured correctly.
   - Check that **Port 0** is enabled in the SWV configuration.

2. **Data Not Received**:
   - Ensure both nRF24L01+ modules are on the same **channel** and **address**.
   - Verify SPI connections and ensure **CE/CSN pins** are handled correctly.

3. **Baud Rate Issues**:
   - Ensure the correct **Core Clock** is set in the SWV configuration (72 MHz for F303 and 100 MHz for F411).

---



## **How to Use This Project**

1. **Clone the Repository**:

   git clone https://github.com/your-username/stm32-nrf24-spi-swv.git

2. **Open the Project** in STM32CubeIDE.

3. **Build and Flash the Code** to the STM32 boards.

4. **Use SWV Console** to view the output on the **receiver**.


---

## **Author**

[Anas Alsayar](https://github.com/anasalsayar)

---

## **Acknowledgments**

- STM32 HAL documentation
- nRF24L01+ datasheet
- STM32CubeMX and STM32CubeIDE
- SWV and ITM documentation

---

This README ensures that all necessary steps and configurations are covered to print data via **SWV** using `printf` and **SPI communication** between the STM32 boards with the **nRF24L01+ modules**. Let me know if any more adjustments are needed!
