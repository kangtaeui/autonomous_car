# Raspberrypi RC CAR ( AUTONOMOUS DRIVE )
Use Nano editor
# Pin setting ( code )
1) Ultra Sonic 3ea ( HC-SR04 )
   
   GPIO0 = TRIG
   
   GPIO1 = ECHO
   
   GPIO5 = TRIG
   
   GPIO6 = ECHO
   
   GPIO26 = TRIG
   
   GPIO16 = ECHO

3) Motor Driver ( L298N )
   
   GPIO22 = IN1
   
   GPIO23 = IN2
   
   GPIO17 = IN3
   
   GPIO18 = IN4

   GPIO12 = ENA
   
   GPIO13 = ENB

4) Bluetooth ( HC-06 )
   
   GPIO15 = Tx -> Rx ( MCU )
   
   GPIO14 = Rx -> Tx ( MCU )

   


# Schematic
![All_sch](https://github.com/kangtaeui/autonomous_car/assets/141004174/4167fc3b-3a01-47a6-bb7d-8fc95c948a99)

# Flow chart

![sche](https://github.com/kangtaeui/autonomous_car/assets/141004174/f4d7d44e-80ba-49b7-83fd-b36869b7ec0f)


// C = CENTER // R = RIGHT // L = LEFT  // ALL = ALL DISTANCE


# Video
https://youtu.be/9TxE9km_hUE?si=-QDGubOTF2yqUKG1

