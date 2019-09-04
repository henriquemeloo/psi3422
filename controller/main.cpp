#include "mbed.h"
#include "nRF24L01P.h"

// PC serial communication / tx, rx
Serial pc(USBTX, USBRX);

// NRF24L01P ports / mosi, miso, sck, csn, ce, irq
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13);

// LED for debugging
DigitalOut myled1(LED1);


int main() {
    #define TRANSFER_SIZE 1

    char txData[TRANSFER_SIZE];
    int txDataCnt = 0;

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

    while (1) {
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
            myled1 = !myled1;
        }
    }
}
