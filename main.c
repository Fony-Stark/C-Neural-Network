#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct NN {
    double *input_layer;
    double *hidden_layer;
    int in, hid, out;
    int score;
};

struct Board {
    int score;
    double *board;
    int size;
};

void initiaize_board(struct Board *in, int size);
void initiaize_nn(int neurons_input, int neurons_hidden, int neurons_out, struct NN *weights);
int calculate_output(struct NN weights, struct Board current_board);
void layer_transit(double *res, double* input, double *transit_weights, int n_layer_one, int n_layer_two);
double sigmoid(double d);
void generate_new_weights(double *weights, int num_weights);
int game_play(struct NN ann, int size, int _bug_fix);
int create_new_element(struct Board *in);
void make_game_move(struct Board *b, int direction, int _bug_fix);
void start_recusive_move(struct Board *b, int movement, int start_index, int max_index, int current_index, int s, int incre);
void recursive_move(struct Board *b, int movement, int start_index, int max_index, int current_index, int biggerQ);
void print_board(struct Board b);
int compare(int a, int b, int method);
int save_weights(char *name_of_file, struct NN weights);
void swap(struct NN *s, int index_a, int index_b);
int partition(struct NN *s, int left, int right, int pivot);
void swap(struct NN *s, int index_a, int index_b);
void quickSort(struct NN *s, int left, int right);
void kill_half_generation(struct NN *gen, int size_gen, int kill);
void fix_arr(struct NN *gen, int index_empty, int max_index);
void create_child(struct NN *gen, int parrent_index, int child_index);
void rebuild_generation(struct NN *gen, int start_index, int max_index);
void train_NN(struct NN start_weigts, int NN_per_generation, int size);
void save_board(struct Board k);
int load_weights(char *name_of_file, struct NN weights);
double abs_val(double d);
void show_progress(struct NN item);
int move_possible(struct Board *b);

int main(void){
    struct NN first_instance;
    /*struct Board in;*/
    srand((unsigned)time(NULL));

    /* initiaize_board(&in, 16); */
    first_instance.input_layer = NULL;
    initiaize_nn(16, 32, 4, &first_instance);
    if(load_weights("./weights",first_instance) == 404){
        generate_new_weights(first_instance.input_layer, first_instance.in * first_instance.hid);
        generate_new_weights(first_instance.hidden_layer, first_instance.hid * first_instance.out);
    }

    train_NN(first_instance, 500, 16);

    return 0;
}

void print_board(struct Board b){
    int sqr_size = (int) sqrt((double) b.size), i, j;
    for(i = 0; i < b.size; i++){
        if(i % sqr_size == 0){
            (i == 0) ? 1 : printf("|\n");
            for(j = 0; j < 7*sqr_size + 1; j++){
                printf("_");
            }
            printf("\n");
        }
        printf("| %4.0f ", b.board[i]);
    }
    printf("|\n");
    for(j = 0; j < 7*sqr_size + 1; j++){
        printf("_");
    }
    printf("\n");
}

void initiaize_board(struct Board *in, int size){
    int i;
    in->board = calloc(size, sizeof(double));
    in->score = 0;
    in->size = size;
    for(i = 0; i < size; i++){
        in->board[i] = 0;
    }
}

void initiaize_nn(int neurons_input, int neurons_hidden, int neurons_out, struct NN *weights){
    if(weights->input_layer == NULL){
        weights->input_layer = (double*)calloc(neurons_input * neurons_hidden, sizeof(double));
        weights->hidden_layer = (double*)calloc(neurons_hidden * neurons_out, sizeof(double));
    }
    weights->in = neurons_input;
    weights->hid = neurons_hidden;
    weights->out = neurons_out;
    weights->score = 0;
}

void generate_new_weights(double *weights, int num_weights){
    int i = 0;
    for(; i < num_weights; i++){
        weights[i] = (double)(rand() % 100000 + 1) / 10000;
    }
}

