/*
 * wavemon - a wireless network monitoring aplication
 *
 * Copyright (c) 2001-2002 Jan Morgenstern <jan@jm-music.de>
 * Copyright (c) 2009      Gerrit Renker <gerrit@erg.abdn.ac.uk>
 *
 * wavemon is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * wavemon is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with wavemon; see the file COPYING.  If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#include <ncurses.h>

#include "llist.h"

#define CFNAME	".wavemonrc"
/*
 * Minimum screen dimensions.
 * The number of lines depends on the size requirements of scr_info(), which
 * is 24 + 1 lines (1 line is needed for the menubar). The number of columns
 * comes from the menubar length (10 items of length 6 plus the 'Fxx').
 * A size of 25x80 is favourable to support commandline/tty mode.
 */
#define MIN_SCREEN_LINES   25
#define MIN_SCREEN_COLS    80
/*
 * Screen layout constants.
 *
 * All windows extend over the whole screen width; the vertical number of
 * rows is reduced by one due to the menubar at the bottom of the screen.
 */
#define WAV_WIDTH	(COLS)
#define WAV_HEIGHT	(LINES-1)
/*
 * Maximum lengths/coordinates inside the bordered screen.
 *
 * The printable window area is constrained by the frame lines connecting
 * the corner points (0, 0), (0, COLS-1), (LINES-1, 0), (LINES-1, COLS-1).
 */
#define MAXXLEN		(WAV_WIDTH  - 2)
#define MAXYLEN		(WAV_HEIGHT - 2)

/*
 * Symbolic names of actions to take when crossing thresholds.
 * These actions invoke the corresponding ncurses functions.
 */
enum threshold_action {
	TA_DISABLED,
	TA_BEEP,
	TA_FLASH,
	TA_BEEP_FLASH
};

static inline void threshold_action(enum threshold_action action)
{
	if (action & TA_FLASH)
		flash();
	if (action & TA_BEEP)
		beep();
}

/*
 * Global in-memory representation of current wavemon configuration state
 */
extern struct wavemon_conf {
	char	ifname[LISTVAL_MAX];

	int	stat_iv,
		info_iv;

	int	sig_min, sig_max,
		noise_min, noise_max;

	int	lthreshold,
		hthreshold;

	int	slotsize,
		meter_decay;

	/* Boolean values which are 'char' for consistency with item->dep */
	char	random,
		override_bounds;

	char	lthreshold_action,	/* disabled|beep|flash|beep+flash */
		hthreshold_action;

	char	startup_scr;		/* info|histogram|aplist */
} conf;

/*
 * Initialisation & Configuration functions
 */
extern void getconf(int argc, char *argv[]);
extern void reinit_on_changes(void);

/*
 * Configuration items to manipulate the current configuration
 */
struct conf_item {
	char	*name,		/* name for preferences screen */
		*cfname;	/* name for ~/.wavemonrc */

	enum {			/* type of parameter */
		t_int,
		t_string,
		t_listval,
		t_switch,
		t_list,
		t_sep,		/* dummy */
		t_func		/* void (*fp) (void) */
	} type;

	union {			/* type-dependent container for value */
		int	*i;	/* t_int */
		char	*s;	/* t_string, t_listval */
		char	*b;	/*
				 * t_switch: a boolean type
				 * t_list: an enum type where 0 means "off"
				 * A pointer is needed to propagate the changes. See
				 * the 'char' types in the above wavemon_conf struct.
				 */
		void (*fp)();	/* t_func */
	} v;

	int	list;		/* list of available settings (for t_list) */
	char	*dep;		/* dependency (must be t_switch) */

	double	min,		/* value boundaries */
		max,
		inc;		/* increment for value changes */

	char	*unit;		/* name of units to display */
};

/*
 * Handling of Configuration Items
 */
extern int  conf_items;

/*
 *	Screen functions
 */
