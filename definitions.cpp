#include "main.h"
#include <stdio.h>

// A global pointer to the log file
FILE *StdLog = stdout;

// The global verbosity_level, used by BTouch and libcommon (MySignal)
int VERBOSITY_LEVEL;

// Only for the linking with libcommon
char *My_Parser = const_cast<char*>(MY_PARSER_DEFAULT);
char *Suffisso = const_cast<char*>("<BTo>");
int use_ssl = false;
char *ssl_cert_key_path = NULL;
char *ssl_certificate_path = NULL;

