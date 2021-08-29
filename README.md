# wave.c
Generates .wav in SuperCollider wavetable format.
Important to only have numSamples as power-of-twos.
<wave> <file-name> <numSamples> <waveform>

At present only outputs a fixed waveform, with customizable name and numSamples.
Waveforms that are available are the traditional:
>    - sawtooth || saw ||
>    - square   || sqr || softsquare
>    - Triangle || tri ||
>    - Sine     || sin ||
>    - (Noise, to be completed)<br/>
  
  if name of waveform is omitted, you will be given one at random.
  
  In wavetable.scd is template for playing waveforms.
  The .wav-files must be placed in same folder as .scd-file for 
  easy access.

 
