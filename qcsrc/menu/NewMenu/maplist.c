#ifdef INTERFACE
CLASS(NewMenuMapList) EXTENDS(NewMenuListBox)
	METHOD(NewMenuMapList, configureNewMenuMapList, void(entity))
	ATTRIB(NewMenuMapList, rowsPerItem, float, 4)
	METHOD(NewMenuMapList, draw, void(entity))
	METHOD(NewMenuMapList, drawListBoxItem, void(entity, float, vector, float))
	METHOD(NewMenuMapList, clickListBoxItem, void(entity, float, vector))
	METHOD(NewMenuMapList, resizeNotify, void(entity, vector, vector, vector, vector))
	METHOD(NewMenuMapList, refilter, void(entity))
	METHOD(NewMenuMapList, refilterCallback, void(entity, entity))
	METHOD(NewMenuMapList, keyDown, float(entity, float, float, float))

	ATTRIB(NewMenuMapList, realFontSize, vector, '0 0 0')
	ATTRIB(NewMenuMapList, columnPreviewOrigin, float, 0)
	ATTRIB(NewMenuMapList, columnPreviewSize, float, 0)
	ATTRIB(NewMenuMapList, columnNameOrigin, float, 0)
	ATTRIB(NewMenuMapList, columnNameSize, float, 0)
	ATTRIB(NewMenuMapList, checkMarkOrigin, vector, '0 0 0')
	ATTRIB(NewMenuMapList, checkMarkSize, vector, '0 0 0')
	ATTRIB(NewMenuMapList, realUpperMargin1, float, 0)
	ATTRIB(NewMenuMapList, realUpperMargin2, float, 0)

	ATTRIB(NewMenuMapList, lastClickedMap, float, -1)
	ATTRIB(NewMenuMapList, lastClickedTime, float, 0)

	ATTRIB(NewMenuMapList, lastGametype, float, 0)
	ATTRIB(NewMenuMapList, lastFeatures, float, 0)

	ATTRIB(NewMenuMapList, origin, vector, '0 0 0')
	ATTRIB(NewMenuMapList, itemAbsSize, vector, '0 0 0')

	ATTRIB(NewMenuMapList, g_maplistCache, string, string_null)
	METHOD(NewMenuMapList, g_maplistCacheToggle, void(entity, float))
	METHOD(NewMenuMapList, g_maplistCacheQuery, float(entity, float))

	ATTRIB(NewMenuMapList, startButton, entity, NULL)

	METHOD(NewMenuMapList, loadCvars, void(entity))

	ATTRIB(NewMenuMapList, typeToSearchString, string, string_null)
	ATTRIB(NewMenuMapList, typeToSearchTime, float, 0)

	METHOD(NewMenuMapList, destroy, void(entity))
ENDCLASS(NewMenuMapList)
entity makeNewMenuMapList();
void MapList_All(entity btn, entity me);
void MapList_None(entity btn, entity me);
void MapList_LoadMap(entity btn, entity me);
#endif

#ifdef IMPLEMENTATION
void destroyNewMenuMapList(entity me)
{
	MapInfo_Shutdown();
}

entity makeNewMenuMapList()
{
	entity me;
	me = spawnNewMenuMapList();
	me.configureNewMenuMapList(me);
	return me;
}

void configureNewMenuMapListNewMenuMapList(entity me)
{
	me.configureNewMenuListBox(me);
	me.refilter(me);
}

void loadCvarsNewMenuMapList(entity me)
{
	me.refilter(me);
}

float g_maplistCacheQueryNewMenuMapList(entity me, float i)
{
	return stof(substring(me.g_maplistCache, i, 1));
}
void g_maplistCacheToggleNewMenuMapList(entity me, float i)
{
	string a, b, c, s, bspname;
	float n;
	s = me.g_maplistCache;
	if not(s)
		return;
	b = substring(s, i, 1);
	if(b == "0")
		b = "1";
	else if(b == "1")
		b = "0";
	else
		return; // nothing happens
	a = substring(s, 0, i);
	c = substring(s, i+1, strlen(s) - (i+1));
	strunzone(s);
	me.g_maplistCache = strzone(strcat(a, b, c));
	// TODO also update the actual cvar
	if not((bspname = MapInfo_BSPName_ByID(i)))
		return;
	if(b == "1")
		cvar_set("g_maplist", strcat(bspname, " ", CVAR_STR(g_maplist)));
	else
	{
		s = "";
		n = tokenize_console(CVAR_STR(g_maplist));
		for(i = 0; i < n; ++i)
			if(argv(i) != bspname)
				s = strcat(s, " ", argv(i));
		cvar_set("g_maplist", substring(s, 1, strlen(s) - 1));
	}
}

void drawNewMenuMapList(entity me)
{
	if(me.startButton)
		me.startButton.disabled = ((me.selectedItem < 0) || (me.selectedItem >= me.nItems));
	drawListBox(me);
}

