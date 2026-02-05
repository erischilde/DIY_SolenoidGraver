# GraverController
This is a fork of https://github.com/Savage-Sabrina/DIY_SolenoidGraver. 

As is standard operating procedure, i decided to make my life infintely more difficult by learning how to engrave while/after building my own engraving machine. 
I wanted to avoid standard air based gravers for couple reasons: 
- noise: i live in a rental condo unit, noise travels, compressors can be loud.
- i'm a night owl, even quiet compressors would be loud at 2 or 3am
- engraving machines are expensive, and cheap ones are cheap; since i cannot tell a good one from a bad one, i wouldn't know what i am trading off.
- i never learn that making your own is often more expensive than buying, but i still learn a lot from the process. 

The first version of this device i set about building was based off https://github.com/alastair-duncan/SuperEZGraver, and then i found the SolenoidGraver, and decided to build off both. 

Starting with the DIY_SolenoidGraver as a base, I made some changes to suit my hardware and available tools: 
- i have two devices on the go, one with an Arduino Uno that has an LCD and Button Shield. As the original, it outputs through a mosfet module.
- The second device in progress also has the option of outputting through an H bridge.
- I'm experimenting with the idea of using what would be better described as a "linear actuator" or "linear motor" than a solenoid; instead of a mild steel plunger, i'm using a stacked set of magnets. The H bridge allows me to reverse the polarity of the coil, removing the need for a spring all together.
- a third option is a kind of hybrid, with a spring to return the magnet-plunger.

I decided to try this since it appears you can generate greater acceleration with a magnetic linear actuator than depending on the steel rod stauration. 
Downsides include increased complexity of the build, but may be worth it and interesting at least. 

Solenoids, magnets, and linear motors are complicated for such simple little devices. The details of magnetic flux, shaping the plunger, material saturation, is was over my head for detailed and realistic calculation. I'm depending a lot on the internet and testing. 


---
# **Engraver Body**
- i have left in the STL and CAD files from the orginal project, though i am machining my engraver body instead. I do test with 3d printed bodies, but because i've changed so much of the solenoid component, the stl files don't really apply anymore.
- my first iteration was an aluminum tubular body, drilled out to accept the 13-30 solenoid, and then tapped at the impact side to accept the thread on the solenoid body. I used brass tube to hold a GRS style quick change tip, easy to shape and file in a locating tab.
- Second version for the larger solenoid, also had an alumnium outer body, but i put a steel sleeve around the solenoid itself. The jf-1039 is an "open frame" style solenoid, i carefully removed the metal U shaped component and used the naked solenoid.
- at the impact side, i used some M10 threaded rod, drilled out to 6.35mm diam to fit the QC holders, and a nut. A steel plug was turned and "compression fit" (read, hammered) into the bottom of the aluminum tube.
- I tapped the bottom of the steel plug to M10 as well. This is so the bit holder can be screwed in and out to adjust sizing, and using a nut on the outside to lock it in place. None of these are final. 

---
# **The solenoid**
- my initial build was based on a 13-30 solenoid from Aliexpress; the shape and format is awesome to work with, but i found it too light, but i also don't know what i'm doing. I'll keep tinkering with it for more delicate work.
- second version is based off of a JF-1039 solenoid; maybe a bit overkill but gives me something to work with. They're relatively inexpensive, and easily available.
- third version is a customized coil i hand wrapped. 

Using standard solenoids seemed ideal, since you could get them fairly cheap and then build around them, but in the end i ended up customizing or building my own. 
- tubular solenoids are fantastic for shape and having a steel outer case, and a threaded tip allowing easy installation into a body, but they can get pricy and once you start modifying the plunger, things can fall apart.
- Using typical "open frame" solenoids is uncomfortable since they're bulky and rectangular.

## **Power/voltage**
- the resistance of the solenoid defines the power usage
- even at 20% duty cycle, many small solenoids will heat up. Additionally, we tend to over-volt them to provide a better "kick", so that can add to heating issue.
- The aim is a solenoid that will operate between 12v and 24v, under 2 amps for current, ideally with enough overhead to avoid dangerous or deleterious heating. 

## **Solenoid/Actuator Coil Construction**
The math is far over my head, but some things i've found that i believe i can work with experimentally and have been working: 
1. wrapping the outside in mild steel increases the flux acting on the plunger. It shapes the direction of the magnetic field.
2. Having a steel plate/washer at the impact side again can help focus the flux in that direction.
3. If using a steel body to make the engraver body, or to wrap the coils, it may be necessary to make longitudinal slits in the frame to disrupt eddy currents. Apparently they can slow the response of the coil switching, meaning you'd be unable to acheive 40hz or higher.
4. An aluminum body for a graver would act as a heatsink.
5. One can increase the power significantly by wrapping two parallel wires around the bobbin instead of one; this would cut the resistance in half and increase the current drastically. Would have to pay close attention to heat and power usage.

## **Custom Solenoid**
(This is left over from Sabrina_Savage's build. i have not altered it and haven't gotten into detailed plunger construction yet.)
If you intend to produce your own solenoid there are some considerations that should be put in place that will make your life easier:

1. The plunger for the solenoid should be floating so as to regulate the amplitude of the impact, a smaller amplitude can lead to faster and more delicate impacts, a greater amplitude can be good for the bezel setting or steel engraving. Free floating solenoids like the 1335 above are regulated by putting some limiter in the back to reduce or increase its amplitude like some sort of screw.
1. The plunger weight should be around 6-8 grams to allow flexibility of energy and speed. A 6 g plunger with the right spring and coil can get up to 60 HZ with an amplitude of 3mm. An 8 gram plunger can even engrave steel, with a 5mm amplitude.
1. An open design of the coil to dissipate the heat will make it last and allow it to use the handpiece for longer since it won’t be too hot.
1. Make sure the wire for the coil can handle the voltage and current to avoid melting and increased temperature.
2. 
A plunger with the dimensions below and a handmade spring with a 0.2mm wire can yield great results. The advantage of this is to manufacture something that is exclusive to your needs. With these dimensions I was able to engrave steel and also fine tune it to set 1.3mm stones pave style. The 0.7 mm step is the place to fit the spring, this solenoid should be powered at 12V 25% duty cycle maximun.

![](Pictures/Aspose.Words.a1b2dc12-f32a-4c42-b293-afb59d340340.003.png)

---


---
# **Controller**
- at this time i don't have a diagram or parts list, i will work on adding that.
- 


---
***

