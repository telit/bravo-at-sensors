Telit IoT AppZone Bravo Board Sensors App - leverage onboard sensors

## Abstract

This repository contains Telit IoT AppZone C sample app exposing Bravo Board sensors, allowing to access data from them through a dedicated custom AT command

**The minimum firmware version to build the samples is 30.00.xx8, but it is suggested to use the latest 30.01.xx0**.

---

# AT#BSENS Command
This command allows to read one of the sensors values on the bravo board




   <img src="../../pics/set.png" height="16">  **AT\#BSENS=&lt;sensorId&gt;**

Set command allows read the provided sensor data.

Parameter:
---------------------------------------------------------------------
**Name**            **Type**    **Default**    **Description**
------------------- ----------  -------------  ------------------------
**&lt;sensor Id&gt;**   integer    -             Sensor Id.

                                               Values:
                                                 1   :   Enviroment

                                                 2   :   3D vector

                                                 3   :   Tampering

---------------------------------------------------------------------

Upon set command execution, a response will be provided with different formats depending on the sensor:

**Environment**

**\#BSENS: 1,&lt;temp&gt;,&lt;pressure&gt;,&lt;humidity&gt;,&lt;airQ&gt;**

Where

 - &lt;temp&gt;: temperature as a floating point value in Celsius degrees (range -40 – 80)
 - &lt;pressure&gt;: pressure as a floating point in hPa (range 300-1100)
 - &lt;humidity&gt;: relative humidity as a floating point in %RH (range 0-100)
 - &lt;airQ&gt;: air quality, integer value (range 0-1000)

For example

**\#BSENS: 1,20.2,1014.5,42.4,25**



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
  ---------------------------------------------------------------------------------------- ---------------------------------------------------------------------------------------------------------
   <img src="../../pics/query.png" height="16"> **AT\#BSENS=?**

Test command reports the available range of values for parameter &lt;sensorId&gt; in the format:

 **\#BSENS: (list of supported &lt;sensorId&gt; values)**

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


**External Libraries**

To build the application it is required to put `libalgobsec.ar` file into the project's BOSCH/BSEC folder. The library can be retrieved at the link
https://www.bosch-sensortec.com/software-tools/software/bsec/ . Download the BSEC 1.4.8.0 v3 version archive, then extract the library `libalgobsec.a` from the ZIP file and rename as `libalgobsec.ar`. It can be found in the archive directory
*BSEC_1.4.8.0_Generic_Release_updated_v3/algo/normal_version/bin/gcc/Cortex_A7/without_FPIC*


Please note: all the apps using BSEC library configure the device with the **18v3s_4d** option. If a different version of the library is in use, please replace the BOSCH/BME680/bsec_serialized_configurations_iaq.c file in the project with the one inside `BSEC_x.x.x.x_Generic_Release/config/generic_18v_3s_4d/`


---


