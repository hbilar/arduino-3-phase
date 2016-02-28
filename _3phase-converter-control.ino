/*
 * Single to 3 phase converter circuit control code.
 *
 *
 * IMPORTANT NOTE: This device can kill you, as it's basically manipulating 240V electricity.
 *                 Do NOT build one unless you know what you're doing - I take no responsibility
 *                 for what you do with the code and/or hardware. The code is presented here 
 *                 for educational purposes only.
 *
 * 
 * This arduino code is used to control two relays, which in turn drive two contactors to 
 * start a 3 phase motor connected to single phase power (by means of an unbalancing 
 * starting cap). After a while (~ 500ms), the starting cap is taken out of the circuit.
 *
 * Sketch of circuit:
 *                                start       start
 *                              contactor   capacitor
 *                            .------/ --------| |---------+--------- Manufactured phase
 *                            |                            |
 *   240V  live    -----/ ----+-----------+-------------.  |
 *                  run_contactor         |              ( M )
 *         neutral -----/ --------------- ) ------+-----'
 *                                        |       '------------------ Phase 2
 *                                        | 
 *                                        '-------------------------- Phase 1 
 * 
 * You can add run capacitors between the three phases to alter the voltage, if 
 * required, but at least in my application the motor generates a voltage that is
 * close enough to use (I end up with about 240, 235 and 220V). 
 *
 * My 3 phase motor used to generate the third phase is a 3hp (2.2kW) motor. It's connected
 * in 240V delta. The starting cap is 100mF, and the motor starts up in less than 0.5s.
 *
 * The arduino controls the circuit using the run contactor and start contactor. You have to
 * use a 240V relay to drive the contactors, as they can draw several amps when closing (mine
 * draws about 2 amps for ~1 second).
 *
 *
 * Pins on the arduino board 
 * 
 * 2    power led
 * 3    starting led    
 * 5    button 1
 * 7    button 2
 * 9    relay 1
 * 11   relay 2
 * 
 * 
 * Written by Henrik Bilar (henrik@bilar.co.uk). You can use this code under the GPLv3 license.
 */


const byte POWER_LED = 2;
const byte START_LED = 3;
const byte BUTTON_1 = 5;
const byte BUTTON_2 = 7;
const byte RELAY_1 = 9;
const byte RELAY_2 = 11;

/* number of milliseconds to enable RELAY_2 after start button has been pressed */
const int START_MS = 500; 

const int LED_BLINK_LENGTH = 1000;  // ms

byte LED_STATE[15];
byte BUTTON_STATE[15];
byte RELAY_STATE[15];

int last_button_state = 0;



// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);

  SetRelayStop();

  Serial.begin(9600);          //  setup serial

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(START_LED, OUTPUT);
  pinMode(POWER_LED, OUTPUT);

  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);

  digitalWrite(BUTTON_1, HIGH);  // enable pull up resistors
  digitalWrite(BUTTON_2, HIGH);  

  for (int n = 0; n <= sizeof(LED_STATE); n++) {
    LED_STATE[n] = 0;
  }
  for (int n = 0; n <= sizeof(BUTTON_STATE); n++) {
    BUTTON_STATE[n] = 0;
  }
  for (int n = 0; n <= sizeof(RELAY_STATE); n++) {
    RELAY_STATE[n] = 0;
  }
  UpdateRelay();

  BUTTON_STATE[BUTTON_1] = digitalRead(BUTTON_1);
  BUTTON_STATE[BUTTON_2] = digitalRead(BUTTON_2);

  last_button_state = 1;
}


/* Use the LED state to set the power on or off */
void DisplayLED()
{
  digitalWrite(START_LED, LED_STATE[START_LED]);
  digitalWrite(POWER_LED, LED_STATE[POWER_LED]);
}


int STATE = 0;
/* states:
   0    Initial - power on
   1    Powering on 3 phase motor (both relays)
   2    3 phase motor running (1 relay only)
*/

int led_last_blink = 0;
int last_state_change = 0;

void ResetLED()
{
  LED_STATE[START_LED] = 0;
  LED_STATE[POWER_LED] = 0;
}


void UpdateRelay()
{
  digitalWrite(RELAY_1, RELAY_STATE[RELAY_1]);
  digitalWrite(RELAY_2, RELAY_STATE[RELAY_2]);
}


void SetRelayStop()
{
  RELAY_STATE[RELAY_1] = 0;
  RELAY_STATE[RELAY_2] = 0;

  UpdateRelay();
}


void SetRelayStarting()
{
  RELAY_STATE[RELAY_1] = 1;
  RELAY_STATE[RELAY_2] = 1;

  UpdateRelay();
}


void SetRelayRunning()
{
  RELAY_STATE[RELAY_1] = 1;
  RELAY_STATE[RELAY_2] = 0;

  UpdateRelay();
}



/* sets the LEDs to periodically blink */
void BlinkLED()
{
  int m = millis();

  switch (STATE) {

  case 0:
    /* both blinking */
    if (m - led_last_blink > LED_BLINK_LENGTH) {
      // switch state 
      LED_STATE[START_LED] = LED_STATE[START_LED] ? 0 : 1;
      LED_STATE[POWER_LED] = LED_STATE[POWER_LED] ? 0 : 1;
      
       led_last_blink = m;
    }
    break;
    ;;
  case 1:
    /* POWER LED off, START LED on */
    LED_STATE[START_LED] = 1;
    LED_STATE[POWER_LED] = 0;
    break;
    ;;
  case 2:
    /* POWER LED on, START LED off */
    LED_STATE[START_LED] = 0;
    LED_STATE[POWER_LED] = 1;
    break;
    ;;
  default:
    /* both on */
    LED_STATE[START_LED] = 1;
    LED_STATE[POWER_LED] = 1;
    ;;
  }
}


int last_bounce = 0;
void loop() {
  int m = millis();

  switch (STATE) {
  case 1:
    /* starting up */
    if (m - last_state_change > START_MS) {
      /* timer for both relays has now expired - alter state */
      STATE=2;

      SetRelayRunning();

      Serial.println("Altered state to 2");
    }
    break;
    ;;
  }

  BlinkLED();

  /* check buttons */
  int button_1 = digitalRead(BUTTON_1);
  if (button_1 != last_button_state) {
    /* reset bounce timer */
    last_bounce = m;
  }

  if (m - last_bounce > 50) {
    /* button really changed (debounce) */
    if (BUTTON_STATE[BUTTON_1] != button_1) {
      BUTTON_STATE[BUTTON_1] = button_1;
      if (button_1) {
	/* button has been released */
	Serial.println("Button released");
      } else {
	/* button has been pressed */
	Serial.println("Button pressed");
	
	switch (STATE) {
	case 0: 
	  /* change to state 1 - power on */
	  STATE = 1;
	  last_state_change = m;

	  SetRelayStarting();

	  Serial.println("Altered state to 1");

	  /* should set relays etc here */
	  break;
	  ;;
	default:
	  /* button press in any other state 
	     results in everything turning off */

	  /* turn off relays */
	  last_state_change = m;
	  STATE = 0;

	  SetRelayStop();

	  ResetLED();
	  Serial.println("Altered state to 0");
	  ;;
	}
      }
    }
  }
  last_button_state = button_1;

  DisplayLED();

  char buf[100];
  snprintf(buf, sizeof(buf), "STATE %d", STATE);
  //  Serial.println(buf); 
}
