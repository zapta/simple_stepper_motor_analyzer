# Simple Stepper Motor Analyzer


<span style="color:red"><b>
NOTE (June 2021): The old design of the Simple Stepper Motor analyzer which was based on the STM32 was moved to https://github.com/zapta/simple_stepper_motor_analyzer. This is a new and improve design that is based on the Raspberry Pi Pico and a 'bare' TFT panel.</b></span> 

&nbsp;

https://github.com/zapta/simple_stepper_motor_analyzer

&nbsp;


![](./www/overall.jpg)

&nbsp;

![](./www/overall4.jpg)

&nbsp;

## Demonstration

Michael from the Teaching Tech youtube channel made [a great video explaining how stepper motors work](https://youtu.be/4iII25DGIdA) and used the Simple Stepper Motor Analyzer for some of the demonstrations. 

![](./www/teaching_tech_thumbnail.png)


&nbsp;

## Highlights
* Low cost and minimalist DYI hardware design.
* Standalone. Doesn't require a computer to operate.
* Analyzes the stepper motor signals in the actual system (e.g. a 3D printer).
* Galvanically isolated current sensors. Does not interfere with the operating equipment, even when its turned off.
* Easy to connect using a pair 4 wire pass through connectors.
* Intuitive user interface with color display and a capacitive touch.
* Microstepping resolution of 1% of a full step.
* Provides momentary readings, signal patterns, and statistical measurements.
* Hardware and software are open sourced with a generous license.
* Based on free tools (Kicad, platformio, LVGL) and the Raspberry Pi Pico.
* Can be easily customized (new featured, cost reduction, data link to a computer, etc).
* Powered 5VDC, 200ma via a USB-C connector.


## Overview
Diagnosing the operation of stepper motor in 3D printers and CNC machines used to require stepper motor analyzers that costs thousands of dollar. Not anymore! The Simple Stepper Analyzer ('Analyzer' in short) brings these capabilities to every maker for a tiny fraction of the price and with a minimalist DIY electronic design that leaves the heavy lifting to the firmware.

The Analyzer is self contained, has two 4-wires pass-through connectors connecting the stepper motor, is powered from a common 5V charger with a micro USB cable, provides a simple to use graphical user interface with a 3.5" color touch screen, and provides a plethora of stepper motor measurements.

The Analyzer is provided to the community as a public domain reference design that can be built and used as is, or be customized with additional features or for a lower cost as desired. The hardware and firmware come with a liberal open source license (CC0) that allows commercial usages and doesn't require code source code sharing.

---
## Disclaimer
THE DESIGN IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE DESIGN OR THE USE OR OTHER DEALINGS IN THE DESIGN.


---
## How does it work?
The principle of operation is very simple, and so is the hardware design. The stepper motor is connected via 4 wire pass-though connectors that allow the Analyzer to monitor the current through the stepper's coils. The Analyzer's user low impedance current sensors that are galvanically isolated such does not interfere with the operation of the 3D printer or other equipment it monitors. The Analyzer measures the coils currents 100k times a second and uses firmware that analyzes the data, extracts the desired measurements, and present them on the screen. For example, this is how an analysis of the distribution of distance by speed distribution looks like:


![](./www/distance_by_speed_screen.png)

---
## Operation is simple and intuitive

1. Turn off power to the equipment (e.g. 3D printer).
2. Disconnect the 4 wires of the stepper motor and connect them through the Analyzer two pass through connectors.
3. Power the analyzer using a 5V charger and a micro USB cable.
4. Turn on the equipment and get the stepper going.
5. Use the Analyzer's screen to view the measurements you want.

**NOTE:** The two pass-through connectors of the Analyzer are symmetric and  each can act as an input or output. 

**CAUTION:** Always connect and disconnect stepper motors when their controller is not energized, to reduce the risk of damage to the controller due to high voltage spikes. The analyser itself is not subject to that risk since its uses galvanically isolated current sensors.

![](./www/wiring_diagram.png)

**NOTE:** The only requirement for proper wiring is that the two wires of one coil should go through pins pair 1,2 and the two wires of the other coil should go through pin pair 3,4. The Analyzer doesn't care about the +/- polarity of the wires in each pair, nor does it care which of the two pass-through connectors is on the controller side and which one is on the stepper side.

**TRIVIA:**: The pin-out and the connector type of the Analyzer follows the Duet3D conventions, except that is uses right angle connector to save space.

---
## The Home Page
The Home page is first page shown when the Analyzer is powered on. It provides summary information as well as access to the Settings page.

![](./www/home_screen.png)

&nbsp;

#### Page Data

Field | Description
:------------ | :-------------
 COIL&nbsp;1 | Momentary coil 1 current in Amps.
 COIL&nbsp;2 | Momentary coil 2 current in Amps.
 ERRORS | The number of phase errors detected so far. Phase errors happens when the current patterns are so distorted that a forward/backward movement cannot be determined.
 POWER | Indicates if the stepper motor is currently energized. This is determined by the sum of currents in they two coils.
 IDLES | The number of times that stepper motor change status from energized to non energized.
 STEPS | The distance the stepper moved so far. Each full step forward increments this value by one and each full step backward decrements it by one. When the stepper motor is energized, the display also shows fractional steps in resolution of 1/100th of a full step.


HINT: The fractional step value cannot be reset because it is derived from the momentary currents in the two coils. Only the stepper motor driver can reset the fraction by positioning the stepper motor on an exact full step.

HINT: To swap forward/backward directions of the Analyzer use the Settings page. This does not affect the stepper motor itself.


&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.
![](./www/settings.png) | Open the Settings page.

&nbsp;

---

## The Settings Page

This is a special page that is accessible from the Home page and allows to calibrate the Analyzer. The settings are stored in a non volatile memory and are preserved when the Analyzer is turned off.

![](./www/settings_screen.png)

&nbsp;
#### Page Data

Data | Description
:------------: | :-------------
COIL&nbsp;1 | The momentary current of stepper coil 1 in Amp units. Should be zero, or very close to zero, when the stepper motor is disconnected.
COIL&nbsp;2 | The momentary current of stepper coil 2 in Amp units. Should be zero, or very close to zero, when the stepper motor is disconnected.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
SET ZERO | Used to calibrate the zero reading of the current sensors. To do so, Disconnect the stepper motor and press this button.
REVERSE STEPS DIRECTION | Changes the forward/backward step directions of the Analyzer. Does not affect the stepper motor itself. Having the correct direction is important for measurements such as the Retraction analysis.
![](./www/ok.png) | Press exist the Settings page.

&nbsp;

---

## The Speed Dial Page

This page measure the stepper speed in units of full steps per second. 

![](./www/speed_gauge_screen.png)

&nbsp;
#### Page Data

Data | Description
:------------ | :-------------
Speed&nbsp;Dial | The speed dial shows the momentary stepper speed in full steps per second units. Forward speed is on the right side and backward speed is on the left.
Speed&nbsp;Value | This field shows the speed in a numeric form. IT is updated less frequently than the dial to allow easy reading.

&nbsp;
**HINT:** The Settings page allows to swap the forward/backward directions.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clears all data.
Tap the dial |Toggles the dial's speed range.

&nbsp;

---

## The Distance Graph Page

This page shows scrolling graph with the total number of full steps vs. time.

![](./www/steps_chart_screen.png)

&nbsp;

#### Page Data

Data | Description
:------------ | :-------------
Distance&nbsp;Graph | This graph shows how the distance, in units of full steps, changes over time.
Distance&nbsp;value | The numeric value of the distance in full steps.  This fields is identical to the STEPS field in the Home page.

&nbsp;

#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clears all data.
Tap the graph | Toggles the range of the vertical axis.


&nbsp;

---

#### The Retraction Graph Page

This page is useful to analyze the retraction in of 3D printer extruder.

![](./www/retraction_graph_screen.png)

&nbsp;
**TRIVIA:** The graph above was taken while testing a direct extruder printer with [Thing #909901](https://www.thingiverse.com/thing:909901).
&nbsp;
#### Page Data

Data | Description
:------------ | :-------------
Retraction&nbsp;Graph | This graphs tracks the retraction of 3D extruder full step units.  Retraction is defined as the filament length that was inside the extruder and is now retracted outside of the extruder. 
Retraction value |  This field shows the current retraction level in a numeric form.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.
Tap the graph | Changes the range of the vertical axis.

&nbsp;

---

## The Time by Speed Page

This page shows an histogram with the distributing of time by stepper speed. That is how much time was spent in each speed range. The values are normalized such that the longest bar is always at 100%.

![](./www/time_by_speed_screen.png)

&nbsp;
#### Page Data

Data | Description
:------------ | :-------------
Time&nbsp;Histogram | The horizontal axis indicates speed in full steps per second units. The height of each bar indicate the time of all steps in that speed range, with the length of the longest bar normalized to 100%.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.

&nbsp;

---


## The Distance by Speed Page

This page shows an histogram with the the distribution of distance by stepper speed.

![](./www/distance_by_speed_screen.png)

&nbsp;
#### Page Data

Data | Description
:------------ | :-------------
Distance&nbsp;Histogram |  The horizontal axis indicates speed in full steps per second units. The height of each bar indicate the distance or number of steps in that speed range, with the length of the longest bar normalized to 100%.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.

&nbsp;

---

## The Current by Speed Page

This page measures the ability of the stepper motor's controller to maintain peak current at high speeds.

The screenshot below shows a healthy system where the peak current is virtually flat across the speed range.


![](./www/current_by_speed_screen.png)

The screenshot below shows a system that is not capable to maintain peak current and starves the stepper motor at high speeds, resulting in poor torque and possible skipped steps.


![](./www/current_by_speed2_screen.png)


&nbsp;
#### Page Data

Data | Description
:------------ | :-------------
Peak&nbsp;Current&nbsp;Histogram | The horizontal axis indicates speed in full steps per second units. The height of each bar indicates the average peak current in Amps of  steps that speed range. 

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.

&nbsp;

---

## Coil Current Patterns Page

This page shows the current patterns in the two stepper coils.

The screenshot below shows a healthy system where the signals are nice and clean and reaches the target peak current.

![](./www/signal_graph_screen.png)

&nbsp;

The screenshot below is from a poor system where high speed results in distorted and insufficient current, resulting in loss of torque and a potential for skipped steps.

![](./www/signal_graph2_screen.png)

&nbsp;

**GENERAL INFORMATION:** Each cycle of the stepper current pattern represent 4 full steps. The centers of those steps are at on the four vertical dotted lines where the currents of the two coils are identical, in the same or in  opposite polarities. The transitions between the steps are on the 4 vertical solid lines where one of the coils is at zero current. This cycle of 4 full steps is sometimes referred to as a single 'electric rotation'. The analyzer computes the exact phase on this cycle using the standard trigonometric function atan2(current1, current2) which can also be calculated manually from the current readings in the Home page.

![](./www/quadrants_plot.png)


&nbsp;

#### Page Data

Data | Description
:------------ | :-------------
Current&nbsp;graph | The horizontal axis indicates time in milliseconds. The vertical axis indicate coil current level in Amps. The two graphs show the current patterns of the respective stepper coils.

**NOTE:** The graph is updated only when sufficient stepper motor movement is detected.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.
![](./www/pause.png) | When running, pauses the display updates.
![](./www/run.png) | When paused, resume the display updates.
Tap the graph | This toggles the range of the horizontal axis.


&nbsp;

---

## Coil Current Phase Page

This page draws the current patterns of the two coils as a [Lissajous curve](https://en.wikipedia.org/wiki/Lissajous_curve) which allows to examine visually distortions such as non sinusoidal current, phase distortion, and reduced amplitude.

In an ideal system, the graph is a perfect smooth circle, that doesn't change its size when the stepper's speed changes. Patterns that significantly diverge from that indicates a sub optimal configuration and operation.

Following picture shows a pattern from a well functioning system. The graph is round, reasonable smooth, and has sufficient size which indicates good current drive. 

The horizontal axis indicates the coil 1 current and the vertical axis indicates coil 2 current. In this example, both span nicely between -2A to +2A.

![](./www/phase_screen_good.png)

&nbsp;

The screenshot below is from a poor system where high speed results in distorted and insufficient current, resulting in loss of torque and a potential for skipped steps. The pattern is far from a clean circle and the shape has reduced size.

![](./www/phase_screen_starved.png)

&nbsp;

The screenshot below shows a sub optimal operation where a a speed increase of the stepper results in small (less current and torque) and even more distorted curve.

![](./www/phase_screen_accelerating.png)


&nbsp;

The screenshot below shows a well functioning system. The smaller size of the circle compare to the example above indicates that it operates on lower current.

![](./www/phase_screen_good_small.png)


&nbsp;

The screenshot below show an operation of a stepper in full steps mode. The dimension of the pattern indicate good current feed, and the square  shape is the best a full step driver can deliver.

![](./www/phase_screen_full_steps.png)


&nbsp;

The screenshot below show an operation of a stepper in half steps mode. The dimension of the pattern indicate good current feed, and the shape is somewhere between the square shape of full step mode and the round shape of micro stepping interpolation mode.

![](./www/phase_screen_half_steps.png)


&nbsp;

#### Page Data

Data | Description
:------------ | :-------------
Lissajous&nbsp;curve | A Lissajous curve drawn from the current pattern of coil 1 (x) and coil2(y). 
Capture speed | Shows the signal capture time. Fast mode works better with fast moving steppers and slow mode works with slow moving steppers. Tap the graph to toggle.

**NOTES** 
* The graph is updated only when sufficient stepper motor movement is detected.
* To view the current pattern that resulted in the current phase pattern, use the navigation button to switch the current patters page, and vice versa.

&nbsp;
#### Page Actions

Action | Description
:------------: | :-------------
![](./www/trash.png) | Clear all data.
![](./www/pause.png) | When running, pauses the display updates.
![](./www/run.png) | When paused, resume the display updates.
Tap the graph | This toggles the capture time between fast and slow speeds.


&nbsp;

---

## Reference Design Specification

Item | Specification
:------------ | :------------- 
Power consumption | 5V, 175ma.
Power connector | micro USB
MCU | Raspberry Pi RP2040 (Pico module).
Flashing | Drag and drop to a virtual disk.
Data link | USB/Serial link available but not used.
Current measurement | +/-2.5A per coil. (+/-5A sensors optional).
PCB | 102mm x 57mm, two layers
Enclosure | 3D printed, 108.4mm x 63.4mm x 16.6mm
Display | 3.5" 480 x 320, Color, Landscape mode.
Touch screen | Capacitive
Sensor isolation | See ACS70331 data sheet
Sampling rate | 100Khz per channel.
Sampling resolution | 12bits
Current accuracy | estimated at +/- 1%
Max step rate | Software dependent. Currently 2K full steps/sec.
Step resolution | 1/100th of a full step.
User settings storage | On board FLASH.
GUI framework | LVGL library. R3G3B2 color depth.
Programming language | C++
Programming IDE | VSCode, Platformio, WizIO.
Electronic design software | Kicad.
Mechanical design software | Onshape. 


&nbsp;

---


## I want to build me one

The electronic schematic, PCB layout, gerber files, and the firmware source files and released binary are all available on this github repository.

[Schematic](./kicad/stepper_analyzer.pdf)

[BOM]()./kicad/BOM.xlsx)

&nbsp;

**Instructions:** Order the PCB with JLCPB with SMD service (see JLCPCB directory with gerbers and bom and pos files). Attach the Pico with XFasten tape and solder. Cut the bottom pins of the KF2510 connectors to 1mm length and solder from the top of the PCB only. Solder the current sensors, attach the TFT to the back of the PCB with XFasten tape and carefully connect the two FPC cables. Program the Pico by connecting it to a computer while it's bootsel button is pressed and drag he uf2 release file to the virtual disk you will see on your computer.



![](./www/assembly1.jpg)

&nbsp;

![](./www/assembly2.jpg)

&nbsp;

![](./www/assembly3.jpg)

&nbsp;

![](./www/assembly4.jpg)

&nbsp;

![](./www/assembly5.jpg)


## Mechanical
3D print the two part of the enclosure (STL files in this github repository), insert the four M2.5x3.5x3 heated inserts with a solder iron, drill the four  2.5 holes in the cover, insert the PCB to the front part, make sure the TFT seats flush with front and secure the back with four M2.5x10 hex cup screws.


## Ideas for improvements
The Analyzer is provided as a fully functional reference design and changes and the developers encourage improvement/modifications of the hardware and/or firmware are encouraged. This can include new features, new measurements and pages, extending the design to support 2 or 3 steppers at once to track the x/y/z of a 3D printer, reducing the hardware cost using less expensive screen TFT, or even eliminating the screen altogether and using the Analyzer as a dongle tethered to a computer via the existing USB/Serial link.

&nbsp;

---

## FAQ

#### Q: Can I buy from you a few units?

A: We cannot provide assembled units for the general public. For that reason, we released the design with CC Zero license with the hope that other entrepreneurs will do so.

#### Q: Where do you source the TFT panels?

A: We buy the '3.5 LCD CTP' variant here https://www.aliexpress.com/item/32862869103.html. The choice of TFT panel and the mechanical design of the PCB follows Dustin Watts' ESP32 Touchdown project https://www.tindie.com/products/dustinwattsnl/esp32-touchdown/ (with extra 2mm to the PCB's length for extra clearance for the threaded heat inserts).

#### Q: Can the firmware of the analyzer be updated?

A: Yes. New firmware is released as .uf2 files. To flash the analyzer with a compatible .uf2 follow the standard procedure of the Raspberry Pi Pico. Access to the BOTSEL button is provided by a hole at the back of the 3D printed enclosure.

#### Q: Why some footprints on the PCB are not populated?

A: The PCB design includes circuitry that helps developing firmware for the analyzer that is not necessary or helpful for normal opeation of the analyzer.

#### Q: Can the current scale of the two channels increased from +/-2.5A?

A: The PCB was designed to accept also +/-5A variants of the current sensors and the optionals resistors R8 R9 were added to allow configuring the firmware to know about the larger current scale. This however was not tried yet since most 3D printer steppers do not exceed currents of 2.5A.

#### Q: Why not a SPI based TFT?

A: We could not achieve fast enough display refresh rate with SPI so switched to 16 bit parallel data transfers. With the optimized firmware we can update rate equivalent to 200Mhz SPI. 

#### Q: Can a cheaper TFT/touch be used?
A: Yes, but it will require design changes.

#### Q: Can the TFT screen be eliminated?
A: Yes. The Analyzer has a USB/Serial output can a small Analyzer dongle can communicate with a computer program. This will require firmware changes.
  

#### Q: Can you add feature X or customization Y?

A: Most likely not. The reference design is provided for the community to change and mix.

#### Q: I want to commercialize or crowd source it, can you help me?

A: Not really. The reference design is open source and you will need to find others to help you. We don't mind though to answer questions.

#### Q: Where do I file a bug?

A: Use the github repository.

#### Q: Is it ok to connect/disconnect power to the Analyzer while my 3D printer prints?

A: Of course. The Analyzer is isolated from the stepper wires and should not interfere with the operation of the printer.

#### Q: The Analyzer uses LVGL lib, what is the license of that library?

A: At the time of writing this, the LVGL license is available here https://github.com/lvgl/lvgl/blob/master/LICENCE.txt.  The Analyzer's developers are not affiliated with LVGL.

#### Q: Is the LVGL library included in the Analyzer's repository?

A: It's not needed. LVGL is supported by platform.io so the Analyzer's project is set for platform.io to fetch LVGL automatically.

#### Q: Can the firmware be built with the standard Raspberry Pi Pico C++ SDK?

A: It should be but we didn't try it. Installing the SDK, libraries and tools using platformio and the WizIO Pico 'baremetal' distribution is easier.