int calculate_output(struct NN weights, struct Board current_board){
    /* 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT, 0 = debugging */
    double *hidden_res, *output_res, output_variable = 0;
    int output_value = 0, i;
    hidden_res = calloc(weights.hid, sizeof(double));
    output_res = calloc(weights.out, sizeof(double));

    layer_transit(hidden_res, current_board.board, weights.input_layer, weights.in, weights.hid);
    layer_transit(output_res, hidden_res, weights.hidden_layer, weights.hid, weights.out);

    /*
    printf("Hidden:\n");
    for(i = 0; i < weights.hid; i++){
        printf("Index %2d - %5d\n", i, hidden_res[i]);
    }*/

    /*printf("Output variable:\n");*/
    for(i = 0; i < weights.out; i++){
        /*printf("Value %2d had %4.0f votes.\n", i+1, output_res[i]);*/
        if(output_value < output_res[i]){
            output_variable = i+1;
            output_value = output_res[i];
        }
    }
    free(hidden_res);
    free(output_res);
    return output_variable;
}

void layer_transit(double *res, double *input, double *transit_weights, int n_layer_one, int n_layer_two){
    int i, j;
    for(i = 0; i < n_layer_two; i++){
        res[i] = 0;
        for(j = i * n_layer_one; j < n_layer_one * (i + 1); j++){
            res[i] += sigmoid(input[j % n_layer_one]) * transit_weights[j];
            /*printf("[%d]sum: %5d, just added %d * %d\n", i, res[i], input[j % n_layer_one], transit_weights[j]);*/
        }
    }
}

double sigmoid(double d){
    return (d/(sqrt(1 + abs_val(d))));
}

double abs_val(double d){
    if(d > 0){
        return d;
    } else {
        return -d;
    }
}

int game_play(struct NN ann, int size, int _bug_fix){
    struct Board b;
    int direction, i, directions[4], score_loss = 1, total_dir, delta;
    int biggest[3], amounts[3];
    amounts[0] = 0; amounts[1] = 0; amounts[2] = 0;
    biggest[0] = 0; biggest[1] = 0; biggest[2] = 0;
    directions[0] = 0; directions[1] = 0; directions[2] = 0; directions[3] = 0;
    initiaize_board(&b, size);
    sqr_size = (int)sqrt(size);

    while(create_new_element(&b) == 0){
        direction = calculate_output(ann, b);
        if(direction == 0){
            break;
        }
        directions[direction - 1] += 1;
        make_game_move(&b, direction, _bug_fix);
    }
    for(i = 0; i < size; i++){
      if(b.board[i] > biggest[0]){
        biggest[0] = b.board[i];
        amounts[0] = 1;
      } else if(b.board[i] == biggest[0]){
        amounts[0] += 1;
      } else if(b.board[i] > biggest[1]){
        biggest[1] = b.board[i];
        amounts[1] = 1;
      } else if(b.board[i] == biggest[1]){
        amounts[1] += 1;
      } else if(b.board[i] > biggest[2]){
        biggest[2] = b.board[i];
        amounts[2] = 1;
      } else if(b.board[i] == biggest[2]){
        amounts[2] += 1;
      } 
    }
    total_dir = directions[0] + directions[1] + directions[2] + directions[3];
    if((directions[0] + directions[1]) >= total_dir - total_dir * 0.2){
        score_loss = 5;
    } else if((directions[2] + directions[3]) >= total_dir - total_dir * 0.2){
        score_loss = 5;
    }

    score_loss += move_possible(&b); 
    delta = (biggest[0] == b.board[b.size - 4]) ? 3 : 1;
    free(b.board);
    return (((b.score + biggest[0] * amounts[0] * 5 + biggest[1] + amounts[1] * 3 + biggest[2] * amounts[2] * 2) * delta) / score_loss);
}

int move_possible(struct Board *b){
    int start_score, return_value = 0;
    start_score = b->score;

    make_game_move(b, 1, -1);
    make_game_move(b, 2, -1);
    make_game_move(b, 3, -1);
    make_game_move(b, 4, -1);
    
    if(start_score != b->score){
        return_value = 2;
    }

    return return_value;
}

