# Simple STM32F4 Vocoder


Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018
**Project Description**
-----------------------
This application receives two analog signals, a voice and a
modulator, and outputs the voice signal at the same pitch as the
modulator in real time. It can also display pitch values of each
input to an attached LCD display.
This project originally aimed to implement a phase-vocoder for
use on an STM32F4 Discovery board. Due to memory and processing
power limitations, we opted to implement a TD-PSOLA based
algorithm instead.
**Hardware connections**
------------------------
* For LCD connections, see LCD.h
* The voice input is handled by the onboard microphone
* The modulator input enters through PC2. Either use the supplied
	3.5mm audio jack or manually connect another device to PC2.
* The output signal is sent through the 3.5mm audio jack on the
	board. Note that volume can be adjusted in headphone.c
**Current State**
-----------------
Application successfully alters the pitch of the voice input if the 
input is of a relatively consistent pitch. It is best demonstrated
by using two sine waves of different frequencies, one being played
from a tuner, phone, or wave function generator into the modulator
input, and the other being played audibly into the microphone on the
board.
The application tends to get stuck in the ADC interrupt from time to
time if no signals are being sent in. We have not determined how to
avoid this yet. A simple restart fixes it. Note that loud white noise
may be output when this happens.
Obvious areas of improvement include
* Implementing phase smoothing to avoid clicks in the output signal
* Investigating memory/processing usage further to gauge how much
	larger the sample buffers can be made if unnecessary storage is
	avoided.
* Properly implementing a true phase-vocoder method using FFTs, iFFTs,
	resampling, etc. At a first pass, this was too memory-intensive to 
	be viable.
* Figuring out why the LCD can't be used concurrently with sound output.
	Something about the LCD code throws off audio playback.
	
**Credit**
----------
This program is primarily based on the realtime audio DSP project written
by Christopher Lauer. Many edits and additional comments have been made.
Pitch detection uses a slightly-altered version of Antoine Schmitt's
Dynamic Pitch Tracking Library (Dywapitchtrack).
ADC code for modulator input was based code from the STM32F4 Discovery 
Vocoder project from user Shaylin.
TD-PSOLA algorithm was adapted from Terry Kong's Phase Vocoder project.
