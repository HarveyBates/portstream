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

#include "ui.h"

void ui_draw_line_ascii(char character, uint8_t len) {
  uint8_t c;
  for (c = 0; c < len; c++) {
    printf("%c", character);
  }
}

void ui_display_header(const char* port, const char* baud_rate,
                       SerialSettings* serialSettings) {
  /* Timestamp information with incoming messages */
  char* timestamps = "disabled";
  if (serialSettings->with_timestamp) {
    timestamps = "enabled";
  }

  ui_draw_line_ascii('*', 34);
  printf(" PortStream ");
  ui_draw_line_ascii('*', 34);
  printf("\n* Port: %70s *\n", port);
  printf("* Baud-rate: %65s *\n", baud_rate);
  printf("* Timestamps: %64s *\n", timestamps);
  ui_draw_line_ascii('*', 80);
  printf("\n");
}