void make_game_move(struct Board *b, int direction, int _bug_fix){
    /* 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT, 0 = debugging */
    int sqr_size = (int) sqrt((double) b->size);
    switch(direction){
        /*                   Board, Movement    , Start_index       , max_index               , current_index, s, incre*/
        case 1:
            start_recusive_move(b, sqr_size     , 0                 , b->size - sqr_size       , sqr_size, sqr_size, 1);
            break;
        case 2:
            start_recusive_move(b, - sqr_size   , b->size - sqr_size , 0                       , b->size - (2*sqr_size), sqr_size, 1);
            break;
        case 3:
            start_recusive_move(b, 1            , 0                 , sqr_size - 1            , 1, sqr_size, sqr_size);
            break;
        case 4:
            start_recusive_move(b, -1           , sqr_size - 1      , 0                       , sqr_size - 2, sqr_size, sqr_size);
            break;
        default:
            printf("ERROR in make_game_move: input - %d nr: %d\n", direction, _bug_fix);
            print_board(*b);
    }
}

void start_recusive_move(struct Board *b, int movement, int start_index, int max_index, int current_index, int s, int incre){
    int i;
    int d = (start_index < max_index) ? 1 : 0;
    for(i = 0; i < s; i++){
        recursive_move(b, movement, start_index + incre*i, max_index + incre*i, current_index + incre*i, d);
    }
}

void recursive_move(struct Board *b, int movement, int start_index, int max_index, int current_index, int biggerQ){
    if(b->board[start_index] == 0 && b->board[current_index] != 0){
        b->board[start_index] = b->board[current_index];
        b->board[current_index] = 0;

        recursive_move(b, movement, start_index, max_index, start_index + movement, biggerQ);
    } else if(b->board[start_index] == 0 && b->board[current_index] == 0){
        if(compare(current_index + movement, max_index, biggerQ)){
            recursive_move(b, movement, start_index, max_index, current_index + movement, biggerQ);
        }
    } else if(b->board[start_index] == b->board[current_index]){
        b->board[start_index] = b->board[start_index] * 2;
        b->board[current_index] = 0;
        b->score += b->board[start_index];

        if(compare(start_index + 2 * movement, max_index, biggerQ)){
            recursive_move(b, movement, start_index + movement, max_index, start_index + 2 * movement, biggerQ);
        }
    } else if(b->board[start_index] != 0 && b->board[current_index] == 0){
        if(compare(current_index + movement, max_index, biggerQ)){
            recursive_move(b, movement, start_index, max_index, current_index + movement, biggerQ);
        }
    } else if(compare(start_index + 2*movement, max_index, biggerQ)){
        recursive_move(b, movement, start_index + movement, max_index, start_index + 2*movement, biggerQ);
    }
}

int compare(int a, int b, int method){
    if(method == 1){
        return a <= b;
    } else {
        return a >= b;
    }
}

int create_new_element(struct Board *in){
    int *empty_indexs, i, j = 0, rand_value, index;

    empty_indexs = calloc(in->size, sizeof(int));

    empty_indexs[0] = -1;
    for(i = 0; i < in->size; i++){
        if(in->board[i] == 0){
            empty_indexs[j] = i;
            j++;
            if(j < in->size){
                empty_indexs[j] = -1;
            }
        }
    }
    if(j == 0){
        free(empty_indexs);
        return -1;
    }
    rand_value = rand() % 5;
    rand_value = (rand_value < 4) ? 2 : 4;
    index = rand() % j;

    in->score += rand_value;

    in->board[empty_indexs[index]] = rand_value;
    free(empty_indexs);
    return 0;
}

