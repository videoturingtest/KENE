#include "kene-tool.h"
namespace NS_KENE {

int GetLine(istream &is, Uchar *line, Uint size, bool &b_over) {
	Uchar *s = line, *end = s + (size - 1); int c;
	while ((c = is.get()) != '\n' && c != EOF)
		if (s < end) *s++ = (Uchar)c;
		else { *s = '\0'; b_over = true; return (int)(s - line); }
	b_over = false; *s = '\0';
	if (c == '\n') return (int)(s - line);
	return (s == line) ? -1 : (int)(s - line);
}

int CvLineCmt(Uchar *s, Uchar **forms, int max, Uchar dc) {
	for (Uchar **up = forms + max; 1; *s++ = '\0') {
		while (*s && *s <= 0x20) s++;
		if (!*s || *s == dc) return (int)(max - (up - forms));
		if (forms >= up) return -1;
		*forms++ = s++;
		while (*s > 0x20) s++;
		if (!*s) return (int)(max - (up - forms));
	}
}

}
