#include "mbed.h"
#include "nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13);    // mosi, miso, sck, csn, ce, irq

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

// PTC1: motor 1 (esquerdo) - horario 
// PTC2: motor 1 (esquerdo) - antihorario
// PTB2: motor 2 (direito) - horario 
// PTB3: motor 2 (direito) - antihorario
DigitalOut motorLeftClk(PTC1);
DigitalOut motorLeftNClk(PTC2);
DigitalOut motorRightClk(PTB3);
DigitalOut motorRightNClk(PTB2);

DigitalIn encLeft(PTA12); // enc1
DigitalIn encRight(PTD4); // enc2

void stop(){
    motorLeftClk = 0;
    motorLeftNClk = 0;
    motorRightClk = 0;
    motorRightNClk = 0;
}

void fwd(){
    motorLeftClk = 1;
    motorLeftNClk = 0;
    motorRightClk = 1;
    motorRightNClk = 0;
}

void bkw(){
    motorLeftClk = 0;
    motorLeftNClk = 1;
    motorRightClk = 0;
    motorRightNClk = 1;
}

void left(){
    motorLeftClk = 1;
    motorLeftNClk = 0;
    motorRightClk = 0;
    motorRightNClk = 0;
}

void right(){
    motorLeftClk = 0;
    motorLeftNClk = 0;
    motorRightClk = 1;
    motorRightNClk = 0;
}

int main() {

// The nRF24L01+ supports transfers from 1 to 32 bytes, but Sparkfun's
//  "Nordic Serial Interface Board" (http://www.sparkfun.com/products/9019)
//  only handles 4 byte transfers in the ATMega code.
#define TRANSFER_SIZE 1

    char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;
    char lastRxData[1];

    my_nrf24l01p.powerUp();
    
    my_nrf24l01p.setRfFrequency(2450);
    my_nrf24l01p.setTxAddress(0x1A000000AA, 5);
    my_nrf24l01p.setRxAddress(0x1A000000AA, 5, 0);
    
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
        //pc.printf("\r%d\n", encLeft.read());
              
        // If we've received anything over the host serial link...
        if ( pc.readable() ) {

            // ...add it to the transmit buffer
            txData[txDataCnt++] = pc.getc();

            // If the transmit buffer is full
            if ( txDataCnt >= sizeof( txData ) ) {

                // Send the transmitbuffer via the nRF24L01+
                my_nrf24l01p.write( NRF24L01P_PIPE_P0, txData, txDataCnt );

                txDataCnt = 0;
            }

            // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
            //myled1 = !myled1;
        }

        // If we've received anything in the nRF24L01+...
        if ( my_nrf24l01p.readable() ) {
        //if ( pc.readable() ) {

            // ...read the data into the receive buffer
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );
            //rxData[0] = pc.getc();
            
            // acendendo LED verde baseado nos 2 ultimos comandos
            if ((lastRxData[0] == 'a') && (rxData[0] == 'c')){
                myled2 = 0;
            } else{
                myled2 = 1;
            }
            lastRxData[0] = rxData[0];
            
            // controle dos motores
            if (rxData[0] == 'w'){
                // ligar ambos sentido horario
                fwd();
            } else if (rxData[0] == 'a'){
                // ligar esquerdo sentido horario
                int lastPosLeft = encLeft.read();
                int rotCount = 0;
                left();
                while (rotCount < 20){
                    if(encLeft.read() != lastPosLeft){
                        lastPosLeft = encLeft.read();
                        rotCount++;
                    }
                    pc.printf("%d", rotCount);
                }
                stop();
            } else if (rxData[0] == 'd'){
                // ligar direito sentido horario
                right();
            } else if (rxData[0] == 's'){
                // parar ambos
                stop();
            }
            
            
            // Display the receive buffer contents via the host serial link
            for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {

                pc.putc( rxData[i] );
                
                
            }

            // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
            //myled2 = !myled2;
        }
    }
}
