#include "MotorDriver.h"
#include "EncoderDriver.h"
#include "UltrasonicDriver.h"
#include "mbed.h"
#include "nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx

// LED for debugging
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

#define TRANSFER_SIZE 1
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13);    // mosi, miso, sck, csn, ce, irq

// PTC1: motor 1 (esquerdo) - horario 
// PTC2: motor 1 (esquerdo) - antihorario
// PTB2: motor 2 (direito) - horario 
// PTB3: motor 2 (direito) - antihorario
MotorDriver motor(PTC1, PTC2, PTB3, PTB2);

#define PULSES_90_DEG 19
EncoderDriver encoderLeft(PTA12); // enc1
EncoderDriver encoderRight(PTD4); // enc2

void avoidObstacle();
void dist(int distance) {
    //put code here to happen when the distance is changed
    if (distance < 150){
        myled2 = 0;
        avoidObstacle();
    } else {
        myled2 = 1;
    }
}
// tx, rx, update interval, timeout, method when distance changed
UltrasonicDriver ultrasonic(PTA2, PTA1, .1, 1, &dist);

void spin(bool clockwise) {
    if (clockwise){
        motor.clk();
    }
    else {
        motor.cclk();
    }
    encoderRight.waitPulses(PULSES_90_DEG);
    motor.stop();
}

void runDistance(int distance) {
    int pulses = encoderRight.distanceToPulses(distance);
    int lastEncoderSignal = encoderRight.encoder->read();
    int pulsesCount = 0;

    motor.fwd();
    while (pulsesCount < pulses) {
        ultrasonic.checkDistance();
        wait_ms(30);
        if(encoderRight.encoder->read() != lastEncoderSignal){
            lastEncoderSignal = encoderRight.encoder->read();
            pulsesCount++;
        }
    }
    motor.stop();
}

void avoidObstacle() {
    int avoidX = 61; // run 61 cm to the left to avoid obstacle
    int avoidY = 50; // run 50 cm forward to avoid obstacle

    // spin clockwise 90 deg
    spin(1);
    // run right
    runDistance(avoidX);
    // spin counter-clockwise 90 deg
    spin(0);
    // run forward
    runDistance(avoidY);
    // spin counter-clockwise 90 deg
    spin(0);
    // run left
    runDistance(avoidX);
    // spin clockwise 90 deg
    spin(1);
}

void mapMode (char* rxData, int rxDataCnt) {
    myled1 = 0;
    int coordsLen = 3;
    pc.printf(
        "Map mode =======\r\n\tType x coordinate in cm with %d digits followed by y coordinate in cm with %d digits\r\n",
        coordsLen, coordsLen
    );
    char xCoord[coordsLen], yCoord[coordsLen];
    int xCnt = 0;
    int yCnt = 0;
    int x, y;
    // receber coordenadas
    while ((xCnt < coordsLen) || (yCnt < coordsLen)) {
        if ( my_nrf24l01p.readable() ) {
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
    
    sscanf(xCoord, "%d", &x);
    sscanf(yCoord, "%d", &y);
    pc.printf("(x, y) = (%d, %d) \r\n", x, y);

    // mover na direcao y
    motor.fwd();
    encoderRight.waitPulses(encoderRight.distanceToPulses(y));
    motor.stop();

    //girar 90 graus no sentido horario
    motor.clk();
    encoderRight.waitPulses(PULSES_90_DEG);
    motor.stop();

    // mover na direcao x
    motor.fwd();
    encoderRight.waitPulses(encoderRight.distanceToPulses(x));
    motor.stop();
    
    myled1 = 1;
}


int main() {
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

    ultrasonic.startUpdates(); //start mesuring the distance

    while (1) {
        // call checkDistance() as much as possible, as this is where
        // the class checks if dist needs to be called.

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
                runDistance(30);
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
            else if (rxData[0] == 'o'){
                // testar desvio de obstaculo
                avoidObstacle();
            }
            else if (rxData[0] == 'c'){
                spin(1);
            }
            else if (rxData[0] == 'z'){
                spin(0);
            }
            
            // Display the receive buffer contents via the host serial link
            for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {
                pc.putc( rxData[i] );   
            }
        }
    }
}
