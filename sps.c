#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_COMMANDS 1000
#define MAX_IN_COMMAND 1000
#define MAX_TMP_VARS 10


typedef struct commands {

    char *item[MAX_COMMANDS];
    int size;

} Commands;

typedef struct cell {

    int capacity;
    int size;
    char *word;

} Cell;

typedef struct row {

    int capacity;
    int size;
    Cell *cells;

} Row;

typedef struct table {

    int capacity;
    int size;
    int max_cells;
    Row *rows;

} Table;

typedef struct coords {

    int row_start;
    int row_finish;
    int col_start;
    int col_finish;

} Coordinates;

typedef struct variable {

    int size;
    Cell cell[MAX_TMP_VARS];

} Variables;

void array_ctor(Cell *a);

int array_fill(FILE *f, Table *t, Cell *a, char *delim);

void array_resize(FILE *f, Table *t, Cell *a, int i);

void array_append(FILE *f, Table *t, Cell *a, int c);

void array_dtor(Cell *a);

void row_ctor(Row *r);

int row_fill(FILE *f, Table *t, Row *r, char *delim);

void row_resize(FILE *f, Table *t, Row *r, int i);

void row_append(FILE *f, Table *t, Row *r, Cell *a, int idx);

void row_dtor(Row *r);

void table_ctor(Table *t);

void table_fill(FILE *f, Table *t, char *delim);

void table_resize(FILE *f, Table *t, int i);

void table_append(FILE *f, Table *t, Row *r);

void table_print(Table *t, char delim);

void table_dtor(Table *t);

void parse_commands(char *argv, Commands *commands);

void commands_use(FILE *f, Commands *commds, Table *t, Coordinates *coords, Variables *tmp_vars);

void table_normalize(FILE *f, Table *t, int max_cells, int max_rows);

void coordinates_change(FILE *f, Table *t, Coordinates *coords, char *r_start, char *c_start, char *r_finish, char *c_finish);

Row row_create(FILE *f, Table *t);

void problem(FILE *f, Table *t, int i);

bool is_digit(char *string);

void irow(FILE *f, Table *t, Coordinates *coords);

void arow(FILE *f, Table *t, Coordinates *coords);

void row_insert(FILE *f, Table *t, int idx);

void drow(FILE *f, Table *t, Coordinates *coords);

void table_sort(FILE *f, Table *t);

void icol(FILE *f, Table *t, Coordinates *coords);

void array_insert(FILE *f, Table *t, int idx);

Cell array_create(FILE *f, Table *t);

void acol(FILE *f, Table *t, Coordinates *coords);

void dcol(FILE *f, Table *t, Coordinates *coords);

void row_sort(FILE *f, Table *t, Row *r, int i);

void find_max(FILE *f, Table *t, Coordinates *coords);

void find_min(FILE *f, Table *t, Coordinates *coords);

void find_STR(FILE *f, Table *t, Coordinates *coords, char *STR);

void set_STR(FILE *f, Table *t, Coordinates *coords, char *STR);

void clear(FILE *f, Table *t, Coordinates *coords);

void swap(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2);

void swap_cells(FILE *f, Table *t, Cell *a, Cell *b);

void sum(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2);

char *cell_to_string(FILE *f, Table *t, Cell *cell);

void cell_set_word(FILE *f, Table *t, Cell *a, char *word);

Cell * get_cell(FILE *f, Table *t, int y, int x);

void avg(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2);

void count(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2);

void len(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2);

void define(FILE *f, Table *t, Coordinates *coords, Variables *tmp_vars, char *command1);

void tmp_vars_init(FILE *f, Table *t, Variables *vars);

void tmp_vars_dtor(Variables *vars);

void use(FILE *f, Table *t, Coordinates *coords, Variables *vars, char *command1);

void inc(FILE *f, Table *t, Variables *vars, char *command1);

void cell_copy(FILE *f, Table *t, Cell *a, Cell b);

