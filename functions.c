void buf_move_left(Buffer* curr_buf){
  buf.x--;
  if( buf.x < 0){
    buf_move_up(curr_buf);
    if( buf.y == 0)
      buf.x = 0;
  }
}

void buf_move_right(Buffer* curr_buf)
{
  buf.x++;
  if( buf.x >= MAX_CHARS - 1){    
    buf.x = 0;
    buf_move_down(curr_buf);
  }
}

void buf_move_up(Buffer* curr_buf)
{
  if( !on_line_flag){
    buf.y--;
    if( buf.y < 0){
      buf.y = 0;
    }
  }
}

void buf_move_down(Buffer* curr_buf)
{
   if( !on_line_flag){
     buf.y++;
     if( buf.y > MAX_LINES){
       endwin();
       fprintf(stderr,"Buffer size exceeded.\n");
       exit(1);
     }
   }
}

void scr_move_right(Buffer* curr_buf,int flag)
{
  if( scr.x >= 0 && scr.x < maxCols){
    scr.x++;
  }
  else{
    scr_move_down(curr_buf,!flag);
    if( on_line_flag && buf_line_nindex == 0 && curr_buf->text[buf.y].line_span > 0 && flag){
      buf.x = 0;
      cols_visible = maxCols - 1;
    }
    else if( on_line_flag && buf_line_nindex < curr_buf->text[buf.y].line_span && flag){
      cols_visible = maxCols - 1;
    }
    else if( on_line_flag && buf_line_nindex == curr_buf->text[buf.y].line_span && flag){
      cols_visible = curr_buf->text[buf.y].length - (curr_buf->text[buf.y].line_span * maxCols);
    }
    else if(flag){
      cols_visible = curr_buf->text[buf.y].length;
      scr.x = 0;
    }
    else
      scr.x = 1;
      cols_visible = 1;
  }
}

void scr_move_left(Buffer *curr_buf)
{
  if( scr.x > 0 && scr.x < maxCols ){
    scr.x--;
  }
  else{
    scr_move_up(curr_buf);
     // It is the first time i am stepping into a multi line segment.
    if( on_line_flag && curr_buf->text[buf.y].line_span == buf_line_nindex){
      scr.x = curr_buf->text[buf.y].length - (maxCols * buf_line_nindex);
      buf.x = curr_buf->text[buf.y].length;
      cols_visible = scr.x;
    }
    // I am on the multi line segment.
    else if( on_line_flag && buf_line_nindex >= 0){
      scr.x = maxCols - 1;
      cols_visible = maxCols - 1;
    }
    //When i am on single lines.
    else{
      scr.x = curr_buf->text[buf.y].length;
      buf.x = curr_buf->text[buf.y].length;
      cols_visible = curr_buf->text[buf.y].length;
    }
  }
}
void scr_move_up(Buffer* curr_buf)
{
  if( scr.y > 0 ){
    scr.y--;
  }
  else{
    if( nindex > 0){
      decrement_nindex(&nindex,curr_buf);
      scr.y = curr_buf->text[nindex].line_span;
    }
    else 
      scr.y = 0;
  }
  if( curr_buf->text[buf.y].line_span > 0 && !on_line_flag){
    buf_line_nindex = curr_buf->text[buf.y].line_span;
    on_line_flag = TRUE;
  }
  else if( on_line_flag ){
    decrement_buf_line_nindex(&buf_line_nindex,curr_buf);
    if(buf_line_nindex == -1 && buf.y != 0){
      on_line_flag = FALSE;
      buf_line_nindex = 0;
      buf_move_up(curr_buf);
    }
  }
}

void scr_move_down(Buffer* curr_buf,int flag)
{
//Here Flag is used to differentiate between scr_move_right()[flag = 1] and direct scr_move_down()[flag = 0] functions.
  if( scr.y < (maxRows - 1)){
    scr.y++;
  }
  else{
    if( curr_buf->text[nindex].line_span > 0){
      scr.y = maxRows - 1 - curr_buf->text[nindex].line_span;
    }
    else
      scr.y = maxRows - 1;
    increment_nindex(&nindex,curr_buf);
/*
    if( line_span_counter > 1)
//      buf.y += line_span_counter - 1;
      nindex += line_span_counter - 1;
      line_span_counter--;
      if( line_span_counter < 1)
        line_span_counter = 1; */
  }
  if( curr_buf->text[buf.y].line_span > 0 && !on_line_flag ){
    if(flag)
      buf_line_nindex = 1;
    else
      buf_line_nindex = 0;
    on_line_flag = TRUE;
  }
  else if(on_line_flag){
    if( flag )
      increment_buf_line_nindex(&buf_line_nindex,curr_buf,flag);
    else
      increment_buf_line_nindex(&buf_line_nindex,curr_buf,!flag);
    if( buf_line_nindex == 0){
      on_line_flag = FALSE;
      buf_move_down(curr_buf);
    }
  }
}


