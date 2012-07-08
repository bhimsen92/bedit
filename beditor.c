/**
@PENDING:
  1.cols_visible needs proper adjusting as we move on multi line segment. //done
  2.Enter key needs to be enhanced. // done
  3.Backspace key needs to be enhanced. //done
  4.Have to add HOME,END,DEL,CTLR-S commands. //done
  5.Mechanism for Syntax Highlighting. // 90% done
  6.Mechanism for Writing the buffer to a file.
  7.Mechanism for filling the buffer from a file. 
@MODIFICATIONS:
 1.A new variable is added: line_span_counter
   @reason: for synchronizing buf.y and scr.y
   @date: 8:12:2011 
   @time: 3:07:57 AM
 2. buf_move_up() and buf_move_down() functions are deleted from decrement_buf_line_nindex and increment_buf_line_nindex 
    and moved to scr_move_up and scr_move_down funtion.
    @reason: buf.y was not decrementing and incrmenting properly because on_line_flag was set even though we are out of multiline segment.
    @date: 10:12:2011
    @time: 5:31:50 PM
 3. scr_move_left() and scr_move_right() functions are completely changed and previous definitions are put in newDummy.c file.
    @reason: The functions were not working well when left key is pressed at scr.x = 0 and scr_move_right() 
    functions is changed because scr_move_left() got changed.(No logical reason!!!).
 4. line_span_counter can be removed in future.
    @reason: Its not being used right now.Scrolling is done through "nindex" variable and buf.y and scr.y are synchronized
    through "line_span" variable of buffer strutcure.
*/
//#define _DEBUG

#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
int   maxRows,maxCols;
int   cols_visible = 0,rows_visible = 0; // cols_visible and rows_visible are used to avoid unneccessary movement of cursor.
int   on_line_flag = FALSE;
int   buf_line_nindex = 0;
int   nindex = 0;
int line_span_counter = 1;
#define ON_MULTI_LINE 1
#define OFF_MULTI_LINE 0
#define DEL_KEY 1
#define BACKSPACE_KEY 0
#include "header.h"
#include "functions.c"