int main(int argc, char **argv)
{

//    argv[1] = "irow";
//    argv[2] = "tab1.txt";
//    argc = 3;


    char *delim = NULL;
    char *filename = NULL;

    Commands commands = {{NULL}, 0};

    if (argc == 3)
    {

        delim = " ";
        parse_commands(argv[1], &commands);

        filename = argv[2];

    }
    else if ((argc == 5) && ((strcmp(argv[1],"-d")) == 0))
    {

        delim = argv[2];
        (parse_commands(argv[3], &commands));
        filename = argv[4];

    }
    else
    {
        problem(NULL,NULL,6);
        return 1;
    }


    FILE *f = fopen(filename,"r");
    if (NULL == f)
    {
        problem(f, NULL, 2);
    }

    Table t;


    table_ctor(&t);
    table_fill(f, &t, delim);       //      filling table with words
    table_normalize(f, &t, t.max_cells-1, t.size-1);        //      normalizing table (all rows has one column number)

    Variables temporary_variables;
    tmp_vars_init(f, &t, &temporary_variables);

    Coordinates c;
    coordinates_change(f, &t, &c, "1", "1", "1", "1");      //      creating first coordinates waypoint [1,1]

    commands_use(f, &commands, &t, &c, &temporary_variables);     //using all commands

    table_print(&t, delim[0]);

    tmp_vars_dtor(&temporary_variables);
    table_dtor(&t);         //      end of work
    fclose(f);

    return 0;
}

void tmp_vars_dtor(Variables *vars) {

    for (int i = 0; i < vars->size; ++i) {

        array_dtor(&vars->cell[i]);

    }

//    free(vars->cell);

}

void tmp_vars_init(FILE *f, Table *t, Variables *vars) {

    (void) f;
    (void) t;

//    vars->cell = malloc(MAX_TMP_VARS * sizeof(Cell));
    vars->size = 0;

    printf("Tmp vars initialized!\n");

}

void
coordinates_change(FILE *f, Table *t, Coordinates *coords, char *r_start, char *c_start, char *r_finish, char *c_finish) {

    int row_start_int;
    int row_finish_int;
    int col_start_int;
    int col_finish_int;


    // row start
    if  (r_start) {

        if (strcmp(r_start, "_") == 0) {
            coords->row_start = 0;
            coords->row_finish = t->size - 1;

        } else if (is_digit(r_start)) {

            row_start_int = (int) atoi(r_start) - 1;

            coords->row_start = row_start_int;
            coords->row_finish = row_start_int;
            if (row_start_int < 0)
                problem(f, t, 5);

        }

    }
    else
        problem(f,t,5);
    //


    // col start
    if  (c_start) {

        if (strcmp(c_start, "_") == 0) {
            coords->col_start = 0;
            coords->col_finish = t->max_cells - 1;

        } else if (is_digit(c_start)) {

            col_start_int = (int) atoi(c_start) - 1;

            coords->col_start = col_start_int;
            coords->col_finish = col_start_int;
            if (col_start_int < 0)
                problem(f, t, 5);

        }

    }
    else
        problem(f,t,5);
    //


    //row finish
    if  ((r_finish) && (strcmp(r_finish,"_")) == 0)
    {
            coords->row_finish = t->size - 1;

    }
    else if ((r_finish) && (is_digit(r_finish)))
    {

        row_finish_int = (int) atoi(r_finish) - 1;

        if  (coords->row_finish <= row_finish_int)
        {

            coords->row_finish = row_finish_int;

            if (row_finish_int < 0)
                problem(f, t, 5);

        }else{
            problem(f,t,5);
        }

    }
    //


    //col finish
    if  ((c_finish) && (strcmp(c_finish,"_")) == 0)
    {

        coords->col_finish = t->max_cells - 1;

    }
    else if ((c_finish) && (is_digit(c_finish)))
    {

        col_finish_int = (int) atoi(c_finish) - 1;

        if  (coords->col_finish <= col_finish_int)
        {

            coords->col_finish = col_finish_int;

            if  (col_finish_int < 0)
            {
                problem(f,t,5);

            }


        }else{
            problem(f,t,5);
        }



    }
    //

    if  (t->size - 1 < coords->row_finish || t->max_cells - 1 < coords->col_finish)
    {

        table_normalize(f,t,coords->col_finish,coords->row_finish);

    }




//    printf("Row start: %i, finish: %i\n", coords->row_start, coords->row_finish);
//    printf("Col start: %i, finish: %i\n", coords->col_start, coords->col_finish);


}

