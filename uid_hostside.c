// Thanks to: https://www.pololu.com/docs/0J73/15.6

 
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <math.h>

#define MAX_PROGRAM_SIZE    8192
int bcd_to_decimal(unsigned char x);
uint8_t decimal_to_bcd(int y);

void print_error(const char * context)
{
  DWORD error_code = GetLastError();
  char buffer[256];
  DWORD size = FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
    NULL, error_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
    buffer, sizeof(buffer), NULL);
  if (size == 0) { buffer[0] = 0; }
  fprintf(stderr, "%s: %s\n", context, buffer);
}
 
// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
HANDLE open_serial_port(const char * device, uint32_t baud_rate)
{
  HANDLE port = CreateFileA(device, GENERIC_READ | GENERIC_WRITE, 0, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (port == INVALID_HANDLE_VALUE)
  {
    print_error(device);
    return INVALID_HANDLE_VALUE;
  }
 
    if(SetupComm(port, 8192, 8192) == 0){
        print_error("UID_hostside > Failed to set port I/O buffer size!");
    }

  // Flush away any bytes previously read or written.
  BOOL success = FlushFileBuffers(port);
  if (!success)
  {
    print_error("UID_hostside > Failed to flush serial port!");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }
 
  // Configure read and write operations to time out after 100 ms.
  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutConstant = 5000;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 5000;
  timeouts.WriteTotalTimeoutMultiplier = 0;
 
  success = SetCommTimeouts(port, &timeouts);
  if (!success)
  {
    print_error("UID_hostside > Failed to set serial timeouts!");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }
 
  // Set the baud rate and other options.
  DCB state = {0};
  state.DCBlength = sizeof(DCB);
  state.BaudRate = baud_rate;
  state.ByteSize = 8;
  state.Parity = NOPARITY;
  state.StopBits = ONESTOPBIT;
  success = SetCommState(port, &state);
  if (!success)
  {
    print_error("UID_hostside > Failed to set serial settings");
    CloseHandle(port);
    return INVALID_HANDLE_VALUE;
  }
 
  return port;
}
 
// Writes bytes to the serial port, returning 0 on success and -1 on failure.
int write_port(HANDLE port, uint8_t * buffer, size_t size)
{
  DWORD written;
  BOOL success = WriteFile(port, buffer, size, &written, NULL);
  if (!success)
  {
    print_error("UID_hostside > Failed to write to port");
    return -1;
  }
  if (written != size)
  {
    print_error("UID_hostside > Failed to write all bytes to port");
    printf("UID_hostside > Only %d bytes written!\n", written);
    return -1;
  }
  return 0;
}
 
// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
SSIZE_T read_port(HANDLE port, uint8_t * buffer, size_t size)
{
  DWORD received;
  BOOL success = ReadFile(port, buffer, size, &received, NULL);
  if (!success)
  {
    print_error("UID_hostside > Failed to read from port");
    return -1;
  }
  return received;
}
 
int main(int argc, char ** argv){

    SYSTEMTIME t;
    int resultA, resultB;
    int num_inputs = 0;
    uint8_t byte_cntr;
    unsigned char uuid_data[10];
    char device[20] = "\\\\.\\";
    // ^---------COM ports higher than COM9 need the \\.\ prefix, 
    // which is written as "\\\\.\\" in C because we need to escape backslashes.  

    if(argc != 3){
        printf("UID_hostside > Usage: $ ./UID_hostside.exe COM<port_number> <baud_rate>\n");
        exit(-1);
    }else{
        printf("\nWelcome to the hostside test harness of the UID generation firmware.\n");
    }

    strcat(device, argv[1]);

    uint32_t baud_rate = atoi(argv[2]);

    printf("UID_hostside > Opening serial port (%s)...\n", argv[1]);
    HANDLE port = open_serial_port(device, baud_rate);
    if (port == INVALID_HANDLE_VALUE) { 
      printf("UID_hostside > Failed to open (%s)...\n", argv[1]);
      return 1; 
    }

    // Read incoming datetime from devboard_8051
    num_inputs = read_port(port, uuid_data, 16);
        
    if(num_inputs != 16){
        printf("UID_hostside > Read Datetime from devboard failed!\n");
        return(1);
    }else{
        printf("UID_hostside > The following Unique ID was generated by firmware on target:\n\n\t");
        printf("<");
        for(byte_cntr=0;byte_cntr<16;byte_cntr++){
            printf("%02x", uuid_data[byte_cntr]);
        }
        printf(">\n\nUID_hostside > Thanks Ripple...!");
    }        

    CloseHandle(port);
    return 0;
}