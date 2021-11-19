

# AppZone m2mb Bravo AT sensors App



Package Version: **1.0.5**

Firmware Version: **30.01.XX0.0**


## Features

This package goal is to provide sample source code for Bravo AT sensors demo app.
 


## Deployment Instructions


To manually deploy the Sample application on the devices perform the following steps:

1. Have **30.01.XX0.0** FW version flashed (`AT#SWPKGV` will give you the FW version)

1. Copy _m2mapz.bin_ to _/mod/_ 
	```
	AT#M2MWRITE="/mod/m2mapz.bin",\<size\>,1
	```
1. Run `AT#M2MRUN=2,m2mapz.bin`
1. Run `AT+M2M=4,10`



## Known Issues



## Troubleshooting

* Application does not work/start:
	+ Delete application binary and retry
    ```
    AT#M2MDEL="/mod/m2mapz.bin"
    ```
	+ Delete everything, reflash and retry
    ```
    AT#M2MDEL="/mod/m2mapz.bin"
    AT#M2MDEL="/mod/appcfg.ini"
    ```
      
* Application project does not compile
	+ Right click on project name
	+ Select Properties
	+ Select AppZone tab
	+ Select the right plugin (firmware) version
	+ Press "Restore Defaults", then "Apply", then "OK"
	+ Build project again

---

## Making source code changes

### Folder structure

The applications code follow the structure below:

* `hdr`: header files used by the application
    * `app_cfg.h`: the main configuration file for the application
* `src`: source code specific to the application
* `BOSCH`: sources to manage Bosch sensors and devices mounted on the Bravo EVK board
* `azx`: helpful utilities used by the application (for GPIOs, LOGGING etc)
    * `hdr`: generic utilities' header files
    * `src`: generic utilities' source files
* `Makefile.in`: customization of the Make process


## Applications 

Telit IoT AppZone Bravo Board Sensors App - leverage onboard sensors

## Abstract

This repository contains Telit IoT AppZone C sample app exposing Bravo Board sensors, allowing to access data from them through a dedicated custom AT command

**The minimum firmware version to build the sample is 30.01.xx0****.

---

# AT#BSENS Command
This command allows to read one of the sensors values on the bravo board




   <img src="./pics/set.png" height="16">  **AT\#BSENS=&lt;sensor_id&gt;[,&lt;accel_th&gt;]**

Set command allows read the provided sensor data.

Parameters:

| **Name**      	|   **Type**  	| **Default** 	| **Description**  	|
|-----------	|:-------:	|---------	|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	|
| **&lt;sensor_id&gt;** 	| integer 	| -       	| Sensor Id<br><br>  Values:<br>    1 : Enviroment<br>    2 : 3D vector<br>    3 : Tampering<br>    4 : Linear Acceleration<br><br> |
| **&lt;accel_th&gt;**  	| integer 	| -       	| Acceleration threshold for Linear Acceleration sensor, as a **g** (Earth gravitational acceleration) multiple. If > 0, every time the module of the 3 axes acceleration exceeds the threshold, a counter will be increased. The same counter is reset to 0 when **&lt;accel_th&gt;** is set to 0 	|

---------------------------------------------------------------------

Upon set command execution, a response will be provided with different formats depending on the sensor:

**Environment**

**\#BSENS: 1,&lt;temp&gt;,&lt;humidity&gt;,&lt;pressure&gt;,&lt;airQ&gt;**

Where

 - &lt;temp&gt;: temperature as a floating point value in Celsius degrees (range -40 – 80)
 - &lt;humidity&gt;: relative humidity as a floating point in %RH (range 0-100)
 - &lt;pressure&gt;: pressure as a floating point in hPa (range 300-1100)
 - &lt;airQ&gt;: air quality, integer value (range 0-1000)

For example

**\#BSENS: 1,20.2,42.4,1014.5,25**



**3D Vector**

**\#BSENS: 2,&lt;intensity&gt;,&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;accuracy&gt;**

Where

 - &lt;intensity&gt;: vector intensity as a floating point
 - &lt;x&gt;: x component of the vector as a floating point
 - &lt;y&gt;: y component of the vector as a floating point
 - &lt;z&gt;: z component of the vector as a floating point
 - &lt;accuracy&gt; reading accuracy as an integer

For example:

**\#BSENS: 2,0.84,-0.12,-0.003,0.02,10000**


**Tampering**

**\#BSENS: 3,&lt;status&gt;**

Where

&lt;status&gt; the current tampering status:

 - 0 still status ended
 - 1 walking status ended
 - 2 running status ended
 - 3 bicycle status ended
 - 4 vehicle status ended
 - 5 tilting status ended
 - 8 still status started
 - 9 walking status started
 - 10 running status started
 - 11 bicycle status started
 - 12 vehicle status started
 - 13 tilting status started

For example:

**\#BSENS: 3,0**

**Linear Acceleration**

If &lt;accel_th&gt; is > 0:

**\#BSENS: 4,&lt;accel_th&gt;,&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;events_counter&gt;**

Where

 - &lt;accel_th&gt;: current threshold value
 - &lt;x&gt;: x component of the acceleration as a floating point, in m/s^2
 - &lt;y&gt;: y component of the acceleration as a floating point, in m/s^2
 - &lt;z&gt;: z component of the acceleration as a floating point, in m/s^2
 - &lt;events_counter&gt; how many times the threshold has been exceeded since the check has been enabled

For example:

**\#BSENS: 4,8,0.012,-0.003,0.02,2**

If &lt;accel_th&gt; is 0:

**\#BSENS: 4,0**

---------------------------------------------------------------------------------------- ---------------------------------------------------------------------------------------------------------
   <img src="./pics/query.png" height="16"> **AT\#BSENS=?**

Test command reports the available range of values for parameter &lt;sensorId&gt; in the format:

 **\#BSENS: (list of supported &lt;sensorId&gt; values)**

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


**External Libraries**

To build the application it is required to put `libalgobsec.ar` file into the project's BOSCH/BSEC folder. The library can be retrieved at the link
https://www.bosch-sensortec.com/software-tools/software/bsec/ . Download the BSEC 1.4.8.0 v3 version archive, then extract the library `libalgobsec.a` from the ZIP file and rename as `libalgobsec.ar`. It can be found in the archive directory
*BSEC_1.4.8.0_Generic_Release_updated_v3/algo/normal_version/bin/gcc/Cortex_A7/without_FPIC*


Please note: all the apps using BSEC library configure the device with the **18v3s_4d** option. If a different version of the library is in use, please replace the BOSCH/BME680/bsec_serialized_configurations_iaq.c file in the project with the one inside `BSEC_x.x.x.x_Generic_Release/config/generic_18v_3s_4d/`


---


MultiSensors Ondemand AT application. Debug prints on **MAIN UART**


