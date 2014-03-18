//gsoapopt c

#import "import/soap12.h"
#import "import/xop.h"
#import "import/xmime5.h"

//gsoap ows schema namespace: http://oracc.org/ws/ows
struct ows__Data
{
  @char *ows_status; // return status for method
  _xop__Include xop__Include; // attachment
  @char *xmime5__contentType; // and its contentType
};

//gsoap ows service method-mime-type: ows__ox text/xml
int ows__ox(char *args[], char *session, struct ows__Data *infile, struct ows__Data *logfile);

/*

//gsoap ows service method-mime-type: ows__ox text/xml
int ows__build(char *project, char *scope, char *session, struct ows__Data *in, struct ows__Data *logfile);

//gsoap ows service method-mime-type: ows__ox text/xml
int ows__check(char *project, char *scope, char *session, struct ows__Data *in, struct ows__Data *logfile);

//gsoap ows service method-mime-type: ows__ox text/xml
int ows__lemmatize(char *args[], char *session, struct ows__Data *in, struct ows__Data *atffile);

int ows__status(char *session, char *status);

 */
