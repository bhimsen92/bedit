#define MAX_LINES 200
#define MAX_CHARS 1024
#define ENTER 10

/**
  Buffer is doubley linked list.
  So that i can move forward and backward with ease.
*/

struct Chars
{
  char line[MAX_CHARS];
  int  length;
  int  line_span;
};
struct Buffer
{
  struct Chars   text[MAX_LINES];
  struct Buffer *next_buffer;
  struct Buffer *prev_buffer;
};

struct BufferCoordinates
{
  int y;
  int x;
};

struct ScreenCoordinates
{
  int y;
  int x;
};

typedef struct Buffer Buffer;
typedef struct BufferCoordinates BufferCoords;
typedef struct ScreenCoordinates ScreenCoords;
typedef struct Chars Chars;
int begin = 0,forward = -1;
int  quote_first_seen = 0;
int  quote_last_seen = 0;
int  end = 1;
int  comment_first = 0;
int  comment_second = 0;
int  comment_intermediate_state = 0;
int  c_end = 1;
int  line_comment = 0;
const char* keywords_first_band[] = {
                                       "int",
                                       "float",
                                       "double",
                                       "char",
                                       "auto",
                                       "register",
                                       "extern",
                                       "volatile",
                                       "const",
                                       "short",
                                       "long",
                                       "static",
                                       "signed",
                                       "void",
                                       "unsigned",
                                       NULL
                                    };
const char* keywords_second_band[] = {
                                       "if",
                                       "else",
                                       "do",
                                       "while",
                                       "for",
                                       "typedef",
                                       "struct",
                                       "return",
                                       "case",
                                       "break",
                                       "continue",
                                       "default",
                                       "enum",
                                       "goto",
                                       "switch",
                                       "sizeof",
                                       "union",
                                       "#include",
                                       NULL
                                    };                              


Buffer *head = NULL;
BufferCoords buf = {
                     y:0,
                     x:0
                   };
ScreenCoords scr = {
                     y:0,
                     x:0
                   };

Buffer* create_buffer(void);
//Buffer* get_prev_buffer(Buffer* currBuffer);
//Buffer* get_next_buffer(Buffer* currBuffer);
void scr_move_left(Buffer*); //This function modifies global data
void scr_move_right(Buffer*,int);
void scr_move_up(Buffer*);
void scr_move_down(Buffer*,int);

void buf_move_left(Buffer*);
void buf_move_right(Buffer*);
void buf_move_up(Buffer*);
void buf_move_down(Buffer*);

void decrement_buf_line_nindex(int*,Buffer*);
void increment_buf_line_nindex(int*,Buffer*,int);

void decrement_nindex(int*,Buffer*);
void increment_nindex(int*,Buffer*);

void init_buffer(Chars* buf);
void print_buf_to_screen(Chars *buf,int start_ypos,int end_ypos);
void insert_into_buffer(Chars *editor,int y,int x,int in_char);
void remove_char_frm_buffer(Chars*,int,int,int);

void scroll_up(Chars* editor,int start_ypos,int end_ypos);
void scroll_down(Chars* editor,int start_ypos,int end_ypos);
int  chk_for_multi_line_segemet(Buffer*,int);
void create_new_line(int,int,Buffer*,int,int);
void trasnfer_data_frm_prev_line_to_next_line(int prev_line,int prev_x,int next_line,int next_x,Buffer* curr_buf);
void clear_line(int current_line,int current_x,Buffer* curr_buf);
void set_the_cursor_at_home(Buffer*,int);
void set_the_cursor_at_end(Buffer*,int);
void append_curr_line_to_prev_line(int curr_line,int prev_line,Buffer* curr_buf,int line_count,int flag);
void color_print(char*,int,int);
int  tokenizer(char*,char*,int);
int  compare(char*);
