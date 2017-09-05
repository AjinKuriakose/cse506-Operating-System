
int my_strlen(char *str) {
  int i;
  for (i = 0; *str != '\0'; str++)
    i++;
  return i;
}
