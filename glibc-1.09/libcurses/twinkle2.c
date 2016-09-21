#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>

/*
 * the idea for this program was a product of the imagination of
 * Kurt Schoens.  Not responsible for minds lost or stolen.
 */

# define        NCOLS   80
# define        NLINES  24
# define        MAXPATTERNS     4

typedef struct {
        int     y, x;
} LOCS;

LOCS    Layout[NCOLS * NLINES]; /* current board layout */

int     Pattern,                /* current pattern number */
        Numstars;               /* number of stars in pattern */

char    *getenv();

int     die();


extern int	_putchar();

main()
{
	register char	*sp;

	srand(getpid());		/* initialize random sequence */

	if (isatty(0)) {
	       gettmode();
	       if ((sp = getenv("TERM")) != NULL)
		       setterm(sp);
		signal(SIGINT, exit);
	}
	else {
		printf("Need a terminal on %d\n", _tty_ch);
		exit(1);
	}
	_puts(TI);
	_puts(VS);

	noecho();
	nonl();
	tputs(CL, 24, _putchar);
	for (;;) {
		makeboard();		/* make the board setup */
		puton('*');		/* put on '*'s */
		puton(' ');		/* cover up with ' 's */
	}
}

puton(ch)
char	ch;
{
	register LOCS	*lp;
	register int		r;
	register LOCS	*end;
	LOCS		temp;
	static int	lasty, lastx;

	end = &Layout[Numstars];
	for (lp = Layout; lp < end; lp++) {
		r = rand() % Numstars;
		temp = *lp;
		*lp = Layout[r];
		Layout[r] = temp;
	}

	for (lp = Layout; lp < end; lp++)
			/* prevent scrolling */
		if (!AM || (lp->y < NLINES - 1 || lp->x < NCOLS - 1)) {
			mvcur(lasty, lastx, lp->y, lp->x);
			putchar(ch);
			lasty = lp->y;
			if ((lastx = lp->x + 1) >= NCOLS)
				if (AM) {
					lastx = 0;
					lasty++;
				}
				else
					lastx = NCOLS - 1;
		}
}

/*
 * Make the current board setup.  It picks a random pattern and
 * calls ison() to determine if the character is on that pattern
 * or not.
 */
makeboard()
{
        reg int         y, x;
        reg LOCS        *lp;

        Pattern = rand() % MAXPATTERNS;
        lp = Layout;
        for (y = 0; y < NLINES; y++)
                for (x = 0; x < NCOLS; x++)
                        if (ison(y, x)) {
                                lp->y = y;
                                lp->x = x;
                                lp++;
                        }
        Numstars = lp - Layout;
}

/*
 * Return TRUE if (y, x) is on the current pattern.
 */
ison(y, x)
reg int y, x; {

        switch (Pattern) {
          case 0:       /* alternating lines */
                return !(y & 01);
          case 1:       /* box */
                if (x >= LINES && y >= NCOLS)
                        return FALSE;
                if (y < 3 || y >= NLINES - 3)
                        return TRUE;
                return (x < 3 || x >= NCOLS - 3);
          case 2:       /* holy pattern! */
                return ((x + y) & 01);
          case 3:       /* bar across center */
                return (y >= 9 && y <= 15);
        }
        /* NOTREACHED */
}

