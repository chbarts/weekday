#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _GNU_SOURCE
#include <getopt.h>

static int zeller_gregorian(int m, int d, int y)
{
    int K, J, res;

    if (m < 3) {
        m += 12;
        y--;
    }

    K = y % 100;
    J = y / 100;

    res = (d + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 - 2 * J) % 7;

    return (res + 6) % 7;
}

static int zeller_julian(int m, int d, int y)
{
    int K, J, res;

    if (m < 3) {
        m += 12;
        y--;
    }

    K = y % 100;
    J = y / 100;

    res = (d + (13 * (m + 1)) / 5 + K + K / 4 + 5 - J) % 7;

    return (res + 6) % 7;
}

static int isleap(int y, int j)
{
    if (j) {
        if ((y % 4) == 0) {
            return 1;
        } else {
            return  0;
        }
    }

    if ((y % 400) == 0) {
        return 1;
    } else if ((y % 100) == 0) {
        return 0;
    } else if ((y % 4) == 0) {
        return 1;
    } else {
        return 0;
    }
}

static void set_date(int *m, int *d, int *y, int *w)
{
    time_t now;
    struct tm *ltime;

    if ((now = time(NULL)) == ((time_t) -1)) {
        perror("weekday: couldn't get time");
        exit(EXIT_FAILURE);
    }

    ltime = localtime(&now);

    *d = ltime->tm_mday;
    *m = ltime->tm_mon + 1;
    *y = ltime->tm_year + 1900;
    *w = ltime->tm_wday;
}

static void version(void)
{
    puts("weekday version 2.0");
    puts("Copyright 2023, Chris Barts <chbarts@gmail.com>");
    puts("Licensed under the GNU General Public License, version 3.0 or, at your option, any later version.");
    puts("This software has NO WARRANTY, even for USABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
}

static void help(char name[])
{
    printf("%s [OPTION]...\n", name);
    puts("Display the day of week for the given date,");
    puts("with intelligent defaulting.");
    puts("");
    puts("  -m, --month MONTH       month, from 1 (January) to 12 (December)");
    puts("  -d, --day DAY           day of month, from 1 to 28, 30, or 31)");
    puts("  -y, --year YEAR         year, from 1 to INT_MAX");
    puts("  -j, --julian            use Julian calendar instead of Gregorian");
    puts("");
    puts("If an option is missing or zero, assumes current value.");
    puts("The year must be complete; two-digit years will (usually)");
    puts("not give the desired result.");
    puts("No range checking is done to determine if date falls within the");
    puts("period of a given calendar's use in any particular area,");
    puts("or if the date was skipped in any particular area.");
    puts("");
    puts("Examples:");
    puts("  weekday               Output the current day of the week.");
    puts("  weekday -m 12 -d 25   Output when Christmas will fall this year.");
    puts("  weekday --year 1984   Output what day of the week fell on this");
    puts("                        month and day in 1984.");
    puts("");
    puts("Report bugs to <chbarts@gmail.com>.");
}

int main(int argc, char *argv[])
{
    char *pnam;
    int m, d, y, w, c, r, j = 0, res, lind;
    struct option longopts[] = {
        { "month", 1, 0, 0 },
        { "day", 1, 0, 0 },
        { "year", 1, 0, 0 },
        { "julian", 0, 0, 0 },
        { "help", 0, 0, 0 },
        { "version", 0, 0, 0 },
        { 0, 0, 0, 0 }
    };

    char *days[] =
        { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    int months[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    pnam = argv[0];

    set_date(&m, &d, &y, &w);

    if (1 == argc) {
        puts(days[w]);
        return 0;
    }

    while ((c =
            getopt_long(argc, argv, "m:d:y:jhv", longopts, &lind)) != -1) {
        switch (c) {
        case 0:
            switch (lind) {
            case 0:
                m = atoi(optarg);
                break;
            case 1:
                d = atoi(optarg);
                break;
            case 2:
                y = atoi(optarg);
                break;
            case 3:
                j = 1;
                break;
            case 4:
                help(pnam);
                exit(EXIT_SUCCESS);
            case 5:
                version();
                exit(EXIT_SUCCESS);
            default:
                help(pnam);
                exit(EXIT_FAILURE);
            }

            break;
        case 'm':
            m = atoi(optarg);
            break;
        case 'd':
            d = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case 'j':
            j = 1;
            break;
        case 'h':
            help(pnam);
            exit(EXIT_SUCCESS);
        case 'v':
            version();
            exit(EXIT_SUCCESS);
        default:
            help(pnam);
            exit(EXIT_FAILURE);
        }
    }

    if ((m <= 0) || (d <= 0) || (m > 12)) {
        fprintf(stderr, "weekday: meaningless date year: %d month: %d day: %d\n", y, m, d);
        exit(EXIT_FAILURE);
    }

    if (2 == m) {
        if (isleap(y, j)) {
            if (d > 29) {
                fprintf(stderr, "weekday: Impossible date\n");
                exit(EXIT_FAILURE);
            }

        } else {
            if (d > 28) {
                fprintf(stderr, "weekday: Impossible date\n");
                exit(EXIT_FAILURE);
            }
        }

    } else {
        if (d > months[m - 1]) {
            fprintf(stderr, "weekday: Impossible date\n");
            exit(EXIT_FAILURE);
        }
    }

    if (j) {
        res = zeller_julian(m, d, y);
    } else {
        res = zeller_gregorian(m, d, y);
    }

    puts(days[res]);

    return 0;
}
