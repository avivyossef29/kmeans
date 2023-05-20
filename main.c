#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef double *Data; // Data is an alias to a pointer to a double
int N = 0, K = 0, iterations = 0;
int vector_size = 0;
double epsilon = 0.001;

struct linked_list {
    Data d;
    struct linked_list *next;
};

typedef struct linked_list Element; // Element is an alias to a linked_list
typedef Element *Link;              // Link is defined as a pointer to an Element

Data calc_vector(const char *input_line, int line_size, int vector_size) {
    Data d = (Data) malloc(sizeof(double) * vector_size);
    if (d == NULL)
    {
        printf("\nAn Error Has Occurred\n");
        exit(0);
    }
    int counter = 0, start = 0, i;
    for (i = 0; i < line_size; i++) {
        if (input_line[i] == ',' || input_line[i] == '\n') {
            size_t entrance_length = i - start;
            char *vector_entrance = (char *) malloc(entrance_length + 1); // Add +1 for the null terminator
            if (vector_entrance == NULL)
            {
                printf("\nAn Error Has Occurred\n");
                exit(0);
            }
            int j;
            for (j = 0; j < entrance_length; j++) {
                vector_entrance[j] = input_line[start + j];
            }
            vector_entrance[entrance_length] = '\0'; // Add the null terminator
            d[counter] = atof(vector_entrance);
            start = i + 1;
            free(vector_entrance);
            counter++;
        }
    }
    return d;
}

int calc_vector_size(const char *input_line, int line_size) {
    int size = 1, i;
    for (i = 0; i < line_size; i++) {
        if (input_line[i] == ',') {
            size++;
        }
    }
    return size;
}

Link initialize_linked_list(char *input_line, int vector_size, int line_size) {
    Link head = (Link) malloc(sizeof(Element));
    if (head == NULL)
    {
        printf("\nAn Error Has Occurred\n");
        exit(0);
    }
    head->d = calc_vector(input_line, line_size, vector_size);
    head->next = NULL;
    return head;
}

void link_next_line(Link curr_element, char *input_line, int vector_size, int line_size) {
    Link next_element = (Link) malloc(sizeof(Element));
    if (next_element == NULL)
    {
        printf("\nAn Error Has Occurred\n");
        exit(0);
    }
    next_element->d = calc_vector(input_line, line_size, vector_size);
    next_element->next = NULL;
    curr_element->next = next_element;
}

Data *list_to_matrix(Link head, int n, int vector_size) {
    Data p;
    Data *a;
    int i;
    p = (Data) calloc(n * vector_size, sizeof(double)); // initialize a one dimensional array of size n * vector_size
    a = (Data *) calloc(n, sizeof(double *));           // initialize an array that represents the rows of the matrix
    if (a == NULL || p == NULL) {
        printf("\nAn Error Has Occurred\n");
        exit(0);
    }
    for (i = 0; i < n; i++) {
        int j;
        a[i] = p + i * vector_size;
        for (j = 0; j < vector_size; j++) {
            a[i][j] = head->d[j];
        }
        head = head->next;
    }
    return a;
}

Data *handle_stream(const char *file_name) {
    FILE *file;
    char *input_line = NULL;
    size_t line_size = 0;
    ssize_t read;
    int counter = 0;
    Link head = NULL, curr_element = NULL;
    file = fopen(file_name, "r");
    if (file == NULL) {
        printf("\nAn Error Has Occurred\n");
        exit(0);
    }
    while ((read = getline(&input_line, &line_size, file)) != -1) {
        if (counter == 0) {
            vector_size = calc_vector_size(input_line, (int) read);
            head = initialize_linked_list(input_line, vector_size, (int) read);
            curr_element = head;
        } else {
            link_next_line(curr_element, input_line, vector_size, (int) read);
            curr_element = curr_element->next;
        }
        counter++;

        // Reset line_size to 0 for the next iteration
        line_size = 0;
    }
    free(input_line);
    N = counter;
    Data *matrix = list_to_matrix(head, N, vector_size);
    return matrix;
}

typedef struct cluster {
    Data centroid;
    Data members;
} cluster;

