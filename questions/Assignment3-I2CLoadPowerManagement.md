Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements in the "Default" configuration of the profiler to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See [Shared document](https://docs.google.com/document/d/1Ro9G2Nsr_ZXDhBYJ6YyF9CPivb--6UjhHRmVhDGySag/edit?usp=sharing) for instructions.* 

1. What is the average current per period?   
   Answer: 141.59 uA

   <br>Screenshot:  
   ![Avg_current_per_period](assignment3_screenshots/Assignment3Q1_Screenshot.PNG)


2. What is the average current when the Si7021 is Load Power Management OFF?  
   Answer: 2.78 uA

   <br>Screenshot:  
   ![Avg_current_LMP_Off](assignment3_screenshots/Assignment3Q2_Screenshot.PNG)

3. What is the average current when the Si7021 is Load Power Management ON?  
   Answer: 4.32 mA

   <br>Screenshot:  
   ![Avg_current_LMP_On](assignment3_screenshots/Assignment3Q3_Screenshot.PNG)

4. How long is the Si7021 Load Power Management ON for 1 temperature reading?  
   Answer: 88 msec - this matches the delay time to enable the SI7021 set in SW and allow the device to stablize (see 2nd screenshot below).

   <br>Screenshot:  
   ![LMP_On_duration](assignment3_screenshots/Assignment3Q4_Screenshot1.PNG)

   <br>Screenshot:  
   ![enable_si7021_code_delay](assignment3_screenshots/Assignment3Q4_Screenshot2.PNG)
