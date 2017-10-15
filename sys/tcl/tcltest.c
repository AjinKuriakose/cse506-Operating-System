//#include <stdio.h>
#include <tcl/tcl.h>

/*
 * Compile : gcc tcltest.c -ltcl
 */

int main (int argc, char *argv[]) {

  Tcl_Interp *myinterp;
  //char *action1 = "set a [expr 5 * 8]; puts $a";
  char *action2 = "puts \"Hello World!\"";
  int status;

  //printf ("Your Program will run ... \n");

  myinterp = Tcl_CreateInterp();
  //status = Tcl_Eval(myinterp,action1, 0, NULL);
  status = Tcl_Eval(myinterp,action2, 0, NULL);
  status = Tcl_Eval(myinterp,action2, 0, NULL);
  status = Tcl_Eval(myinterp,action2, 0, NULL);

  //printf ("Your Program has completed\n");

  return 0;
}

