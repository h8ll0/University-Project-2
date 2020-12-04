#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_COMMANDS 1000
#define MAX_IN_COMMAND 1000

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

void commands_use(FILE *f, Commands *commds, Table *t, Coordinates *coords);

void table_normalize(FILE *f, Table *t, int max_cells, int max_rows);

void coordinates_change(FILE *f, Table *t, Coordinates *coords, char *r_start, char *c_start, char *r_finish, char *c_finish);

Row row_create(Table *t, FILE *f);

void problem(FILE *f, Table *t, int i);

bool is_digit(char *string);

void irow(FILE *f, Table *t, Coordinates *coords);

void arow(FILE *f, Table *t, Coordinates *coords);

void row_insert(FILE *f, Table *t, int idx);

void drow(FILE *f, Table *t, Coordinates *coords);

void table_sort(FILE *f, Table *t);

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
    table_print(&t, delim[0]);
    printf("\n\n");


    Coordinates c;
    coordinates_change(f, &t, &c, "1", "1", "1", "1");      //      creating first coordinates waypoint [1,1]

    commands_use(f, &commands, &t, &c);     //using all commands

    printf("\n");
    table_print(&t, delim[0]);


    table_dtor(&t);         //      end of work
    fclose(f);

    return 0;
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




    printf("Row start: %i, finish: %i\n", coords->row_start, coords->row_finish);
    printf("Col start: %i, finish: %i\n", coords->col_start, coords->col_finish);


}

bool is_digit(char *string) {

    bool result = 0;

    for (int i = 0; i < (int) strlen(string); ++i) {

        if  (string[i] > 47 && string[i] < 59)
        {
            result = 1;
        }
        else
        {
            result = 0;
        }

    }

    return result;
}

void table_normalize(FILE *f, Table *t, int max_cells, int max_rows) {

    printf("Max cells: %i\n",max_cells);
    printf("Max rows: %i\n",max_rows);

    for (int i = 0; i < t->size; i++) {

        if  (t->rows[i].size <= max_cells)
        {

            for (int j = t->rows[i].size; j <= max_cells; ++j) {

                Cell a;
                array_ctor(&a);
                row_append(f, t, &t->rows[i], &a, -1);

            }


        }

    }

    for (int i = t->size; i <= max_rows; i++) {

        Row tmp_row = row_create(t,f);
        table_append(f, t, &tmp_row);

    }

}

void commands_use(FILE *f, Commands *commds, Table *t, Coordinates *coords) {

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
            printf("cmnd: max\n");
            continue;
        }
        else if (strcmp(commds->item[i], "[min]") == 0)
        {
            printf("cmnd: min\n");
            continue;
        }
        else if (strcmp(commds->item[i], "[_]") == 0)
        {
            printf("cmnd: \'_\'\n");
            continue;
        }
        else if (strcmp(commds->item[i], "irow") == 0)
        {
//            printf("cmnd: irow\n");
            irow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "arow") == 0)
        {
//            printf("cmnd: arow\n");
            arow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "drow") == 0)
        {
            printf("cmnd: drow\n");
            drow(f,t,coords);
            continue;
        }
        else if (strcmp(commds->item[i], "icol") == 0)
        {
            printf("cmnd: icol\n");
            continue;
        }
        else if (strcmp(commds->item[i], "acol") == 0)
        {
            printf("cmnd: acol\n");
            continue;
        }
        else if (strcmp(commds->item[i], "dcol") == 0)
        {
            printf("cmnd: dcol\n");
            continue;
        }
        else if (strcmp(commds->item[i], "clear") == 0)
        {
            printf("cmnd: clear\n");
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
            printf("cmnd find: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "swap [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd swap: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "sum [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd sum: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "avg [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd avg: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "count [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd count: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "len [%[^,],%[^]]]", command1, command2) == 2)
        {
            printf("cmnd len: \'%s\' \'%s\'\n",command1,command2);
            continue;
        }
        else if (sscanf(commds->item[i], "set %s", command1) == 1)
        {
            printf("cmnd set: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "def _%s", command1) == 1)
        {
            printf("cmnd def: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "use _%s", command1) == 1)
        {
            printf("cmnd use: \'%s\'\n",command1);
            continue;
        }
        else if (sscanf(commds->item[i], "inc _%s", command1) == 1)
        {
            printf("cmnd inc: \'%s\'\n",command1);
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

void drow(FILE *f, Table *t, Coordinates *coords) {

    for (int i = coords->row_start; i <= coords->row_finish; ++i) {

        row_dtor(&t->rows[i]);

    }

    table_sort(f,t);

}

void table_sort(FILE *f, Table *t) {

    printf("Table size: %i\n",t->size);
    for (int i = 0; i < t->size; i++) {

        if (!t->rows[i].cells)

        {
            for (int j = i; j < (*t).size; ++j) {

                printf("Row %i is NULL\n", i);
                (*t).rows[i] = (*t).rows[i + 1];


            }

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

        t->rows[idx] = row_create(t,f);
        t->size++;
    }

}

void irow(FILE *f, Table *t, Coordinates *coords) {

    int idx = coords->row_start;
    row_insert(f, t, idx);

}

Row row_create(Table *t, FILE *f) {

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

                printf("%c",t->rows[i].cells[j].word[k]);

            }

            if (t->rows[i].size-1 != j)
                printf("%c",delim);

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