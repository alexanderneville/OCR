#include "../includes/clustering.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

point * new_point(int x, int y) {
    point * new_point = (point *) malloc(sizeof(point));
    new_point->current_cluster = - 1;
    new_point->x = x;
    new_point->y = y;
    return new_point;
}

tmp_points matrix_to_points(matrix * matrix_p) {

    tmp_points set;
    set.num_points = 0;
    set.points = (point **) malloc(sizeof(point *) * matrix_p->y * matrix_p->x);

    for (int y = 0; y < matrix_p->y; y ++){
        for (int x = 0; x < matrix_p->x; x ++){
            if (matrix_p->array[y * matrix_p->x + x] >= 255.0) {
                set.points[set.num_points] = new_point(x, y);
                set.num_points++;
            }
        }
    }

    return set;
}


float distance_between_points(point one, point two) {
    float distance = sqrt(((two.x - one.x) * (two.x - one.x)) + ((two.y - one.y) * (two.y - one.y)));
    return distance;
}

point * new_random_centroid(int max_x, int max_y) {
    printf("new centroid ... ");
    int x = rand() % max_x;
    int y = rand() % max_y;
    printf("x:%d,y:%d\n", x, y);
    point * new_centroid = new_point(x, y);
    return new_centroid;
}

point ** initialise_centroids(int num_centroids, int max_x, int max_y) {
    printf("initialising centroids:\n");
    srand(time(0));
    point ** centroids = (point **) malloc(sizeof(point *) * num_centroids);
    for (int centroid = 0; centroid < num_centroids; centroid++) {
        centroids[centroid] = new_random_centroid(max_x, max_y);
    }
    printf("\n");
    return centroids;
}

void assign_centroids(point ** centroids, point ** points, int num_centroids, int num_points) {
    for (int point = 0; point < num_points; point++) {
        float * distances = (float *) malloc(sizeof(float) * num_centroids);
        int closest_centroid_index = 0;
        for (int centroid_number = 0; centroid_number < num_centroids; centroid_number++) {
            distances[centroid_number] = distance_between_points(* (centroids[centroid_number]), * (points[point]));
            if(distances[centroid_number] < distances[closest_centroid_index])
                closest_centroid_index = centroid_number;
        }
        points[point]->current_cluster = closest_centroid_index; 
    }
}


void update_centroids(point ** centroids, point ** points, int num_centroids, int num_points) {

    for (int centroid = 0; centroid < num_centroids; centroid++) {
        
        int total_points = 0;
        int sum_x = 0;
        int sum_y = 0;

        for (int point = 0; point < num_points; point++) {
            if (points[point]->current_cluster == centroid) {
                total_points++;
                sum_x += points[point]->x;
                sum_y += points[point]->y;
            } 
        }

        printf("%d\n", total_points);
        centroids[centroid]->x = sum_x / total_points;
        centroids[centroid]->y = sum_y / total_points;

    }

}

point ** k_means_segmentation(matrix * matrix_p, int num_clusters) {

    tmp_points set = matrix_to_points(matrix_p);
    int num_points = set.num_points;
    point ** points = set.points;
    point ** centroids = initialise_centroids(num_clusters, matrix_p->x, matrix_p->y);
    for (int iterations = 0; iterations < 1000; iterations ++) {
        assign_centroids(centroids, points, num_clusters, num_points);
        update_centroids(centroids, points, num_clusters, num_points);
    }
    return centroids;
}

void outline_clusters(matrix * matrix_p, point ** centroids, int num_clusters) {

    int dimensions = 9;

    for (int centroid = 0; centroid < num_clusters; centroid++) {

        int origin[2] = {centroids[centroid]->x - ((dimensions-1)/2), centroids[centroid]->y - ((dimensions-1)/2)};
        for (int cursor = 0; cursor < dimensions * dimensions; cursor ++) {

            int x_pos = cursor % dimensions;
            int y_pos = cursor / dimensions;

            if (origin[1] + y_pos >= 0 &&
                origin[1] + y_pos < matrix_p->y &&
                origin[0] + x_pos >= 0 &&
                origin[0] + x_pos < matrix_p->x)
            {
                matrix_p->array[(origin[1] + y_pos) * matrix_p->x + origin[0] + x_pos] = 255.0;
            }

        }
    }
}
