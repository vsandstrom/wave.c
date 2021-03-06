s.waitForBoot({
	var path, filename, buffer;
	Buffer.freeAll(s);
	path = File.getcwd;

	//////////////////////////////////////////////////

	filename = "";  // <--- SET WAVETABLE NAME HERE. 
					 //		 MUST BE IN SAME FOLDER AS
					 //		 THIS SCRIPT.
	
	//////////////////////////////////////////////////

	buffer = Buffer.read(s, path +/+ filename);

	s.sync;

	{Osc.ar(buffer,MouseX.kr(30,1000).round(50),0,0.1)!2}.play;


	// Vector Synthesis: 
	// Uncomment and fill empty strings with 
	// 4 different wavetables.
	// (Comment out line 13 -17 before running)

	// Buffer.read( s, path +/+ "1tri.wav" );
	// Buffer.read( s, path +/+ "1saw.wav" );
	// Buffer.read( s, path +/+ "1sqr.wav" );
	// Buffer.read( s, path +/+ "1sin.wav" );

	// s.sync;
	
	// {VOsc.ar(0 + MouseY.kr(0, 3),MouseX.kr(30,1000).round(50),0,0.1)!2}.play;


	// Oscilloscope 
		
	w = Window.new("my own scope", Rect(1200, 400, 400, 500));
	w.view.decorator = FlowLayout(w.view.bounds);
	c = Stethoscope.new(s, view:w.view);
	w.onClose = { c.free }; // don't forget this
	CmdPeriod.doOnce {w.close};
	w.front;
	w.alwaysOnTop = true;
})
