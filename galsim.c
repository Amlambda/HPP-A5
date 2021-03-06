#include "graphics.h"
//#include "particle_functions.h"
#include "tree_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>

const double particleRadius = 0.005, particleColor = 0;
const int windowWidth = 800;
const double gravConst = 100;
double theta_max;
int n_thread_part;
int n_threads;
int N;

typedef struct args {
  particle_t * particle;
  node_t * root;
  double * xAcc;
  double * yAcc;
  int index;
} args_t;


// Function used to parallelise force and acceleration computation
void * the_thread_func(void* arg) {
  args_t * argPtr = (args_t *)arg;        // Cast input to args struct
  particle_t * target = argPtr->particle; // Set target particle
  node_t * root = argPtr->root;           // Set root node
  double * xAcc = argPtr->xAcc;
  double * yAcc = argPtr->yAcc;

  double forceSumX, forceSumY;

  for (int i = 0; i < n_thread_part; i++) { // Calculate forcesum for specified number of particles
      forceSumX = calc_forcesum(target, root, theta_max, 'x');
      forceSumY = calc_forcesum(target, root, theta_max, 'y');

      *xAcc = -(gravConst/N)*forceSumX;
      *yAcc = -(gravConst/N)*forceSumY;

      target++;
      xAcc++;
      yAcc++;
  }
  return NULL;
}

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
    return seconds;
}

