#ifndef __Device__
#define __Device__

class Device
{
public:
    static Device* GetInstance();
    
    void SetRenderSize(float width, float height);
    void SetDeviceSize(float width, float height);
    
    float GetInputScaleX() const;
    float GetInputScaleY() const;
    
    float renderWidth;
    float renderHeight;
    
    float logicalWidth;
    float logicalHeight;
    
private:
    static Device* instance;
    
    float deviceWidth;
    float deviceHeight;
    
    Device();
};


#endif