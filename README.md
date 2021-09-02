# wave.c
Generates .wav in SuperCollider wavetable format.<br/>
Important to only have numSamples as power-of-twos.<br/>

Usage: \<./wave\> \<file-name\> \<numSamples\> \<waveform\>

At present only outputs a fixed waveform 16bit .wav, <br/>
with customizable name and numSamples. <br/>
Available traditional waveforms are the following:<br/>
>    - sawtooth || saw ||
>    - square   || sqr || softsquare
>    - Triangle || tri ||
>    - Sine     || sin ||
>    - (Noise, to be completed)<br/>
  
  if name of waveform is omitted, you will be given one at random.
  
  In wavetable.scd is a template for playing the waveforms.
  The .wav-files must be placed in the same folder as .scd-file for 
  easy access.<br/>

 TODO: Implement opening and writing 24-bit .wav.<br/>
 TODO: opening other .wav-files and converting them.<br/> 
 
 (For test with VOsc in wavetable.scd, each wavetable must have same sample size)