int main (int argc, char *argv[]) {
  double startTime = get_wall_seconds();
  const double L=1, W=1;    // Dimensions of domain in which particles move

  // Check command line arguments
  if(argc != 8) {   // End program if not 5 input arguments (argv[0] is the program name)
        printf("Error: Expected number of input arguments is 7\n");
        exit(1);
  }
  
  // Read input arguments
  printf("-------- Input Arguments -----------\n");
  N = atoi(argv[1]);                // Number of particles to simulate (atoi = ascii to int)
  printf("N: \t\t\t%d\n", N);
  const char* input_file_name = argv[2];      // Filename of file to read the initial configuration from
  printf("input_file_name: \t%s\n", input_file_name);
  const int nsteps = atoi(argv[3]);           // Number of time steps
  printf("nsteps: \t\t%d\n", nsteps);
  const double delta_t = atof(argv[4]);        // Timestep
  printf("delta_t: \t\t%.5f\n", delta_t);
  theta_max = atof(argv[5]);        // Theta max
  printf("theta_max: \t\t%.1f\n", theta_max);
  const int graphics = atoi(argv[6]);         // 1 or 0 meaning graphics on/off
  printf("graphics: \t\t%d\n", graphics);
  n_threads = atoi(argv[7]);         // Number of threads
  printf("number of threads: \t\t%d\n", n_threads);
  printf("------------------------------------\n\n");


  //COPIED CODE TO READ FILE
  /* Open input file and determine its size. */
  FILE* input_file = fopen(input_file_name, "rb");
  if(!input_file) {
    printf("Error: failed to open input file '%s'.\n", input_file_name);
    return -1;
  }
  /* Get filesize using fseek() and ftell(). */
  fseek(input_file, 0L, SEEK_END);
  size_t fileSize = ftell(input_file);
  /* Now use fseek() again to set file position back to beginning of the file. */
  fseek(input_file, 0L, SEEK_SET);
  printf("fileSize = %lu bytes.\n\n", fileSize);
  /* Allocate buffer into which we can then read the data from input_file. */
  char* buffer = (char*)malloc(fileSize*sizeof(char));
  /* Read contents of input_file into buffer. */
  size_t noOfItemsRead = fread(buffer, sizeof(char), fileSize, input_file);
  if(noOfItemsRead != fileSize) {
    printf("Error: failed to read file contents into buffer.\n");
    return -1;
  }
  /* OK, now we have the file contents in the buffer.
     Since we are finished with the input file, we can close it now. */
  if(fclose(input_file) != 0) {
    printf("Error closing input file.\n");
    return -1;
  }
  // END OF COPIED CODE

  /* Read initial configuration from buffer */
  particle_t particles[N];                 // Arrays of particle attributes statically allocated on stack

  char* ptr;                                    // Pointer to use when extracting doubles from buffer
  const int offset = sizeof(double);            // Constant used as offset into buffer when reading and writing doubles
  int index = 0;
  for (int i = 0; i < fileSize; i+=6*offset) {      // Increase by six*sizeof(double) (six attributes per particle)
    ptr = &buffer[i];
    memcpy(&particles[index].xPos, ptr, sizeof(double));

    ptr = &buffer[i+offset];
    memcpy(&particles[index].yPos, ptr, sizeof(double));

    ptr = &buffer[i+2*offset];
    memcpy(&particles[index].mass, ptr, sizeof(double));
  
    ptr = &buffer[i+3*offset];
    memcpy(&particles[index].xVel, ptr, sizeof(double));

    ptr = &buffer[i+4*offset];
    memcpy(&particles[index].yVel, ptr, sizeof(double));

    ptr = &buffer[i+5*offset];  // Step to brightness but do nothing

    index++;
  }




  /* If graphics are to be used, prepare graphics window */
  if (graphics == 1) {
    InitializeGraphics(argv[0],windowWidth,windowWidth);
    SetCAxes(0,1);
  }

  /* Declare variables */
  particle_t * target;
  double absDist, partDistX, partDistY;
  double xAcc[N]; 
  double yAcc[N];

  /* Initiate threads */
  args_t thread_args[n_threads];
  n_thread_part = (int)(N/n_threads);
  //printf("particles per thread: %d\n",n_thread_part);
  pthread_t thread[n_threads];


  /* Start simulation */
  for (int time_step = 0; time_step < nsteps; time_step++) {   // Loop over all timesteps
    /* Initialize mother node */
    node_t * root = new_node(0, 0, 1);

    double startInsert = get_wall_seconds();
    // Build quadtree with all particles for current time step
    for(int i=0;i<N;i++){
      insert(root, &particles[i]);
    }
    double endInsert = get_wall_seconds();
    //printf("Insert takes %f wall seconds\n", endInsert - startInsert);

    double startCalcCm = get_wall_seconds();
    // Calculate mass center for all nodes 
    calc_cm(root);
    double endCalcCm = get_wall_seconds();
    //printf("Calculation of center fo mass takes %f wall seconds\n", endCalcCm - startCalcCm);


      /* Start thread. */
    for(int i = 0; i<(n_threads-1);i++){
      thread_args[i].particle = &particles[i*n_thread_part];
      thread_args[i].root = root;
      thread_args[i].index = i;
      thread_args[i].xAcc = &xAcc[i*n_thread_part];
      thread_args[i].yAcc = &yAcc[i*n_thread_part];
      pthread_create(&thread[i], NULL, the_thread_func, &thread_args[i]);
    }


    double startCalcAcc = get_wall_seconds();

    /* Compute acceleration of particle i based on force from all other particles */
    for (int i = ((n_threads-1)*n_thread_part); i < N; i++) {
      target = &particles[i];

      double forceSumX = calc_forcesum(target, root, theta_max, 'x');
      double forceSumY = calc_forcesum(target, root, theta_max, 'y');

      xAcc[i] = -(gravConst/N)*forceSumX;
      yAcc[i] = -(gravConst/N)*forceSumY;
    }
    //printf("particles in main: %d\n",(N+1-(n_threads*n_thread_part-1)));


    for(int i = 0; i<(n_threads-1);i++){
      pthread_join(thread[i], NULL);
    }

    double endCalcAcc = get_wall_seconds();
    //printf("Calculation of acceleration in 2D takes %f wall seconds\n", endCalcAcc - startCalcAcc);

    
    double startUpdatePos = get_wall_seconds();
    // /* Update position of particle i with respect to all other particles */
    for (int i = 0; i < N; i++) {
      target = &particles[i];
      // target->xVel = get_vel_1D(xAcc[i], target->xVel, delta_t);
      // target->yVel = get_vel_1D(yAcc[i], target->yVel, delta_t);
      target->xVel += xAcc[i]*delta_t;
      target->yVel += yAcc[i]*delta_t;;

      target->xPos += (target->xVel)*delta_t;   
      target->yPos += (target->yVel)*delta_t;;
      // target->xPos = get_pos_1D(target->xPos, target->xVel, delta_t);   
      // target->yPos = get_pos_1D(target->yPos, target->yVel, delta_t);
    }
    double endUpdatePos = get_wall_seconds();
    //printf("Updating positions in 2D takes %f wall seconds\n", endUpdatePos - startUpdatePos);

    if (graphics == 1) {
      /* Call graphics routines. */
      ClearScreen();
      for (int i = 0; i < N; i++) {
        DrawCircle(particles[i].xPos, particles[i].yPos, L, W, particleRadius, particleColor);
      }
      Refresh();
      // Sleep a short while to avoid screen flickering. (SHOULD ONLY BE USED FOR SMALL N) 
      if(N<500)
        usleep(3000);
    }
    free_tree(root);
  }   

  if (graphics == 1) {
    FlushDisplay();
    CloseDisplay();
  }

  // Overwrite buffer with simulation results 
  index = 0;
  for (int i = 0; i < fileSize; i+=6*offset) {      // Increase by six*sizeof(double) (six attributes per particle)
    ptr = &buffer[i];
    memcpy(ptr, &particles[index].xPos, sizeof(double));

    ptr = &buffer[i+offset];
    memcpy(ptr, &particles[index].yPos, sizeof(double));
    
    ptr = &buffer[i+2*offset];
    memcpy(ptr, &particles[index].mass, sizeof(double));

    ptr = &buffer[i+3*offset];
    memcpy(ptr, &particles[index].xVel, sizeof(double));

    ptr = &buffer[i+4*offset];
    memcpy(ptr, &particles[index].yVel, sizeof(double));
    
    ptr = &buffer[i+5*offset];

    index++;
  }

  // Create output file and write results to it 
  int no_of_chars_to_copy = strlen(input_file_name) - 4;   // Copy input file name except for ".gal"
  char* copy_file_name = (char *)malloc(sizeof(char)*no_of_chars_to_copy);      
  strncpy(copy_file_name, input_file_name, no_of_chars_to_copy);
  // Build output file name based on input file name and number of steps 
  //asprintf(&output_file_name, "%s_after%dsteps_result.gal", copy_file_name, nsteps);
  free(copy_file_name);
  

  char* output_file_name = "result.gal";

  // Open output file for writing
  FILE* output_file = fopen(output_file_name, "wb");
  if(!output_file) {
    printf("Error: failed to open output file '%s' for writing.\n", output_file_name);
    return -1;
  }
  
  /* Write contents of buffer into output_file. */
  noOfItemsRead = fwrite(buffer, sizeof(char), fileSize, output_file);
  if(noOfItemsRead != fileSize) {
    printf("Error: failed to write buffer contents into result file.\n");
    return -1;
  }

  if(fclose(output_file) != 0) {
    printf("Error closing output file.\n");
    return -1;
  }

  printf("Result saved in file '%s'\n", output_file_name);

  free(buffer);
  double endTime = get_wall_seconds();
  printf("Program run took %f wall seconds\n", endTime - startTime);
  return 0;
}
