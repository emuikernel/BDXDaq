/*
*  Fortran callable function returns (by ref) address of argument (passed by ref)
*
*  for interfacing C to Fortran
*
*  ejw, 11-sep-96
*
*/

int *getaddr_(int *var){

  return (int*) var;

}

/*-----------------------------------------------------------------------*/


