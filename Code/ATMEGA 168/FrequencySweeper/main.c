/*
* FrequencySweeper.c
*
* Main execution code
*
* Created: 2020-07-26 5:55:24 PM
* Author : Brandon
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Hardware.h"
#include "IO_Macros.h"
#include "HD44780.h"
#include "AD9837.h"
#include "MCP3004.h"
#include "SPI.h"

const uint16_t tuningVals [] PROGMEM = {
    10, 21, 31, 42, 52, 62, 73, 83, 94, 104, 115, 125, 136, 146, 157, 167,
    178, 188, 199, 209, 220, 230, 241, 251, 262, 272, 283, 294, 304, 315, 325,
    336, 347, 357, 368, 379, 389, 400, 411, 421, 432, 443, 453, 464, 475, 486,
    496, 507, 518, 529, 539, 550, 561, 572, 583, 593, 604, 615, 626, 637, 648,
    659, 669, 680, 691, 702, 713, 724, 735, 746, 757, 768, 779, 789, 800, 811,
    822, 833, 844, 855, 866, 877, 888, 900, 911, 922, 933, 944, 955, 966, 977,
    988, 999, 1010, 1021, 1033, 1044, 1055, 1066, 1077, 1088, 1100, 1111,
    1122, 1133, 1144, 1156, 1167, 1178, 1189, 1201, 1212, 1223, 1234, 1246,
    1257, 1268, 1279, 1291, 1302, 1313, 1325, 1336, 1347, 1359, 1370, 1382,
    1393, 1404, 1416, 1427, 1439, 1450, 1461, 1473, 1484, 1496, 1507, 1519,
    1530, 1542, 1553, 1565, 1576, 1588, 1599, 1611, 1622, 1634, 1645, 1657,
    1669, 1680, 1692, 1703, 1715, 1727, 1738, 1750, 1761, 1773, 1785, 1796,
    1808, 1820, 1831, 1843, 1855, 1867, 1878, 1890, 1902, 1914, 1925, 1937,
    1949, 1961, 1972, 1984, 1996, 2008, 2020, 2031, 2043, 2055, 2067, 2079,
    2091, 2102, 2114, 2126, 2138, 2150, 2162, 2174, 2186, 2198, 2210, 2222,
    2234, 2245, 2257, 2269, 2281, 2293, 2305, 2317, 2329, 2341, 2354, 2366,
    2378, 2390, 2402, 2414, 2426, 2438, 2450, 2462, 2474, 2486, 2499, 2511,
    2523, 2535, 2547, 2559, 2571, 2584, 2596, 2608, 2620, 2632, 2645, 2657,
    2669, 2681, 2694, 2706, 2718, 2731, 2743, 2755, 2767, 2780, 2792, 2804,
    2817, 2829, 2841, 2854, 2866, 2878, 2891, 2903, 2916, 2928, 2940, 2953,
    2965, 2978, 2990, 3003, 3015, 3028, 3040, 3053, 3065, 3078, 3090, 3103,
    3115, 3128, 3140, 3153, 3165, 3178, 3190, 3203, 3216, 3228, 3241, 3253,
    3266, 3279, 3291, 3304, 3317, 3329, 3342, 3355, 3367, 3380, 3393, 3405,
    3418, 3431, 3444, 3456, 3469, 3482, 3495, 3507, 3520, 3533, 3546, 3558,
    3571, 3584, 3597, 3610, 3623, 3635, 3648, 3661, 3674, 3687, 3700, 3713,
    3726, 3739, 3751, 3764, 3777, 3790, 3803, 3816, 3829, 3842, 3855, 3868,
    3881, 3894, 3907, 3920, 3933, 3946, 3959, 3972, 3985, 3998, 4012, 4025,
    4038, 4051, 4064, 4077, 4090, 4103, 4116, 4130, 4143, 4156, 4169, 4182,
    4195, 4209, 4222, 4235, 4248, 4262, 4275, 4288, 4301, 4315, 4328, 4341,
    4354, 4368, 4381, 4394, 4408, 4421, 4434, 4448, 4461, 4474, 4488, 4501,
    4514, 4528, 4541, 4555, 4568, 4581, 4595, 4608, 4622, 4635, 4649, 4662,
    4676, 4689, 4703, 4716, 4730, 4743, 4757, 4770, 4784, 4797, 4811, 4824,
    4838, 4852, 4865, 4879, 4892, 4906, 4920, 4933, 4947, 4960, 4974, 4988,
    5001, 5015, 5029, 5042
};

const float availableCaps[] = {
    0.5, 1, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2, 10, 12, 15,
    18, 22, 27, 33, 39, 47, 56, 68, 82, 100, 120, 150, 180, 220, 270, 330, 390,
    470, 560, 680, 820, 1000, 1200
};

struct capPair {
    float cap1;
    float cap2;
};


void setup() {
    //Setup
    lcd_setup();
    dds_setup();
    adc_setup();
    spi_init();

    //Print
    lcd_goto_xy(0, 0);
    lcd_print_string("AUTO TUNER:");
    lcd_goto_xy(0, 1);
    dds_reset();
    set_freq_adjust_mode(FULL);
}

uint32_t incrementFreq(uint32_t currFreq) {
    currFreq += 20;
    uint32_t freqRegVal = freq_calc(currFreq);
    adjust_freq_32(FREQ0, freqRegVal);
    return currFreq;
}

uint16_t sampleADC(uint8_t sampleCount) {
    uint16_t adcSum = 0;
    for(uint8_t i = 0; i < sampleCount; i++) {
        adcSum += read_adc(0);
    }
    return adcSum/sampleCount;
}

uint16_t lookUpCap(uint32_t frequency) {
    uint32_t roundedFreq = frequency / 100 * 100;
    uint16_t valIndex = (roundedFreq - 131100) / 100;
    return (uint16_t)pgm_read_word(tuningVals + valIndex);
}

struct capPair getAvailCap(uint16_t capacitance) {
    struct capPair pair;
    uint16_t res1, res2;
    uint16_t l = 0;
    uint16_t r = sizeof(availableCaps)/sizeof(availableCaps[0]) - 1;
    uint16_t diff = 65535;
    while(r > l) {
        // Check if this pair is closer than the closest pair so far
        if (abs(availableCaps[l] + availableCaps[r] - capacitance) < diff) {
            res1 = l;
            res2 = r;
            diff = abs(availableCaps[l] + availableCaps[r] - capacitance);
        }

        // If this pair has more sum, move to smaller values.
        if (availableCaps[l] + availableCaps[r] > capacitance) {
            r--;
        }
        // Move to larger values
        else {
            l++;
        }
    }
    pair.cap1 = availableCaps[res2];
    pair.cap2 = availableCaps[res1];
    return pair;
}

int main(void) {
    setup();

    uint32_t baseFreq = 121000;
    uint32_t maxFreq = 141000;
    uint32_t currFreq = baseFreq;
    uint32_t peakFreq = 0;
    uint16_t adcPeak = 0;

    set_dds_mode(SINE);
    select_freq_reg(FREQ0);
    uint32_t freqRegVal = freq_calc(baseFreq);
    adjust_freq_and_mode_32(FREQ0, FULL, freqRegVal);
    while (1) {
        currFreq = incrementFreq(currFreq);
        _delay_us(250);
        uint16_t adcCurrent = sampleADC(10);
        if(adcCurrent > adcPeak) {
            adcPeak = adcCurrent;
            peakFreq = currFreq;
        }
        if(currFreq >= maxFreq) {
            lcd_clear_line(0);
            lcd_clear_line(1);
            if(adcPeak > 100) {
                char outputText[20];
                int peakFreqInt = peakFreq/1000;
                int peakFreqDec = (peakFreq/100 % 10);
                sprintf(outputText, "Frq.:%d.%dKHz", peakFreqInt, peakFreqDec);
                lcd_goto_xy(0, 0);
                lcd_print_string(outputText);

                memset(outputText, 0, strlen(outputText));
                if(peakFreq < 132000 && peakFreq > 130000) {
                    sprintf(outputText, "Frq. In Range");
                } else {
                    struct capPair pair = getAvailCap(lookUpCap(peakFreq));
                    if(ceilf(pair.cap1) == pair.cap1 && ceilf(pair.cap2) == pair.cap2) {
                        sprintf(outputText, "Add:%dpF,%dpF", (int)pair.cap1, (int)pair.cap2);
                    } else if(ceilf(pair.cap1) != pair.cap1 && ceilf(pair.cap2) == pair.cap2) {
                        sprintf(outputText, "Add:%.1fpF,%dpF", pair.cap1, (int)pair.cap2);
                    } else if(ceilf(pair.cap1) == pair.cap1 && ceilf(pair.cap2) != pair.cap2) {
                        sprintf(outputText, "Add:%dpF,%.1fpF", (int)pair.cap1, pair.cap2);
                    } else {
                        sprintf(outputText, "Add:%.1fpF,%.1fpF", pair.cap1, pair.cap2);
                    }
                }

                lcd_goto_xy(0, 1);
                lcd_print_string(outputText);
            } else {
                lcd_goto_xy(0, 0);
                lcd_print_string("AUTO TUNER:");
                lcd_goto_xy(0, 1);
            }
            peakFreq = baseFreq;
            adcPeak = 0;
            currFreq = baseFreq;
        }
    }

    return 0;
}

