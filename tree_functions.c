#include "tree_functions.h"
#include <math.h>
#include <assert.h>

const double eps0 = 0.001;

// Checks is a particle and a node has the same position
int have_same_pos (node_t *node, particle_t * particle){
  return (node->particle->xPos == particle->xPos 
    && node->particle->yPos == particle->yPos);
}

// Calculates theta for target particle and specified node
double get_theta(particle_t * target, node_t * node) {
  double xPosBoxCenter = (node->xPos) + (node->width/2); 
  double yPosBoxCenter = (node->yPos) + (node->width/2);

  // Get distance from target particle to center of box containing other particles
  //double rToGroup = get_abs_dist(target->xPos,target->yPos,xPosBoxCenter, yPosBoxCenter);
  double rToGroup = sqrt(((target->xPos)-xPosBoxCenter)*((target->xPos)-xPosBoxCenter) 
    + ((target->yPos)-yPosBoxCenter)*((target->yPos)-yPosBoxCenter));
  // Compute theta
  double theta = (node->width)/rToGroup;
  return theta;
}

// Calculates the sum in the force equation
double calc_forcesum(particle_t * target, node_t *node, double theta_max, char coord) {
  if(isempty(node) || (isleaf(node) && have_same_pos(node,target))) {
    return 0;
  } else if (ispointer(node)) {
    double theta = get_theta(target, node);
    //printf("Theta is %f\n", theta);
    if (theta > theta_max) {
      return (calc_forcesum(target, node->tl, theta_max, coord) +
        calc_forcesum(target, node->tr, theta_max, coord) +
        calc_forcesum(target, node->bl, theta_max, coord) +
        calc_forcesum(target, node->br, theta_max, coord));
    }
  }

    // Get center of mass of node and treat it like a particle
    cm_t * other = node->nodeCm;  

   // double absDist = get_abs_dist(target->xPos, target->yPos, other->xPos, other->yPos);
    double absDist = sqrt(((target->xPos)-(other->xPos))*((target->xPos)-(other->xPos)) 
      + ((target->yPos)-(other->yPos))*((target->yPos)-(other->yPos)));
    double partDist, forceSum;

    if (coord == 'x') {
      // partDist = get_part_dist_1D(target->xPos, other->xPos);
      // forceSum = get_force_1D(partDist, absDist, other->mass);
      partDist = (target->xPos) - (other->xPos);
      //forceSum = ((other->mass)*partDist)/((absDist + eps0)*(absDist + eps0)*(absDist + eps0));
    } else {
      // partDist = get_part_dist_1D(target->yPos, other->yPos);
      // forceSum = get_force_1D(partDist, absDist, other->mass);
      partDist = (target->yPos) - (other->yPos);
      //forceSum = get_force_1D(partDist, absDist, other->mass);
    }
    forceSum = ((other->mass)*partDist)/((absDist + eps0)*(absDist + eps0)*(absDist + eps0));

    return forceSum;
}


/* Node functions */

void free_tree(node_t * root){
  if(ispointer(root)){
    free_tree(root->tl);
    free_tree(root->tr);
    free_tree(root->bl);
    free_tree(root->br);
  }
  if(root->nodeCm != NULL){
    free(root->nodeCm);
  }
  free(root);
  return;
}

// void set_cm(cm_t * cm, double mass, double x, double y){
//   cm->mass = mass;
//   cm->xPos = x;
//   cm->yPos = y;
// }

void update_cm(cm_t * newCm, cm_t * tlCm, cm_t * trCm, cm_t * blCm, cm_t * brCm){
  cm_t * cmVec[4]; 
  cmVec[0]= tlCm; //Top left node center of mass
  cmVec[1]= trCm;
  cmVec[2]= blCm;
  cmVec[3]= brCm; 
  double totMass = 0;
  double xAvg = 0; 
  double yAvg = 0;
    for(int i=0; i<4; i++){
    if(cmVec[i] != NULL){
      totMass += cmVec[i]->mass;
    }
  }

  //assert(totMass>0);

  for(int i=0; i<4; i++){
    if(cmVec[i] != NULL){
      // Weight with cm's distance to mother nodes center.
      xAvg += (cmVec[i]->xPos)*(cmVec[i]->mass); 
      yAvg += (cmVec[i]->yPos)*(cmVec[i]->mass);
    }
  }
  xAvg /= (totMass);
  yAvg /= (totMass);

  // assert(xAvg>=0);
  // assert(xAvg<=1);
  // assert(yAvg>=0);
  // assert(yAvg<=1);

  newCm->mass = totMass;
  newCm->xPos = xAvg;
  newCm->yPos = yAvg; 
}

