// Tutorial 11a. Temperature controlled fan with TI LM35CZ

// Main parts: Adafruit Metro Mini, TI LM35CZ, 3-wire PC fan,
// Infineon Technologies IRFZ44N MOSFET, ON Semiconductor 1N5819
// Schottky diode

// Variables that remain constant
const byte pinSensor = A0; // Analog input pin from temperature sensor
const byte pinFan = 3; // Digital output pin to the transistor's gate
const byte fanSpeedMin = 16;
const byte fanSpeedMax = 255;
const byte tempMin = 23; // Lower threshold in °C (min. -40°C)
const byte tempMax = 29; // Upper threshold in °C (max. 110°C)
float calibration = 0.1; // Multiplier for raw sensor reading

// Variables that can change
float temperature; // Temperature in millivolts (°C with multiplier)
byte fanSpeed; // Controls the fan speed via PWM of transistor gate

void setup()
{
  // To eliminate typical low-speed hiss/buzz of PWM controlled fans,
  // one can change one of the microcontroller's internal timers from
  // the standard frequency of 490Hz or 980Hz to a far higher 31300Hz.
  // For the Metro Mini, it's timer 2 (= TCCR2B) that is set to that
  // frequency via 0x01, instead of the default 0x04 (490Hz). But do
  // not change timer 0, because it would change delay() or millis()
  TCCR2B = TCCR2B & 0b11111000 | 0x01;

  // Uncomment for debugging
  //Serial.begin(9600);

  // Sets the Metro Mini's internal reference voltage to 1.1V to use
  // the sensor's full bandwidth when obtaining readings, because the
  // LM35CZ has a range from 0 - 1000mV, and setting the internal
  // reference voltage to 1.1V results in much finer resolution in the
  // 0 - 1023 range than the standard 5V ADC reference
  analogReference(INTERNAL);

  // Initialise the sensor pin
  pinMode(pinSensor, INPUT);

  // Initialise output pin to the transistor's gate
  pinMode(pinFan, OUTPUT);

  // Initialise Metro Mini's built-in red LED pin
  pinMode(13, OUTPUT);
}

void loop()
{
  // Read the voltage from the sensor pin and apply multiplier
  temperature = analogRead(pinSensor) * calibration;

  // Uncomment for debugging
  //Serial.print("Temp.: "); Serial.print(temperature, 1); Serial.println(" °C");

  // If the temperature is equal to or below the minimum
  if (temperature <= tempMin)
  {
    // Switch the fan off
    fanSpeed = 0;
    digitalWrite(pinFan, LOW);
  }

  // If the temperature is within the specified range
  if ((temperature  >= tempMin) && (temperature <= tempMax))
  {
    // Scale it into the desired tempMin - 255 value range
    fanSpeed = map(temperature, tempMin, tempMax, fanSpeedMin, fanSpeedMax);

    // Then write that value to the transistor's gate pin
    analogWrite(pinFan, fanSpeed);

    // Uncomment for debugging
    //Serial.print("PWM value: "); Serial.println(fanSpeed);
  }

  // If the temperature is equal to or above the maximum
  if (temperature >= tempMax)
  {
    // Signal that the limit is reached
    digitalWrite(13, HIGH);
  }

  else
    // Otherwise, switch the signal off
  {
    digitalWrite(13, LOW);
  }

  // Use millis() timing method to avoid blocking other code
  delay(2000);
}
