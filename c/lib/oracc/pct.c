int
pct(int amount, int total)
{
  float i = amount;
  i /= total;
  i *= 100;
  return (int)(i+.5);
}