cluster **set_clusters(Data *matrix) {
    cluster **clusters = (cluster **) malloc(K * sizeof(cluster *));
    int i;
    for (i = 0; i < K; i++) {
        clusters[i] = (cluster *) malloc(sizeof(cluster)); // allocate memory for each cluster object in the clusters array
        clusters[i]->centroid = matrix[i]; // writing the first k vectors as the clusters centroids
    }
    return clusters;
}

double distance(Data v1, Data v2) {
    double sum = 0;
    int i;
    for (i = 0; i < vector_size; i++) {
        sum += pow(v1[i] - v2[i], 2);
    }
    return sqrt(sum);
}

int find_cluster(cluster **clusters, Data vector) {
    int index = -1, i;
    double min_distance =
            pow(2, sizeof(double) * 8 - 2) * (1 - pow(2, -52)); // set min_distance to the max possible double value
    double curr_distance;
    Data curr_centroid;
    for (i = 0; i < K; i++) {
        curr_centroid = clusters[i]->centroid;
        curr_distance = distance(curr_centroid, vector);
        if (curr_distance < min_distance) {
            min_distance = curr_distance;
            index = i;
        }
    }
    return index;
}

void add_to_cluster(cluster **clusters, int *cluster_members_counter_copy, int index, Data vector) {
    int i;
    for (i = 0; i < vector_size; i++) {
        clusters[index]->members[(cluster_members_counter_copy[index] - 1) * vector_size + i] = vector[i];
    }
    cluster_members_counter_copy[index]--; // decrease the cluster's members count. Why? because
    // essentially we use cluster_members_counter twice:
    // 1. to calculate how much space to allocate for each cluster members
    // 2. to keep track of where to paste each vector in members array
    // (we paste the vectors in reversed order - from the last position to the
    // first position)

    /*/ debug section start
    printf("\n");
    for (i = 0; i < K; i++) {
        if (cluster_members_counter_copy[2] == 1) {
            continue;
        }
        printf("%d\t", cluster_members_counter_copy[i]);
    }
    printf("\n");
     // debug section start */
}

int *initialize_cluster_members_counter() {
    int i;
    int *cluster_members_counter = (int *) malloc(K * sizeof(int));
    if (cluster_members_counter == NULL) {
        printf("failed to allocate memory for cluster_members_counter");
    }
    for (i = 0; i < K; i++) {
        cluster_members_counter[i] = 0;
    }
    return cluster_members_counter;
}

void set_cluster_members_counter(cluster **clusters, Data *matrix, int *cluster_members_counter) {
    int i, index;
    for (i = 0; i < N; i++) // go over each vector in the matrix
    {
        index = find_cluster(clusters, matrix[i]);
        cluster_members_counter[index]++;
    }
}

void allocate_memory_for_cluster_members(cluster **clusters, int *cluster_members_counter) {
    int i;
    for (i = 0; i < K; i++) {
        size_t members_num = vector_size * sizeof(double) * cluster_members_counter[i];
        clusters[i]->members = (Data) malloc(members_num);
        //debug section start
        int j;
        for (j = 0; j < (vector_size * cluster_members_counter[i]); j++) {
            clusters[i]->members[j] = 0.0;
        }
        //debug section end
    }
}

int *copy_array(int *arr, int arr_length) {
    int i;
    int *copy = (int *) malloc(arr_length * sizeof(int));
    for (i = 0; i < arr_length; i++) {
        copy[i] = arr[i];
    }
    return copy;
}

void add_vectors(Data v1, Data v2) {
    int i;
    for (i = 0; i < vector_size; i++) {
        v1[i] = v1[i] + v2[i];
    }
}

Data calc_centroid(cluster **clusters, const int *cluster_members_counter, int index) {
    Data sum = (Data) malloc(vector_size * sizeof(double)); // allocate memory for the vector 'sum'
    int i;
    for (i = 0; i < vector_size; i++) // initialize 'sum' as the zero vector
    {
        sum[i] = 0;
    }
    int num_of_members = cluster_members_counter[index];
    if (num_of_members == 0) // if the cluster has no members then the centroid stays the same
    {
        return clusters[index]->centroid;
    }
    for (i = 0; i < num_of_members; i++) // sum all the members in the cluster
    {
        add_vectors(sum, clusters[index]->members + i * vector_size);
    }
    for (i = 0; i < vector_size; i++) // divide each entrance of the vector by the number of members in the cluster
    {
        sum[i] = sum[i] / num_of_members;
    }
    return sum;


}

