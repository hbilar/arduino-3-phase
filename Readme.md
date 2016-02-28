## Synopsis

Simple Arduino  control circuit to create 3 phase power from a single phase source. 

## WARNING

IMPORTANT NOTE: This device can kill you, as it's basically manipulating 240V electricity. Do NOT build one unless you know what you're doing - I take no responsibility for what you do with the code and/or hardware. The code is presented here for educational purposes only.


## Example Circuit

```
                                start       start
                              contactor   capacitor
                            .------/ --------| |---------+--------- Manufactured phase
                            |                            |
   240V  live    -----/ ----+-----------+-------------.  |
                  run_contactor         |              ( M )
         neutral -----/ --------------- ) ------+-----'
                                        |       '------------------ Phase 2
                                        | 
                                        '-------------------------- Phase 1 
```
You can add run capacitors between the three phases to alter the voltage, if required, but at least in my application the motor generates a voltage that is close enough to use (I end up with about 240, 235 and 220V). 

My 3 phase motor used to generate the third phase is a 3hp (2.2kW) motor. It's connected in 240V delta. The starting cap is 100mF, and the motor starts up in less than 0.5s.

The arduino controls the circuit using the run contactor and start contactor. You have to use a 240V relay to drive the contactors, as they can draw several amps when closing (mine draws about 2 amps for ~1 second).


## pin out

```
 * 2    power led
 * 3    starting led    
 * 5    button 1
 * 7    button 2
 * 9    relay 1
 * 11   relay 2
```

## Motivation

I've written this code to automate my own 3 phase rotary converter. The circuit is fairly easy to build, and I wanted to have some real use for an Arduino, so this seemed like a decent project.


## License & Author

Written by Henrik Bilar (henrik@bilar.co.uk). This code is released under the GPLv3 license.