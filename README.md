# SevenSeg  
  
A library to control a 7 segment (8 including the decimal place) display using a shift register.  
    
## Wiring Details Example

### 7 Segment Display Layout  

```
Top Row  
1,a,f,2,3,b  
Bottom Row  
e,d,dp,c,g,4    

```  
   AAA  
  F   B  
  F   B  
   GGG  
  E   C  
  E   C   
   DDD  DP  
```  
### Shift Register Layout 

```
          SN74HC595
           ____  ___
   Out1  |    \/   | V+  
   Out2  |         | Out0  
   Out3  |         | Data  
   Out4  |         | OE (GND or ~ for brightness 0 = high)
   Out5  |         | LATCH  
   Out6  |         | CLOCK  
   Out7  |         | MR (V+)  
   GND   |_________| LED7' (not needed)  
```
   
### Wiring Explanation  
  
Esentialy the 4 digits pins are controlled directly by the arduino while the segment pins are controlled via the Shift register  
  
7Seg Top Pin 1 -> controls digit 1 -> via resistor to Arduino Pin "Digit Pin 1"  
7Seg Top Pin 2 -> controls A seg   -> Shift Register Out0  
7Seg Top Pin 3 -> controls F seg   -> Shift Register Out5  
7Seg Top Pin 4 -> controls digit 2 -> via resistor to arudino pin "Digit Pin 2"  
7Seg Top Pin 5 -> controls digit 3 -> via resistor to arduino pin "Digit Pin 3"  
7Seg Top Pin 6 -> controls B seg   -> Shift Register Out1  
7Seg Bottom Pin 1 -> controls E seg -> Shift Register Out4  
7Seg Bottom Pin 2 -> controls D seg -> Shift Register Out3  
7Seg Bottom Pin 3 -> controls decimal -> Shift Register Out7  
7Seg Bottom Pin 4 -> controls C seg -> Shift Register Out2  
7Seg Bottom Pin 5 -> controls G seg -> Shift Register Out6  
7Seg Bottom Pin 6 -> controls digit 4 -> via resistor to arduino pin "Digit Pin 4"  
Shift Register V,GND -> power and ground  
Shift Register LATCH,CLOCK,DATA -> arudino  

The end result is 7 pins used on the arudino rather than 12  