void resizeNotifyNewMenuMapList(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	me.itemAbsSize = '0 0 0';
	resizeNotifyNewMenuListBox(me, relOrigin, relSize, absOrigin, absSize);

	me.realFontSize_y = me.fontSize / (me.itemAbsSize_y = (absSize_y * me.itemHeight));
	me.realFontSize_x = me.fontSize / (me.itemAbsSize_x = (absSize_x * (1 - me.controlWidth)));
	me.realUpperMargin1 = 0.5 * (1 - 2.5 * me.realFontSize_y);
	me.realUpperMargin2 = me.realUpperMargin1 + 1.5 * me.realFontSize_y;

	me.columnPreviewOrigin = 0;
	me.columnPreviewSize = me.itemAbsSize_y / me.itemAbsSize_x * 4 / 3;
	me.columnNameOrigin = me.columnPreviewOrigin + me.columnPreviewSize + me.realFontSize_x;
	me.columnNameSize = 1 - me.columnPreviewSize - 2 * me.realFontSize_x;

	me.checkMarkSize = (eX * (me.itemAbsSize_y / me.itemAbsSize_x) + eY) * 0.5;
	me.checkMarkOrigin = eY + eX * (me.columnPreviewOrigin + me.columnPreviewSize) - me.checkMarkSize;
}

void clickListBoxItemNewMenuMapList(entity me, float i, vector where)
{
	if(where_x <= me.columnPreviewOrigin + me.columnPreviewSize)
	{
		if(where_x >= 0)
			me.g_maplistCacheToggle(me, i);
	}
	if(where_x >= me.columnNameOrigin)
		if(where_x <= 1)
			{
				if(i == me.lastClickedMap)
					if(time < me.lastClickedTime + 0.3)
					{
						// DOUBLE CLICK!
						// pop up map info screen
						main.mapInfoDialog.loadMapInfo(main.mapInfoDialog, i, me);
						DialogOpenButton_Click_withCoords(NULL, main.mapInfoDialog, me.origin + eX * (me.columnNameOrigin * me.size_x) + eY * ((me.itemHeight * i - me.scrollPos) * me.size_y), eY * me.itemAbsSize_y + eX * (me.itemAbsSize_x * me.columnNameSize));
						return;
					}
				me.lastClickedMap = i;
				me.lastClickedTime = time;
			}
}

void drawListBoxItemNewMenuMapList(entity me, float i, vector absSize, float isSelected)
{
	// layout: Ping, Map name, Map name, NP, TP, MP
	string s;
	float p;
	float theAlpha;
	float included;

	if(!MapInfo_Get_ByID(i))
		return;

	included = me.g_maplistCacheQuery(me, i);
	if(included || isSelected)
		theAlpha = SKINALPHA_MAPLIST_INCLUDEDFG;
	else
		theAlpha = SKINALPHA_MAPLIST_NOTINCLUDEDFG;

	if(isSelected)
		draw_Fill('0 0 0', '1 1 0', SKINCOLOR_LISTBOX_SELECTED, SKINALPHA_LISTBOX_SELECTED);
	else if(included)
		draw_Fill('0 0 0', '1 1 0', SKINCOLOR_MAPLIST_INCLUDEDBG, SKINALPHA_MAPLIST_INCLUDEDBG);

	s = ftos(p);
	draw_Picture(me.columnPreviewOrigin * eX, strcat("/maps/", MapInfo_Map_bspname), me.columnPreviewSize * eX + eY, '1 1 1', theAlpha);
	if(included)
		draw_Picture(me.checkMarkOrigin, "checkmark", me.checkMarkSize, '1 1 1', 1);
	s = draw_TextShortenToWidth(strcat(MapInfo_Map_bspname, ": ", MapInfo_Map_title), me.columnNameSize / me.realFontSize_x, 0);
	draw_Text(me.realUpperMargin1 * eY + (me.columnNameOrigin + 0.00 * (me.columnNameSize - draw_TextWidth(s, 0) * me.realFontSize_x)) * eX, s, me.realFontSize, SKINCOLOR_MAPLIST_TITLE, theAlpha, 0);
	s = draw_TextShortenToWidth(MapInfo_Map_author, me.columnNameSize / me.realFontSize_x, 0);
	draw_Text(me.realUpperMargin2 * eY + (me.columnNameOrigin + 1.00 * (me.columnNameSize - draw_TextWidth(s, 0) * me.realFontSize_x)) * eX, s, me.realFontSize, SKINCOLOR_MAPLIST_AUTHOR, theAlpha, 0);

	MapInfo_ClearTemps();
}

