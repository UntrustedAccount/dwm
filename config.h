#include <X11/XF86keysym.h>
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int barpadding = 6;
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 8;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 8;       /* vert inner gap between windows */
static const unsigned int gappoh    = 8;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 8;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 6;   /* systray spacing */
static const unsigned int systrayiconsize = 24; /* systray icon size in px */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
static const int topbar             = 0;     /* 0 means bottom bar */
#define ICONSIZE (bh - 2 * barpadding)   /* icon size */
#define ICONSPACING 5 /* space between icon and title */
static const unsigned int colorfultag = 1; /* 0 means use SchemeSel for selected tag */
static const int focusonwheel       = 0;
/* static const char *fonts[]          = { "Iosevka Aile:size=14:style=Regular", "Material\\-Design\\-Iconic\\-Font:style=Design-Iconic-Font:size=16", "Iosevka Nerd Font Mono:size=14:style=Regular" }; */
static const char *fonts[]          = { "Iosevka Term:size=14:style=Regular", "Material\\-Design\\-Iconic\\-Font:style=Design-Iconic-Font:size=16" };
static const char dmenufont[]       = "Iosevka Term:size=14";
static const char col_gray1[] = "#020203";
static const char col_gray2[] = "#1c252c";
static const char col_gray3[] = "#6b7078";
static const char col_gray4[] = "#e3e6eb";
static const char col_red[] = "#e05f65";
static const char col_orange[] = "#f1cf8a";
static const char col_green[] = "#78dba9";
static const char col_cyan[] = "#74bee9";
static const char col_blue[] = "#70a5eb";
static const char col_purple[] = "#c68aee";
static const char col_borderbar[] = "#020203";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_cyan, col_gray1, col_gray1 },
	[SchemeLayout] = { col_gray1, col_cyan, col_gray1 },
	[SchemeSel]  = { col_blue, col_gray1,  col_blue  },
  [SchemeTag]        = { col_gray3, col_gray1, col_gray3 },
  [SchemeTag1]       = { col_gray1, col_blue,  col_gray3 },
  [SchemeTag2]       = { col_gray1, col_red,  col_gray3 },
  [SchemeTag3]       = { col_gray1, col_orange,  col_gray3 },
  [SchemeTag4]       = { col_gray1, col_green,  col_gray3 },
  [SchemeTag5]       = { col_gray1, col_purple,  col_gray3 },
};

/* tagging */
static const char *tags[] = {"", "", "", "", ""};
static const char *alttags[] = { "", "", "", "", "" };
static const int tagschemes[] = { SchemeTag1, SchemeTag2, SchemeTag3,
                                  SchemeTag4, SchemeTag5
                                };

static const unsigned int ulinepad	= 6;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
  {"Nemo", NULL, NULL, 0, 1, -1},
  {"CEmu", NULL, NULL, 0, 1, -1},
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",      tile },    /* first entry is default */
	{ "Floating",      NULL },    /* no layout function means floating behavior */
	{ "Monocle",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m",  dmenumon,   "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3,   "-sb", col_gray1, "-sf", col_blue, "-p",  "Search:", NULL};
static const char *termcmd[]  = { "wezterm", NULL };
static const char *filecmd[]  = { "nemo", NULL };
static const char *screenshotcmd[] = {
    "/bin/sh", "-c", "maim -su | xclip -selection clipboard -t image/png", NULL};
static const char *lockscreencmd[] = {"slock", NULL};

/* Volume commands */
static const char *upvol[] = {"pamixer", "-i", "5", NULL};
static const char *downvol[] = {"pamixer", "-d", "5", NULL};
static const char *mutevol[] = {"pamixer", "-t", NULL};

/*Brightness commands*/
static const char *upbri[] = {"xbacklight", "-inc", "5", NULL};
static const char *downbri[] = {"xbacklight", "-dec", "5", NULL};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY | ShiftMask,                       XK_f, spawn,          {.v = filecmd } },
  {MODKEY | ShiftMask, XK_s, spawn, {.v = screenshotcmd}},
  {MODKEY | Mod1Mask, XK_x, spawn, {.v = lockscreencmd}},
  {0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
  {0, XF86XK_AudioMute, spawn, {.v = mutevol}},
  {0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
  {0, XF86XK_MonBrightnessUp, spawn, {.v = upbri}},
  {0, XF86XK_MonBrightnessDown, spawn, {.v = downbri}},
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
	{ MODKEY,                       XK_z,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_Left,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_Right, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Left,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Right, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