bool is_digit(char *string) {

    for (int i = 0; i < (int) strlen(string); ++i) {

        if (string[i] < 48 || string[i] > 57) {
            return 0;
        }

    }

    return true;

}

void table_normalize(FILE *f, Table *t, int max_cells, int max_rows) {

//    printf("Max cells: %i\n",max_cells);
//    printf("Max rows: %i\n",max_rows);

    for (int i = 0; i < t->size; i++) {

        if  (t->rows[i].size <= max_cells)
        {

            for (int j = t->rows[i].size; j <= max_cells; ++j) {

                Cell a;
                array_ctor(&a);
                row_append(f, t, &t->rows[i], &a, -1);

            }
            t->max_cells = max_cells+1;


        }

    }

    for (int i = t->size; i <= max_rows; i++) {

        Row tmp_row = row_create(f, t);
        table_append(f, t, &tmp_row);

    }

}

void commands_use(FILE *f, Commands *commds, Table *t, Coordinates *coords, Variables *tmp_vars) {

    char command1[50];
    char command2[50];
    char command3[50];
    char command4[50];

    for (int i = 0; i < commds->size; i++)
    {
        printf("%s\n", commds->item[i]);
        if (strcmp(commds->item[i], "[set]") == 0)
        {
            printf("cmnd: set ""\n");
            continue;
        }
        else if (strcmp(commds->item[i], "[max]") == 0)
        {
            find_max(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "[min]") == 0)
        {
            find_min(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "[_]") == 0)
        {
            printf("cmnd: \'_\'\n");
            continue;
        }
        else if (strcmp(commds->item[i], "irow") == 0)
        {
            irow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "arow") == 0)
        {
            arow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "drow") == 0)
        {
            drow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "icol") == 0)
        {
            icol(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "acol") == 0)
        {
            acol(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "dcol") == 0)
        {
            dcol(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "clear") == 0)
        {
            clear(f,t,coords);
            continue;
        }
        else if (sscanf(commds->item[i], "[%[^,],%[^,],%[^,],%[^]]]", command1, command2, command3, command4) == 4)
        {
//            printf("cmnd choose1: \'%s\' \'%s\' \'%s\' \'%s\'\n",command1,command2,command3,command4);
            coordinates_change(f, t, coords, command1, command2, command3, command4);
            continue;
        }
        else if (sscanf(commds->item[i], "[%[^,],%[^]]]", command1, command2) == 2)
        {
//            printf("cmnd choose2: \'%s\' \'%s\'\n",command1,command2);
            coordinates_change(f, t, coords, command1, command2, NULL, NULL);
            continue;
        }
        else if (sscanf(commds->item[i], "[find %[^]]]", command1) == 1)
        {
            find_STR(f, t, coords, command1);
            continue;
        }
        else if (sscanf(commds->item[i], "swap [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd swap: \'%s\' \'%s\'\n",command1,command2);
            swap(f,t,coords,command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "sum [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd sum: \'%s\' \'%s\'\n",command1,command2);
            sum(f, t, coords, command1, command2);
            continue;
        }
        else if (sscanf(commds->item[i], "avg [%[^,],%[^]]]", command1, command2) == 2)
        {
            avg(f,t,coords,command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "count [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd count: \'%s\' \'%s\'\n",command1,command2);
            count(f,t,coords,command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "len [%[^,],%[^]]]", command1, command2) == 2)
        {
            len(f,t,coords,command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "set %s", command1) == 1)
        {
            set_STR(f,t,coords,command1);
            continue;
        }
        else if (sscanf(commds->item[i], "def _%s", command1) == 1)
        {
            printf("cmnd def: \'%s\'\n",command1);
            define(f, t, coords, tmp_vars, command1);
            continue;
        }
        else if (sscanf(commds->item[i], "use _%s", command1) == 1)
        {
            printf("cmnd use: \'%s\'\n",command1);
            use(f,t,coords,tmp_vars,command1);
            continue;
        }
        else if (sscanf(commds->item[i], "inc _%s", command1) == 1)
        {
            printf("cmnd inc: \'%s\'\n",command1);
            inc(f,t,tmp_vars,command1);
            continue;
        }
        else if (sscanf(commds->item[i], "goto +%s", command1) == 1)
        {
            printf("cmnd goto+: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "goto -%s", command1) == 1)
        {
            printf("cmnd goto-: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "iszero _%[^ ]%s", command1, command2) == 2)
        {
            printf("cmnd iszero: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "sub _%[^ ] _%s", command1, command2) == 2)
        {
            printf("cmnd sub: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }

    }


}

void inc(FILE *f, Table *t, Variables *vars, char *command1) {

    (void) vars;
    int number = 0;

    if (is_digit(command1)){

        number = (int) atoi(command1);
    }

    if  (number < 0 || number > 9)
        problem(f,t,7);



}

void use(FILE *f, Table *t, Coordinates *coords, Variables *vars, char *command1) {

    int number = 0;

    if (is_digit(command1)){

        number = (int) atoi(command1);
    }

    if  (number < 0 || number > 9)
        problem(f,t,7);

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

//            cell_copy(f,t,&t->rows[i].cells[j],vars->cell[number]);
//            t->rows[i].cells[j] = vars->cell[number];
//            array_dtor(&t->rows[i].cells[j]);
//            t->rows[i].cells[j] = array_create(f,t);
//            memcpy(&t->rows[i].cells[j],&vars->cell[number],sizeof(Cell));
            cell_copy(f,t,&t->rows[i].cells[j],vars->cell[number]);

        }

    }

}

void cell_copy(FILE *f, Table *t, Cell *a, Cell b) {

    int size = b.size;
    char *tmp = realloc(a->word,size * sizeof(char));

    if (NULL == tmp)
        problem(f,t,1);

    a->word = tmp;
    a->size = size;
    a->capacity = size;

    memcpy(a->word,b.word,size * sizeof(char));

}

void define(FILE *f, Table *t, Coordinates *coords, Variables *tmp_vars, char *command1) {

    int number = 0;

    if (is_digit(command1)){

        number = (int) atoi(command1);
    }

    if  (number < 0 || number > 9)
        problem(f,t,7);

//    tmp_vars->cell[number] = t->rows[coords->row_start].cells[coords->col_start];
//    tmp_vars->cell[number] = (*t).rows[coords->row_start].cells[coords->col_start];
//    memcpy(&tmp_vars->cell[number],&t->rows[coords->row_start].cells[coords->col_start],sizeof(Cell));

    Cell a = array_create(f,t);
    cell_copy(f,t,&a,(*t).rows[coords->row_start].cells[coords->col_start]);

    tmp_vars->cell[number] = a;

    if (tmp_vars->size == number)
        tmp_vars->size++;

}

void len(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2) {

    int y = (int) atoi(command1) - 1;
    int x = (int) atoi(command2) - 1;
    int len = 0;
    char tmp[1000];

    table_normalize(f,t,x,y);

    Cell *a = get_cell(f,t,coords->col_start,coords->col_finish);

    for (int i = 0; i < a->size; ++i) {

        if (a->word[i] != '\\')
            len++;

    }

    sprintf(tmp,"%i",len);
    Cell *b = get_cell(f,t,y,x);
    cell_set_word(f,t,b,tmp);


}

void count(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2) {

    int y = (int) atoi(command1) - 1;
    int x = (int) atoi(command2) - 1;

    table_normalize(f,t,x,y);

    char tmp_word[1000];
    int cell_counter = 0;

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            Cell *b = get_cell(f,t,i,j);

            if (b->size > 0)
            {

                cell_counter++;


            }

        }

    }

    Cell *a = get_cell(f,t,y,x);
    sprintf(tmp_word,"%i",cell_counter);
    cell_set_word(f, t, a, tmp_word);

}

void avg(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2) {

    int y = (int) atoi(command1) - 1;
    int x = (int) atoi(command2) - 1;

    table_normalize(f,t,x,y);

    float sum = 0;
    char* tmp_word;
    float cell_counter = 0;
    bool set = false;

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            tmp_word = cell_to_string(f,t,&t->rows[i].cells[j]);

            if (is_digit(tmp_word))
            {

                sum += (float) atof(tmp_word);
                set = true;
                cell_counter++;


            }

        }

    }

    if  (set)
    {

        Cell *a = get_cell(f,t,y,x);
        sprintf(tmp_word,"%g",sum/cell_counter);
        cell_set_word(f, t, a, tmp_word);

    }

    printf("Sum: %g\n",sum);

}

