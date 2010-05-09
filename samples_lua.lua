#!/usr/bin/lua
dofile "libtcodlua.lua"

SAMPLE_SCREEN_WIDTH = 46
SAMPLE_SCREEN_HEIGHT = 20
SAMPLE_SCREEN_X = 20
SAMPLE_SCREEN_Y = 10
tcod.console.setCustomFont("data/fonts/consolas10x10_gs_tc.png", tcod.Greyscale + tcod.LayoutTCOD)
tcod.console.initRoot(80,50,"libtcod lua sample", false, tcod.SDL)
root=libtcod.TCODConsole_root
sampleConsole = tcod.Console(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
first=true
rng=tcod.random.getInstance()

-- ******************************
-- True colors sample (benchmark)
-- ******************************
TOPLEFT = 1
TOPRIGHT = 2
BOTTOMLEFT = 3
BOTTOMRIGHT = 4
-- random starting colors for corners
cols={  tcod.Color(50,40,150), 
	tcod.Color(240,85,5), 
	tcod.Color(50,35,240), 
	tcod.Color(10,200,130) }
dirr={1,-1,1,1}
dirg={1,-1,-1,1}
dirb={1,1,1,-1}
function render_colors(key)
	if first then
		tcod.system.setFps(0) -- unlimited fps
		sampleConsole:clear()
	end
	-- ==== slighty modify the corner colors ====
	for c=1,4,1 do
		-- move each corner color
		component=rng:getInt(0,2)
		if component == 0 then
			cols[c].Red = cols[c].Red + 5*dirr[c]
			if cols[c].Red == 255 then dirr[c]=-1
			elseif cols[c].Red==0 then dirr[c]=1
			end
		elseif component == 1 then
			cols[c].Green = cols[c].Green + 5*dirg[c]
			if cols[c].Green == 255  then dirg[c]=-1
			elseif cols[c].Green==0 then dirg[c]=1
			end
		elseif component == 2 then
			cols[c].Blue = cols[c].Blue + 5*dirb[c]
			if cols[c].Blue == 255 then dirb[c]=-1
			elseif cols[c].Blue==0 then dirb[c]=1
			end
		end
	end
	-- ==== scan the whole screen, interpolating corner colors ====
	for x=0,SAMPLE_SCREEN_WIDTH-1,1 do
		xcoef = x/(SAMPLE_SCREEN_WIDTH-1)
		-- get the current column top and bottom colors
		top = tcod.color.Interpolate(cols[TOPLEFT], cols[TOPRIGHT],xcoef)
		bottom = tcod.color.Interpolate(cols[BOTTOMLEFT], cols[BOTTOMRIGHT],xcoef)
		for y=0,SAMPLE_SCREEN_HEIGHT-1,1 do
			ycoef = y/(SAMPLE_SCREEN_HEIGHT-1)
			-- get the current cell color
			curColor = tcod.color.Interpolate(top,bottom,ycoef)
			sampleConsole:setCharBackground(x,y,curColor,tcod.Set)
		end
	end
	-- ==== print the text with a random color ====
	-- get the background color at the text position
	textColor=sampleConsole:getCharBackground(SAMPLE_SCREEN_WIDTH/2,5)
	-- and invert it
	textColor.Red=255-textColor.Red
	textColor.Green=255-textColor.Green
	textColor.Blue=255-textColor.Blue
	-- put random text (for performance tests) 
	for x=0,SAMPLE_SCREEN_WIDTH-1,1 do
		for y=0,SAMPLE_SCREEN_HEIGHT-1,1 do
			col=sampleConsole:getCharBackground(x,y)
			col=tcod.color.Interpolate(col,tcod.color.black,0.5)
			c=rng:getInt(97,97+25) -- 97 == 'a'
			sampleConsole:setForegroundColor(col)
			sampleConsole:putChar(x,y,c,tcod.None)
		end
	end
	sampleConsole:setForegroundColor(textColor)
	-- the background behind the text is slightly darkened using the Multiply flag
	sampleConsole:setBackgroundColor(tcod.color.grey)
	sampleConsole:printRectEx(SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,
		tcod.Multiply,tcod.CenterAlignment,
		"The Doryen library uses 24 bits colors, for both background and foreground.")
end

-- ***************************
-- offscreen console sample
-- ***************************
-- a console to store the background screen
screenshot = tcod.Console(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT)
-- the offscreen console to blit on top of the background
secondary = tcod.Console(SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2)
off_init=false
off_counter=0
off_x=0
off_y=0
off_xdir=1
off_ydir=1
function render_offscreen(key) 
	if not off_init then
		-- draw the offscreen console only on first frame
		off_init=true
		secondary:printFrame(0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,false,tcod.Set,"Offscreen console")
		secondary:printRectEx(SAMPLE_SCREEN_WIDTH/4,2,SAMPLE_SCREEN_WIDTH/2-2,SAMPLE_SCREEN_HEIGHT/2,
			tcod.None,tcod.CenterAlignment,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.")
	end
	if first then
		tcod.system.setFps(30) -- fps limited to 30
		-- get a "screenshot" of the current sample screen
		tcod.console.blit(sampleConsole,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
							screenshot,0,0);
	end
	off_counter = off_counter + 1
	if off_counter % 20 == 0 then
		-- move the secondary screen every 2 seconds
		off_x = off_x + off_xdir
		off_y = off_y + off_ydir
		if off_x >= SAMPLE_SCREEN_WIDTH/2+5 then off_xdir = -1 
		elseif off_x <= -5 then off_xdir = 1 end
		if off_y >= SAMPLE_SCREEN_HEIGHT/2+5 then off_ydir = -1
		elseif off_y <= -5 then off_ydir = 1 end
	end
	-- restore the initial screen
	tcod.console.blit(screenshot,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
					sampleConsole,0,0)
	-- blit the overlapping screen
	tcod.console.blit(secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,
					sampleConsole,off_x,off_y,1.0,0.75)
end

-- *******************
-- Line drawing sample
-- ******************* 
bk = tcod.Console(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT) -- colored background
bkFlag=tcod.Set
line_init=false
flagNames={
		"None",
		"Set",
		"Multiply",
		"Lighten",
		"Darken",
		"Screen",
		"Dodge",
		"Burn",
		"Add",
		"AddAlpha",
		"Burn",
		"Overlay",
		"Alpha"
}
function render_lines(key) 
	if key.KeyCode == tcod.Enter or key.KeyCode == tcod.KeypadEnter then
		-- switch to the next blending mode
		bkFlag = bkFlag+1
		if bkFlag % 256 > tcod.Alpha then bkFlag=tcod.None end
	end
	if bkFlag % 256 == tcod.Alpha then
		-- for the alpha mode, update alpha every frame
		alpha = (1.0+math.cos(tcod.system.getElapsedSeconds()*2))/2.0
		bkFlag=tcod.console.Alpha(alpha)
	elseif bkFlag % 256 == tcod.AddAlpha then
		-- for the add alpha mode, update alpha every frame
		alpha = (1.0+math.cos(tcod.system.getElapsedSeconds()*2))/2.0
		bkFlag=tcod.console.AddAlpha(alpha)
	end
	if not line_init then
		-- initialize the colored background
		for x=0,SAMPLE_SCREEN_WIDTH,1 do
			for y=0,SAMPLE_SCREEN_HEIGHT,1 do
				col = tcod.Color(0,0,0)
				col.Red = x* 255 / (SAMPLE_SCREEN_WIDTH-1)
				col.Green = (x+y)* 255 / (SAMPLE_SCREEN_WIDTH-1+SAMPLE_SCREEN_HEIGHT-1)
				col.Blue = y* 255 / (SAMPLE_SCREEN_HEIGHT-1)
				bk:setCharBackground(x,y,col)
			end
		end
		line_init=true
	end
	if first then
		tcod.system.setFps(30) -- fps limited to 30
		sampleConsole:setForegroundColor(tcod.color.white)
	end
	-- blit the background
	tcod.console.blit(bk,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,sampleConsole,0,0)
	-- render the gradient
	recty=(SAMPLE_SCREEN_HEIGHT-2)*((1.0+math.cos(tcod.system.getElapsedSeconds()))/2.0)
	for x=0,SAMPLE_SCREEN_WIDTH,1 do
		col = tcod.Color(0,0,0)
		col.Red=x*255/SAMPLE_SCREEN_WIDTH
		col.Green=x*255/SAMPLE_SCREEN_WIDTH
		col.Blue=x*255/SAMPLE_SCREEN_WIDTH
		sampleConsole:setCharBackground(x,recty,col,bkFlag)
		sampleConsole:setCharBackground(x,recty+1,col,bkFlag)
		sampleConsole:setCharBackground(x,recty+2,col,bkFlag)
	end
	-- calculate the segment ends
	angle = tcod.system.getElapsedSeconds()*2.0
	cosAngle=math.cos(angle)
	sinAngle=math.sin(angle)
	xo = SAMPLE_SCREEN_WIDTH/2*(1 + cosAngle)
	yo = SAMPLE_SCREEN_HEIGHT/2 + sinAngle * SAMPLE_SCREEN_WIDTH/2
	xd = SAMPLE_SCREEN_WIDTH/2*(1 - cosAngle)
	yd = SAMPLE_SCREEN_HEIGHT/2 - sinAngle * SAMPLE_SCREEN_WIDTH/2

	-- render the line
	tcod.line.init(xo,yo,xd,yd)
	x=xo
	y=yo
	repeat
		if x >= 0 and y >= 0 and x < SAMPLE_SCREEN_WIDTH and y < SAMPLE_SCREEN_HEIGHT then
			sampleConsole:setCharBackground(x,y,tcod.color.lightBlue,bkFlag)
		end
		lineEnd,x,y=tcod.line.step(x,y)
	until lineEnd
	-- print the current flag
	sampleConsole:print(2,2,string.format("%s (ENTER to change)",flagNames[(bkFlag%256)+1]))
end


creditsEnd=false

-- list of samples
samples = {
	{ name = "  True colors        ", render = render_colors },
	{ name = "  Offscreen console  ", render = render_offscreen },
	{ name = "  Line drawing       ", render = render_lines }
}
nbSamples = table.getn(samples)
curSample = 1

while not tcod.console.isWindowClosed() do
	key=tcod.console.checkForKeypress()

	-- render current sample
	samples[curSample].render(key)
	first=false

	-- and blit it on the root console
	tcod.console.blit(sampleConsole,
		0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT,
		root, SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y)
	if not creditsEnd then
		creditsEnd = tcod.console.renderCredits(60, 43, false)
	end
	-- print the list of samples
	for i=1,nbSamples,1 do
		if i == curSample then
			-- set colors for currently selected sample
			root:setForegroundColor(tcod.color.white)
			root:setBackgroundColor(tcod.color.lightBlue)
		else
			-- set colors for other samples
			root:setForegroundColor(tcod.color.grey)
			root:setBackgroundColor(tcod.color.black)
		end
		-- print the sample name
		root:printEx(2,46-(nbSamples-i),tcod.Set,tcod.LeftAlignment,samples[i].name)
	end	
	-- render stats
	root:setForegroundColor(tcod.color.grey)
	root:printEx(79, 46, tcod.None, tcod.RightAlignment,
		string.format("last frame : %3d ms (%3d fps)" ,	
		math.floor(tcod.system.getLastFrameLength() * 1000.0), 
		tcod.system.getFps()))
	root:printEx(79, 47, tcod.None, tcod.RightAlignment,
		string.format("elapsed : %8d ms %4.2fs",
		tcod.system.getElapsedMilli(),
		tcod.system.getElapsedSeconds()))
	tcod.console.flush()
	if key.KeyCode == tcod.Down then
			-- down arrow : next sample
			curSample = (curSample+1) 
			if curSample == nbSamples+1 then curSample = 1 end
			first=true
	elseif key.KeyCode == tcod.Up then
			-- up arrow : previous sample
			curSample = curSample - 1
			if curSample == 0 then curSample = nbSamples end
			first=true
	elseif key.KeyCode == tcod.PrintScreen then
			-- save screenshot
			tcod.system.saveScreenshot("screenshot0.png")
	end
end

