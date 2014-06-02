/*
 * MeasureFCI - Measurement Frequency, Capacitance, and Inductance
 * 
 * Requires lcmeter_nui shield
 *
 * Requires EEPROMex library: https://github.com/thijse/Arduino-Libraries/tree/master/EEPROMEx
 * which is documented here: http://playground.arduino.cc/Code/EEPROMex
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
#include <FreqCount.h>
#include <EEPROMex.h>
#include "saveCal.h"

// fcal is a configuration structure saved in EEPROM
CalStruct_t fcal = {0, 0};

// configAddr will be initialied in setup() with the address in EEPROM
// where fcal is saved.
static int configAddr;

static const int delay_tic = 50;   // in ms

/*
 * Mode 1 = capacitance/calibrate
 * Mode 0 = inductance 
 */
#define MODE_PIN 2
#define MODE_CAPACITANCE 1
#define MODE_INDUCTANCE 0

#define PUSH_BUTTON_PIN 7


struct measurement_s {
   int mode;  // 1 = Capacitance, 0 = Inductance
   int pb;    // 0 = pressed
   unsigned long freq;
   float fratio;
   float value;
   float c;
   float l;
} measurement;

struct ref_s {
   unsigned long freq;
   float c;
   float l;
} ref;
    
#define NUM_PREFIXES 5
static const float engScale[NUM_PREFIXES] = {1.0, 1.0e3, 1.0e6, 1.0e9, 1.0e12};
static const char *engPrefix[NUM_PREFIXES] = {"", "m", "u", "n", "p"};

void print_eng(float unscaled, char *units)
{
    float scaled;
    int range;
    for (range = 0; range < NUM_PREFIXES; range++)
    {
        scaled = unscaled * engScale[range];
        if (scaled > 1)
        {
            if (scaled >= 10)
            {
                Serial.print(scaled,1);
            }
            else if (scaled >= 100)
            {
                Serial.print(scaled,0);
            }
            else
            {
                Serial.print(scaled,2);
            }
            Serial.print(", ");
            Serial.print(engPrefix[range]);
            Serial.print(units);
            return;
        }
    }
    Serial.print("0");
    Serial.print(", ");
    Serial.print(engPrefix[range]);
    Serial.print(units);
    return;

} // End of print_eng()


void dumpVars() {
    Serial.print(ref.c * 1e12);
    Serial.print(" pF, ");
    Serial.print(ref.l * 1e6);
    Serial.print(" uH, ");
    Serial.print(ref.freq);
    Serial.print(" Hz, ");
    Serial.print(measurement.mode);
    Serial.print(", ");
    Serial.print(measurement.pb);
    Serial.print(", ");
    Serial.print(measurement.freq);
    Serial.print(", ");
    Serial.print(100.0 * measurement.freq / ref.freq);
    Serial.print("% ");
    print_eng(fabs(measurement.c), "F");
    Serial.print(", ");
    print_eng(fabs(measurement.l), "H");
    Serial.print(", ");
    Serial.println("");
}


void printMeas() {
    Serial.print(measurement.freq);
    Serial.print(", Hz, ");
    if (measurement.mode == MODE_CAPACITANCE)
    {
       print_eng(fabs(measurement.c), "F");
    }
    else
    {
       print_eng(fabs(measurement.l), "H");
    }
    Serial.println();
}


