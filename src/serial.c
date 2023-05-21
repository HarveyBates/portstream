/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Harvey Bates
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/
#include "serial.h"

struct SerialDevice {
  char port[128];
  uint32_t baud_rate;
} serialDevice;

SerialSettings serialSettings;
int fd;
struct termios tty;

void serial_enable_timestamp(void) { serialSettings.with_timestamp = true; }
SerialSettings* get_serial_settings(void) { return &serialSettings; }

SerialStatus serial_set_port(const char* port) {
  int res;

  /* Port name is too long */
  if (strlen(port) > sizeof(serialDevice.port)) {
    return SERIAL_ALLOC_ERROR;
  }

  res = snprintf(serialDevice.port, sizeof(serialDevice.port), "%s", port);

  /* Encoding error */
  if (res < 0) {
    return SERIAL_ALLOC_ERROR;
  }

  return SERIAL_OK;
}

SerialStatus serial_set_baud_rate(const char* p_baud_rate) {
  char* ptr;
  uint32_t baud_rate;

  baud_rate = strtol(p_baud_rate, &ptr, 10);

  /* strtol() method returns zero if invalid conversion. If the end pointer
   * does not equal zero then the input was also invalid. */
  if (baud_rate == 0) {
    return SERIAL_INVALID_BAUD_RATE;
  }

  serialDevice.baud_rate = baud_rate;

  return SERIAL_OK;
}

int serial_set_serial_attr(void) {
  /* Setup incoming and outgoing baud rates */
  cfsetospeed(&tty, serialDevice.baud_rate);
  cfsetispeed(&tty, serialDevice.baud_rate);

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ECHONL;
  tty.c_lflag &= ~ISIG;

  /* Disable break processing */
  tty.c_iflag &= ~IGNBRK;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
  tty.c_iflag &= ~INPCK;

  /* No echo, signaling chars or canonical processing */
  tty.c_lflag |= ICANON | ISIG;
  tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

  /* No remapping or delays */
  tty.c_oflag = 0;

  /* Read and do not block (non-blocking) */
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 1; /* 100 ms timeout */

  /* Enable 8-bit characters */
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  /* Disable xon/xoff control and enable reading */
  tty.c_cflag |= (CLOCAL | CREAD);
  /* TODO: Make this an option */
  /* Disable parity */
  tty.c_cflag &= ~(PARENB | PARODD);
  /* Lower modem status lines after hangup */
  tty.c_cflag &= ~HUPCL;
  /* Ignore modem status lines */
  tty.c_cflag &= ~CLOCAL;
  /* Disable RTS/CTS control flow */
  tty.c_cflag &= ~CRTSCTS;

  /* Apply settings */
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    return SERIAL_ERROR;
  }

  return SERIAL_OK;
}

long get_time_ms(void) {
  int res;
  struct timespec spec;

  res = clock_gettime(CLOCK_MONOTONIC, &spec);
  if (res < 0) return 0;

  return spec.tv_nsec / (long)1000000;
}

const char* get_timestamp(struct tm* tm_info, char* fmt_time, uint8_t n_chars) {
  time_t raw_time = time(NULL);
  tm_info = localtime(&raw_time);
  strftime(fmt_time, n_chars, "%d/%m %H:%M:%S", tm_info);
  return fmt_time;
}

void serial_read(void) {
  do {
    long n_bytes;
    char fmt_time[20];
    char buf[128];
    struct tm* tm_info = NULL;

    get_timestamp(tm_info, fmt_time, sizeof(fmt_time));
    n_bytes = read(fd, buf, sizeof(buf) - 1);

    /* -1 is an error 0 is EOL */
    if (n_bytes <= 0) continue;

    buf[n_bytes] = 0;

    if (serialSettings.with_timestamp) {
      printf("[%s.%03ld]: %s", fmt_time, get_time_ms(), buf);
    } else {
      printf("%s", buf);
    }
  } while (1);

  exit(EXIT_FAILURE);
}

SerialStatus serial_connect(void) {
  fd = open(serialDevice.port, O_RDONLY | O_NOCTTY | O_NONBLOCK);

  if (fd < 0) {
    printf("Unable to open port: %s\n", serialDevice.port);
    return SERIAL_ERROR;
  }

  /* Required short sleep to clear the input buffer */
  /* see: stackoverflow.com/questions/13013387 */
  usleep(1000);
  tcflush(fd, TCIOFLUSH);

  if (serial_set_serial_attr() != SERIAL_OK) {
    printf("Unable to apply serial attributes.\n");
    return SERIAL_ERROR;
  }

  serial_read();

  return SERIAL_OK;
}

void serial_disconnect(int sig) {
  close(fd);
  exit(EXIT_FAILURE);
}
