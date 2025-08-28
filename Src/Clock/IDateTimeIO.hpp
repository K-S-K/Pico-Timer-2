#pragma once

#include "DateTime.h"

class IDateTimeIO {
public:
    virtual ~IDateTimeIO() = default;

    virtual void SetTime(const DateTime& newTime) = 0;
    virtual void GetTime(DateTime& outTime) = 0;
};