void sum(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2) {

    int y = (int) atoi(command1) - 1;
    int x = (int) atoi(command2) - 1;

    table_normalize(f,t,x,y);


    float sum = 0;
    char* tmp_word;

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            tmp_word = cell_to_string(f,t,&t->rows[i].cells[j]);

            if (is_digit(tmp_word))
            {

                sum += (float) atof(tmp_word);
                Cell *a = get_cell(f,t,y,x);
                sprintf(tmp_word,"%g",sum);
                cell_set_word(f, t, a, tmp_word);

            }

        }

    }

    printf("Sum: %g\n",sum);

}

Cell *get_cell(FILE *f, Table *t, int y, int x) {

    (void) f;

    return &t->rows[y].cells[x];

}

void cell_set_word(FILE *f, Table *t, Cell *a, char *word) {

    (void) f;
    (void) t;

    int size = (int) strlen(word);
    char *tmp = realloc(a->word,size * sizeof(char));

    for (int i = 0; i < size; ++i) {

        tmp[i] = word[i];

    }

    a->capacity = size;
    a->size = size;
    a->word = tmp;

}

char *cell_to_string(FILE *f, Table *t, Cell *cell) {

    (void) f;
    (void) t;

    char *tmp = realloc(cell->word,(cell->size+1) * sizeof(char));
    tmp[cell->size] = '\0';
    cell->capacity++;
    cell->word = tmp;

    return cell->word;
}

