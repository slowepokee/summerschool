// TODO add declaration for target usage

#pragma omp declare target
double my_sum(double a, double b);
#pragma omp end declare target

// TODO end

double my_sum(double a, double b)
{
  double c = a + b;
  return c;
}

