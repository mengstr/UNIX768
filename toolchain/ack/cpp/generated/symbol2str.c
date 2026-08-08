#include "Lpars.h"

char *
symbol2str(int tok)
{
	static char buf[2] = { '\0', '\0' };

	if (040 <= tok && tok < 0177) {
		buf[0] = tok;
		buf[1] = '\0';
		return buf;
	}
	switch (tok) {
	case IDENTIFIER :
		return "identifier";
	case STRING :
		return "string";
	case FILESPECIFIER :
		return "filespecifier";
	case INTEGER :
		return "integer";
	case PLUSAB :
		return "+=";
	case MINAB :
		return "-=";
	case TIMESAB :
		return "*=";
	case DIVAB :
		return "/=";
	case MODAB :
		return "%=";
	case LEFTAB :
		return "<<=";
	case RIGHTAB :
		return ">>=";
	case ANDAB :
		return "&=";
	case XORAB :
		return "^=";
	case ORAB :
		return "|=";
	case NOTEQUAL :
		return "!=";
	case AND :
		return "&&";
	case PLUSPLUS :
		return "++";
	case MINMIN :
		return "--";
	case ARROW :
		return "->";
	case LEFT :
		return "<<";
	case LESSEQ :
		return "<=";
	case EQUAL :
		return "==";
	case GREATEREQ :
		return ">=";
	case RIGHT :
		return ">>";
	case OR :
		return "||";
	case ELLIPSIS :
		return "...";
	case ERRONEOUS :
		return "erroneous";
	case '\n':
	case '\f':
	case '\v':
	case '\r':
	case '\t':
		buf[0] = tok;
		return buf;
	default:
		return "bad token";
	}
}