void swap(FILE *f, Table *t, Coordinates *coords, char *command1, char *command2) {

    int y = (int) atoi(command1) - 1;
    int x = (int) atoi(command2) - 1;

    table_normalize(f,t,x,y);

    Cell *a = &t->rows[y].cells[x];

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            Cell *b = &t->rows[i].cells[j];
            swap_cells(f,t,b,a);
            a = &t->rows[y].cells[x];

        }

    }


}

void swap_cells(FILE *f, Table *t, Cell *a, Cell *b) {

    (void) f;
    (void) t;
    Cell tmp = *a;
    *a = *b;
    *b = tmp;

}

void clear(FILE *f, Table *t, Coordinates *coords) {

    (void) f;

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            array_dtor(&t->rows[i].cells[j]);

        }

    }

}

void set_STR(FILE *f, Table *t, Coordinates *coords, char *STR) {

    int size = (int) strlen(STR);

    char *tmp_array;

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            t->rows[i].cells[j].size = size;
            tmp_array = realloc(t->rows[i].cells[j].word, sizeof(char) * size);

            if  (NULL == tmp_array)
                problem(f,t,1);

            t->rows[i].cells[j].word = tmp_array;

            for (int k = 0; k < size; ++k)
            {
                t->rows[i].cells[j].word[k] = STR[k];
            }

        }

    }

}

