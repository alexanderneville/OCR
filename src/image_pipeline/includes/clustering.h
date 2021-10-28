#ifndef clustering_H
#define clustering_H

#include "matrix.h"

#define MAX_ITERATIONS 20
#define MAX_CLUSTERS 100

typedef struct point_T {
    int x, y;
    int current_cluster;
} point;

typedef struct tmp_points_T {
    point ** points;
    int num_points;
} tmp_points;

point * new_point(int x, int y);
tmp_points matrix_to_points(matrix * matrix_p);


float distance_between_points(point one, point two);

point * new_random_centroid(int max_x, int max_y);
point ** initialise_centroids(int num_centroids, int max_x, int max_y);
void assign_centroids(point ** centroids, point ** points, int num_centroids, int num_points);
void update_centroids(point ** centroids, point ** points, int num_centroids, int num_points);

point ** k_means_segmentation(matrix * matrix_p, int num_clusters);
void outline_clusters(matrix * matrix_p, point ** centroids, int num_clusters);

#endif
