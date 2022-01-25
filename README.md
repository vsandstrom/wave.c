# wave.c

Generates a wave-file in SuperCollider wavetable format.<br/>
Important to only have a value of the number of samples as a power-of-two.<br/>

````bash
./wave <output name (excluding '.wav')> <number of samples> <type of waveform>
``````

At present this program outputs a wave-file with a fixed waveform at 24bit, <br/>
and a setable length in samples, with a customizable name. <br/>
Available traditional waveforms are the following:<br/>

* 'sawtooth' or 'saw' 
* 'square' or 'sqr' or 'softsquare'
* 'triangle' or 'tri'
* 'sine' or 'sin'
* (noise, to be completed)<br/>
  
If a type of waveform is omitted, you will be given one at random.
  
In wavetable.scd is a template for playing the waveforms.
The .wav-files must be placed in the same folder as .scd-file for 
easy access.<br/>

TODO: Implement opening and writing 24-bit .wav.<br/>
TODO: opening other .wav-files and converting them into SuperCollider wavetable format.<br/> 
 
 (For test with VOsc in wavetable.scd, each wavetable must have same sample size)