void find_STR(FILE *f, Table *t, Coordinates *coords, char *STR) {

    char *tmp = NULL;

    int row_start_int = coords->row_start, row_finish_int = coords->row_finish;
    int col_start_int = coords->col_start, col_finish_int = coords->col_finish;

    for (int i = row_start_int; i <= row_finish_int; ++i) {

        for (int j = col_start_int; j <= col_finish_int; ++j) {

            tmp = cell_to_string(f,t,&t->rows[i].cells[j]);

            if (strcmp(tmp,STR) == 0)
            {

                coords->row_start = coords->row_finish = i;
                coords->col_start = coords->col_finish = j;

            }


        }

    }
//    printf("Row start: %i, finish: %i\n", coords->row_start, coords->row_finish);
//    printf("Col start: %i, finish: %i\n", coords->col_start, coords->col_finish);

}

void find_min(FILE *f, Table *t, Coordinates *coords) {

    (void) f;

    float min = 0;
    char *tmp = NULL;
    bool set = false;
    int row_start_int = coords->row_start, row_finish_int = coords->row_finish;
    int col_start_int = coords->col_start, col_finish_int = coords->col_finish;

    for (int i = row_start_int; i <= row_finish_int; ++i) {

        for (int j = col_start_int; j <= col_finish_int; ++j) {

            tmp = cell_to_string(f,t,&t->rows[i].cells[j]);

            if  (is_digit(tmp))
            {
                printf("%s\n",tmp);
                if  ((float) atof(tmp) < min || !set)
                {
                    min = (float) atof(tmp);
                    set = true;
                    coords->row_start = coords->row_finish = i;
                    coords->col_start = coords->col_finish = j;


                }

            }

        }

    }
//    printf("Min: %f\n",min);
//    printf("Row start: %i, finish: %i\n", coords->row_start, coords->row_finish);
//    printf("Col start: %i, finish: %i\n", coords->col_start, coords->col_finish);

}

void find_max(FILE *f, Table *t, Coordinates *coords) {

    (void) f;

    float max = 0;
    char *tmp = NULL;
    bool set = false;
    int row_start_int = coords->row_start, row_finish_int = coords->row_finish;
    int col_start_int = coords->col_start, col_finish_int = coords->col_finish;

    for (int i = row_start_int; i <= row_finish_int; ++i) {

        for (int j = col_start_int; j <= col_finish_int; ++j) {

            tmp = cell_to_string(f,t,&t->rows[i].cells[j]);

            if  (is_digit(tmp))
                {
                    printf("%s\n",tmp);
                    if  ((float) atof(tmp) > max || !set)
                    {
                        max = (float) atof(tmp);
                        set = true;
                        coords->row_start = coords->row_finish = i;
                        coords->col_start = coords->col_finish = j;


                    }

                }

        }

    }
//    printf("Max: %.f\n",max);
//    printf("Row start: %i, finish: %i\n", coords->row_start, coords->row_finish);
//    printf("Col start: %i, finish: %i\n", coords->col_start, coords->col_finish);


}

void dcol(FILE *f, Table *t, Coordinates *coords) {

    if (coords->col_start == 0 && coords->col_finish == t->max_cells-1)
    {
        table_dtor(t);
        return;
    }

    for (int i = 0; i < t->size; ++i) {

        for (int j = coords->col_start; j <= coords->col_finish; ++j) {

            array_dtor(&t->rows[i].cells[j]);
            t->rows[i].size--;
            row_sort(f, t, &t->rows[i], j);

        }


    }

}

void row_sort(FILE *f, Table *t, Row *r, int i) {

    (void) f;
    (void) t;

    for (int j = i; j < r->size; ++j) {

        r->cells[j] = r->cells[j+1];

    }

}

void acol(FILE *f, Table *t, Coordinates *coords) {


    int idx = coords->col_finish + 1;
    array_insert(f,t,idx);

}

void icol(FILE *f, Table *t, Coordinates *coords) {

    int idx = coords->col_start;
    array_insert(f,t,idx);

}

