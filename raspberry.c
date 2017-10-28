#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>
using namespace std;

#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

char currentPattern = 'A';

int config_serial(char *device, unsigned int baudrate)
{
    struct termios options;
    int fd;

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        /*
        * Could not open the port.
        */

        perror("config_serial: Não pode abrir a serial - ");
        return -1;
    }

    fcntl(fd, F_SETFL, 0);

    /*
    * Get the current options for the port...
    */
    tcgetattr(fd, &options);

    /* sets the terminal to something like the "raw" mode */
    cfmakeraw(&options);

    /*
    * Set the baudrate...
    */
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    /*
    * Enable the receiver and set local mode...
    */
    options.c_cflag |= (CLOCAL | CREAD);

    /*
     * No parit, 1 stop bit, size 8
     */
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    /*
     * Clear old settings
     */
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* non-caninical mode */
    options.c_lflag &= ~ICANON;

    /*
    * Set the new options for the port...
    */
    tcsetattr(fd, TCSANOW, &options);

    /* configura a tty para escritas e leituras não bloqueantes */
    //fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    return fd;
}

static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to open export for writing!\n");
        return (-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return (0);
}

static int GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return (-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return (0);
}

static int GPIODirection(int pin, int dir)
{
    static const char s_directions_str[] = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return (-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
    {
        fprintf(stderr, "Failed to set direction!\n");
        return (-1);
    }

    close(fd);
    return (0);
}

static int GPIORead(int pin)
{
#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return (-1);
    }

    if (-1 == read(fd, value_str, 3))
    {
        fprintf(stderr, "Failed to read value!\n");
        return (-1);
    }

    close(fd);

    return (atoi(value_str));
}

static int GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return (-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
    {
        fprintf(stderr, "Failed to write value!\n");
        return (-1);
    }

    close(fd);
    return (0);
}

int writeOnSerial(char value)
{
    int fd;

    fd = config_serial("/dev/ttyAMA0", B9600);
    if (fd < 0)
        return 0;

    printf("ENVIANDO VALOR %c\n", value);
    write(fd, &value, 1);

    return 0;
}

int readFromSerial(char value) {
    int fd;

    fd = config_serial("/dev/ttyAMA0", B9600);
    if (fd < 0)
        return 0;

    read(fd, &value, 1);

    printf("Value read from serial: %c\n", value);

    return 0;
}

int readButton(int button) {
    GPIODirection(button, OUT);
    GPIOWrite(button, 1);
    GPIODirection(button, IN);
	return GPIORead(button);

}

int previousPattern() {
    
        if (currentPattern == 'A')
            currentPattern = 'C';
        else if (currentPattern == 'B')
            currentPattern = 'A';
        else if (currentPattern == 'C')
            currentPattern = 'B';
    
        return 0;
    }
    
    int nextPattern() {
    
        if (currentPattern == 'A')
            currentPattern = 'B';
        else if (currentPattern == 'B')
            currentPattern = 'C';
        else if (currentPattern == 'C')
            currentPattern = 'A';
    
        return 0;
    }

int main(int argc, char *argv[])
{
    int pushbutton25 = 25; // b1 esquedo
    int pushbutton24 = 24; // b2 meio
    int pushbutton23 = 23; // b3 direita

    do {
        if (readButton(pushbutton25) == 0) {
            printf("Previous sequence \n");
            previousPattern();
            writeOnSerial(currentPattern);
            while(readButton(pushbutton25) == 0);
        }

        if (readButton(pushbutton23) == 0) {
            printf("Next sequence \n");
            nextPattern();
            writeOnSerial(currentPattern);
            while(readButton(pushbutton23) == 0);
        }

        if (readButton(pushbutton24) == 0) {
            printf("Counter \n");
            char d = 'D';
            writeOnSerial(d);

            readFromSerial(d);
            printf("First sequence : %c\n", d);
            readFromSerial(d);
            printf("Second sequence : %c\n", d);
            readFromSerial(d);
            printf("Third sequence : %c\n", d);

            while(readButton(pushbutton24) == 0);
        }

        sleep(0.5);

    } while (1);

    /*
   * Disable GPIO pins
   */
    if (-1 == GPIOUnexport(pushbutton25))
        return (4);

    // if (-1 == GPIOUnexport(pushbutton24))
    //     return (4);

    if (-1 == GPIOUnexport(pushbutton23))
        return (4);

    return (0);
}