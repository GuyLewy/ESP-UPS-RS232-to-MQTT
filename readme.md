# RS232 UPS Interface to MQTT Converter

This code is designed to be run on an esp-32 and converts the input from an RS232 signal and dry contacts of the SG UPS Series by general electric to the MQTT protocol.

<strong> This code is still in beta and has not yet been tested, please only use if you know what you are doing </strong>

## Features

This device so far does not allow for any control over the UPS although this may be possible in the future, currently it only allows for one way communication with data from the UPS being sent via MQTT, it revieves the following data form the UPS:

RS232:

-   [x] Input Voltage
-   [x] Input Fault Voltage
-   [x] Output Voltage
-   [x] Output Current
-   [x] Input Frequency
-   [x] Battery Voltage
-   [x] UPS Temperature
-   [x] Beeper State
-   [x] Shutdown State
-   [x] Whether a test in in progress
-   [x] UPS Standby State
-   [x] UPS Failed State
-   [x] Buck State
-   [x] Battery Low
-   [x] Utility Fail

Dry Contacts:

-   [x] Mains Failure
-   [x] Load on Inverter
-   [x] Stop Operation
-   [x] Load On Mains
-   [x] General Alarm
-   [x] Aucustic Alarm

## Adding Secrets

Before being able to use this program you must set all the values to the `secrets-template.h` file and rename it to `secrets.h`. Then just upload the code to the esp-32 and it should work
