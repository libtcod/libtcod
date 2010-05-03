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

-- True colors sample (benchmark)
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
function render_colors()
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
	-- the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
	sampleConsole:setBackgroundColor(tcod.color.grey)
	sampleConsole:printRectEx(SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,
		tcod.Multiply,tcod.CenterAlignment,
		"The Doryen library uses 24 bits colors, for both background and foreground.")
end

creditsEnd=false

while not tcod.console.isWindowClosed() do
	key=tcod.console.checkForKeypress()
	render_colors()
	first=false

	tcod.console.blit(sampleConsole,
		0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT,
		root, SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y)
	if not creditsEnd then
		creditsEnd = tcod.console.renderCredits(60, 43, false)
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
			-- save screenshot
			tcod.system.saveScreenshot("screenshot0.png")
	end
end

