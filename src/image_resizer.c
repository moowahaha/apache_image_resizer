#include "cgic.h"
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int cgiMain() {
  // cgiGetenv(&blah, "terry");
  /* Send the content type, letting the browser know this is HTML */
  cgiHeaderContentType("text/html");
  /* Top of the page */
  fprintf(cgiOut, "<HTML><HEAD>\n");
  fprintf(cgiOut, "<TITLE>cgic test</TITLE></HEAD>\n");
  fprintf(cgiOut, "<BODY><H1>cgic test</H1>\n");
  fprintf(cgiOut, "path: %s<br/>", cgiPathInfo);

  char *blah;
  blah = getenv("TERRY");
  fprintf(cgiOut, "env: %s<br/>", blah);

  char queryParam[1024];
  cgiFormStringNoNewlines("a", queryParam, sizeof(queryParam));
  fprintf(cgiOut, "param: %s<br/>", queryParam);
  fprintf(cgiOut, "</BODY></HTML>\n");
  return 0;
}
