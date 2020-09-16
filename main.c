#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct NN {
    int *input_layer;
    int *hidden_layer;
    int in, hid, out;
};

struct Board {
    int score;
    int *board;
    int size;
};

void initiaize_board(struct Board *in, int size);
void initiaize_nn(int neurons_input, int neurons_hidden, int neurons_out, struct NN *weights);
int calculate_output(struct NN weights, struct Board current_board);
void layer_transit(int *res, int* input, int *transit_weights, int n_layer_one, int n_layer_two);
void generate_new_weights(int *weights, int num_weights);
int game_play(struct NN ann, int size);
int create_new_element(struct Board in);
void make_game_move(struct Board b, int direction);
void start_recusive_move(struct Board b, int movement, int start_index, int max_index, int current_index, int s, int incre);
void recursive_move(struct Board b, int movement, int start_index, int max_index, int current_index, int biggerQ);
void print_board(struct Board b);
int compare(int a, int b, int method);

int main(void){
    struct NN first_instance;
    int output;
    struct Board in;
    srand((unsigned)time(NULL));

    initiaize_board(&in, 16);
    initiaize_nn(in.size, 10, 4, &first_instance);
    generate_new_weights(first_instance.input_layer, first_instance.in * first_instance.hid);
    generate_new_weights(first_instance.hidden_layer, first_instance.hid * first_instance.out);
    
    create_new_element(in);
    create_new_element(in);
    create_new_element(in);

    printf("Standard:\n");
    print_board(in);

    make_game_move(in, 2);
    printf("DOWN[2]:\n");
    print_board(in);

    make_game_move(in, 1);
    printf("UP[1]:\n");
    print_board(in);

    make_game_move(in, 4);
    printf("RIGHT[4]:\n");
    print_board(in);

    make_game_move(in, 3);
    printf("LEFT[3]:\n");
    print_board(in);

    make_game_move(in, 2);
    printf("DOWN[2]:\n");
    print_board(in);

    output = calculate_output(first_instance, in);

    printf("%d\n", output);
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
        printf("| %4d ", b.board[i]);
    }
    printf("|\n");
    for(j = 0; j < 7*sqr_size + 1; j++){
        printf("_");
    }
    printf("\n");
}

void initiaize_board(struct Board *in, int size){
    int i;
    in->board = calloc(size, sizeof(int));
    in->score = 0;
    in->size = size;
    for(i = 0; i < size; i++){
        in->board[i] = 0;
    }
}

void initiaize_nn(int neurons_input, int neurons_hidden, int neurons_out, struct NN *weights){
    weights->input_layer = (int*)calloc(neurons_input * neurons_hidden, sizeof(int));
    weights->hidden_layer = (int*)calloc(neurons_hidden * neurons_out, sizeof(int));
    weights->in = neurons_input;
    weights->hid = neurons_hidden;
    weights->out = neurons_out;
}

void generate_new_weights(int *weights, int num_weights){
    int i = 0;
    for(; i < num_weights; i++){
        weights[i] = rand() % 10;
    }
}

int calculate_output(struct NN weights, struct Board current_board){
    /* 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT, 0 = debugging */
    int *hidden_res, *output_res, output_variable = 0, output_value = 0, i;
    hidden_res = calloc(weights.hid, sizeof(int));
    output_res = calloc(weights.out, sizeof(int));

    layer_transit(hidden_res, current_board.board, weights.input_layer, weights.in, weights.hid);
    layer_transit(output_res, hidden_res, weights.hidden_layer, weights.hid, weights.out);

    /*
    printf("Hidden:\n");
    for(i = 0; i < weights.hid; i++){
        printf("Index %2d - %5d\n", i, hidden_res[i]);
    }*/

    printf("Output variable:\n");
    for(i = 0; i < weights.out; i++){
        printf("Value %2d had %4d votes.\n", i+1, output_res[i]);
        if(output_value < output_res[i]){
            output_variable = i+1;
            output_value = output_res[i];
        }
    }
    return output_variable;
}

void layer_transit(int *res, int *input, int *transit_weights, int n_layer_one, int n_layer_two){
    int i, j;
    for(i = 0; i < n_layer_two; i++){
        res[i] = 0;
        for(j = i * n_layer_one; j < n_layer_one * (i + 1); j++){
            res[i] += (input[j % n_layer_one] * transit_weights[j]);
            /*printf("[%d]sum: %5d, just added %d * %d\n", i, res[i], input[j % n_layer_one], transit_weights[j]);*/
        }
    }
}

int game_play(struct NN ann, int size){
    struct Board b;
    int direction;
    initiaize_board(&b, size);
    while(create_new_element(b) == 0){
        direction = calculate_output(ann, b);
        make_game_move(b, direction);
    }

    return b.score;
}

void make_game_move(struct Board b, int direction){
    /* 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT, 0 = debugging */
    int sqr_size = (int) sqrt((double) b.size);
    switch(direction){
        case 1:
            start_recusive_move(b, sqr_size, 0, b.size - sqr_size, sqr_size, sqr_size, 1);
            break;
        case 2:
            start_recusive_move(b, - sqr_size, b.size - sqr_size, 0, b.size - (2*sqr_size), sqr_size, 1);
            break;
        case 3:
            start_recusive_move(b, 1, 0, sqr_size - 1, 1, sqr_size, sqr_size);
            break;
        case 4:
            start_recusive_move(b, -1, sqr_size - 1, 0, sqr_size - 2, sqr_size, sqr_size);
            break;
        default:
            printf("ERROR in make_game_move: input - %d", direction);
    }
}

void start_recusive_move(struct Board b, int movement, int start_index, int max_index, int current_index, int s, int incre){
    int i;
    int d = (start_index < max_index) ? 1 : 0;
    for(i = 0; i < s; i++){
        recursive_move(b, movement, start_index + incre*i, max_index + incre*i, current_index + incre*i, d);
    }
}

void recursive_move(struct Board b, int movement, int start_index, int max_index, int current_index, int biggerQ){
    if(b.board[start_index] == 0 && b.board[current_index] != 0){
        b.board[start_index] = b.board[current_index];
        b.board[current_index] = 0;

        recursive_move(b, movement, start_index, max_index, start_index + movement, biggerQ);
    } else if(b.board[start_index] == 0 && b.board[current_index] == 0){
        if(compare(current_index + movement, max_index, biggerQ)){
            recursive_move(b, movement, start_index, max_index, current_index + movement, biggerQ);
        }
    } else if(b.board[start_index] == b.board[current_index]){
        b.board[start_index] = b.board[start_index] * 2;
        b.board[current_index] = 0;
        b.score += b.board[start_index];

        if(compare(start_index + 2 * movement, max_index, biggerQ)){
            recursive_move(b, movement, start_index + movement, max_index, start_index + 2 * movement, biggerQ);
        }
    } else if(b.board[start_index] != 0 && b.board[current_index] == 0){
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

int create_new_element(struct Board in){
    int *empty_indexs, i, j = 0, rand_value;

    empty_indexs = calloc(in.size, sizeof(int));

    empty_indexs[0] = -1;
    for(i = 0; i < in.size; i++){
        if(in.board[i] == 0){
            empty_indexs[j] = i;
            j++;
            if(j < in.size){
                empty_indexs[j] = -1;
            }
        }
    }
    if(j == 0){
        return -1;
    }
    rand_value = rand() % 5;
    rand_value = (rand_value < 4) ? 2 : 4;

    in.board[empty_indexs[rand() % j]] = rand_value;
    return 0;
}