int load_weights(char *name_of_file, struct NN weights){
    int i, j, integer, decimal, g;
    char temp;
    char number[255];
    FILE *fp;
    fp = fopen(name_of_file, "r");
    if(fp == NULL){
        printf("I couldn't load weights\n");
        return 404;
    } else {
        printf("I loaded weights\n");
        for(i = 0; i < (weights.in * weights.hid); i++){
            j = 0;
            do{
                temp = (char)fgetc(fp);
                if(temp == ' '){
                    break;
                }
                number[j] = temp;
                j++;
            }while(temp != ' ');
            sscanf(number," %d.%d", &integer, &decimal);
            weights.input_layer[i] = (double)integer;
            if(decimal > 0){
                g=1;
                while(1){
                    if(decimal < pow(10,g)){
                        weights.input_layer[i] += (double)decimal * pow(10, -g);
                        break;
                    }
                    g++;
                }
            }
        }

        for(i = 0; i < (weights.out * weights.hid); i++){
            j = 0;
            do{
                temp = (char)fgetc(fp);
                if(temp == ' '){
                    break;
                }
                number[j] = temp;
                j++;
            }while(temp != ' ');

            sscanf(number," %d.%d", &integer, &decimal);
            weights.hidden_layer[i] = (double)integer;
            if(decimal > 0){
                g=1;
                while(1){
                    if(decimal < pow(10,g)){
                        weights.hidden_layer[i] += (double)(decimal) * pow(10, -g);
                        break;
                    }
                    g++;
                }
            }
        }
        fclose(fp);
        return 0;
    }
}

int save_weights(char *name_of_file, struct NN weights){
    int i;
    FILE *fp;
    fp = fopen(name_of_file, "w+");
    for(i = 0; i < weights.in * weights.hid; i++){
        fprintf(fp, "%f ", weights.input_layer[i]);
    }
    for(i = 0; i < weights.hid * weights.out; i++){
        fprintf(fp, "%f ", weights.hidden_layer[i]);
    }
    fclose(fp);

    return 1;
}

void train_NN(struct NN start_weigts, int NN_per_generation, int size){
    struct NN *generation;
    int i, j, average, best = 0;

    generation = calloc(NN_per_generation, sizeof(struct NN));
    generation[0] = start_weigts;
    for(i = 1; i < NN_per_generation; i++){
        initiaize_nn(generation[0].in, generation[0].hid, generation[0].out, &generation[i]);
        create_child(generation, 0, i);
    } 

    j = 0;
    while(1){
        average = 0;
        for(i = 0; i < NN_per_generation; i++){
            generation[i].score = game_play(generation[i], size, i);
        }
        for(i = 0; i < NN_per_generation; i++){
            average += generation[i].score;
        }
        average = average/200;
        if(average > best){
            best = average;
        }
        quickSort(generation, 0, NN_per_generation - 1);
        if(j % 100 == 0){
            printf("this is generation [%5d] - The best NN got %5d - Average: %5d - Best: %5d - dif = %5d\n", j, generation[0].score, average, best, average - best);
        }
        if(j % 500 == 0){
            save_weights("weights", generation[0]);
        }
        if(j % 1000 == 0){
            show_progress(generation[0]);
        }
        kill_half_generation(generation, NN_per_generation, NN_per_generation/2);
        rebuild_generation(generation, NN_per_generation/2, NN_per_generation);
        j++;
    }
}

void show_progress(struct NN item){
    struct Board b;
    int direction;
    int directions[4];
    directions[0] = 0; directions[1] = 0; directions[2] = 0; directions[3] = 0;
    initiaize_board(&b, 16);
    while(create_new_element(&b) == 0){
        direction = calculate_output(item, b);
        if(direction == 0){
            break;
        }
        directions[direction - 1] += 1;
        make_game_move(&b, direction, 0);
        save_board(b);
    } 
    /* 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT, 0 = debugging */
    printf("Did UP:%3d, DOWN:%3d, LEFT:%3d, RIGHT:%3d, SCORE:%6d\n", directions[0], directions[1], directions[2], directions[3], b.score);
    print_board(b);
    free(b.board);
}

