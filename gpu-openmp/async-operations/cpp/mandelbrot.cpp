#include <array>
#include <vector>
#include <iostream>
#include <omp.h>
#include "constants.hpp"
#include "pngwriter.h"

using namespace std;


#pragma omp declare target
int kernel(int xi, int yi);
#pragma omp end declare target

int main() {

  int *image = new int[width * height];
  int num_blocks = 8;
  int block_size = (height / num_blocks) * width;
  int y_block_size = height / num_blocks;

  double st = omp_get_wtime();



  
  #pragma omp target data map(alloc:image[width*height]) 
  for(int block = 0; block < num_blocks; block++ ) {
    int y_start = block * y_block_size;
    int y_end = y_start + y_block_size;

    //collapse seems to give slight speedup
    #pragma omp target loop collapse(2) depend(out:image[y_start]) nowait 
    //outer loop is over y_start, so make dependence on image[y_start]
    //so other loop do not need to be waited for, only the one where corresponding update is done
    for (int y = y_start; y < y_end; y++) {
      for (int x = 0; x < width; x++) {
        
        int ind = y * width + x;
        image[ind] = kernel(x, y);
      }
    }
    //wait for the image to be completed before update, otherwise nowait (ie. can start next loop)
    #pragma omp target update from(image[block_size:block*block_size]) depend(in:image[y_start]) nowait
   

  }
  #pragma omp taskwait

  

  double et = omp_get_wtime();

  cout << "Time: " << (et - st) << " seconds" << endl;
  //int *image_ptr = image.data();
  save_png(image, width, height, "mandelbrot.png");
}
