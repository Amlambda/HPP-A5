#include "particle_functions.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct cm {
	double mass;
	double xPos;
	double yPos;
} cm_t;

typedef struct particle {
	double xPos;
	double yPos;
	double mass;
	double xVel;
	double yVel;
} particle_t;

typedef struct node {
	struct node * tl; //top left child
	struct node * tr; //top right child
	struct node * bl; //bottom left child
	struct node * br; //bottom right child
	particle_t * particle;
	cm_t * nodeCm;

	// Top left corner of quad
	double xPos; 
	double yPos;

	// Width of quad
	double width;

} node_t;


int have_same_pos (node_t *node, particle_t * particle);

//double get_theta(particle_t * target, node_t * node);

double calc_forcesum(particle_t * target, node_t * node, double theta_max, char coord);

void free_tree(node_t * root);

cm_t * calc_cm(node_t *root);

//void set_cm(cm_t * cm, double mass, double y, double x);


// Creates new empty node with specified top left corner coordinates and width 
node_t * new_node(double xPos, double yPos, double width); 

// Checks if node is empty	( = having no children and no particle stored)
int isempty(node_t * node);

// Checks if node is a leaf ( = having a particle stored)
int isleaf(node_t * node);

// Checks if node is pointer ( = having children but no particle stores)
int ispointer(node_t *node);

int split_node(node_t *node);

// Inserts particle into quadtree
int insert(node_t *root, particle_t * particle);

// Gets quadrant which to place particle in
node_t * get_quadrant(node_t * root, particle_t * particle);

// Checks if particles coordinates matches those of quadrant
int match_coords(node_t * quadrant, particle_t * particle);