void save_board(struct Board k){
    int i;
    FILE *fp;
    fp = fopen("prove.txt", "w");

    for(i = 0; i < k.size; i++){
        fprintf(fp, "%d ", (int)k.board[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);
}

void kill_half_generation(struct NN *gen, int generation_size, int kill){
    int i, rand_fact, size_gen = generation_size;
    for(i = 0; i < kill; i++){
        rand_fact = rand() % 100;
        if(rand_fact > 20){
            rand_fact = rand() % size_gen + (int)ceil(size_gen * 0.25);
        } else if(rand_fact > 8){
            rand_fact = rand() % (int)ceil(size_gen * 0.25) + (int)ceil(size_gen * 0.8);
        } else if(rand_fact > 1){
            rand_fact = rand() % (int)ceil(size_gen * 0.8) + (int)ceil(size_gen * 0.1);
        } else {
            rand_fact = rand() % (int)ceil(size_gen * 0.1);
        }
        if(rand_fact > generation_size - i - 1){
            rand_fact = generation_size - i - 1;
        }
        initiaize_nn(gen[0].in, gen[0].hid, gen[0].out, &gen[(int)rand_fact]);
        fix_arr(gen, (int)rand_fact, size_gen - i - 1);
    }
}

void rebuild_generation(struct NN *gen, int start_index, int max_index){
    int i, j;
    for(i = 0; i < start_index; i++){
        create_child(gen, i, start_index+i);
    }
    j=0;
    while(i + start_index < max_index){
        create_child(gen, j, start_index+i);
        j++;
        i++;
    }
}

void create_child(struct NN *gen, int parrent_index, int child_index){
    int i;
    double epsilon = 0.05;
    for(i = 0; i < (gen[parrent_index].in * gen[parrent_index].hid); i++){
        if(rand() % 2 == 0){
            gen[child_index].input_layer[i] = gen[parrent_index].input_layer[i] - gen[parrent_index].input_layer[i] * epsilon;
        } else {
            gen[child_index].input_layer[i] = gen[parrent_index].input_layer[i] + gen[parrent_index].input_layer[i] * epsilon;
        }
    }

    for(i = 0; i < (gen[parrent_index].hid * gen[parrent_index].out); i++){
        if(rand() % 2 == 0){
            gen[child_index].hidden_layer[i] = gen[parrent_index].hidden_layer[i] + gen[parrent_index].hidden_layer[i] * epsilon;
        } else {
            gen[child_index].hidden_layer[i] = gen[parrent_index].hidden_layer[i] - gen[parrent_index].hidden_layer[i] * epsilon;
        }
    }
}

void fix_arr(struct NN *gen, int index_empty, int max_index){
    struct NN temp = gen[index_empty];
    gen[index_empty] = gen[max_index];
    gen[max_index] = temp;
}

void swap(struct NN *s, int index_a, int index_b){
    struct NN temp = s[index_a];
    s[index_a] = s[index_b];
    s[index_b] = temp;
}

int partition(struct NN *s, int left, int right, int pivot){
    int leftPointer = left - 1;
    int rightPointer = right;
    int true = 1;

    while(true){
        while(s[++leftPointer].score > pivot){
            /* Do nothing. Why? Because website says so. */
        }

        while(rightPointer > 0 && s[--rightPointer].score < pivot){
            /* Do nothing. Is this smart? I hope so. Does it work? Yes. */
        }

        if(leftPointer >= rightPointer){
            break;
        } else{
            swap(s, leftPointer, rightPointer);
        }
    }

    swap(s, leftPointer, right);

    return leftPointer;
}

void quickSort(struct NN *s, int left, int right){
    int pivot;
    int partitionPoint;
    if(right-left <= 0){
        return;
    } else {
        pivot = s[right].score;
        partitionPoint = partition(s, left, right, pivot);
        quickSort(s, left, partitionPoint - 1);
        quickSort(s, partitionPoint+1, right);
    }
}
