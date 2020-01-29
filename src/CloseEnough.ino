

/*
 * "Close-enough Sketch"
 *
 * Arduino / EZ1 / Pot / LED
 *    closePin (8) -D-> LED
 *    Analog 0     <-A- EZ1
 *    Analog 1     <-A- Pot
 *
 * The EZ1 is wired in "free run" mode - that is, RX is not connected.
 *
 * User can adjust the "close-enough" value with the pot.  The idea is that:
 *     User places object at "close-enough" range from the EZ1.
 *
 *     User adjusts pot until light comes on / goes off - this sets the pot value.
 *
 *     Object comes and goes - the LED light / turn off.
 *
 *     After object has been close-enough long-enough, light turns off.
 *
 *     If object moves within the close-enough range after timer is off, the
 *     light turns on again.
 */

    int closePin = 8;

    int state = -1;
    unsigned long timeOn = 0;
    int lastRangeDistance = 0;
    
    #define OBJECT_CLOSE  0
    #define OBJECT_FAR    1
    #define TIMER_EXPIRED 2

    void setup()
    {
        pinMode(closePin, OUTPUT);
    
        // After power up, EZ1 takes 250ms to start
        delay(300);
        
        // First read takes 100ms more
        delay(100);
    }

    void loop()
    {
        // pot ranges from 0 - 1024 analog reading
        // for high impedence pots, the fine adjustment may be lost
        // we will set 0 = 12in and 1024 as 4 ft. (for now).
        // so, we have 12" to 48" to be mapped into 0-1024
        // alarmDistance / (48-12) = potValue / (1024-0) + 12
        
        float potValue = analogRead(1);
        float alarmDistance = potValue * 36.0 / 1024.0 + 12.0;
        
        // The EZ1 is wired to analog 0 which can be read at any time
        // The values must be converted - see the EZ1 datasheet AND
        // also know that EZ1 reports 9-bit precision, but Arduino
        // reads 10-bit precision.
        float rangeValue = analogRead(0);
        float rangeDistance = (254.0 / 1024.0) * 2.0 * rangeValue;
        
        // Sort out the event - event_close and event_far have precedent over timer_expired.

        int event = OBJECT_CLOSE;
        
        if (rangeDistance < alarmDistance)
        {
            event = OBJECT_CLOSE;
            if (state == -1) state = 2;
            else if (millis() - timeOn > 1000UL * 60UL * 5UL) event = TIMER_EXPIRED;
        }
        else
        {
            if (state == -1) state = 0;
            event = OBJECT_FAR;
        }
        
        // Run the state machine
        
        switch (state)
        {
            case 0 : // object is close, timer not expired
                switch (event)
                {
                    case OBJECT_CLOSE :
                        if (rangeDistance < lastRangeDistance-1 ||
                            rangeDistance > lastRangeDistance+1)
                            timeOn = millis();
                        state = 0;
                        break;
                    case TIMER_EXPIRED :
                        digitalWrite(closePin, LOW);
                        state = 1;
                        break;
                    case OBJECT_FAR :
                        digitalWrite(closePin, LOW);
                        state = 2;
                        break;
                }
                
                break;
                
            case 1 : // object is close, timer expired
                switch (event)
                {
                    case OBJECT_CLOSE :
                    case TIMER_EXPIRED :
                        if (rangeDistance < lastRangeDistance-1 ||
                            rangeDistance > lastRangeDistance+1)
                        {
                            digitalWrite(closePin, HIGH);
                            state = 0;
                            timeOn = millis();
                        }
                        else
                            state = 1;
                        break;
                    case OBJECT_FAR :
                        // LED should be turned off if we're in this state
                        // ensure that it is
                        digitalWrite(closePin, LOW);
                        state = 2;
                        break;
                }
                break;
                
            case 2 : // object not close
                switch (event)
                {
                    case OBJECT_CLOSE : /* break; WHY IS THIS HERE? */
                        digitalWrite(closePin, HIGH);
                        timeOn = millis();
                        state = 0;
                        break;
                    case TIMER_EXPIRED :
                        state = 2;
                        break;
                    case OBJECT_FAR :
                        state = 2;
                        break;
                }
                break;
        }

        // Don't go too fast, but shouldn't matter.  The EZ1 takes
        // 50ms to read the distance, so, as long as we don't exceed
        // this limit (20Hz), we should be okay.
        
        lastRangeDistance = rangeDistance;
        
        delay(100);
    }