void increment_nindex(int* line_nindex,Buffer* curr_buf)
{
  (*line_nindex) += 1;
}

void decrement_nindex(int* scroll_nindex,Buffer* curr_buf)
{
  if( *scroll_nindex > 0){
      *scroll_nindex -= 1;
      if( *scroll_nindex < 0)
        *scroll_nindex = 0;
      line_span_counter++;
  }
}

void increment_buf_line_nindex(int* line_nindex,Buffer* curr_buf,int flag)
{
 if( flag){
  (*line_nindex)++;
  if( *line_nindex > curr_buf->text[buf.y].line_span){
    *line_nindex = 0;
  }
 }
}

void decrement_buf_line_nindex(int *line_nindex,Buffer* curr_buf)
{
    (*line_nindex)--;
    int temp;
    if( *line_nindex < 0){
      *line_nindex = -1;
      temp = 23;
    }
  #if 0
  clear();
  mvprintw(15,0,"buf_index: %d,temp: %d",*line_nindex,temp);
  refresh();
  getch();
  #endif
}

Buffer* create_buffer()
{
  Buffer *curr_buffer = head;
  Buffer *prev_buffer = NULL;
  if( head == NULL ){
    curr_buffer = (Buffer*)malloc(sizeof(Buffer));
    curr_buffer->next_buffer = NULL;
    curr_buffer->prev_buffer = NULL;
    init_buffer(curr_buffer->text);
    head = curr_buffer;
  }
  else{
    while( curr_buffer->next_buffer != NULL ){
        prev_buffer = curr_buffer;
        curr_buffer = curr_buffer->next_buffer;
    }
    prev_buffer = curr_buffer;
    curr_buffer->next_buffer = (Buffer*)malloc(sizeof(Buffer));
    curr_buffer = curr_buffer->next_buffer;
    curr_buffer->prev_buffer = prev_buffer;
    curr_buffer->next_buffer = NULL;
    init_buffer(curr_buffer->text);
  }
  return curr_buffer;
}

void init_buffer(Chars* editor)
{
  int row,col;
  for( row = 0;row < MAX_LINES; row++){    
    editor[row].line[0] = '\0';
    editor[row].length = 0;
    editor[row].line_span = 0;
    for(col = 1;col < MAX_CHARS; col++){
      editor[row].line[col] = '\0';
    }
  }
}

void scroll_up(Chars* editor,int start_ypos,int end_ypos)
{
  print_buf_to_screen(editor,start_ypos,end_ypos);
}
void scroll_down(Chars* editor,int start_ypos,int end_ypos)
{
  print_buf_to_screen(editor,start_ypos,end_ypos);
}

void insert_into_buffer(Chars *editor,int y,int x,int in_char)
{
  int new_pos,no_chars_to_move,i;
  if( x < editor[y].length ){
    no_chars_to_move = editor[y].length - x;
    new_pos = editor[y].length;
    for( i = 0; i< no_chars_to_move;i++){
        editor[y].line[new_pos] = editor[y].line[new_pos - 1];
        new_pos--;
    }
    editor[y].line[x] = in_char;
  }
  else{
    editor[y].line[x] = in_char;
  }
  editor[y].length++;
}

void remove_char_frm_buffer(Chars* editor,int y,int x,int flag)
{
  int char_to_be_removed;
  int i;
  if( flag ){
    char_to_be_removed = x;
  }
  else{
    char_to_be_removed = x - 1;
  }
  if( char_to_be_removed >= editor[y].length){
    editor[y].line[char_to_be_removed] = '\0';
  }
  else if( char_to_be_removed >= 0 && char_to_be_removed < editor[y].length){
    for( i = char_to_be_removed; i <= editor[y].length;i++){
      editor[y].line[i] = editor[y].line[i+1];
    }
  }
  if( char_to_be_removed >= 0)
    editor[y].length--;
}

int  chk_for_multi_line_segemet(Buffer* curr_buf,int y)
{
  if( curr_buf->text[y].line_span == 0)
    return 0;
  else
    return 1;
}

