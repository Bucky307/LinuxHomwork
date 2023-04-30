/***********
 * $Id: wyscanner.h,v 1.2 2014/11/05 20:31:36 kbuckner Exp kbuckner $
 * $Author: kbuckner $
 * $Date: 2014/11/05 20:31:36 $
 *
 * $Log: wyscanner.h,v $
 * Revision 1.2  2014/11/05 20:31:36  kbuckner
 * fix headers
 *
 *
 ************/
#ifndef __WY_SCANNER__
#define __WY_SCANNER__

#define QUOTE_ERROR 96
#define ERROR_CHAR 97
#define SYSTEM_ERROR 98
#define EOL 99
#define REDIR_OUT 100
#define REDIR_IN 101
#define APPEND_OUT 102
#define REDIR_ERR 103
#define APPEND_ERR 104
#define REDIR_ERR_OUT 105
#define SEMICOLON 106
#define PIPE 107
#define AMP 108
#define WORD 200

extern char *lexeme;
extern char error_char;
extern int parse_line(const char *);


#endif

