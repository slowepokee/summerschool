/* Main solver routines for heat equation solver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "heat.h"

/* Exchange the boundary values */
void exchange(field *temperature, parallel_data *parallel)
{
    double *data;  
    //double *sbuf_up, *sbuf_down, *rbuf_up, *rbuf_down;

    // TODO start: implement halo exchange  
    //data = temperature->data;
    double *sbuf, *rbuf;
    data = temperature->data;

    sbuf = data + temperature->ny + 2; // upper data
    rbuf = data + (temperature->nx + 1) * (temperature->ny + 2); // lower halo


    // Send to up, receive from down

    MPI_Sendrecv(sbuf, temperature->ny + 2, MPI_DOUBLE, parallel->nup, 2,
                 rbuf, temperature->ny + 2, MPI_DOUBLE, parallel->ndown, MPI_ANY_TAG,
                 parallel->comm, MPI_STATUS_IGNORE);  

    // Send to down, receive from up
    sbuf = data + temperature->nx * (temperature->ny + 2); // lower data
    rbuf = data; // upper halo

    MPI_Sendrecv(sbuf, temperature->ny + 2, MPI_DOUBLE, parallel->ndown, 2,
                 rbuf, temperature->ny + 2, MPI_DOUBLE, parallel->nup, MPI_ANY_TAG,
                 parallel->comm, MPI_STATUS_IGNORE); 

    // TODO end


}

/* Update the temperature values using five-point stencil */
void evolve(field *curr, field *prev, double a, double dt)
{
  double dx2, dy2;
  int nx, ny;
  double *currdata, *prevdata;

  /* HINT: to help the compiler do not access members of structures
   * within OpenACC parallel regions */
  currdata = curr->data;
  prevdata = prev->data;
  nx = curr->nx;
  ny = curr->ny;

  /* Determine the temperature field at next time step
   * As we have fixed boundary conditions, the outermost gridpoints
   * are not updated. */
  dx2 = prev->dx * prev->dx;
  dy2 = prev->dy * prev->dy;
  for (int i = 1; i < nx + 1; i++) {
    for (int j = 1; j < ny + 1; j++) {
            int ind = i * (ny + 2) + j;
            int ip = (i + 1) * (ny + 2) + j;
            int im = (i - 1) * (ny + 2) + j;
	    int jp = i * (ny + 2) + j + 1;
	    int jm = i * (ny + 2) + j - 1;
            currdata[ind] = prevdata[ind] + a * dt *
	      ((prevdata[ip] -2.0 * prevdata[ind] + prevdata[im]) / dx2 +
	       (prevdata[jp] - 2.0 * prevdata[ind] + prevdata[jm]) / dy2);
    }
  }

}