void create_new_line(int y,int x,Buffer* curr_buf,int line_count,int flag)
{
  int prev_line,next_line;
  int prev_line_length = curr_buf->text[y].length;
  next_line = line_count; // points at the end.
  prev_line = next_line - 1;
  while( prev_line != y){
    trasnfer_data_frm_prev_line_to_next_line(prev_line,0,next_line,0,curr_buf);
                         #ifdef _DEBUG
                           clear();
                           mvprintw(12,0,"OK OK");
                           refresh();
                           getch();
                         #endif    
    clear_line(prev_line,0,curr_buf);
    prev_line--;
    next_line--;
  }
  trasnfer_data_frm_prev_line_to_next_line(prev_line,x,next_line,0,curr_buf);
  clear_line(prev_line,x,curr_buf);
  curr_buf->text[next_line].length = (prev_line_length  - x );
  curr_buf->text[prev_line].length =  x ;  //length - ( length - x)
  if( flag ){ // means i am on a multi line i.e on_multi_line flag is sent
    curr_buf->text[prev_line].line_span = (int)(curr_buf->text[prev_line].length / maxCols );
    curr_buf->text[next_line].line_span = (int)(curr_buf->text[next_line].length / maxCols );
    if( curr_buf->text[next_line].line_span > 0){
      buf_line_nindex = 0;
      on_line_flag = TRUE;
    }
    else{
      buf_line_nindex = 0;
      on_line_flag = FALSE;
    }
  }
}

void trasnfer_data_frm_prev_line_to_next_line(int prev_line,int prev_x,int next_line,int next_x,Buffer* curr_buf)
{
  while( prev_x < curr_buf->text[prev_line].length){
      curr_buf->text[next_line].line[next_x] = curr_buf->text[prev_line].line[prev_x];
      prev_x++;
      next_x++;
  }
  curr_buf->text[next_line].length = curr_buf->text[prev_line].length;
  curr_buf->text[next_line].line_span = curr_buf->text[prev_line].line_span;
}

void clear_line(int current_line,int current_x,Buffer* curr_buf)
{
  int i;
  for( i = current_x; i < curr_buf->text[current_line].length;i++){
      curr_buf->text[current_line].line[i] = '\0';
  }
  curr_buf->text[current_line].length = 0;
  curr_buf->text[current_line].line_span = 0;  
}

void set_the_cursor_at_home(Buffer* curr_buf,int flag)
{
  // I am on a multiline segment.
  if( flag ){
    buf.x = 0;
    scr.x = 0;
    scr.y = scr.y - buf_line_nindex;
    #if 0
      mvprintw(10,0,"Home:%d,%d",scr.y,buf_line_nindex);
      refresh();
      getch();    
    #endif
    buf_line_nindex = 0;
    on_line_flag = TRUE;
  }
  else{
    buf.x = 0;
    scr.x = 0;
  }
}

void set_the_cursor_at_end(Buffer* curr_buf,int flag)
{
  // I am on a multi line segment.
  if( flag ){
    buf.x = curr_buf->text[buf.y].length;
    scr.x = curr_buf->text[buf.y].length - ( (curr_buf->text[buf.y].line_span)*maxCols);
    scr.y = scr.y + (curr_buf->text[buf.y].line_span - buf_line_nindex);
    #if 0
      mvprintw(10,0,"End:%d,%d",scr.y,buf_line_nindex);
      refresh();
      getch();    
    #endif
    buf_line_nindex = curr_buf->text[buf.y].line_span;
    on_line_flag = TRUE;
  }
  else{
    buf.x = curr_buf->text[buf.y].length;
    scr.x = buf.x;
  }
}