// Calculates the center of mass for all pointers and leaf in a tree specified by root pointer
cm_t * calc_cm(node_t *root) {

  if (isleaf(root)){
    root->nodeCm = (cm_t*)malloc(sizeof(cm_t));
    //set_cm(root->nodeCm, root->particle->mass, root->particle->xPos, root->particle->yPos);
    root->nodeCm->mass = root->particle->mass;
    root->nodeCm->xPos = root->particle->xPos;
    root->nodeCm->yPos = root->particle->yPos;
    return root->nodeCm;
  }else if(ispointer(root)) {
    
    //Recursively calculate CoM for each quadrant
    //if(!isempty(root->tl)){ 
      if(!(root->tl->tl == NULL
      && root->tl->tr == NULL
      && root->tl->bl == NULL
      && root->tl->br == NULL
      && root->tl->particle == NULL)){ 
      root->tl->nodeCm = calc_cm(root->tl);
    }
    //if(!isempty(root->tr)){
    if(!(root->tr->tl == NULL
      && root->tr->tr == NULL
      && root->tr->bl == NULL
      && root->tr->br == NULL
      && root->tr->particle == NULL)){    
      root->tr->nodeCm  = calc_cm(root->tr);
    }
    //if(!isempty(root->bl)){ 
      if(!(root->bl->tl == NULL
      && root->bl->tr == NULL
      && root->bl->bl == NULL
      && root->bl->br == NULL
      && root->bl->particle == NULL)){  
      root->bl->nodeCm = calc_cm(root->bl);
    }
    //if(!isempty(root->br)){ 
      if(!(root->br->tl == NULL
      && root->br->tr == NULL
      && root->br->bl == NULL
      && root->br->br == NULL
      && root->br->particle == NULL)){ 
      root->br->nodeCm = calc_cm(root->br);   
    }
    
    //Calculate the node's new center of mass from the childrens center of mass
    root->nodeCm = (cm_t*)malloc(sizeof(cm_t));
    update_cm(root->nodeCm, root->tl->nodeCm, root->tr->nodeCm, 
      root->bl->nodeCm,root->br->nodeCm);

    return root->nodeCm;
  }
  return NULL;
}


// Creates new empty node with specified top left corner coordinates and width 
node_t * new_node(double xPos, double yPos, double width) {
	node_t * node;
	if(!(node = malloc(sizeof(*node))))
    	return NULL;
	node->tl = NULL;
  node->tr = NULL;
  node->bl = NULL;
  node->br = NULL;
  node->xPos = xPos;
  node->yPos = yPos;
  node->width = width;
  node->particle = NULL;
  node->nodeCm = NULL;
	return node;
}

// Checks if node is empty	( = having no children and no particle stored)
int isempty(node_t * node){
  return node->tl == NULL
      && node->tr == NULL
      && node->bl == NULL
      && node->br == NULL
      && !isleaf(node);
}

// Checks if node is a leaf ( = having a particle stored)
int isleaf(node_t * node){
  return node->particle != NULL;
}

// Checks if node is pointer ( = having children but no particle stores)
int ispointer(node_t *node){
  return node->tl != NULL
      && node->tr != NULL
      && node->bl != NULL
      && node->br != NULL
      && !isleaf(node);
}

int split_node(node_t *node){
	// Declare children
	node_t * tl;
	node_t * tr;
	node_t * bl;
	node_t * br;

	// Get top left corner coordinates of quadrant
  double x  = node->xPos;
  double y  = node->yPos;

  // Compute width of children quadrants
  double w = node->width / 2;

  // Create children nodes
  if(!(tl = new_node(x, y, w))) return 0;
  if(!(tr = new_node(x + w, y, w))) return 0;
  if(!(bl = new_node(x, y + w, w))) return 0;
  if(!(br = new_node(x + w, y + w, w))) return 0;

  node->tl = tl;
  node->tr = tr;
  node->bl = bl;
  node->br = br;

  // Save pointer to particle which was stored in node before split
  particle_t * old = node->particle;

  // Set particle attribute to null
  node->particle = 0;

	return insert(node, old);
}

// Inserts particle into quad tree
int insert(node_t * root, particle_t * particle) {
	if(isempty(root)){
		root->particle = particle;
		return 1; /* normal insertion flag */
	} else if(isleaf(root)) {
  		if(have_same_pos(root,particle)){
      		printf("Error: Two particles are on the same position.\n");
      		exit(1); /* Break program if two different particles have the same postions */
    	} else {
    		if(!split_node(root)){	
        		return 0; /* failed insertion flag */
      		}
     		insert(root, particle);
     	}
    } else if(ispointer(root)) {
      // Get quadrant which to place particle in
    	node_t * quadrant = get_quadrant(root, particle);		
    	return quadrant == NULL ? 0 : insert(quadrant, particle);
    }
    return 0; /* failed insertion flag */
}

// Gets quadrant which to place particle in
node_t * get_quadrant(node_t * root, particle_t * particle) {
	if (match_coords(root->tl, particle)) return root->tl;
	if (match_coords(root->tr, particle)) return root->tr;
	if (match_coords(root->bl, particle)) return root->bl;
	if (match_coords(root->br, particle)) return root->br;
	return NULL; 
}

// Checks if particles coordinates matches those of quadrant
int match_coords(node_t * quadrant, particle_t * particle) {
      return ((quadrant->xPos) <= (particle->xPos))					// Check left border
      && ((quadrant->xPos + quadrant->width) >= (particle->xPos))		// Check right border
      && ((quadrant->yPos) <= (particle->yPos))						// Check upper border	
      && ((quadrant->yPos + quadrant->width) >= (particle->yPos));	// Check lower border
}

