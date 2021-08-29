s.waitForBoot({
	Buffer.freeAll(s);
	~path = File.getcwd;

	//////////////////////////////////////////////////

	~filename = "";  // <--- SET WAVETABLE NAME HERE. 
					 //		 MUST BE IN SAME FOLDER AS
					 //		 THIS SCRIPT.
	
	//////////////////////////////////////////////////

	~buffer = Buffer.read(s, ~path +/+ ~filename);

	s.sync;

	{Osc.ar(~buffer,MouseX.kr(30,1000).round(50),0,0.1)!2}.play;

		
	w = Window.new("my own scope", Rect(1200, 400, 400, 500));
	w.view.decorator = FlowLayout(w.view.bounds);
	c = Stethoscope.new(s, view:w.view);
	w.onClose = { c.free }; // don't forget this
	CmdPeriod.doOnce {w.close};
	w.front;
	w.alwaysOnTop = true;
})