void append_curr_line_to_prev_line(int curr_line,int prev_line,Buffer* curr_buf,int line_count,int flag)
{
 int prev_line_length = curr_buf->text[prev_line].length;      // saving length
 int prev_line_line_span = curr_buf->text[prev_line].line_span; // saving line_span: These are used for positioning purpose in future.
 int curr_x = buf.x;
 int prev_x = curr_buf->text[prev_line].length;
 int next_line = curr_line + 1;
 #if 0
   mvprintw(10,50,"curr_line: %d, prev_line: %d,prev_line_length: %d,span: %d",curr_line,prev_line,prev_line_length,prev_line_line_span);
   refresh();
   getch(); 
 #endif
 while( curr_x < curr_buf->text[curr_line].length ){
     insert_into_buffer(curr_buf->text,prev_line,prev_x,curr_buf->text[curr_line].line[curr_x]);
     curr_x++;
     prev_x++;
 }
 clear_line(curr_line,0,curr_buf); // removed the current line.
 while( next_line <= line_count ){
     trasnfer_data_frm_prev_line_to_next_line(next_line,0,curr_line,0,curr_buf);
     clear_line(next_line,0,curr_buf);
     curr_line++;
     next_line++;
 }
 //decrement the line count.
 line_count--;
 if( line_count < 0)
   line_count = 0; 
 rows_visible--;
 if( rows_visible < 0)
   rows_visible = 0;
// Flag is used to switch between backspace key and delete key.
if( flag ){
 //move scr.y and buf.y one step backward. 
 on_line_flag = FALSE; // setting it false so that i can use buf_move_up() and scr_move_up()
 curr_buf->text[prev_line].line_span = 0; // setting it to 0 so that i can use buf_move_up() and scr_move_up()
 buf_move_up(curr_buf);
 scr_move_up(curr_buf);
 //resetting line_span,buf_line_nindex variables.
 curr_buf->text[prev_line].line_span = (int)(curr_buf->text[prev_line].length / maxCols );
 buf_line_nindex = prev_line_line_span;
 //set on_line_flag true if line_span > 0
 if( curr_buf->text[prev_line].line_span > 0){
   on_line_flag = TRUE;
   cols_visible = maxCols + 1;
 }
 //set buf.x and scr.x 
 buf.x = prev_line_length;
 scr.x = prev_line_length - ( prev_line_line_span * maxCols);
 #if 0
   mvprintw(11,50,"scr.x: %d, buf.x: %d,span: %d,length: %d",scr.x,buf.x,curr_buf->text[prev_line].line_span,curr_buf->text[prev_line].length);
   refresh();
   getch();
   clear();
 #endif
 }
}

void color_print(char* srcString,int in,int limit)
{
  char temp[1024];
  int  res;
  int  next_pos = in;
  int  i;
  int  len; 
  while(next_pos < limit){
      next_pos = tokenizer(srcString,temp,limit);
      res = compare(temp);
      if( quote_first_seen )
        res = 0;
      switch(res){
        case 1:
                attron(COLOR_PAIR(1)|A_BOLD);
                for(i = 0;i<strlen(temp);i++)
                  wprintw(stdscr,"%c",temp[i]);                  
                attroff(COLOR_PAIR(1)|A_BOLD);
                break;
       case 2:
                attron(COLOR_PAIR(2)|A_BOLD);  
                for(i = 0;i<strlen(temp);i++)
                  wprintw(stdscr,"%c",temp[i]);
                attroff(COLOR_PAIR(2)|A_BOLD);
                break;
       default:
                if( !comment_first && !line_comment && quote_first_seen && end){
                  attron(COLOR_PAIR(3) | A_BOLD);
                  comment_first = 0;
                  line_comment = 0;
                  end = 0;
                }
                else if( !quote_first_seen && !line_comment && comment_first && c_end ){
                  attron(COLOR_PAIR(4) | A_BOLD );
                  quote_first_seen = 0;
                  line_comment = 0;
                  c_end = 0;
                }
                else if( !quote_first_seen && !comment_first && line_comment){
                  attron(COLOR_PAIR(4) | A_BOLD);
                  quote_first_seen = 0;
                  comment_first = 0;
                }
                for(i = 0;i<strlen(temp);i++){
                  wprintw(stdscr,"%c",temp[i]);
                }
      }
      wprintw(stdscr,"%c",srcString[next_pos]); //for printing space. because it has been consumed while tokenizing the string.
      if(quote_first_seen &&  quote_last_seen && !end){
        quote_last_seen = 0;
        quote_first_seen = 0;
        comment_first = 0;
        comment_second = 0;
        comment_intermediate_state = 0;
        line_comment = 0;          
        end = 1;
        attroff(COLOR_PAIR(3) | A_BOLD );
      }
      if( comment_first && comment_second && !c_end ){
        comment_first = 0;
        comment_second = 0;
        comment_intermediate_state = 0;
        quote_last_seen = 0;
        quote_first_seen = 0;
        line_comment = 0;        
        c_end = 1;
        attroff(COLOR_PAIR(4) | A_BOLD );
      }
  }
  begin = 0;
  forward = -1;  
  //attroff(COLOR_PAIR(1) | A_BOLD);
  //attroff(COLOR_PAIR(2) | A_BOLD);
  //attroff(COLOR_PAIR(3) | A_BOLD);  
}
int tokenizer(char* srcString,char* temp,int limit)
{
  int i = 0;
  int len;
  forward++;
  begin = forward;
  // The expressions inside while loop are all delimeters.
  while( srcString[forward] != ' ' && srcString[forward] != '(' && srcString[forward] != ',' && srcString[forward] != ')' && srcString[forward] != ';' && (srcString[forward] != '*' || comment_first) && srcString[forward] != '<' && forward < limit){
      if(!quote_first_seen && !quote_last_seen && srcString[forward] == '"'){
        quote_first_seen = 1;
        break;
      }
      else if(quote_first_seen && !quote_last_seen && srcString[forward] == '"' ){
        quote_last_seen = 1;
        break;
      }
      if( !comment_first && !comment_second && srcString[forward] == '/' && srcString[forward + 1] == '*' ){
        comment_first = 1;
        break;
      }
      else if(comment_first && !comment_intermediate_state &&  !comment_second && srcString[forward] == '*' ){
        comment_intermediate_state = 1; // This state is introduced so that "/" character will be colored.
        break;
      }
      else if (comment_first && comment_intermediate_state && !comment_second && srcString[forward] == '/' ){
        comment_second = 1;
        break;
      }
      if( !line_comment && srcString[forward] == '/' && srcString[forward + 1] == '/'){
        line_comment = 1;
        break;
      }      
      forward++;
  }
  len = (forward - begin);
  for( i = 0; i< len;i++){
    temp[i] = srcString[begin];
    begin++;
  }
  temp[i] = '\0';
  return forward;
}