enum wavemon_screen {
	SCR_INFO,	/* F1 */
	SCR_LHIST,	/* F2 */
	SCR_APLIST,	/* F3 */
	SCR_EMPTY_F4,	/* placeholder */
	SCR_EMPTY_F5,	/* placeholder */
	SCR_EMPTY_F6,	/* placeholder */
	SCR_CONF,	/* F7 */
	SCR_HELP,	/* F8 */
	SCR_ABOUT,	/* F9 */
	SCR_QUIT	/* F10 */
};

extern enum wavemon_screen  scr_info(WINDOW *w_menu);
extern enum wavemon_screen  scr_lhist(WINDOW *w_menu);
extern enum wavemon_screen  scr_aplst(WINDOW *w_menu);
extern enum wavemon_screen  scr_conf(WINDOW *w_menu);
extern enum wavemon_screen  scr_help(WINDOW *w_menu);
extern enum wavemon_screen  scr_about(WINDOW *w_menu);

/*
 *	Ncurses definitions and functions
 */
extern WINDOW *newwin_title(int y, int h, const char *title, bool nobottom);
extern WINDOW *wmenubar(const enum wavemon_screen active);

extern void wclrtoborder(WINDOW *win);
extern void mvwclrtoborder(WINDOW *win, int y, int x);
extern void waddstr_b(WINDOW * win, const char *s);
extern void waddstr_center(WINDOW * win, int y, const char *s);

extern void waddbar(WINDOW *win, int y, float v, float min, float max,
		    char *cscale, bool rev);
extern void waddthreshold(WINDOW *win, int y, float v, float tv,
			  float minv, float maxv, char *cscale, chtype tch);
enum colour_pair {
	CP_STANDARD = 1,
	CP_SCALEHI,
	CP_SCALEMID,
	CP_SCALELOW,
	CP_WTITLE,
	CP_INACTIVE,
	CP_ACTIVE,
	CP_STATSIG,
	CP_STATNOISE,
	CP_STATSNR,
	CP_STATBKG,
	CP_STATSIG_S,
	CP_STATNOISE_S,
	CP_PREF_NORMAL,
	CP_PREF_SELECT,
	CP_PREF_ARROW
};

static inline int cp_from_scale(float value, const char *cscale, bool reverse)
{
	enum colour_pair cp;

	if (value < (float)cscale[0])
		cp = reverse ? CP_SCALEHI : CP_SCALELOW;
	else if (value < (float)cscale[1])
		cp = CP_SCALEMID;
	else
		cp = reverse ? CP_SCALELOW : CP_SCALEHI;

	return COLOR_PAIR(cp);
}

/*
 *	Wireless interfaces
 */
extern int iw_get_interface_list(void);
extern void dump_parameters(void);

/*
 *	Timers
 */
struct timer {
	unsigned long long	stime;
	unsigned long		duration;
};

extern void start_timer(struct timer *t, unsigned long d);
extern int end_timer(struct timer *t);

/*
 *	Error handling
 */
extern void fatal_error(char *format, ...);

/*
 *	Helper functions
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static inline void str_tolower(char *s)
{
	while (s && *s != '\0')
		*s++ = tolower(*s);
}

/* number of digits needed to display integer part of @val */
static inline int num_int_digits(const double val)
{
	return 1 + (val > 1.0 ? log10(val) : val < -1.0 ? log10(-val) : 0);
}

static inline int max(const int a, const int b)
{
	return a > b ? a : b;
}

/* SI units -- see units(7) */
static inline char *byte_units(const double bytes)
{
	static char result[0x100];

	if (bytes >= 1 << 30)
		sprintf(result, "%0.2lf GiB", bytes / (1 << 30));
	else if (bytes >= 1 << 20)
		sprintf(result, "%0.2lf MiB", bytes / (1 << 20));
	else if (bytes >= 1 << 10)
		sprintf(result, "%0.2lf KiB", bytes / (1 << 10));
	else
		sprintf(result, "%.0lf B", bytes);

	return result;
}

/**
 * Compute exponentially weighted moving average
 * @mavg:	old value of the moving average
 * @sample:	new sample to update @mavg
 * @weight:	decay factor for new samples, 0 < weight <= 1
 */
static inline double ewma(double mavg, double sample, double weight)
{
	return mavg == 0 ? sample : weight * mavg + (1.0 - weight) * sample;
}
