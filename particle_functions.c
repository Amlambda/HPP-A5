// #include "particle_functions.h"
// #include <math.h>
						
//const double eps0 = 0.001;

//Calculated position in 1D
// double get_pos_1D(double pos, double vel, double delta_t) {
// 	return pos + delta_t*vel;
// }

// //Calculates velocity in 1D
// double get_vel_1D(double acc, double vel, double delta_t){
// 	return vel + (delta_t*acc);
// }

// //Calculates the distance between two particles in 1D
// double get_part_dist_1D(double posTarget, double posOther){
// 	return posTarget-posOther;
// }

//Calculates the absolute distance between two particles in 2D
// double get_abs_dist(double xPosTarget, double yPosTarget, double xPosOther, double yPosOther){
// 	return sqrt((xPosTarget-xPosOther)*(xPosTarget-xPosOther) + (yPosTarget-yPosOther)*(yPosTarget-yPosOther));
// }

// Calculates the contributioin to the force sum in 1D on target particle from one other particle 
// double get_force_1D(double partDist, double absDist, double massOther) {
// 	// Plummer sphere modification, r<<1 
// 	return (massOther*partDist)/((absDist + eps0)*(absDist + eps0)*(absDist + eps0));
// }