int main(int argc, char *argv[]) {
    Data *matrix;
    K = atoi(argv[1]);
    if (argc == 4) // max iterations parameter is given
    {
        iterations = atoi(argv[2]);
        matrix = handle_stream(argv[3]); // each row in matrix is a vector given in input
    } else if (argc == 3) // max iterations parameter not given
    {
        iterations = 200;
        matrix = handle_stream(argv[2]); // each row in matrix is a vector given in input
    } else // not enough arguments given
    {
        printf("Invalid command line arguments.\n");
        return 1;
    }
    if (K <= 1 || K >= N) {
        printf("Invalid number of clusters!\n");
    }
    int valid = 0;
    cluster **clusters = set_clusters(matrix); // initialize an array with K pointers to clusters
    // and set the first K vectors in matrix the centroids

    // debug section start
    printf("the first K centroids are:\n");
    int i;
    for (i = 0; i < K; i++) // for every cluster
    {
        int j;
        for (j = 0; j < vector_size; j++) {
            printf("%f\t", clusters[i]->centroid[j]);
        }
        printf("\n");
    }
    // debug section end

    while (iterations > 0 && !valid) // line 67 on python file
    {
        valid = 1;
        int i, index;
        int *cluster_members_counter;
        cluster_members_counter = initialize_cluster_members_counter();

        // debug section start
        for (i = 0; i < K; i++) {
            printf("%d\t", cluster_members_counter[i]); // 0       0       0
        }
        printf("\n");
        //debug section end

        set_cluster_members_counter(clusters, matrix, cluster_members_counter);

        // debug section start
        for (i = 0; i < K; i++) {
            printf("%d\t", cluster_members_counter[i]); // 438     219     143
        }
        printf("\n");
        //debug section end

        allocate_memory_for_cluster_members(clusters, cluster_members_counter);

        /*/debug section start
        for (i = 0; i < K; i++) {
            int j;
            for (j = 0; j < vector_size * cluster_members_counter[i]; j++) {
                if ((j % vector_size) == 0) {
                    printf("\n");
                }
                printf("%f\t", clusters[i]->members[j]); // 0.000000        0.000000        0.000000
            }
        }
        printf("\n");
        //debug section end */

        int *cluster_members_counter_copy;
        cluster_members_counter_copy = copy_array(cluster_members_counter, vector_size);

        /*/ debug section start
        for (i = 0; i < K; i++) {
            printf("%d\t", cluster_members_counter_copy[i]); // 438     219     143
        }
        printf("\n");
        //debug section end */

        for (i = 0; i < N; i++) // for each vector given in input
        {
            index = find_cluster(clusters, matrix[i]); // find the vector's cluster

            /*/ debug section start
            printf("%d\n", index);
            //debug section end */

            // add the vector in the i'th row to its cluster
            add_to_cluster(clusters, cluster_members_counter_copy, index, matrix[i]);
        }
        free(cluster_members_counter_copy);

        /*debug section start
        for (i = 0; i < K; i++) {
            printf("######################################\n");
            printf("###    this is cluster number %d    ###\n", i);
            printf("######################################\n");
            int j;
            for (j = 0; j < vector_size * cluster_members_counter[i]; j++) {
                if ((j % vector_size) == 0) {
                    printf("\n");
                }
                printf("%f\t", clusters[i]->members[j]); // 0.000000        0.000000        0.000000
            }
            printf("\n\n");
        }
        printf("\n");
        //debug section end */

        for (i = 0; i < K; i++) {
            Data new_centroid = calc_centroid(clusters, cluster_members_counter, i);
            if (valid) {
                double delta = distance(new_centroid, clusters[i]->centroid);
                if (delta > epsilon) {
                    valid = 0;
                }
            }
            clusters[i]->centroid = new_centroid;
            free(clusters[i]->members);
        }
        iterations--;
    }
    //int i;
    for (i = 0; i < K; i++) // for every cluster
    {
        int j;
        for (j = 0; j < vector_size; j++) {
            printf("%.4f\t", clusters[i]->centroid[j]);
        }
        printf("\n");
    }
}
