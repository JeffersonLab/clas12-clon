/* clonutil.h */

#ifdef  __cplusplus
extern "C" {
#endif

  /*
int get_next_line(ifstream &file, char buffer[], int buflen);
int get_next_line(istrstream &str, char buffer[], int buflen);
  */
int get_next_line(FILE *file, char buffer[], int buflen);

#ifdef  __cplusplus
}
#endif
