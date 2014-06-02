/*
 * saveCal.ino - Save Calibration Values in EEPROM
 *
 * Requires EEPROMex library: https://github.com/thijse/Arduino-Libraries/tree/master/EEPROMEx
 * which is documented here: http://playground.arduino.cc/Code/EEPROMex
 *
 * Based on concepts from the Aruduino Playground sketch and comments by
 * Joghurt 2010 and Thijs Elenbaas 2012-07-30:
 * http://playground.arduino.cc/Code/EEPROMLoadAndSaveSettings
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
#include <EEPROMex.h>
#include "saveCal.h"


int getConfigAddr()
{
    EEPROM.setMemPool(BASE_ADDR, EEPROMSizeUno); // Assume Arduino UNO board
    return (EEPROM.getAddress(sizeof(struct CalStruct_s)));
}


void saveConfig(int addr, CalStruct_t *cal)
{
   cal->structVersion = CAL_STRUCT_VER;
   /*
   Serial.println(addr);
   Serial.println(cal->freq);
   Serial.print(cal->structVersion);
   */
   EEPROM.updateBlock(addr, *cal);
   return;
}


bool loadConfig(int addr, CalStruct_t *cal)
{
   EEPROM.readBlock(addr, *cal);
   //Serial.println(cal->freq);
   //Serial.println(cal->structVersion);
   return(cal->structVersion == CAL_STRUCT_VER);
}