int compare(char* temp)
{
  int f_i = 0,s_i = 0;
  while( keywords_first_band[f_i] != NULL || keywords_second_band[s_i] != NULL){ 
      if( keywords_first_band[f_i] != NULL ){
        if( strcmp(keywords_first_band[f_i],temp) == 0)
          return 1;
        f_i++;
      }
      if( keywords_second_band[s_i] != NULL ){
        if( strcmp(keywords_second_band[s_i],temp) == 0)
          return 2;
        s_i++;
      }
  }
  return 0;
}

void print_buf_to_screen(Chars* editor,int start_ypos,int end_ypos)
{
  // j  is used for indexing line.
  // m  is used for indexing characters in the current line.  
  int limit;
  int i,j = start_ypos;
  int l,line = 0,span = 0,next = 0;
  int m = 0;
  int counter = 1;
  int s = 0;

  /* means modify only one line */
  if( end_ypos == 0){ 
    move(start_ypos,0);
    wprintw(stdscr,"%s",editor[start_ypos].line);
  }
  else{
    limit = end_ypos;
    //for( i = 0;i<limit;i++){
    while( line < limit){
      move(line,0);
      if( editor[j].line_span > 0){
        m = editor[j].line_span;
        while( span <= m){
//            for( l = next;l<(maxCols * counter);l++)
//                wprintw(stdscr,"%c",editor[j].line[l]);
//            mvprintw(12,0,"%s",editor[j].line+next);
//            refresh();
//            getch();
//            clear();
            color_print(editor[j].line+next,0,(maxCols));
            next = next + maxCols;
            span++;
            line++;
            counter++;
            move(line,0);
//            forward = next - 1;
        }
        counter = 1;
        next = 0;
        span = 0;
        if( line_comment)
          attroff(COLOR_PAIR(4) | A_BOLD);
        line_comment = 0;        
        quote_last_seen = 0;
        quote_first_seen = 0;
        end = 1;
        attroff(COLOR_PAIR(3) | A_BOLD );        
//        forward = -1;
//        begin = 0;
      }
      else{
/*        for(l = 0;l<maxCols;l++){
            if( editor[j].line[l] == '/' && editor[j].line[l + 1] == '*' && !s){
              attron(A_BOLD);
              s = 1;
            }
            else if( !s )
              attroff(A_BOLD);
            wprintw(stdscr,"%c",editor[j].line[l]);
            if(s && editor[j].line[l] == '*' && editor[j].line[l + 1] == '/'){
              attroff(A_BOLD);
              s = 0;
            }
        }*/
//        wprintw(stdscr,"p: %c",editor[j].line[0]);
//        getch();
        color_print(editor[j].line,0,maxCols);
        if( line_comment )
          attroff(COLOR_PAIR(4) | A_BOLD);
        line_comment = 0;
        quote_last_seen = 0;
        quote_first_seen = 0;
        end = 1;
        attroff(COLOR_PAIR(3) | A_BOLD );        
        line++;
      }
      j++;
    }
  }
        // Reinitializing the quote states.
        quote_last_seen = 0;
        quote_first_seen = 0;
        end = 1;
        attroff(COLOR_PAIR(3) | A_BOLD );
        // Reinitializing the comment states.
        comment_first = 0;
        comment_second = 0;
        comment_intermediate_state = 0;  
        line_comment = 0;
        c_end = 1;
 //       if( !line_comment )
          attroff(COLOR_PAIR(4) | A_BOLD );
}
