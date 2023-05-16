Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does a single LED draw when the output drive is set to "Strong" with the original code?**
BI: ((Average on current) - (Average off current)) = (4.78mA - 4.27mA) = 0.51mA

**2. After commenting out the standard output drive and uncommenting "Weak" drive, how much current does a single LED draw?**
BI: ((Average on current) - (Average off current)) = (4.77mA - 4.27mA) = 0.50mA

**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate.**
BI: The data shows that there isn't a significant difference in current when taking the average current values during an on-cycle and off-cycle for Strong and Weak drive outputs. Both currents measured are within the AEM's range (0.1uA to 95 mA) and the schematic shows both User LEDs are in series with 3K resistors. While the GPIO output drive level changes the total current supplied to the LED, the AEM current measure is unchanged as this captures current draw for the entire board and not the individual User LEDs. If a current sense line was applied to the User LEDs and the same test was conducted, you should then be able to measure the changes in current based on the GPIO drive level supplied more accurately.

**4. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with only LED1 turning on in the main loop?**
BI: With only LED1, average current is 4.75 mA and average energy is: (2.64mWh / 600.55sec) = 0.00440 mWh/sec.

**5. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with both LED1 and LED0 turning on in the main loop?**
BI: With only LED1, average current is 4.92 mA and average energy is: (2.15mWh / 470.81sec) = 0.00456 mWh/sec.
