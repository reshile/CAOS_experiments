#pragma once

typedef enum {
    Zero = 0x00, // done
    MinusZero = 0x01, // done
    Inf = 0xF0, // done
    MinusInf = 0xF1, // done
    Regular = 0x10, // done
    MinusRegular = 0x11, // done
    Denormal = 0x20, // done
    MinusDenormal = 0x21, // done
    NaN = 0x30
} float_class_t;

float_class_t classify(double value);
