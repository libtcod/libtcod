-- load libtcod wrapper
if type(loadlib) == "function" then
	init=loadlib
else
	init=package.loadlib
end
a,b,c=init("./liblibtcod-lua.so","luaopen_libtcod")
if a == nil then
	a,b,c=init("./liblibtcod-lua.dll","luaopen_libtcod")
end
a()

-- improve the wrapper
tcod = { }
for key, value in pairs(libtcod) do
	if type(key) == "string" then                     
		local library, name = key:match("^TCOD([^_]*)_(.*)")
		if library ~= nil then
			-- replace libtcod.TCODConsole_bla() with tcod.console.bla()
			library = library:lower( )
			tcod[library] = tcod[library] or { }
			tcod[library][name] = value
		else
			library = key:match("^TCOD([^_]*)")
			if library ~= nil then
				-- replace libtcod.TCODConsole() with tcod.Console()
				tcod[library]=value
			else
				tcod[key] = value
			end
		end
	else
		tcod[key] = value
	end
end
-- now rename userdata imported from C++
for key,value in pairs(getmetatable(libtcod)[".get"]) do
	if type(key) == "string" then                     
		local library, name = key:match("^TCOD([^_]*)_(.*)")
		if library ~= nil then
			-- replace libtcod.TCODColor_grey with tcod.color.grey			
			library = library:lower( )
			tcod[library] = tcod[library] or { }
			tcod[library][name] = value()
		end
	end
end
function _alpha(alpha)
	return tcod.Alpha + math.floor(alpha*255)*(2^8)
end
function _addAlpha(alpha)
	return tcod.AddAlpha + math.floor(alpha*255)*(2^8)
end
tcod.console.Alpha=_alpha
tcod.console.AddAlpha=_addAlpha
