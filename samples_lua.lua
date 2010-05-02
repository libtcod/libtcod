#!/usr/bin/lua
a,b,c=loadlib("./liblibtcod-lua.so","luaopen_libtcod")
a()

SAMPLE_SCREEN_WIDTH = 46
SAMPLE_SCREEN_HEIGHT = 20
SAMPLE_SCREEN_X = 20
SAMPLE_SCREEN_Y = 10
libtcod.TCODConsole_setCustomFont("data/fonts/consolas10x10_gs_tc.png", libtcod.Greyscale + libtcod.LayoutTCOD)
libtcod.TCODConsole_initRoot(80,50,"libtcod lua sample", false, libtcod.SDL)
root=libtcod.TCODConsole_root
sampleConsole = libtcod.TCODConsole (SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
first=true
rng=libtcod.TCODRandom_getInstance()

-- True colors sample (benchmark)
TOPLEFT = 1
TOPRIGHT = 2
BOTTOMLEFT = 3
BOTTOMRIGHT = 4
-- random starting colors for corners
cols={  libtcod.TCODColor(50,40,150), 
	libtcod.TCODColor(240,85,5), 
	libtcod.TCODColor(50,35,240), 
	libtcod.TCODColor(10,200,130) }
dirr={1,-1,1,1}
dirg={1,-1,-1,1}
dirb={1,1,1,-1}
function render_colors()
	if first then
		libtcod.TCODSystem_setFps(0) -- unlimited fps
		sampleConsole.clear(sampleConsole)
	end
	-- ==== slighty modify the corner colors ====
	for c=1,4,1 do
		-- move each corner color
		component=rng.getInt(rng,0,2)
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
		top = libtcod.TCODColor_Interpolate(cols[TOPLEFT], cols[TOPRIGHT],xcoef)
		bottom = libtcod.TCODColor_Interpolate(cols[BOTTOMLEFT], cols[BOTTOMRIGHT],xcoef)
		for y=0,SAMPLE_SCREEN_HEIGHT-1,1 do
			ycoef = y/(SAMPLE_SCREEN_HEIGHT-1)
			-- get the current cell color
			curColor = libtcod.TCODColor_Interpolate(top,bottom,ycoef)
			sampleConsole.setCharBackground(sampleConsole,x,y,curColor,libtcod.Set)
		end
	end
	-- ==== print the text with a random color ====
	-- get the background color at the text position
	textColor=sampleConsole.getCharBackground(sampleConsole,SAMPLE_SCREEN_WIDTH/2,5)
	-- and invert it
	textColor.Red=255-textColor.Red
	textColor.Green=255-textColor.Green
	textColor.Blue=255-textColor.Blue
	-- put random text (for performance tests) 
	for x=0,SAMPLE_SCREEN_WIDTH-1,1 do
		for y=0,SAMPLE_SCREEN_HEIGHT-1,1 do
			col=sampleConsole.getCharBackground(sampleConsole,x,y)
			col=libtcod.TCODColor_Interpolate(col,libtcod.TCODColor_black,0.5)
			c=rng.getInt(rng,97,97+25) -- 97 == 'a'
			sampleConsole.setForegroundColor(sampleConsole,col)
			sampleConsole.putChar(sampleConsole,x,y,c,libtcod.None)
		end
	end
	sampleConsole.setForegroundColor(sampleConsole,textColor)
	-- the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
	sampleConsole.setBackgroundColor(sampleConsole,libtcod.TCODColor_grey)
	sampleConsole.printRectEx(sampleConsole,SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,
		libtcod.Multiply,libtcod.CenterAlignment,
		"The Doryen library uses 24 bits colors, for both background and foreground.")
end

creditsEnd=false

while not libtcod.TCODConsole_isWindowClosed() do
	key=libtcod.TCODConsole_checkForKeypress()
	render_colors()
	first=false

	libtcod.TCODConsole_blit(sampleConsole,
		0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT,
		root, SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y)
	if not creditsEnd then
		creditsEnd = libtcod.TCODConsole_renderCredits(60, 43, false)
	end
	-- render stats
	root.setForegroundColor(root,libtcod.TCODColor_grey)
	root.printEx(root, 79, 46, libtcod.None, libtcod.RightAlignment,
		string.format("last frame : %3d ms (%3d fps)" ,	
		math.floor(libtcod.TCODSystem_getLastFrameLength() * 1000.0), 
		libtcod.TCODSystem_getFps()))
	root.printEx(root, 79, 47, libtcod.None, libtcod.RightAlignment,
		string.format("elapsed : %8d ms %4.2fs",
		libtcod.TCODSystem_getElapsedMilli(),
		libtcod.TCODSystem_getElapsedSeconds()))
	libtcod.TCODConsole_flush()
	if key.KeyCode == libtcod.Down then
			-- save screenshot
			libtcod.TCODSystem_saveScreenshot("screenshot0.png")
	end
end

