#include <stdlib.h>
#include <stdio.h>
typedef double* Data;

int N;
int size_vector;
int k;
int iterations;

struct linked_list
{
    Data d;
    struct linked_list* next;
};

typedef struct linked_list Element;
typedef Element* Link;

int calc_vector_size(char* input_line, int line_size);
void link_next_line(Link curr, char* input_line, int size_vector,int size_line);
Data calc_vector(char* input_line, int size_line, int size_vector);
Link create_link_list( char* input_line, int size_vector , int size_line);
Data* list_to_matrix(Link head ,int n , int size_vector);
Data* handle_stream();

int calc_vector_size(char* input_line, int line_size)
{
    int size_vector = 1;
    int i = 0;
    for (i = 0; i < line_size; i++)
    {
        if (input_line[i] == ',')
        {
            size_vector++;
        }
    }
    return size_vector;
}

Link create_link_list( char* input_line, int size_vector , int size_line){
    Link head = (Link)malloc(sizeof(Element));
    head -> d = calc_vector(input_line , size_line , size_vector);
    head -> next = NULL;
    return head;
}

void link_next_line(Link curr, char* input_line, int size_vector , int size_line)
{
    Link nextt = (Link)malloc(sizeof(Element));
    nextt->d = calc_vector(input_line , size_line , size_vector);
    nextt->next = NULL;
    curr->next = nextt ;
    // Implement the function as per your requirements
}

Data calc_vector(char* input_line, int size_line, int size_vector)
{
    Data d = (Data)malloc(sizeof(double) * size_vector);
    int counter = 0;
    int i;
    for (i = 0; i < size_line; i++)
    {
        if (input_line[i] == ',')
        {
            d[counter] = atof(&input_line[i + 1]);
            counter++;
        }
    }
    return d;
}

Data* list_to_matrix(Link head ,int n , int size_vector){
    Data p;
    Data *a;
    p = malloc(n * size_vector * sizeof(double));
    a = malloc(n * sizeof(double*));
    if (a== NULL || p ==NULL)
    for (int i = 0; i < n; i++) {
        a[i] = p + i * size_vector;
        for (int j = 0; j < size_vector; j++) {
            a[i][j] = head->d[j];
        }
        head = head->next;
    }
    return a;
}

Data* handle_stream(){
    char* input_line = NULL;
    size_t line_size = 0;
    ssize_t read;
    int counter = 0;
    int size_vector = 0;
    Link head = NULL;
    Link curr = NULL;


    while ((read = getline(&input_line, &line_size, stdin)) != -1)
        {
        if (counter == 0){
            size_vector = calc_vector_size(input_line, read);
            counter = 1;
            printf("%d\n", size_vector);
            head = create_link_list(input_line, size_vector , line_size);
            curr = head;
            }
            else{
                link_next_line(curr, input_line,size_vector , line_size);
                curr = curr->next;
            }
            counter ++;
        }
        free(input_line);
        N = counter;
        Data *matrix = list_to_matrix(head ,N , size_vector);
        return matrix;
    }

int main(int argc, char* argv[])
{
    int clusters = atoi(argv[1]);
    int iterations;
    if (argc == 3)
    {
        iterations = atoi(argv[2]);   
    }
    else if(argc == 2){
            iterations = 200;
        }
        else{    
            printf("Invalid command line arguments.\n");
            return 1;
            }
Data*  matrix = handle_stream();
}
    
