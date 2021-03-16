# Display BLE Example

### Overview

This example combines the LVGL display example with the peripheral_lbs example from the nRF Connect SDK. 
The display shows the state of the BLE connection (idle, advertising or connected), and visualizes the LED/Button service when connected. 
A button on the display allows the state of the button to be sent to the connected peer, and when LED updates is received from the peer it will be visualized in the GUI. 
All the gui configuration is handled by a dedicated source file, gui.c, and a separate thread is used to update the GUI independent of other threads in the system. 
A message queue is used in the GUI module to allow the application to safely send commands from anywhere in the code, including in interrupt handlers. 

### Requirements

The example is tested using the [adafruit_2_8_tft_touch_v2](https://docs.zephyrproject.org/latest/boards/shields/adafruit_2_8_tft_touch_v2/doc/index.html) display, and one of the following boards:

- nrf52840dk_nrf52840

- nrf5340dk_nrf5340_cpuapp 

NOTE: The current version of the nrf5340 board files in Zephyr does not fully support the Arduino definitions required by the adafruit display. In order to correct this a patch is included, which can be applied as follows:
  1) Move the nrf5340_display.patch file from this repo into your Zephyr base directory
  2) From the Unix bash (or Git command line in Windows) run *"git apply nrf5340_display.patch"*

### Building and Running

Build the example as normal (setting the shield explicitly is not necessary, as this is configured in the CMakeLists.txt file):
*west build -b nrf5340dk_nrf5340_cpuapp

### LVGL documentation

The full documentation for the LVGL library can be found [here](https://docs.lvgl.io/latest/en/html/index.html).

For generating images and fonts for use with LVGL please refer to the [Online Image Converter](https://lvgl.io/tools/imageconverter) and the [Online Font Converter](https://lvgl.io/tools/fontconverter). The example includes a couple of images and fonts generated using these tools. 