void array_insert(FILE *f, Table *t, int idx) {

    for (int i = 0; i < t->size; ++i) {

        if  (t->rows[i].size == t->rows[i].capacity)
        {
            row_resize(f,t,&t->rows[i],1);
        }

        for (int j = t->rows[i].size - 1; j >= idx; j--) {

            t->rows[i].cells[j + 1] = t->rows[i].cells[j];

        }
        t->rows[i].cells[idx] = array_create(f,t);
        t->rows[i].size++;

    }

}

Cell array_create(FILE *f, Table *t) {

    (void) f;
    (void) t;
    Cell result;
    array_ctor(&result);

    return result;
}

void drow(FILE *f, Table *t, Coordinates *coords) {

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        row_dtor(&t->rows[i]);

    }

    table_sort(f,t);

}

void table_sort(FILE *f, Table *t) {

    (void) f;
    for (int i = 0; i < t->size; ++i) {

        if  (t->rows[i].cells == NULL)
        {
            for (int j = i; j < t->size-1; ++j) {

                t->rows[j] = t->rows[j+1];

            }
            t->size--;

        }

    }



}

void arow(FILE *f, Table *t, Coordinates *coords) {

    int idx = coords->row_finish + 1;
    row_insert(f, t, idx);

}

void row_insert(FILE *f, Table *t, int idx) {

    if (t->size == t->capacity)
        table_resize(f,t,1);

    if (t->size < t->capacity)
    {
        for (int i = t->size - 1; i >= idx; i--)
            t->rows[i+1] = t->rows[i];

        t->rows[idx] = row_create(f, t);
        t->size++;
    }

}

void irow(FILE *f, Table *t, Coordinates *coords) {

    int idx = coords->row_start;
    row_insert(f, t, idx);

}

Row row_create(FILE *f, Table *t) {

    (void) f;
    Row new_row;
    row_ctor(&new_row);

    for (int j = 0; j < t->max_cells; j++) {

        Cell a;
        array_ctor(&a);
        row_append(f, t, &new_row, &a, -1);

    }


    return new_row;
}

void problem(FILE *f, Table *t, int i) {

    switch (i) {

        case 1:
            fprintf(stderr,"OS didn't give memory (realloc failed)\n");
            break;
        case 2:
            fprintf(stderr,"file does not exist\n");
            break;
        case 3:
            fprintf(stderr,"too big args\n");
            break;
        case 4:
            fprintf(stderr,"too much args\n");
            break;
        case 5:
            fprintf(stderr,"bad coordinates\n");
            break;
        case 6:
            fprintf(stderr,"bad args\n");
            break;
        case 7:
            fprintf(stderr,"bad var choise\n");

            break;
        default:
            fprintf(stderr,"some problems\n");
    }

    if  (t)
        table_dtor(t);
    if (f)
        fclose(f);

    exit(1);
}

void parse_commands(char *argv, Commands *commands) {

    int i = 0;
    char *result = strtok (argv,";");
    while (result != NULL)
    {
        if (i < MAX_COMMANDS)
        {
            if  ( (int) strlen(result) <= MAX_IN_COMMAND)
            {
                commands->item[i++] = result;
                result = strtok(NULL,";");

            }
            else
            {
                problem(NULL, NULL, 3);
                return;
            }


        }
        else
        {
            problem(NULL, NULL, 4);
            return;
        }
    }
    commands->size = i;

}

void table_dtor(Table *t) {

    while (t->size != 0)
    {

        row_dtor(&t->rows[t->size - 1]);
        t->size--;

    }

    if  (t->capacity)
        free(t->rows);

    t->rows = NULL;
    t->capacity = t->size = 0;

}

