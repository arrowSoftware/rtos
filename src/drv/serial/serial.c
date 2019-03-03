#include "arch/i386/sysI386Lib.h"
/* https://wiki.osdev.org/Serial_ports */
/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
 */

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    sysOutByte(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    sysOutByte(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    sysOutByte(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

/*
Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
Content: | d | b | prty  | s | dl  |

Name    Description
d       Enables (d = 1) or disables (d = 0) DLAB
b       If break control is enabled (b = 1) or disabled (b = 0)
prty    The number of parity bits to use
s       The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
dl      Describes the length of the data
*/
/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set to have a
 *  data length of 8 bits, no parity bits, one stop bit and break control
 *  disabled.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(unsigned short com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */
    sysOutByte(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/*
Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
Content: | lvl | bs | r | dma | clt | clr | e |

Name    Description
lvl     How many bytes should be stored in the FIFO buffers
bs      If the buffers should be 16 or 64 bytes large
r       Reserved for future use
dma     How the serial port data should be accessed
clt     Clear the transmission FIFO buffer
clr     Clear the receiver FIFO buffer
e       If the FIFO buffer should be enabled or not

We use the value 0xC7 = 11000111 that:
Enables FIFO
Clear both receiver and transmission FIFO queues
Use 14 bytes as size of queue
*/

/*
Configuring the Modem

The modem configuration byte is shown in the following figure:

Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |

Name    Description
r       Reserved
af      Autoflow control enabled
lb      Loopback mode (used for debugging serial ports)
ao2     Auxiliary output 2, used for receiving interrupts
ao1     Auxiliary output 1
rts     Ready To Transmit
dtr     Data Terminal Ready
We don’t need to enable interrupts, because we won’t handle any received data.
Therefore we use the configuration value 0x03 = 00000011 (RTS = 1 and DTS = 1).
*/

/** serial_is_transmit_fifo_empty:
*  Checks whether the transmit FIFO queue is empty or not for the given COM
*  port.
*
*  @param  com The COM port
*  @return 0 if the transmit FIFO queue is not empty
*          1 if the transmit FIFO queue is empty
*/
int serial_is_transmit_fifo_empty(unsigned int com)
{
   /* 0x20 = 0010 0000 */
   return sysInByte(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}


#define PORT 0x3f8   /* COM1 */

void serialSetBaud(short divisor)
{
    sysOutByte(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    sysOutByte(PORT + 0, 0x01);    // Set divisor to 3 (lo byte) 38400 baud
    sysOutByte(PORT + 1, 0x00);    //                  (hi byte)
}

int serialGetBaud()
{
    return sysInByte(PORT + 0);
}

void init_serial() {
   sysOutByte(PORT + 1, 0x00);    // Disable all interrupts
   sysOutByte(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   sysOutByte(PORT + 0, 0x01);    // Set divisor to 3 (lo byte) 38400 baud
   sysOutByte(PORT + 1, 0x00);    //                  (hi byte)
   sysOutByte(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   sysOutByte(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   sysOutByte(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_received()
{
   return sysInByte(PORT + 5) & 1;
}

char read_serial()
{
   while (serial_received() == 0);

   return sysInByte(PORT);
}

int is_transmit_empty()
{
   return sysInByte(PORT + 5) & 0x20;
}

void write_serial(char a)
{
   while (is_transmit_empty() == 0);

   sysOutByte(PORT,a);
}

void write_serial_str(char *buffer, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
    {
        write_serial(buffer[i]);
    }
}

void serial_test(void)
{
    short i = 0;
    for (i = 0; i < 127; i++)
    {
        write_serial(i);
    }
}