void refilterNewMenuMapList(entity me)
{
	float i, j, n;
	string s;
	float gt, f;
	gt = gametype_ID_to_MapID(gametype_GetMenu());
	f = MapInfo_CurrentFeatures();
	MapInfo_FilterGametype(gt, f, MapInfo_RequiredFlags(), MapInfo_ForbiddenFlags(), 0);
	me.nItems = MapInfo_count;
	for(i = 0; i < MapInfo_count; ++i)
		draw_PreloadPicture(strcat("/maps/", MapInfo_BSPName_ByID(i)));
	if(me.g_maplistCache)
		strunzone(me.g_maplistCache);
	s = "0";
	for(i = 1; i < MapInfo_count; i *= 2)
		s = strcat(s, s);
	n = tokenize_console(CVAR_STR(g_maplist));
	for(i = 0; i < n; ++i)
	{
		j = MapInfo_FindName(argv(i));
		if(j >= 0)
			s = strcat(
				substring(s, 0, j),
				"1",
				substring(s, j+1, MapInfo_count - (j+1))
			);
	}
	me.g_maplistCache = strzone(s);
	if(gt != me.lastGametype || f != me.lastFeatures)
	{
		me.lastGametype = gt;
		me.lastFeatures = f;
		me.setSelected(me, 0);
	}
}

void refilterCallbackNewMenuMapList(entity me, entity cb)
{
	me.refilter(me);
}

void MapList_All(entity btn, entity me)
{
	float i;
	string s;
	MapInfo_FilterGametype(MAPINFO_TYPE_ALL, 0, 0, MAPINFO_FLAG_FORBIDDEN, 0); // all
	s = "";
	for(i = 0; i < MapInfo_count; ++i)
		s = strcat(s, " ", MapInfo_BSPName_ByID(i));
	cvar_set("g_maplist", substring(s, 1, strlen(s) - 1));
	me.refilter(me);
}

void MapList_None(entity btn, entity me)
{
	cvar_set("g_maplist", "");
	me.refilter(me);
}

void MapList_LoadMap(entity btn, entity me)
{
	string m;
	float i;

	i = me.selectedItem;

	if(btn.parent.instanceOfNewMenuMapInfoDialog)
	{
		i = btn.parent.currentMapIndex;
		Dialog_Close(btn, btn.parent);
	}

	if(i >= me.nItems || i < 0)
		return;

	m = MapInfo_BSPName_ByID(i);
	if not(m)
	{
		print("Huh? Can't play this (m is NULL). Refiltering so this won't happen again.\n");
		return;
	}
	if(MapInfo_CheckMap(m, gametype_ID_to_MapID(gametype_GetMenu())))
	{
		localcmd("\nmenu_loadmap_prepare\n");
		if(CVAR(menu_use_default_hostname))
			localcmd("hostname \"", strdecolorize(CVAR_STR(_cl_name)), "'s Game server\"\n");
		MapInfo_LoadMap(m);
	}
	else
	{
		print("Huh? Can't play this (invalid game type). Refiltering so this won't happen again.\n");
		me.refilter(me);
	}
}

float keyDownNewMenuMapList(entity me, float scan, float ascii, float shift)
{
	string ch, save;
	if(scan == K_ENTER)
	{
		// pop up map info screen
		main.mapInfoDialog.loadMapInfo(main.mapInfoDialog, me.selectedItem, me);
		DialogOpenButton_Click_withCoords(NULL, main.mapInfoDialog, me.origin + eX * (me.columnNameOrigin * me.size_x) + eY * ((me.itemHeight * me.selectedItem - me.scrollPos) * me.size_y), eY * me.itemAbsSize_y + eX * (me.itemAbsSize_x * me.columnNameSize));
	}
	else if(scan == K_SPACE)
	{
		me.g_maplistCacheToggle(me, me.selectedItem);
	}
	else if(ascii == 43) // +
	{
		if not(me.g_maplistCacheQuery(me, me.selectedItem))
			me.g_maplistCacheToggle(me, me.selectedItem);
	}
	else if(ascii == 45) // -
	{
		if(me.g_maplistCacheQuery(me, me.selectedItem))
			me.g_maplistCacheToggle(me, me.selectedItem);
	}
	else if(scan == K_BACKSPACE)
	{
		if(time < me.typeToSearchTime)
		{
			save = substring(me.typeToSearchString, 0, strlen(me.typeToSearchString) - 1);
			if(me.typeToSearchString)
				strunzone(me.typeToSearchString);
			me.typeToSearchString = strzone(save);
			me.typeToSearchTime = time + 0.5;
			if(strlen(me.typeToSearchString))
			{
				MapInfo_FindName(me.typeToSearchString);
				if(MapInfo_FindName_firstResult >= 0)
					me.setSelected(me, MapInfo_FindName_firstResult);
			}
		}
	}
	else if(ascii >= 32 && ascii != 127)
	{
		ch = chr(ascii);
		if(time > me.typeToSearchTime)
			save = ch;
		else
			save = strcat(me.typeToSearchString, ch);
		if(me.typeToSearchString)
			strunzone(me.typeToSearchString);
		me.typeToSearchString = strzone(save);
		me.typeToSearchTime = time + 0.5;
		MapInfo_FindName(me.typeToSearchString);
		if(MapInfo_FindName_firstResult >= 0)
			me.setSelected(me, MapInfo_FindName_firstResult);
	}
	else
		return keyDownListBox(me, scan, ascii, shift);
	return 1;
}

#endif
