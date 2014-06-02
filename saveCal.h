/*
 * saveCal.h - Save Calibration Values in EEPROM
 *
 * Copyright 2014 Jerry Dunmire
 *
 * This file is part of MeasureFCI
 *
 *  MeasureFCI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  MeasureFCI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MeasureFCI.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAVECAL_H
#define SAVECAL_H

#define CAL_STRUCT_VER 2

#define BASE_ADDR 32  // Somewhat arbitrary in this case. Differnent values
                      // could be used to divide up the EEPROM for
                      // use by different applications.

typedef struct CalStruct_s {
    unsigned long freq;
    unsigned char structVersion;  // Set to CAL_STRUCT_VER by saveConfig()
} CalStruct_t;

int getConfigAddr(void);
void saveConfig(int addr, CalStruct_t *);
bool loadConfig(int addr, CalStruct_t *);

#endif
