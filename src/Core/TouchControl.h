#ifndef _TOUCH_CONTROL_H_
#define _TOUCH_CONTROL_H_

class TTouchControl
{
public:
    TTouchControl();
    
    typedef enum TouchPhase
    {
        TouchPhaseBegan,             // whenever a finger touches the surface.
        TouchPhaseMoved,             // whenever a finger moves on the surface.
        TouchPhaseStationary,        // whenever a finger is touching the surface but hasn't moved since the previous event.
        TouchPhaseEnded,             // whenever a finger leaves the surface.
        TouchPhaseCancelled,         // whenever a touch doesn't end but we need to stop tracking (e.g. putting device to face)
    } TouchPhase;
    
    bool operator()(const TTouchControl& left,const TTouchControl& right);
    
    bool GetDistance() const;
    
	void *pointer;
	float xo;
	float yo;
	float x;
	float y;
    double time;
    double touchDownTime;
	TouchPhase phase;	
    int tag;
};

#endif