void table_print(Table *t, char delim) {

//    printf("Table has size: %i, cap %i\n------------------------\n",t->size,t->capacity);
    for (int i = 0; i < t->size; i++) {

//        printf("Row %i has size: %i, cap: %i\n",i,t->rows[i].size,t->rows[i].capacity);


        for (int j = 0; j < t->rows[i].size; j++) {

            for (int k = 0; k < t->rows[i].cells[j].size; ++k) {

                printf("%c", t->rows[i].cells[j].word[k]);

            }

            if (t->rows[i].size - 1 != j)
                printf("%c", delim);

        }

        if (t->size-1 != i)
            printf("\n");


    }

}

void table_fill(FILE *f, Table *t, char *delim) {

    int choose = 0;

    while (choose != 1)
    {

        Row r;
        row_ctor(&r);
        choose = row_fill(f, t, &r, delim);
        table_append(f, t, &r);

    }

}

void table_append(FILE *f, Table *t, Row *r) {

    table_resize(f, t, 1);

    t->rows[t->size] = *r;
    t->size += 1;

}

void table_resize(FILE *f, Table *t, int i) {

    Row *tmp_table;

    tmp_table = realloc(t->rows, (t->capacity + i) * sizeof(Cell));

    if (NULL == tmp_table)
    {
        problem(f,t,1);
        return;
    }

    t->rows = tmp_table;
    t->capacity += i;

}

void table_ctor(Table *t) {

    t->capacity = 0;
    t->size = 0;
    t->rows = 0;
    t->max_cells = 0;

}

int row_fill(FILE *f, Table *t, Row *r, char *delim) {

    int choose = 0;
    int i = 0;

    while (choose != 1 && choose != 2)
    {

        Cell a;
        array_ctor(&a);
        choose = array_fill(f, t, &a, delim);

        row_append(f, t, r, &a, -1);

        i++;

    }

    if  (i > t->max_cells)
    {
        t->max_cells = i;
    }

    return choose;

}

void row_append(FILE *f, Table *t, Row *r, Cell *a, int idx) {

    if  (idx == -1){

        row_resize(f, t, r, 1);

        r->cells[r->size] = *a;
        r->size += 1;

    }

}

void row_resize(FILE *f, Table *t, Row *r, int i) {

    Cell *tmp_row;
    tmp_row = realloc(r->cells, (r->capacity + i) * sizeof(Cell));

    if (NULL == tmp_row)
    {
        problem(f,t,1);
        return;
    }


    r->cells = tmp_row;
    r->capacity += i;

}

void row_ctor(Row *r) {

    r->capacity = 0;
    r->size = 0;
    r->cells = NULL;

}

void row_dtor(Row *r) {

    while (r->size != 0)
    {

        array_dtor(&r->cells[r->size - 1]);
        r->size--;

    }

    if  (r->capacity)
        free(r->cells);

    r->cells = NULL;
    r->capacity = r->size = 0;

}

void array_ctor(Cell *a)
{

    a->capacity = 0;
    a->size = 0;
    a->word = NULL;

}

void array_resize(FILE *f, Table *t, Cell *a, int i)
{

    char *tmp_string;
    tmp_string = realloc(a->word, (a->capacity + i) * sizeof(char));

    if (NULL == tmp_string)
    {
        problem(f,t,1);
        return;
    }

    a->word = tmp_string;
    a->capacity += i;

}

void array_append(FILE *f, Table *t, Cell *a, int c)
{

    array_resize(f, t, a, 1);
    a->word[a->size] = (char) c;
    a->size += 1;

}

int array_fill(FILE *f, Table *t, Cell *a, char *delim) {

    int c = getc(f);

    while (1)
    {
        if (c == EOF)
            return 1;
        if (c == '\n')
            return 2;

        if (c == '\\')
        {

            array_append(f, t, a, c);
            c = getc(f);
            array_append(f, t, a, c);
            c = getc(f);

        }
        for (int i = 0; delim[i] ; i++)
        {

            if (c == delim[i])
                return 0;

        }


        array_append(f, t, a, c);
        c = getc(f);

    }

}

void array_dtor(Cell *a)
{

    if (a->capacity)
        free(a->word);
    a->word = NULL;
    a->capacity = a->size = 0;

}