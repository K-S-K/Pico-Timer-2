#pragma once

#include "../MenuLogic/MenuEvent.h"

enum class EventProcessingResult {
    Continue,
    Cancel,
    Apply,
};

class IPage
{
public:
    virtual ~IPage() = default;
    virtual void Render() = 0;
    virtual void PrepareDisplay();
    virtual EventProcessingResult ProcessMenuEvent(MenuEvent event) = 0;
};
