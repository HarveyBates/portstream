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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "serial.h"
#include "ui.h"

int main(int argc, char** argv) {
  int opt;
  char *port, *baud_rate;
  bool clean = false;
  SerialStatus set_port, set_br;
  SerialSettings* serialSettings;

  /* Exit cleanly on ctrl-c */
  signal(SIGINT, serial_disconnect);

  while ((opt = getopt(argc, argv, "p:b:tc")) != -1) {
    switch (opt) {
      case 'p': /* Set the serial port */
        set_port = serial_set_port(optarg);
        port = optarg;
        break;
      case 'b': /* Set the baud-rate */
        set_br = serial_set_baud_rate(optarg);
        baud_rate = optarg;
        break;
      case 't': /* Enable timestamps with messages */
        serial_enable_timestamp();
        break;
      case 'c': /* Clean UI, no information on startup */
        clean = true;
        break;
      case ':':
        printf("option req value\n");
        break;
      default:
        fprintf(stderr, "USAGE: %s [-ilw] [file...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (set_port != SERIAL_OK) {
    printf("Unable to connect to port: %s\n", port);
    exit(EXIT_FAILURE);
  }

  if (set_br != SERIAL_OK) {
    printf("Setup of baud-rate: %s failed.\n", baud_rate);
    exit(EXIT_FAILURE);
  }

  if (!clean) {
    serialSettings = get_serial_settings();
    ui_display_header(port, baud_rate, serialSettings);
  }

  serial_connect();

  return 0;
}
