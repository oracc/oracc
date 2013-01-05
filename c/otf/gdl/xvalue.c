static int
x_value(unsigned const char *v)
{
  int n = strlen((const char *)v);
  return n > 3 && v[n-1] == 0x93 && v[n-2] == 0x82 && v[n-3] == 0xe2;
}
