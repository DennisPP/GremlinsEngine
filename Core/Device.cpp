#include "Device.h"

#include <stddef.h>

#include <stdio.h>

Device* Device::instance = NULL;

Device* Device::GetInstance()
{
    if(!instance)
    {
        instance = new Device();
    }
    return instance;
}


Device::Device()
{
    logicalWidth = 1024.0f;
    logicalHeight = 768.0f;
    renderWidth = 1024.0f;
    renderHeight = 768.0f;
    deviceWidth = 1024.0f;
    deviceHeight = 768.0f;
}

void Device::SetRenderSize(float w, float h)
{
    printf("Set Device Render Size to:[%f %f]\n", w, h);
    renderWidth = w;
    renderHeight = h;
}

void Device::SetDeviceSize(float width, float height)
{
    printf("Set Device Size to:[%f %f]\n", width, height);
    
    deviceWidth = width;
    deviceHeight = height;
}


float Device::GetInputScaleX() const
{
    return logicalWidth / deviceWidth;
}


float Device::GetInputScaleY() const
{
    return logicalHeight / deviceHeight;
}
