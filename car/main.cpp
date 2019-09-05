#include "MotorDriver.h"
#include "EncoderDriver.h"
#include "UltrasonicDriver.h"
#include "mbed.h"
#include "nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13);    // mosi, miso, sck, csn, ce, irq

// PTC1: motor 1 (esquerdo) - horario 
// PTC2: motor 1 (esquerdo) - antihorario
// PTB2: motor 2 (direito) - horario 
// PTB3: motor 2 (direito) - antihorario
MotorDriver motor(PTC1, PTC2, PTB3, PTB2);

EncoderDriver encoderLeft(PTA12); // enc1
EncoderDriver encoderRight(PTD4); // enc2

UltrasonicDriver ultrasonic(PTA2, PTA1); // tx, rx

// LED for debugging
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

#define PULSES_90_DEG 18

void mapMode (char* rxData, int rxDataCnt) {
    myled1 = 0;
    int coordsLen = 4;
    char xCoord[coordsLen], yCoord[coordsLen];
    int xCnt = 0;
    int yCnt = 0;
    int x, y;
    // receber coordenadas
    while ((xCnt < coordsLen) & (yCnt < coordsLen)) {
        if ( my_nrf24l01p.readable() ) {
            // ...read the data into the receive buffer
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );
            // fill xCoord first then yCoord
            if (xCnt < coordsLen) {
                xCoord[xCnt] = rxData[0];
                xCnt++;
            }
            else if (yCnt < coordsLen){
                yCoord[yCnt] = rxData[0];
                yCnt++;
            }
        }
    }
    x = (int)xCoord;
    y = (int)yCoord;

    // mover na direcao y
    motor.fwd();
    encoderLeft.waitPulses(encoderLeft.distanceToPulses(y));
    motor.stop();

    //girar 90 graus no sentido horario
    motor.clk();
    encoderLeft.waitPulses(PULSES_90_DEG);
    motor.stop();

    // mover na direcao x
    motor.fwd();
    encoderLeft.waitPulses(encoderLeft.distanceToPulses(x));
    motor.stop();
    
    myled1 = 1;
}

int main() {
    #define TRANSFER_SIZE 1

    char rxData[TRANSFER_SIZE];
    int rxDataCnt = 0;

    my_nrf24l01p.powerUp();
    
    my_nrf24l01p.setRfFrequency(2510);
    my_nrf24l01p.setTxAddress(0x1A001000AA, 5);
    my_nrf24l01p.setRxAddress(0x1A001000AA, 5, 0);
    
    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );

    pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE );

    my_nrf24l01p.setTransferSize( TRANSFER_SIZE );

    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();

    myled1 = 1;
    myled2 = 1;

    while (1) {
        // Basic ultrasonic test
        if (ultrasonic.read() < 20){
            myled2 = 0;
        } else{
            myled2 = 1;
        }

        // If we've received anything in the nRF24L01+...
        if ( my_nrf24l01p.readable() ) {
            // ...read the data into the receive buffer
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );

            // map mode
            if (rxData[0] == 'M'){
                mapMode(rxData, rxDataCnt);
            }

            // controle dos motores
            else if (rxData[0] == 'w'){
                // ligar ambos sentido horario
                motor.fwd();
            }
            else if (rxData[0] == 'a'){
                // ligar esquerdo sentido horario
                motor.left();
                encoderLeft.waitPulses(20); 
                motor.stop();
            }
            else if (rxData[0] == 'd'){
                // ligar direito sentido horario
                motor.right();

            }
            else if (rxData[0] == 's'){
                // parar ambos
                motor.stop();
            }
            else if (rxData[0] == 'e'){
                // girar sentido horario
                motor.clk();
            }
            else if (rxData[0] == 'q'){
                // girar sentido antihorario
                motor.cclk();
            }
            

            // Display the receive buffer contents via the host serial link
            for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {
                pc.putc( rxData[i] );   
            }
        }
    }
}
