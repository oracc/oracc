int
pct(int amount, int total)
{
  float i = amount;
  i /= total;
  i *= 100;
  return (int)(i+.5);
}

double
pctd(double amount, double total) {
  return (amount / total) * 100;
}
