# CloseEnough
Proximity alarm for the Arduino.

This project uses the Maxbotix Ultrasonic Rangefinder as the main component in a "close enough" parking sensor.

Because people don't always park the same distance away from an object as others, the "close enough" parking
sensor features a potenitameter that allows adjustment of the "close enough" distance.

When the car comes within the distance as set by the pot, the LED illuminates.  After a set period of time,
the LED will turn off.

The code implements a state machine that takes the current state and an input event.  Each transition, based
on the state and input event, defines an action and next state.  For this project, the state machine is very
simple.

The Maxbotix is run in "free-run" mode in which analog values can be read anytime.  It also supports other modes,
but for this project, free-run simplifies coding.

The entire project is "home-built" using a home-built DC-DC power supply, custom I/O distribution boards, and
a custom Arduino mini board.

Wiring details can be found in the sketch.