int main(int argc,char* argv[])
{
  int   in_char;  
  Buffer *curr_buf;
  int   line_count = 0;  
  FILE *fp;
  if( argc < 2 || argc >= 3){
    fprintf(stderr,"Usage: ./beditor <file_name>\n");
    exit(1);
  }
  initscr();
  if( has_colors() == FALSE ){
    endwin();
    fprintf(stderr,"Terminal has no color capability\n");
    exit(1);
  }
  start_color();
  use_default_colors();
  init_pair(1,COLOR_GREEN,-1);
  init_pair(2,COLOR_MAGENTA,-1);
  init_pair(3,COLOR_RED,-1);
  init_pair(4,COLOR_CYAN,-1);
  curr_buf = create_buffer();
  getmaxyx(stdscr,maxRows,maxCols);
//  maxCols = maxCols - 1;
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  init_buffer(curr_buf->text);
//  print_buf_to_screen(curr_buf->text,0,maxRows);
  move(scr.y,scr.x); 
  /**
    buf.x and scr.x are in sync.
    need to mange buf.y and scr.y separately.
  */
  while( (in_char = wgetch(stdscr)) != KEY_F(2)){
    switch(in_char){
      case KEY_LEFT:
                    //cols_visible && scr.x & buf.x is handled inside the function.
                    buf_move_left(curr_buf);
                    scr_move_left(curr_buf);
                   break;
      case KEY_RIGHT:
                    //cols_visible && scr.x & buf.x is handled inside the function.      
                    if( scr.x < cols_visible ){
                      buf_move_right(curr_buf);
                      scr_move_right(curr_buf,1);
                    }
                    break;
      case KEY_UP:if( buf.y >= 0 || nindex > 0){
                    buf_move_up(curr_buf);
                    scr_move_up(curr_buf);                              
                    if( on_line_flag && buf_line_nindex == curr_buf->text[buf.y].line_span)
                      cols_visible = curr_buf->text[buf.y].length - (curr_buf->text[buf.y].line_span * (maxCols));
                    else if( on_line_flag && scr.y == 0)
                      cols_visible = maxCols - 1;
                    else
                      cols_visible = curr_buf->text[buf.y].length;
                    if( scr.x >= cols_visible){
                      scr.x = cols_visible;
                    }
                    if( on_line_flag && (buf_line_nindex == curr_buf->text[buf.y].line_span) )
                      buf.x = ((curr_buf->text[buf.y].line_span) * maxCols) + scr.x;
                    else if( (on_line_flag && (buf_line_nindex > 0) )|| scr.y == 0)
                      buf.x = (buf_line_nindex * maxCols) + scr.x;
                    else
                      buf.x = scr.x;
                  }
                  break;
      case KEY_DOWN:
                    if( buf.y < rows_visible){
                      buf_move_down(curr_buf);
                      scr_move_down(curr_buf,0);
                    if( (on_line_flag && buf_line_nindex == 0) || (on_line_flag && buf_line_nindex > 0 && buf_line_nindex != curr_buf->text[buf.y].line_span))
                      cols_visible = maxCols - 1;
                    else if(on_line_flag && (buf_line_nindex == curr_buf->text[buf.y].line_span))
                      cols_visible = curr_buf->text[buf.y].length - (curr_buf->text[buf.y].line_span * maxCols);
                    else
                      cols_visible = curr_buf->text[buf.y].length;
                      // logic for adjusting buf.x and scr.x
                      if( scr.x >= cols_visible){
                        scr.x = cols_visible;
                      }
                        if( !on_line_flag ){
                          buf.x = scr.x;
                        }
                        else if( on_line_flag && (buf_line_nindex == 0)){ // I steping into a multiline segment for the first time
                          buf.x = scr.x;
                        }
                        else if( on_line_flag && (buf_line_nindex > 0)){  // while moving on multiline segment.
                          buf.x = (buf_line_nindex)*maxCols + scr.x;
                        }
                    }
                    break;
      case ENTER:  if( buf.y < line_count + 1){
                     if( chk_for_multi_line_segemet(curr_buf,buf.y) ){
                         create_new_line(buf.y,buf.x,curr_buf,line_count + 1,ON_MULTI_LINE);
                     }
                     else{
                       // since scr.x and buf.x are in sync when the line is not a multi line segment.
                       if( buf.x >= 0 && buf.x <= curr_buf->text[buf.y].length ){
                         #ifdef _DEBUG
                           clear();
                           mvprintw(12,0,"OK");
                           refresh();
                           getch();
                         #endif
                         create_new_line(buf.y,buf.x,curr_buf,line_count + 1,OFF_MULTI_LINE);
//                         buf_move_down(curr_buf);
//                         scr_move_down(curr_buf,0);
//                         buf.x = 0;
//                         scr.x = 0;
                       }
                     }
                   }
                      buf_line_nindex = 0;
                      on_line_flag = FALSE;
                      buf_move_down(curr_buf);
                      scr_move_down(curr_buf,0);
                      scr.x = 0;
                      buf.x = 0;
                      cols_visible = curr_buf->text[buf.y].length;
                    if( rows_visible < MAX_LINES){
                      rows_visible++;
                    }
                    line_count++;
                    break;
      case KEY_BACKSPACE:
                    if( scr.x <= 0 && buf.x <= 0 && ( buf.y != 0)){
                      append_curr_line_to_prev_line(buf.y,buf.y - 1,curr_buf,line_count,1);
                    }
                    else if( buf.y != 0 || buf.x > 0){
                      remove_char_frm_buffer(curr_buf->text,buf.y,buf.x,BACKSPACE_KEY);
                      //new addition of code starts
                      curr_buf->text[buf.y].line_span = (int)(curr_buf->text[buf.y].length / maxCols);
                      if( curr_buf->text[buf.y].line_span == 0){
                        buf_line_nindex = 0;
                        on_line_flag = FALSE;
                      }
                      //new Addition Ends
                      buf_move_left(curr_buf);
                      scr_move_left(curr_buf);
                      cols_visible--;
                      if( cols_visible < 0)
                        cols_visible = 0;
                      
                   }
                   break;
      case KEY_HOME:
                    if( chk_for_multi_line_segemet(curr_buf,buf.y) ){
                      set_the_cursor_at_home(curr_buf,ON_MULTI_LINE);
                    }
                    else{
                      set_the_cursor_at_home(curr_buf,OFF_MULTI_LINE);
                    }
                    break;
      case KEY_END: 
                    if( chk_for_multi_line_segemet(curr_buf,buf.y) ){
                      set_the_cursor_at_end(curr_buf,ON_MULTI_LINE);
                    }
                    else{
                      set_the_cursor_at_end(curr_buf,OFF_MULTI_LINE);
                    }
                    break;
      case KEY_DC:  
                    if( scr.x <= 0 && buf.x <= 0 && curr_buf->text[buf.y].length == 0 && buf.y <= line_count){
                      append_curr_line_to_prev_line(buf.y,buf.y - 1,curr_buf,line_count,0);
                    }
                    else if((buf.x >= 0 && curr_buf->text[buf.y].length > 0 && buf.y <= line_count)){
                      remove_char_frm_buffer(curr_buf->text,buf.y,buf.x,DEL_KEY);
                      cols_visible--;
                      if( cols_visible < 0)
                        cols_visible = 0;
                      
                   }                          
                    break;
      default:
               if( buf.x < MAX_CHARS - 1){
                 insert_into_buffer(curr_buf->text,buf.y,buf.x,in_char);
                   if(curr_buf->text[buf.y].line_span == 0 && buf.x > maxCols - 1){ //to avoid multiple increments to line_span
                     curr_buf->text[buf.y].line_span++;
                     rows_visible++;
                     line_span_counter++;
                   }
                   else if(curr_buf->text[buf.y].line_span > 0 && buf.x >= ((maxCols)*(curr_buf->text[buf.y].line_span+1))){
                     curr_buf->text[buf.y].line_span++;
                     rows_visible++;
                     line_span_counter++;
                   }
               }
               else{
                 insert_into_buffer(curr_buf->text,buf.y,buf.x,'\n');
                 rows_visible++;
               }
               #if 0
      mvprintw(12,10,"y: %d, x: %d, sy: %d, sx: %d,nindex: %d, buf_line_nindex: %d, line_span: %d, present: %d, flag: %d,length: %d,gl:%d, ml:%d",buf.y,buf.x,scr.y,scr.x,nindex,buf_line_nindex,curr_buf->text[buf.y].line_span,cols_visible,on_line_flag,curr_buf->text[buf.y].length,gl,ml);
                 refresh();
                 getch();
               #endif
               buf_move_right(curr_buf);
               scr_move_right(curr_buf,0);
               cols_visible++;
               if( cols_visible > maxCols - 1 && !on_line_flag)
                 cols_visible = 1;
               else if( on_line_flag){
                 if (buf_line_nindex > 0)
                   cols_visible = curr_buf->text[buf.y].length - (maxCols * buf_line_nindex);
                 else
                   cols_visible = maxCols;
               }
     }
 
    clear();
    print_buf_to_screen(curr_buf->text,nindex,maxRows);
    #if 0
      mvprintw(10,0,"cols_visible: %d,scr.y: %d scr.x:%d buf.x: %d, buf.y:%d, flag:%d, buf_line_index: %d, index: %d, rows_visible: %d, line_count: %d, length: %d",cols_visible,scr.y,scr.x,buf.x,buf.y,on_line_flag,buf_line_nindex,nindex,rows_visible,line_count,curr_buf->text[buf.y].length);
      refresh();
      getch();
    clear();
    print_buf_to_screen(curr_buf->text,nindex,maxRows);
    #endif
    move(scr.y,scr.x);
    refresh();
 }
  refresh();
  getch();
  endwin();
}