bool calibrate()
{
    int requiredSamples = 4;
    unsigned long freqSum = 0;
    unsigned long freqOld = ref.freq;

    // Component tolerance stack is +/- 6% so frequencies
    // that differ from the nominal value by more than that indicate a
    // problem.
    unsigned long freq2low = (ref.freq * 94)/100;
    unsigned long freq2high = (ref.freq * 106)/100;

    Serial.print("Calibrating .");
    Serial.flush();

    // Throw out first measurement in case it is a short cycle
    while(FreqCount.available() == false)
    {
        delay(delay_tic);
    }
    (void)FreqCount.read();
    Serial.print(".");
    Serial.flush();

    // average four samples
    for (int count = 0; count < requiredSamples; count++)
    {
        unsigned long freq;
        while(FreqCount.available() == false)
        {
            delay(delay_tic);
        }
        freq = FreqCount.read();
        if ((freq < freq2low) || (freq > freq2high))
        {
            return(false);
        }

        freqSum += freq;
        Serial.print(".");
        Serial.flush();
    }
    ref.freq = freqSum / requiredSamples;
    //Serial.println("save config");
    fcal.freq = ref.freq;
    saveConfig(configAddr, &fcal);

    Serial.println(" DONE");
    Serial.print("New Ref Freq = ");
    Serial.print(ref.freq);
    Serial.println(" Hz");
    Serial.print("Old Ref Freq = ");
    Serial.print(freqOld);
    Serial.println(" Hz");
    Serial.print("Nom L/C Freq = ");
    Serial.print(1.0 / (2.0 * PI * sqrt(ref.c * ref.l)), 0);
    Serial.println(" Hz");

    return(true);

}  // End of calibrate()


void waitForPB()
{
    // Wait for the button to be released
    // This is a crude debounce to make sure that we don't re-detect the
    // last button press.
    while (digitalRead(PUSH_BUTTON_PIN) == 0)
    {
        delay(delay_tic);
    }

    // Now wait for the button to be pressed
    while (digitalRead(PUSH_BUTTON_PIN) == 1)
    {
        delay(delay_tic);
    }

}  // End of waitForPB()

        
void setup()
{
    Serial.begin(115200);
    pinMode(MODE_PIN, INPUT_PULLUP);
    pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);

    FreqCount.begin(1000);

    // Nominal values
    ref.c = 1000e-12;
    ref.l = 220e-6;

    // Load reference frequency from EEPROM
    configAddr = getConfigAddr();
    if (loadConfig(configAddr, &fcal) == false)
    {
        Serial.println("Initializing with nominal values...");
        fcal.freq = 1.0 / (2.0 * PI * sqrt(ref.c * ref.l));
        saveConfig(configAddr, &fcal);
        if (loadConfig(configAddr, &fcal) == false)
        {
            Serial.println("WARNING: can not save configuration.");
            fcal.freq = 1.0 / (2.0 * PI * sqrt(ref.c * ref.l));
        }
    }
    ref.freq = fcal.freq;

    //Serial.println(fcal.freq);
    //Serial.println(ref.freq);
    //Serial.println(fcal.structVersion);
    Serial.println("Hold push-button to enter calibration mode.");
    Serial.println("Freq, Units, Value, Units");
}


void loop() {
    int hold_delay = 2000 / delay_tic; // button hold time in delay_tics

    while ((measurement.pb = digitalRead(PUSH_BUTTON_PIN)) == 1)
    {
        if (FreqCount.available())
        {
            measurement.mode = digitalRead(MODE_PIN);
            measurement.freq = FreqCount.read();
            measurement.fratio = (((float)ref.freq * (float)ref.freq)
                      / ((float)measurement.freq *
                      (float)measurement.freq) - 1.0);
            measurement.c = ref.c * measurement.fratio;
            measurement.l = ref.l * measurement.fratio;
            printMeas();
            //dumpVars();
        }
    }

    while ((digitalRead(PUSH_BUTTON_PIN) == 0)
           && hold_delay > 0)
    {
        delay(delay_tic);
        hold_delay--;
    }

    // User held the button long enough so enter calibrate mode
    if (digitalRead(PUSH_BUTTON_PIN) == 0)
    {
        bool calComplete = false;

        do
        {
            Serial.println("CALIBRATION:");
            Serial.println("  1 - remove test component");
            Serial.println("  2 - set mode to Capacitance");
            Serial.println("  3 - press button");
            waitForPB();

            if (digitalRead(MODE_PIN) == 0)
            {
                Serial.print("Switch to Capacitance mode. Waiting ...");
                Serial.flush();
                while (digitalRead(MODE_PIN) == 0)
                {
                    delay(delay_tic);
                }
                delay(1000);  // Allow for switch move time
                Serial.println("");
            }

            if ((calComplete = calibrate()) == false)
            {
                Serial.println();
                Serial.println("Calibration FAILED");
                Serial.println("Remove test component and check switches.");
                Serial.println("");
            }
        } while (calComplete == false);
    }
}

