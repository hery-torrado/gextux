#define _GNU_SOURCE              // Enables GNU-specific extensions, often for POSIX/XSI features like wcwidth.
#define _POSIX_C_SOURCE 200809L  // Defines the version of the POSIX standard to adhere to for compatibility.
#define _XOPEN_SOURCE_EXTENDED 1 // Enables X/Open System Interfaces (XSI) extensions, for types like cchar_t.

#include <wchar.h>    // For wide character functions (wcwidth, mbtowc, setcchar) and types (wchar_t, cchar_t).
#include <ncurses.h>  // For the ncurses terminal handling library; should ideally be after wchar.h if dependent.
#include <stdlib.h>   // For standard library functions like memory allocation (malloc, free), string conversion (atoi).
#include <string.h>   // For string manipulation functions (strcpy, strlen, strcmp).
#include <strings.h>  // For case-insensitive string comparison (strcasecmp).
#include <sqlite3.h>  // For SQLite database interface functions and types.
#include <ctype.h>    // For character classification functions (isprint, toupper, isspace).
#include <time.h>     // For time and date functions, used for displaying current time.
#include <stdarg.h>   // For variable argument list handling (va_list, va_start, va_end).
#include <stdio.h>    // For standard input/output functions (popen, pclose, fprintf, snprintf).
#include <sys/wait.h> // For process waiting functions (WIFEXITED, WEXITSTATUS), used with pclose.
#include <signal.h>   // For signal handling (SIGWINCH for resize, SIGINT/SIGTERM for exit).
#include <unistd.h>   // For POSIX operating system API (execlp for executing programs, getopt for command-line options).
#include <stdbool.h>  // For the boolean type (bool) and its values (true, false).
#include <locale.h>   // Required for setlocale, to enable non-ASCII (UTF-8) character support.

// --- Retro-Futuristic Look Character Definitions ---
// These definitions require a UTF-8 capable terminal and the ncursesw library (wide character support).

// Wide character constants for border elements and single characters using UTF-8 literals.
#define WC_RF_VLINE        L'║' // Defines a wide character for a double vertical line border segment.
#define WC_RF_HLINE        L'═' // Defines a wide character for a double horizontal line border segment.
#define WC_RF_ULCORNER     L'╔' // Defines a wide character for a double upper-left corner border segment.
#define WC_RF_URCORNER     L'╗' // Defines a wide character for a double upper-right corner border segment.
#define WC_RF_LLCORNER     L'╚' // Defines a wide character for a double lower-left corner border segment.
#define WC_RF_LRCORNER     L'╝' // Defines a wide character for a double lower-right corner border segment.

#define WC_RF_TITLE_SEP_CHAR L'━' // Defines a wide character for a heavy horizontal line, used as a title separator.
#define WC_RF_PANE_VSEP      L'┃' // Defines a wide character for a heavy vertical line, used for separating panes.

// String constants (UTF-8 encoded) - these are standard C strings, but represent UTF-8 characters.
#define RF_MENU_SELECTOR_STR "➔ "    // Defines the string for the menu item selector (arrow and space).
#define RF_MENU_SELECTOR_VISUAL_LEN 2 // Defines the visual character length of the menu selector string.

#define RF_INPUT_PROMPT_STR  "» "    // Defines the string for the input field prompt.
#define RF_INPUT_PROMPT_VISUAL_LEN 2 // Defines the visual character length of the input prompt string.

#define RF_STATUS_TITLE_LEFT_STR   "▐" // Defines the left decorative character for the status bar title.
#define RF_STATUS_TITLE_LEFT_VISUAL_LEN 1 // Defines the visual length of the left status title decoration.
#define RF_STATUS_TITLE_RIGHT_STR  "▌" // Defines the right decorative character for the status bar title.
#define RF_STATUS_TITLE_RIGHT_VISUAL_LEN 1 // Defines the visual length of the right status title decoration.

#define RF_STATUS_TIME_LEFT_STR    "«" // Defines the left decorative character for the status bar time display.
#define RF_STATUS_TIME_LEFT_VISUAL_LEN 1 // Defines the visual length of the left status time decoration.
#define RF_STATUS_TIME_RIGHT_STR   "»" // Defines the right decorative character for the status bar time display.
#define RF_STATUS_TIME_RIGHT_VISUAL_LEN 1 // Defines the visual length of the right status time decoration.

#define RF_LOADING_TEXT_STR        "[⢿ LOADING ⢿]" // Defines the text string displayed during loading operations.
#define RF_LOADING_CLEAR_TEXT_STR  "             " // Defines a string of spaces to clear the loading text.
#define RF_LOADING_TEXT_VISUAL_LEN 13             // Defines the visual character length of the loading text string.
// --- End Retro-Futuristic Look Definitions ---

// --- Configuration & Constants ---
#define DEFAULT_DB_NAME "gextux.db"         // Defines the default filename for the SQLite database.
#define MAX_STR_LEN 256                     // Defines a general maximum length for string buffers.
#define MAX_NOTES_LEN 101                   // Defines the maximum length for client notes (100 characters + null terminator).
#define INPUT_WIN_HEIGHT 3                  // Defines the height (in lines) of the dedicated input window.
#define PRINT_CMD "lp"                      // Defines the system command for printing (not used in this editor part).
#define CANCEL_INPUT_STRING "!!CANCEL!!"    // Defines a special string input by the user to cancel an operation.
#define STATUS_BAR_TITLE " GexTuX Customer Management v1.0 - May 2025 " // Defines the title text for the status bar.

// UI Constants
#define MAIN_WIN_BORDER_WIDTH 2     // Defines the visual width of the border around the main window.
#define INPUT_PROMPT_X 1            // Defines the starting X (column) position for prompts in the input window.
#define INPUT_PROMPT_Y 1            // Defines the starting Y (row) position for prompts in the input window.
#define MENU_INDENT 4               // Defines the indentation (in spaces) for menu items.
#define LIST_COL_PADDING 1          // Defines the padding (in spaces) between columns in list displays.
#define DETAIL_LABEL_WIDTH 18       // Defines the fixed width for labels in the client detail view.
#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S" // Defines the format string for displaying date and time.
#define DATETIME_STR_LEN 19         // Defines the length of the string generated by DATETIME_FORMAT (excluding null).

// New Screen Layout Constants
#define SCREEN_TITLE_Y (MAIN_WIN_BORDER_WIDTH - 1) // Defines the Y-coordinate (row) for screen titles within the main window.
#define SCREEN_SEPARATOR_Y (SCREEN_TITLE_Y + 1)   // Defines the Y-coordinate for the separator line below screen titles.
#define SCREEN_CONTENT_Y_STD (SCREEN_SEPARATOR_Y + 1) // Defines the starting Y-coordinate for standard content area below the separator.
#define SCREEN_CONTENT_Y_MENU (SCREEN_SEPARATOR_Y + 2) // Defines the starting Y-coordinate for menu content, allowing for extra spacing.
#define MIN_SEPARATOR_WIDTH 30                   // Defines the minimum width for the title separator line.

// Interactive List Pane Constants
#define LIST_PANE_PERCENT 0.50      // Defines the percentage of available width the list pane should occupy.
#define DETAIL_PANE_MIN_WIDTH 45    // Defines the minimum width (in characters) for the detail view pane.
#define PANE_SEPARATOR_WIDTH 1      // Defines the width (in characters) of the visual separator between panes.

// Key Constants - mapping ncurses key codes or characters to symbolic names for actions.
#define KEY_NAV_UP       KEY_UP        // Defines navigation key: Up arrow.
#define KEY_NAV_DOWN     KEY_DOWN      // Defines navigation key: Down arrow.
#define KEY_NAV_LEFT     KEY_LEFT      // Defines navigation key: Left arrow.
#define KEY_NAV_RIGHT    KEY_RIGHT     // Defines navigation key: Right arrow.
#define KEY_NAV_PPAGE    KEY_PPAGE     // Defines navigation key: Page Up.
#define KEY_NAV_NPAGE    KEY_NPAGE     // Defines navigation key: Page Down.
#define KEY_NAV_HOME     KEY_HOME      // Defines navigation key: Home.
#define KEY_NAV_END      KEY_END       // Defines navigation key: End.
#define KEY_ACTION_SELECT '\n'         // Defines action key: Select (typically Enter/Return key, represented as newline).
#define KEY_ACTION_ENTER KEY_ENTER     // Defines action key: Enter (ncurses specific constant for Enter/Return).
#define KEY_ACTION_BACK  'b'           // Defines action key: Back (lowercase 'b').
#define KEY_ACTION_BACK_ALT 'B'        // Defines action key: Back (uppercase 'B').
#define KEY_ACTION_QUIT  'q'           // Defines action key: Quit (lowercase 'q').
#define KEY_ACTION_QUIT_ALT 'Q'        // Defines action key: Quit (uppercase 'Q').
#define KEY_EDIT_CLIENT  'e'           // Defines action key: Edit client (lowercase 'e').
#define KEY_EDIT_CLIENT_ALT 'E'        // Defines action key: Edit client (uppercase 'E').
#define KEY_ACTION_DELETE  'd'         // Defines action key: Delete (lowercase 'd').
#define KEY_ACTION_DELETE_ALT 'D'      // Defines action key: Delete (uppercase 'D').
#define KEY_ESC          27            // Defines the ASCII value for the Escape key.

// --- Structures ---
typedef struct { // Defines the structure for storing comprehensive client data.
    int id;                             // Unique identifier for the client (typically auto-incremented).
    char business_name[MAX_STR_LEN];    // Name of the client's business.
    char email[MAX_STR_LEN];            // Primary email address for the client's business.
    char phone[MAX_STR_LEN];            // Primary phone number for the client's business.
    char website[MAX_STR_LEN];          // Website URL for the client's business.
    char street[MAX_STR_LEN];           // Street address of the client.
    char city[MAX_STR_LEN];             // City of the client's address.
    char state[MAX_STR_LEN];            // State or province of the client's address.
    char zip_code[32];                  // Postal or ZIP code for the client's address.
    char country[MAX_STR_LEN];          // Country of the client's address.
    char tax_number[MAX_STR_LEN];       // Tax identification number for the client.
    int num_employees;                  // Number of employees in the client's business.
    char industry[MAX_STR_LEN];         // Industry sector the client operates in.
    char contact_person[MAX_STR_LEN];   // Name of the primary contact person at the client's business.
    char contact_email[MAX_STR_LEN];    // Email address of the primary contact person.
    char contact_phone[MAX_STR_LEN];    // Phone number of the primary contact person.
    char status[MAX_STR_LEN];           // Current status of the client (e.g., "Active", "Prospect").
    char notes[MAX_NOTES_LEN];          // Additional notes or comments about the client.
    char created_at[MAX_STR_LEN];       // Timestamp of when the client record was created.
} Client;

typedef struct { // Defines a structure for storing summarized client data, used in list views.
    int id;                             // Unique identifier for the client.
    char business_name[MAX_STR_LEN];    // Name of the client's business.
    char city[MAX_STR_LEN];             // City of the client's address.
    char phone[MAX_STR_LEN];            // Primary phone number for the client's business.
    char email[MAX_STR_LEN];            // Primary email address for the client's business.
    char contact_person[MAX_STR_LEN];   // Name of the primary contact person.
} ClientListItem;

typedef struct { // Defines a structure to pass data to the SQLite callback for fetching list items.
    ClientListItem **items_ptr;         // Pointer to a dynamic array of ClientListItem structures.
    int *count_ptr;                     // Pointer to the current number of items in the array.
    int *capacity_ptr;                  // Pointer to the current allocated capacity of the array.
} FetchListData;

typedef struct { // Defines a structure to hold calculated column widths for list displays.
    int id_width;                       // Calculated width for the ID column in a list.
    int name_width;                     // Calculated width for the Name (Business Name) column in a list.
    int name_col_start;                 // Calculated starting X-coordinate (column) for the Name column.
} ListColumnWidths;

typedef enum { // Defines an enumeration for possible actions originating from an interactive list selection.
    INTERACTIVE_LIST_ACTION_EDIT,       // Indicates that the selected item should be edited.
    INTERACTIVE_LIST_ACTION_DELETE      // Indicates that the selected item should be deleted.
} InteractiveListAction;


// --- Global Variables ---
sqlite3 *db = NULL;                     // Global pointer to the SQLite database connection object. Initialized to NULL.
WINDOW *main_win = NULL, *input_win = NULL, *status_win = NULL; // Global pointers for ncurses windows. Initialized to NULL.
int max_y, max_x;                       // Global variables to store the terminal's maximum rows (max_y) and columns (max_x).
volatile sig_atomic_t resize_pending = 0; // A volatile flag indicating if a SIGWINCH (resize) signal is pending.
volatile sig_atomic_t exit_requested = 0; // A volatile flag indicating if a SIGINT or SIGTERM signal has been received.
char db_path[MAX_STR_LEN];              // Global buffer to store the path to the SQLite database file.

// --- Function Prototypes ---
// Ncurses & Windowing related function declarations.
void init_ncurses();                    // Initializes the ncurses environment.
void cleanup_ncurses();                 // Cleans up ncurses resources before program termination.
void create_windows();                  // Creates the main application windows (main, input, status).
void destroy_windows();                 // Destroys/deletes the ncurses windows.
void handle_resize(int sig);            // Signal handler for SIGWINCH (terminal resize).
void check_and_handle_resize();         // Checks the resize_pending flag and processes resize if needed.
void handle_exit_signal(int sig);       // Signal handler for SIGINT/SIGTERM (exit signals).
void draw_custom_box(WINDOW *win);      // Draws a custom border around a specified ncurses window.

// Status Bar related function declarations.
void clear_status();                    // Clears the status bar and redraws default elements (title, time).
void show_status(const char *fmt, ...); // Displays a formatted message on the status bar.
void show_error(const char *fmt, ...);  // Displays a formatted error message on the status bar.
void update_status_bar_datetime();      // Updates the date and time display on the status bar.
void show_loading_indicator(bool show); // Shows or hides a loading indicator on the status bar.

// Database related function declarations.
int init_db(const char* db_filename);   // Initializes the database connection and schema.
void close_db();                        // Closes the database connection.
int db_execute(const char *sql, int (*callback)(void*,int,char**,char**), void *data); // Executes an SQL query.
static int check_column_exists(const char *table_name, const char *column_name); // Checks if a column exists in a table (static linkage).
int fetch_client_by_id(int id, Client *client); // Fetches a single client's full details by ID.
int db_insert_client(const Client *client_data); // Inserts a new client record into the database.
int db_update_client(const Client *client_data); // Updates an existing client record in the database.
int db_delete_client(int client_id);    // Deletes a client record from the database by ID.

// Input Helper function declarations.
int get_string_input(WINDOW *win, int y, int x, const char *prompt, char *buffer, int max_len, bool allow_empty, const char *current_value_display); // Gets string input from the user.
int get_int_input(WINDOW *win, int y, int x, const char *prompt, int *value, int current_value); // Gets integer input from the user.
int select_client_status(char *selected_status, const char *current_status); // Allows user to select a client status from a list.

// Core Screens & UI Logic function declarations.
void display_editor_main_menu();        // Displays the main menu of the customer editor.
void add_new_customer_screen();         // Displays the screen/form for adding a new customer.
void customer_search_workflow(const char *screen_title, const char *search_prompt_detail, InteractiveListAction action); // Manages the customer search and subsequent action.
void edit_customer_form_screen(int client_id); // Displays the screen/form for editing an existing customer.

// New Interactive List with Detail Pane function declarations.
void display_interactive_client_list(const char *title, const char *sql_query, InteractiveListAction action_type); // Displays a list of clients with a detail pane.
void calculate_list_column_widths_for_pane(ListColumnWidths *widths, int pane_content_width); // Calculates column widths for the list pane.
void draw_list_header_in_pane(WINDOW *win, const ListColumnWidths *col_widths, int pane_start_y, int pane_start_x, int pane_content_width); // Draws the header for the list pane.
void draw_list_item_in_pane(WINDOW *win, int y_on_screen, const ClientListItem *item, const ListColumnWidths *col_widths, bool highlighted, int pane_start_x, int pane_content_width); // Draws a single item in the list pane.
void draw_client_details_in_pane(WINDOW *win, const Client *client, int pane_start_y, int pane_start_x, int pane_content_width); // Draws client details in the detail pane.
void wclr_pane_line(WINDOW *win, int y, int x, int width); // Clears a line segment within a pane.


// Other utility function declarations.
void execute_gextux_crm();              // Executes the main GexTuX CRM program.

// Callback function declarations (used with SQLite).
int fetch_list_items_callback(void *data, int argc, char **argv, char **azColName); // SQLite callback for fetching client list items.

// --- Color Pair Definitions --- (Symbolic names for ncurses color pairs)
#define COLOR_PAIR_DEFAULT 1        // Default color pair for general text.
#define COLOR_PAIR_ERROR 2          // Color pair for error messages.
#define COLOR_PAIR_HIGHLIGHT 3      // Color pair for highlighted items (e.g., selected menu option).
#define COLOR_PAIR_STATUS_BG 4      // Color pair for the status bar background.
#define COLOR_PAIR_STATUS_TEXT 5    // Color pair for text on the status bar.
#define COLOR_PAIR_LOADING 6        // Color pair for the loading indicator text.
#define COLOR_PAIR_INPUT_CURRENT 7  // Color pair for displaying current values in input prompts.
#define COLOR_PAIR_PANE_SEPARATOR 8 // Color pair for the visual separator between panes.
#define COLOR_PAIR_LIST_HEADER 9    // Color pair for the header row in list views.


// --- Signal Handlers ---
void handle_resize(int sig) {
    (void)sig;
    resize_pending = 1;
}

void handle_exit_signal(int sig) {
    (void)sig;
    exit_requested = 1;
}

// --- Ncurses Initialization and Cleanup ---
void draw_custom_box(WINDOW *win) {
    cchar_t ls, rs, ts, bs, tl, tr, bl, br;

    setcchar(&ls, (const wchar_t[]){WC_RF_VLINE, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&rs, (const wchar_t[]){WC_RF_VLINE, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&ts, (const wchar_t[]){WC_RF_HLINE, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&bs, (const wchar_t[]){WC_RF_HLINE, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&tl, (const wchar_t[]){WC_RF_ULCORNER, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&tr, (const wchar_t[]){WC_RF_URCORNER, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&bl, (const wchar_t[]){WC_RF_LLCORNER, L'\0'}, A_NORMAL, 0, NULL);
    setcchar(&br, (const wchar_t[]){WC_RF_LRCORNER, L'\0'}, A_NORMAL, 0, NULL);

    wborder_set(win, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
}

void check_and_handle_resize() {
    if (resize_pending) {
        resize_pending = 0;
        endwin();
        refresh();
        clear();
        getmaxyx(stdscr, max_y, max_x);
        destroy_windows();
        create_windows();
        clear();
        refresh();
    }
}

void init_ncurses() {
    if (setlocale(LC_ALL, "") == NULL) {
        // Log error
    }
    mbtowc(NULL, NULL, 0);


    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(COLOR_PAIR_DEFAULT, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_PAIR_ERROR, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_PAIR_STATUS_BG, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_PAIR_STATUS_TEXT, COLOR_WHITE, COLOR_YELLOW);
        init_pair(COLOR_PAIR_LOADING, COLOR_CYAN, COLOR_YELLOW);
        init_pair(COLOR_PAIR_INPUT_CURRENT, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_PAIR_PANE_SEPARATOR, COLOR_BLUE, COLOR_BLACK);

        if (COLORS >= 256) {
            init_pair(COLOR_PAIR_LIST_HEADER, COLOR_BLACK, 214);
        } else {
            init_pair(COLOR_PAIR_LIST_HEADER, COLOR_BLACK, COLOR_CYAN);
        }
    }

    getmaxyx(stdscr, max_y, max_x);
    create_windows();
    clear();
    refresh();

    signal(SIGWINCH, handle_resize);
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);
}

void cleanup_ncurses() {
    destroy_windows();
    curs_set(1);
    endwin();
}

void create_windows() {
    main_win = newwin(max_y - INPUT_WIN_HEIGHT - 1, max_x, 0, 0);
    if (has_colors()) wbkgd(main_win, COLOR_PAIR(COLOR_PAIR_DEFAULT));
    else wbkgd(main_win, A_NORMAL);
    draw_custom_box(main_win);
    scrollok(main_win, TRUE);
    keypad(main_win, TRUE);

    input_win = newwin(INPUT_WIN_HEIGHT, max_x, max_y - INPUT_WIN_HEIGHT - 1, 0);
    if (has_colors()) wbkgd(input_win, COLOR_PAIR(COLOR_PAIR_DEFAULT));
    else wbkgd(input_win, A_NORMAL);
    draw_custom_box(input_win);
    keypad(input_win, TRUE);

    status_win = newwin(1, max_x, max_y - 1, 0);
    if (has_colors()) {
        wbkgd(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_BG));
        wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
    } else {
        wbkgd(status_win, A_REVERSE);
        wattron(status_win, A_BOLD);
    }
    clear_status();

    wrefresh(main_win);
    wrefresh(input_win);
    wrefresh(status_win);
}

void destroy_windows() {
    if(main_win) { delwin(main_win); main_win = NULL; }
    if(input_win) { delwin(input_win); input_win = NULL; }
    if(status_win) { delwin(status_win); status_win = NULL; }
}

// --- Status Bar ---
void update_status_bar_datetime() {
    if (!status_win) return;

    int wrapped_time_visual_len = RF_STATUS_TIME_LEFT_VISUAL_LEN + DATETIME_STR_LEN + RF_STATUS_TIME_RIGHT_VISUAL_LEN;
    if (max_x < wrapped_time_visual_len + 2) return;

    char time_buf[DATETIME_STR_LEN + 1];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL) return;
    strftime(time_buf, sizeof(time_buf), DATETIME_FORMAT, t);

    int time_x = max_x - wrapped_time_visual_len -1;
    if (time_x < 1) time_x = 1;

    attr_t current_attrs; short current_pair; int current_y, current_x_ignored;
    wattr_get(status_win, &current_attrs, &current_pair, NULL);
    getyx(status_win, current_y, current_x_ignored);

    if (has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
    else wattron(status_win, A_BOLD);

    mvwprintw(status_win, 0, time_x, "%s%s%s", RF_STATUS_TIME_LEFT_STR, time_buf, RF_STATUS_TIME_RIGHT_STR);

    wattr_set(status_win, current_attrs, current_pair, NULL);
    wmove(status_win, current_y, current_x_ignored);
}

void clear_status() {
    if (!status_win) return;
    werase(status_win);
    if (has_colors()) {
        wbkgd(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_BG));
        wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
    } else {
        wbkgd(status_win, A_REVERSE);
        wattron(status_win, A_BOLD);
    }

    int title_text_visual_len = strlen(STATUS_BAR_TITLE);
    int full_banner_visual_len = RF_STATUS_TITLE_LEFT_VISUAL_LEN + title_text_visual_len + RF_STATUS_TITLE_RIGHT_VISUAL_LEN;

    mvwprintw(status_win, 0, 1, "%s%s%s", RF_STATUS_TITLE_LEFT_STR, STATUS_BAR_TITLE, RF_STATUS_TITLE_RIGHT_STR);

    wmove(status_win, 0, 1 + full_banner_visual_len + 2);
    update_status_bar_datetime();
    wrefresh(status_win);
}

void show_status(const char *fmt, ...) {
    if (!status_win) return;
    va_list args; va_start(args, fmt);

    int title_text_visual_len = strlen(STATUS_BAR_TITLE);
    int full_banner_visual_len = RF_STATUS_TITLE_LEFT_VISUAL_LEN + title_text_visual_len + RF_STATUS_TITLE_RIGHT_VISUAL_LEN;
    int start_x = 1 + full_banner_visual_len + 2;

    wmove(status_win, 0, start_x);
    wclrtoeol(status_win);

    if (has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
    else wattron(status_win, A_BOLD);

    int wrapped_time_visual_len = RF_STATUS_TIME_LEFT_VISUAL_LEN + DATETIME_STR_LEN + RF_STATUS_TIME_RIGHT_VISUAL_LEN;
    if (start_x < max_x - wrapped_time_visual_len - 2) {
        vw_printw(status_win, fmt, args);
    }

    update_status_bar_datetime();
    wrefresh(status_win);
    va_end(args);
}

void show_error(const char *fmt, ...) {
    if (!status_win) return;
    va_list args; va_start(args, fmt);

    int title_text_visual_len = strlen(STATUS_BAR_TITLE);
    int full_banner_visual_len = RF_STATUS_TITLE_LEFT_VISUAL_LEN + title_text_visual_len + RF_STATUS_TITLE_RIGHT_VISUAL_LEN;
    int start_x = 1 + full_banner_visual_len + 2;

    wmove(status_win, 0, start_x);
    wclrtoeol(status_win);

    if(has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_ERROR) | A_BOLD | A_BLINK);
    else wattron(status_win, A_REVERSE | A_BOLD | A_BLINK);

    mvwprintw(status_win, 0, start_x, "ERROR: ");
    int cy, cx; getyx(status_win, cy, cx);

    int wrapped_time_visual_len = RF_STATUS_TIME_LEFT_VISUAL_LEN + DATETIME_STR_LEN + RF_STATUS_TIME_RIGHT_VISUAL_LEN;
    if (cx < max_x - wrapped_time_visual_len - 2) {
        vw_printw(status_win, fmt, args);
    }

    if(has_colors()) {
        wattroff(status_win, COLOR_PAIR(COLOR_PAIR_ERROR) | A_BOLD | A_BLINK);
        wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
    } else {
        wattroff(status_win, A_REVERSE | A_BOLD | A_BLINK);
        wattron(status_win, A_BOLD);
    }

    update_status_bar_datetime();
    wrefresh(status_win);
    beep();

    if (input_win) {
        mvwprintw(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Error. Press any key...");
        draw_custom_box(input_win); wrefresh(input_win); wgetch(input_win);
        werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
    } else {
        napms(2000);
    }
    clear_status();
    va_end(args);
}

void show_loading_indicator(bool show_ind) {
     if (!status_win) return;

     int title_text_visual_len = strlen(STATUS_BAR_TITLE);
     int full_banner_visual_len = RF_STATUS_TITLE_LEFT_VISUAL_LEN + title_text_visual_len + RF_STATUS_TITLE_RIGHT_VISUAL_LEN;
     int wrapped_time_visual_len = RF_STATUS_TIME_LEFT_VISUAL_LEN + DATETIME_STR_LEN + RF_STATUS_TIME_RIGHT_VISUAL_LEN;

     int indicator_x = max_x - wrapped_time_visual_len - RF_LOADING_TEXT_VISUAL_LEN - 2;

     if (indicator_x <= (1 + full_banner_visual_len + 2)) indicator_x = 1 + full_banner_visual_len + 2;
     if (indicator_x >= max_x -1 || indicator_x + RF_LOADING_TEXT_VISUAL_LEN >= max_x - wrapped_time_visual_len -1) {
         return;
     }

     wmove(status_win, 0, indicator_x);
     if (show_ind) {
         if (has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_LOADING) | A_BOLD);
         else wattron(status_win, A_REVERSE | A_BOLD);
         wprintw(status_win, "%s", RF_LOADING_TEXT_STR);
         if (has_colors()) wattroff(status_win, COLOR_PAIR(COLOR_PAIR_LOADING) | A_BOLD);
         else wattroff(status_win, A_REVERSE | A_BOLD);
         if (has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
         else wattron(status_win, A_BOLD);
     } else {
         if (has_colors()) wattron(status_win, COLOR_PAIR(COLOR_PAIR_STATUS_TEXT) | A_BOLD);
         else wattron(status_win, A_BOLD);
         wprintw(status_win, "%s", RF_LOADING_CLEAR_TEXT_STR);
     }
     update_status_bar_datetime();
     wrefresh(status_win);
}

// --- Database Interaction ---
static int check_column_exists(const char *table_name, const char *column_name) {
    if (!db) return -1;
    char *sql = sqlite3_mprintf("PRAGMA table_info(%q);", table_name);
    if (!sql) {
        if(status_win) show_error("Memory allocation failed checking column existence.");
        return -1;
    }

    sqlite3_stmt *stmt;
    int column_found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        if (sqlite3_errcode(db) != SQLITE_ERROR || strstr(sqlite3_errmsg(db), "no such table") == NULL) {
             if(status_win) show_error("DB error checking column: %s", sqlite3_errmsg(db));
        }
        sqlite3_free(sql);
        return 0;
    }
    sqlite3_free(sql);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        if (name && strcmp((const char*)name, column_name) == 0) {
            column_found = 1;
            break;
        }
    }
    sqlite3_finalize(stmt);
    return column_found;
}

int init_db(const char* db_filename) {
    int rc = sqlite3_open(db_filename, &db);
    if (rc) {
        char err_buf[MAX_STR_LEN];
        snprintf(err_buf, sizeof(err_buf), "Can't open database '%s': %s", db_filename, sqlite3_errmsg(db));
        if (status_win) show_error("%s", err_buf); else fprintf(stderr, "%s\n", err_buf);
        sqlite3_close(db);
        db = NULL;
        return 0;
    }

    const char *sql_create_table =
        "CREATE TABLE IF NOT EXISTS \"clients\" ("
        "\"id\"	INTEGER,"
        "\"business_name\"	TEXT NOT NULL UNIQUE COLLATE NOCASE,"
        "\"email\"	TEXT,"
        "\"phone\"	TEXT,"
        "\"website\"	TEXT,"
        "\"street\"	TEXT,"
        "\"city\"	TEXT,"
        "\"state\"	TEXT,"
        "\"zip_code\"	TEXT,"
        "\"country\"	TEXT,"
        "\"tax_number\"	TEXT,"
        "\"num_employees\"	INTEGER DEFAULT 0,"
        "\"industry\"	TEXT,"
        "\"contact_person\"	TEXT,"
        "\"contact_email\"	TEXT,"
        "\"contact_phone\"	TEXT,"
        "\"status\"	TEXT DEFAULT 'Active' CHECK(\"status\" IN ('Active', 'Inactive', 'Prospect', 'Lead', 'Former')),"
        "\"notes\"	TEXT,"
        "\"created_at\"	DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY(\"id\" AUTOINCREMENT)"
        ");";

    if (!db_execute(sql_create_table, NULL, NULL)) {
        if (db) { sqlite3_close(db); db = NULL; }
        return 0;
    }

    if (!check_column_exists("clients", "tax_number")) {
        if (!db_execute("ALTER TABLE clients ADD COLUMN tax_number TEXT;", NULL, NULL)) {
            // Non-fatal
        }
    }
    if (!check_column_exists("clients", "zip_code")) {
        if (!db_execute("ALTER TABLE clients ADD COLUMN zip_code TEXT;", NULL, NULL)) {
            // Non-fatal
        }
    }
    return 1;
}

void close_db() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_execute(const char *sql, int (*callback)(void*,int,char**,char**), void *data) {
    if (!db) {
        if(status_win) show_error("Database connection is not valid.");
        return 0;
    }
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, callback, data, &err_msg);
    if (rc != SQLITE_OK && rc != SQLITE_ABORT) {
        if(status_win) show_error("SQL error: %s (Query: %.50s...)", err_msg, sql);
        sqlite3_free(err_msg);
        return 0;
    }
    sqlite3_free(err_msg);
    return 1;
}

int fetch_client_by_id(int id, Client *client) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, business_name, email, phone, website, street, city, state, zip_code, country, "
                      "tax_number, num_employees, industry, contact_person, contact_email, contact_phone, "
                      "status, notes, strftime('%Y-%m-%d %H:%M:%S', created_at) "
                      "FROM clients WHERE id = ?;";
    int found = 0;

    if (!db) {
        if(status_win) show_error("DB connection invalid in fetch_client_by_id.");
        return 0;
    }

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        if(status_win) show_error("Failed to prepare select: %s", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        found = 1;
        memset(client, 0, sizeof(Client));
        client->id = sqlite3_column_int(stmt, 0);

        const unsigned char *text_val;
        #define SAFE_STRNCPY_FETCH(dest, src_idx, maxlen) \
            text_val = sqlite3_column_text(stmt, src_idx); \
            if (text_val) strncpy(dest, (const char*)text_val, maxlen - 1); else dest[0] = '\0'; \
            dest[maxlen - 1] = '\0'

        SAFE_STRNCPY_FETCH(client->business_name, 1, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->email, 2, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->phone, 3, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->website, 4, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->street, 5, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->city, 6, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->state, 7, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->zip_code, 8, sizeof(client->zip_code));
        SAFE_STRNCPY_FETCH(client->country, 9, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->tax_number, 10, MAX_STR_LEN);
        client->num_employees = sqlite3_column_int(stmt, 11);
        SAFE_STRNCPY_FETCH(client->industry, 12, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->contact_person, 13, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->contact_email, 14, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->contact_phone, 15, MAX_STR_LEN);
        SAFE_STRNCPY_FETCH(client->status, 16, MAX_STR_LEN);
        if (strlen(client->status) == 0) strcpy(client->status, "Active");
        SAFE_STRNCPY_FETCH(client->notes, 17, MAX_NOTES_LEN);
        SAFE_STRNCPY_FETCH(client->created_at, 18, MAX_STR_LEN);
        #undef SAFE_STRNCPY_FETCH
    } else if (rc != SQLITE_DONE) {
        if(status_win) show_error("Failed to step select: %s", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_insert_client(const Client *c) {
    if (!db) { if(status_win) show_error("DB not connected for insert."); return 0; }
    const char *sql = "INSERT INTO clients (business_name, email, phone, website, street, city, state, zip_code, country, "
                      "tax_number, num_employees, industry, contact_person, contact_email, contact_phone, status, notes) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        if(status_win) show_error("DB prepare INSERT failed: %s", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(stmt, 1, c->business_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, c->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, c->website, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, c->street, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, c->city, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, c->state, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, c->zip_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, c->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, c->tax_number, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 11, c->num_employees);
    sqlite3_bind_text(stmt, 12, c->industry, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, c->contact_person, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 14, c->contact_email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 15, c->contact_phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 16, c->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 17, c->notes, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        if (sqlite3_errcode(db) == SQLITE_CONSTRAINT_UNIQUE) {
             if(status_win) show_error("Insert failed: Business Name '%s' already exists.", c->business_name);
        } else {
             if(status_win) show_error("DB execute INSERT failed: %s", sqlite3_errmsg(db));
        }
        return 0;
    }
    return 1;
}

int db_update_client(const Client *c) {
    if (!db) { if(status_win) show_error("DB not connected for update."); return 0; }
    const char *sql = "UPDATE clients SET business_name=?, email=?, phone=?, website=?, street=?, city=?, state=?, zip_code=?, country=?, "
                      "tax_number=?, num_employees=?, industry=?, contact_person=?, contact_email=?, contact_phone=?, status=?, notes=? "
                      "WHERE id=?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        if(status_win) show_error("DB prepare UPDATE failed: %s", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(stmt, 1, c->business_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, c->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, c->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, c->website, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, c->street, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, c->city, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, c->state, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, c->zip_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, c->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, c->tax_number, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 11, c->num_employees);
    sqlite3_bind_text(stmt, 12, c->industry, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, c->contact_person, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 14, c->contact_email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 15, c->contact_phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 16, c->status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 17, c->notes, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 18, c->id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        if (sqlite3_errcode(db) == SQLITE_CONSTRAINT_UNIQUE) {
             if(status_win) show_error("Update failed: Business Name '%s' already exists for another customer.", c->business_name);
        } else {
             if(status_win) show_error("DB execute UPDATE failed: %s", sqlite3_errmsg(db));
        }
        return 0;
    }
    return 1;
}

int db_delete_client(int client_id) {
    if (!db) { if(status_win) show_error("DB not connected for delete."); return 0; }
    const char *sql = "DELETE FROM clients WHERE id = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        if(status_win) show_error("DB prepare DELETE failed: %s", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, client_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        if(status_win) show_error("DB execute DELETE failed: %s", sqlite3_errmsg(db));
        return 0;
    }
    return 1;
}


// --- Input Helpers ---
int get_string_input(WINDOW *win, int y, int x, const char *prompt, char *buffer, int max_len, bool allow_empty, const char *current_value_display) {
    if (!win) return -2;
    if (max_len <= 0) {
        if(status_win) show_error("Internal error: max_len <= 0 in get_string_input for %s", prompt);
        return 0;
    }

    char temp_buffer[max_len];
    temp_buffer[0] = '\0';

    werase(win); draw_custom_box(win);
    curs_set(1);
    echo();

    wmove(win, y, x);
    int prompt_text_visual_len = RF_INPUT_PROMPT_VISUAL_LEN + strlen(prompt) + 1;

    wprintw(win, "%s%s ", RF_INPUT_PROMPT_STR, prompt);
    if (current_value_display && strlen(current_value_display) > 0) {
        if(has_colors()) wattron(win, COLOR_PAIR(COLOR_PAIR_INPUT_CURRENT));

        int available_for_current = getmaxx(win) - x - prompt_text_visual_len - 3 - 2;
        if (available_for_current < 3) available_for_current = 3;

        wprintw(win, "[%.*s%s]",
                available_for_current - (strlen(current_value_display) > (size_t)available_for_current ? 2:0),
                current_value_display,
                strlen(current_value_display) > (size_t)available_for_current ? ".." : "");

        if(has_colors()) wattroff(win, COLOR_PAIR(COLOR_PAIR_INPUT_CURRENT));
    }
    wprintw(win, ": ");
    wrefresh(win);

    int result = wgetnstr(win, temp_buffer, max_len - 1);

    noecho();
    curs_set(0);
    werase(win); draw_custom_box(win); wrefresh(win);

    temp_buffer[strcspn(temp_buffer, "\r\n")] = 0;

    if (result == ERR) {
        if (current_value_display) {
            strncpy(buffer, current_value_display, max_len - 1);
        } else {
            buffer[0] = '\0';
        }
        buffer[max_len - 1] = '\0';
        return -1;
    }

    if (strlen(temp_buffer) == 0) {
        if (current_value_display) {
            strncpy(buffer, current_value_display, max_len - 1);
            buffer[max_len - 1] = '\0';
            return 1;
        } else {
            if (!allow_empty) {
                show_error("Input for '%s' cannot be empty.", prompt);
                buffer[0] = '\0';
                return 0;
            }
            buffer[0] = '\0';
            return 1;
        }
    }

    strncpy(buffer, temp_buffer, max_len - 1);
    buffer[max_len - 1] = '\0';
    return 1;
}

int get_int_input(WINDOW *win, int y, int x, const char *prompt, int *value_ptr, int current_value) {
    char buffer[MAX_STR_LEN];
    char current_val_str[32];
    snprintf(current_val_str, sizeof(current_val_str), "%d", current_value);

    int res = get_string_input(win, y, x, prompt, buffer, sizeof(buffer), true, current_val_str);

    if (res == -1) return -1;
    if (res == 0 ) return 0;

    if (strlen(buffer) == 0 || strcmp(buffer, current_val_str) == 0) {
        *value_ptr = current_value;
        return 1;
    }

    char *endptr;
    long num = strtol(buffer, &endptr, 10);

    if (*endptr != '\0' || endptr == buffer) {
        show_error("Invalid number for '%s': %s", prompt, buffer);
        *value_ptr = current_value;
        return 0;
    }
    *value_ptr = (int)num;
    return 1;
}

int select_client_status(char *selected_status, const char *current_status) {
    const char *statuses[] = {"Active", "Inactive", "Prospect", "Lead", "Former", "(Cancel)"};
    int n_statuses = sizeof(statuses) / sizeof(statuses[0]);
    int choice = 0;
    int key;

    if (current_status && strlen(current_status) > 0) {
        for (int i = 0; i < n_statuses - 1; ++i) {
            if (strcasecmp(current_status, statuses[i]) == 0) {
                choice = i;
                break;
            }
        }
    } else {
      choice = 0;
    }

    curs_set(0); noecho();

    while(1) {
        check_and_handle_resize();
        if (!input_win) return -1;

        werase(input_win); draw_custom_box(input_win);
        mvwprintw(input_win, 0, 1, "Select Status (Current: %s). Arrows, Enter, ESC.", current_status ? current_status : "N/A");

        int current_x_pos = 1;
        for (int i = 0; i < n_statuses; ++i) {
            if (i == choice) {
                wattron(input_win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);
            }
            mvwprintw(input_win, 1, current_x_pos, " %s ", statuses[i]);
            if (i == choice) {
                wattroff(input_win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);
            }
            current_x_pos += strlen(statuses[i]) + 3;
        }
        wrefresh(input_win);

        key = wgetch(input_win);

        switch(key) {
            case KEY_NAV_LEFT: choice = (choice - 1 + n_statuses) % n_statuses; break;
            case KEY_NAV_RIGHT: choice = (choice + 1) % n_statuses; break;
            case KEY_ACTION_SELECT:
            case KEY_ACTION_ENTER:
                werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
                if (choice == n_statuses - 1) {
                    if (current_status) strncpy(selected_status, current_status, MAX_STR_LEN -1);
                    else strcpy(selected_status, "Active");
                    selected_status[MAX_STR_LEN-1] = '\0';
                    return -1;
                }
                strncpy(selected_status, statuses[choice], MAX_STR_LEN - 1);
                selected_status[MAX_STR_LEN - 1] = '\0';
                return 1;
            case KEY_ESC:
                werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
                if (current_status) strncpy(selected_status, current_status, MAX_STR_LEN -1);
                else strcpy(selected_status, "Active");
                selected_status[MAX_STR_LEN-1] = '\0';
                return -1;
        }
    }
}

// --- Core Screens & UI Logic ---
void display_editor_main_menu() {
    const char *options[] = {
        "1. Add New Customer",
        "2. Edit/Search/View Customer",
        "3. Delete Customer",
        "4. Return to Main Program",
        "Q. Quit"
    };
    int n_options = sizeof(options) / sizeof(options[0]);
    int choice = 0;
    int key;

    cchar_t title_sep_char;
    setcchar(&title_sep_char, (const wchar_t[]){WC_RF_TITLE_SEP_CHAR, L'\0'}, A_NORMAL, 0, NULL);


    while (!exit_requested) {
        check_and_handle_resize();
        if (!main_win || !input_win || !status_win) {
            if(exit_requested) break;
            napms(100);
            if(!main_win) continue;
        }

        werase(main_win); draw_custom_box(main_win);

        const char *menu_title = "GEXTUX CUSTOMER MANAGEMENT";
        mvwprintw(main_win, SCREEN_TITLE_Y, (getmaxx(main_win) - strlen(menu_title)) / 2, "%s", menu_title);

        int sep_len = strlen(menu_title);
        if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
        int max_sep_len = getmaxx(main_win) - (2 * MAIN_WIN_BORDER_WIDTH);
        if (max_sep_len < 0) max_sep_len = 0;
        if (sep_len > max_sep_len) sep_len = max_sep_len;

        if (sep_len > 0) {
            int sep_x = (getmaxx(main_win) - sep_len) / 2;
            wmove(main_win, SCREEN_SEPARATOR_Y, sep_x);
            for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
        }

        for (int i = 0; i < n_options; ++i) {
            int y_pos = SCREEN_CONTENT_Y_MENU + i;
            if (y_pos >= getmaxy(main_win) - 1 -1) break;
            if (i == choice) {
                wattron(main_win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);
                mvwprintw(main_win, y_pos, MENU_INDENT, "%s%s", RF_MENU_SELECTOR_STR, options[i]);
                wattroff(main_win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);
            } else {
                char padding[RF_MENU_SELECTOR_VISUAL_LEN + 1];
                memset(padding, ' ', RF_MENU_SELECTOR_VISUAL_LEN);
                padding[RF_MENU_SELECTOR_VISUAL_LEN] = '\0';
                mvwprintw(main_win, y_pos, MENU_INDENT, "%s%s", padding, options[i]);
            }
        }
        mvwprintw(main_win, getmaxy(main_win) - 2, MAIN_WIN_BORDER_WIDTH, "Use UP/DOWN, ENTER, Number, or Q.");
        wrefresh(main_win);

        werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
        clear_status();

        key = wgetch(main_win);
        if (key == ERR && !exit_requested) { continue; }

        switch (key) {
            case KEY_NAV_UP: choice = (choice - 1 + n_options) % n_options; break;
            case KEY_NAV_DOWN: choice = (choice + 1) % n_options; break;
            case KEY_ACTION_SELECT:
            case KEY_ACTION_ENTER:
                if (choice == 0) add_new_customer_screen();
                else if (choice == 1) customer_search_workflow("EDIT CUSTOMER SEARCH", "Enter ID or part of Name, Contact, Email, City.", INTERACTIVE_LIST_ACTION_EDIT);
                else if (choice == 2) customer_search_workflow("DELETE CUSTOMER SEARCH", "Enter ID or part of Name, Contact, Email, City.", INTERACTIVE_LIST_ACTION_DELETE);
                else if (choice == 3) { execute_gextux_crm(); return; }
                else if (choice == 4) exit_requested = 1;
                break;
            case '1': add_new_customer_screen(); break;
            case '2': customer_search_workflow("EDIT CUSTOMER SEARCH", "Enter ID or part of Name, Contact, Email, City.", INTERACTIVE_LIST_ACTION_EDIT); break;
            case '3': customer_search_workflow("DELETE CUSTOMER SEARCH", "Enter ID or part of Name, Contact, Email, City.", INTERACTIVE_LIST_ACTION_DELETE); break;
            case '4': execute_gextux_crm(); return;
            case KEY_ACTION_QUIT:
            case KEY_ACTION_QUIT_ALT:
                exit_requested = 1;
                break;
            default:
                 if (key >= '1' && key <= '4') {
                    choice = key - '1';
                    ungetch(KEY_ACTION_SELECT);
                 } else {
                    show_status("Invalid choice. Use Arrows, Numbers (1-4), or Q."); beep(); napms(1000);
                 }
                 break;
        }
    }
}

void add_new_customer_screen() {
    Client new_client;
    memset(&new_client, 0, sizeof(Client));
    strcpy(new_client.status, "Active");

    cchar_t title_sep_char;
    setcchar(&title_sep_char, (const wchar_t[]){WC_RF_TITLE_SEP_CHAR, L'\0'}, A_NORMAL, 0, NULL);

    werase(main_win); draw_custom_box(main_win);
    const char *form_title_text = "ADD NEW CUSTOMER";
    mvwprintw(main_win, SCREEN_TITLE_Y, (getmaxx(main_win) - strlen(form_title_text)) / 2, "%s", form_title_text);

    int sep_len = strlen(form_title_text);
    if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
    int max_sep_len = getmaxx(main_win) - (2 * MAIN_WIN_BORDER_WIDTH);
    if (max_sep_len < 0) max_sep_len = 0;
    if (sep_len > max_sep_len) sep_len = max_sep_len;

    if (sep_len > 0) {
        int sep_x = (getmaxx(main_win) - sep_len) / 2;
        wmove(main_win, SCREEN_SEPARATOR_Y, sep_x);
        for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
    }

    mvwprintw(main_win, SCREEN_CONTENT_Y_STD, MAIN_WIN_BORDER_WIDTH, "ESC cancels field, type '%s' and Enter to abort all.", CANCEL_INPUT_STRING);
    wrefresh(main_win);

    int input_res;
    #define GET_STR_FIELD(prompt_str, field_name, max_len_val, allow_empty_val, current_val_display_ptr) \
        input_res = get_string_input(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, prompt_str, new_client.field_name, max_len_val, allow_empty_val, current_val_display_ptr); \
        if (input_res == -1) { \
            show_status("Field input cancelled. Field is empty/default."); napms(700); \
        } else if (input_res == 0) { \
            show_status("Aborting add customer due to invalid input."); napms(1000); return; \
        } \
        if (input_res == 1 && strcmp(new_client.field_name, CANCEL_INPUT_STRING) == 0) { \
            show_status("Add customer cancelled by user."); napms(1000); return; \
        }

    GET_STR_FIELD("Business Name*", business_name, MAX_STR_LEN, false, NULL);
    GET_STR_FIELD("Email", email, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Phone", phone, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Website", website, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Street", street, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("City", city, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("State", state, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Zip Code", zip_code, sizeof(new_client.zip_code), true, NULL);
    GET_STR_FIELD("Country", country, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Tax Number", tax_number, MAX_STR_LEN, true, NULL);

    input_res = get_int_input(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Num Employees", &new_client.num_employees, 0);
    if (input_res == -1) { show_status("Field input cancelled. Using default 0."); new_client.num_employees = 0; napms(700); }
    else if (input_res == 0) { show_status("Aborting add customer due to invalid input."); napms(1000); return; }

    GET_STR_FIELD("Industry", industry, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Contact Person", contact_person, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Contact Email", contact_email, MAX_STR_LEN, true, NULL);
    GET_STR_FIELD("Contact Phone", contact_phone, MAX_STR_LEN, true, NULL);

    input_res = select_client_status(new_client.status, "Active");
    if (input_res == -1) { strcpy(new_client.status, "Active"); show_status("Status selection cancelled. Defaulting to 'Active'."); napms(700); }

    show_status("Enter Notes (up to %d chars):", MAX_NOTES_LEN -1); napms(500);
    GET_STR_FIELD("Notes", notes, MAX_NOTES_LEN, true, NULL);
    #undef GET_STR_FIELD

    werase(input_win); draw_custom_box(input_win);
    mvwprintw(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Save new customer '%s'? (Y/N): ", new_client.business_name);
    wrefresh(input_win);
    int confirm_key = wgetch(input_win);
    werase(input_win); draw_custom_box(input_win); wrefresh(input_win);

    if (toupper(confirm_key) == 'Y') {
        show_loading_indicator(true);
        if (db_insert_client(&new_client)) show_status("Customer '%s' added.", new_client.business_name);
        show_loading_indicator(false);
    } else {
        show_status("Add customer cancelled.");
    }
    napms(1500);
}

void customer_search_workflow(const char *screen_title, const char *search_prompt_detail, InteractiveListAction action) {
    char search_term[MAX_STR_LEN];
    char *sql_query = NULL;

    cchar_t title_sep_char;
    setcchar(&title_sep_char, (const wchar_t[]){WC_RF_TITLE_SEP_CHAR, L'\0'}, A_NORMAL, 0, NULL);

    werase(main_win); draw_custom_box(main_win);
    mvwprintw(main_win, SCREEN_TITLE_Y, (getmaxx(main_win) - strlen(screen_title)) / 2, "%s", screen_title);

    int sep_len = strlen(screen_title);
    if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
    int max_sep_len = getmaxx(main_win) - (2 * MAIN_WIN_BORDER_WIDTH);
    if (max_sep_len < 0) max_sep_len = 0;
    if (sep_len > max_sep_len) sep_len = max_sep_len;

    if (sep_len > 0) {
        int sep_x = (getmaxx(main_win) - sep_len) / 2;
        wmove(main_win, SCREEN_SEPARATOR_Y, sep_x);
        for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
    }

    mvwprintw(main_win, SCREEN_CONTENT_Y_STD, MAIN_WIN_BORDER_WIDTH, "%s", search_prompt_detail);
    wrefresh(main_win);

    if (get_string_input(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Search", search_term, MAX_STR_LEN, false, NULL) <= 0) {
        show_status("Search cancelled or empty input."); napms(1000);
        return;
    }
    if (strlen(search_term) == 0) {
        show_status("Search term cannot be empty."); napms(1000);
        return;
    }

    char *end_ptr;
    long id_val = strtol(search_term, &end_ptr, 10);

    if (*end_ptr == '\0' && search_term != end_ptr) {
        sql_query = sqlite3_mprintf("SELECT id, business_name, city, phone, email, contact_person FROM clients WHERE id = %ld ORDER BY business_name COLLATE NOCASE;", id_val);
    } else {
        char *pattern = sqlite3_mprintf("%%%s%%", search_term);
        if (!pattern) { show_error("Memory allocation failed for search pattern."); return; }
        sql_query = sqlite3_mprintf(
            "SELECT id, business_name, city, phone, email, contact_person FROM clients "
            "WHERE business_name LIKE %Q "
            "OR contact_person LIKE %Q "
            "OR email LIKE %Q "
            "OR city LIKE %Q "
            "ORDER BY business_name COLLATE NOCASE;",
            pattern, pattern, pattern, pattern);
        sqlite3_free(pattern);
    }

    if (!sql_query) { show_error("Failed to construct search query."); return; }

    display_interactive_client_list(screen_title, sql_query, action);
    sqlite3_free(sql_query);
}


void edit_customer_form_screen(int client_id) {
    Client client, original_client;

    cchar_t title_sep_char;
    setcchar(&title_sep_char, (const wchar_t[]){WC_RF_TITLE_SEP_CHAR, L'\0'}, A_NORMAL, 0, NULL);

    if (!fetch_client_by_id(client_id, &client)) {
        show_error("Could not fetch details for customer ID %d to edit.", client_id);
        return;
    }
    original_client = client;

    werase(main_win); draw_custom_box(main_win);
    char form_title_text[MAX_STR_LEN + 50];
    snprintf(form_title_text, sizeof(form_title_text), "--- Edit Customer: %s (ID: %d) ---", client.business_name, client.id);
    mvwprintw(main_win, SCREEN_TITLE_Y, (getmaxx(main_win) - strlen(form_title_text)) / 2, "%s", form_title_text);

    int sep_len = strlen(form_title_text);
    if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
    int max_sep_len = getmaxx(main_win) - (2 * MAIN_WIN_BORDER_WIDTH);
    if (max_sep_len < 0) max_sep_len = 0;
    if (sep_len > max_sep_len) sep_len = max_sep_len;

    if (sep_len > 0) {
        int sep_x = (getmaxx(main_win) - sep_len) / 2;
        wmove(main_win, SCREEN_SEPARATOR_Y, sep_x);
        for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
    }

    mvwprintw(main_win, SCREEN_CONTENT_Y_STD, MAIN_WIN_BORDER_WIDTH, "Enter to keep current, ESC cancels field, type '%s' and Enter to abort all.", CANCEL_INPUT_STRING);
    wrefresh(main_win);

    int input_res;
    #define EDIT_STR_FIELD(prompt_str, field_name, max_len_val, allow_empty_val) \
        input_res = get_string_input(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, prompt_str, client.field_name, max_len_val, allow_empty_val, original_client.field_name); \
        if (input_res == -1) { \
            show_status("Field edit cancelled, value reverted."); napms(700); \
        } else if (input_res == 0) { \
            show_status("Aborting edit due to invalid input."); napms(1000); return; \
        } \
        if (input_res == 1 && strcmp(client.field_name, CANCEL_INPUT_STRING) == 0) { \
            show_status("Edit customer cancelled by user."); napms(1000); return; \
        }

    EDIT_STR_FIELD("Business Name*", business_name, MAX_STR_LEN, false);
    EDIT_STR_FIELD("Email", email, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Phone", phone, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Website", website, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Street", street, MAX_STR_LEN, true);
    EDIT_STR_FIELD("City", city, MAX_STR_LEN, true);
    EDIT_STR_FIELD("State", state, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Zip Code", zip_code, sizeof(client.zip_code), true);
    EDIT_STR_FIELD("Country", country, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Tax Number", tax_number, MAX_STR_LEN, true);

    input_res = get_int_input(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Num Employees", &client.num_employees, original_client.num_employees);
    if (input_res == -1) { client.num_employees = original_client.num_employees; show_status("Field edit cancelled, value reverted."); napms(700); }
    else if (input_res == 0) { show_status("Aborting edit due to invalid input."); napms(1000); return; }

    EDIT_STR_FIELD("Industry", industry, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Contact Person", contact_person, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Contact Email", contact_email, MAX_STR_LEN, true);
    EDIT_STR_FIELD("Contact Phone", contact_phone, MAX_STR_LEN, true);

    input_res = select_client_status(client.status, original_client.status);
    if (input_res == -1) { strncpy(client.status, original_client.status, MAX_STR_LEN-1); client.status[MAX_STR_LEN-1]='\0'; show_status("Status selection cancelled, value reverted."); napms(700); }

    show_status("Edit Notes (up to %d chars):", MAX_NOTES_LEN -1); napms(500);
    EDIT_STR_FIELD("Notes", notes, MAX_NOTES_LEN, true);
    #undef EDIT_STR_FIELD

    werase(input_win); draw_custom_box(input_win);
    mvwprintw(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "Save changes to '%s'? (Y/N): ", client.business_name);
    wrefresh(input_win);
    int confirm_key = wgetch(input_win);
    werase(input_win); draw_custom_box(input_win); wrefresh(input_win);

    if (toupper(confirm_key) == 'Y') {
        show_loading_indicator(true);
        if (db_update_client(&client)) show_status("Customer '%s' updated.", client.business_name);
        show_loading_indicator(false);
    } else {
        show_status("Edit customer cancelled. No changes saved.");
    }
    napms(1500);
}

// --- New Interactive List with Detail Pane ---
void wclr_pane_line(WINDOW *win, int y, int x, int width) {
    wmove(win, y, x);
    for (int i = 0; i < width; ++i) {
        if (x + i >= getmaxx(win)) break;
        waddch(win, ' ');
    }
}

void calculate_list_column_widths_for_pane(ListColumnWidths *widths, int pane_content_width) {
    int min_id_w = 4;
    int padding = LIST_COL_PADDING;

    memset(widths, 0, sizeof(ListColumnWidths));

    if (pane_content_width <= 0) return;

    if (pane_content_width < min_id_w + padding + 1) {
        widths->id_width = pane_content_width;
        widths->name_width = 0;
        widths->name_col_start = pane_content_width;
    }
    else {
        widths->id_width = min_id_w;
        widths->name_width = pane_content_width - widths->id_width - padding;
        if (widths->name_width < 0) widths->name_width = 0;
        widths->name_col_start = widths->id_width + padding;
    }
    if (widths->id_width < 0) widths->id_width = 0;
    if (widths->name_width < 0) widths->name_width = 0;
}

void draw_list_header_in_pane(WINDOW *win, const ListColumnWidths *col_widths, int pane_start_y, int pane_start_x, int pane_content_width) {
    if (pane_content_width <=0) return;

    if (has_colors()) {
        wattron(win, COLOR_PAIR(COLOR_PAIR_LIST_HEADER) | A_BOLD);
    } else {
        wattron(win, A_BOLD | A_REVERSE);
    }

    wmove(win, pane_start_y, pane_start_x);
    for(int i = 0; i < pane_content_width; ++i) {
        if (pane_start_x + i >= getmaxx(win)) break;
        waddch(win, ' ');
    }
    wmove(win, pane_start_y, pane_start_x);

    if (col_widths->id_width > 0) {
        mvwprintw(win, pane_start_y, pane_start_x, "%-*.*s", col_widths->id_width, col_widths->id_width, "ID");
    }
    if (col_widths->name_width > 0) {
        if (pane_start_x + col_widths->name_col_start < pane_start_x + pane_content_width) {
             mvwprintw(win, pane_start_y, pane_start_x + col_widths->name_col_start, "%-*.*s", col_widths->name_width, col_widths->name_width, "Business Name");
        }
    }

    if (has_colors()) {
        wattroff(win, COLOR_PAIR(COLOR_PAIR_LIST_HEADER) | A_BOLD);
    } else {
        wattroff(win, A_BOLD | A_REVERSE);
    }
}

void draw_list_item_in_pane(WINDOW *win, int y_on_screen, const ClientListItem *item, const ListColumnWidths *col_widths, bool highlighted, int pane_start_x, int pane_content_width) {
    if (pane_content_width <= 0) return;
    if (highlighted) wattron(win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);

    wclr_pane_line(win, y_on_screen, pane_start_x, pane_content_width);

    if (col_widths->id_width > 0) {
        mvwprintw(win, y_on_screen, pane_start_x, "%-*d", col_widths->id_width, item->id);
    }
    if (col_widths->name_width > 0) {
        if (pane_start_x + col_widths->name_col_start < pane_start_x + pane_content_width) {
            mvwprintw(win, y_on_screen, pane_start_x + col_widths->name_col_start, "%.*s", col_widths->name_width, item->business_name);
        }
    }

    if (highlighted) wattroff(win, has_colors() ? COLOR_PAIR(COLOR_PAIR_HIGHLIGHT) : A_REVERSE);
}

void draw_client_details_in_pane(WINDOW *win, const Client *client, int pane_start_y, int pane_start_x, int pane_content_width) {
    if (!win) return;

    bool color_was_set = false;
    if (has_colors()) {
        wattron(win, COLOR_PAIR(COLOR_PAIR_INPUT_CURRENT));
        color_was_set = true;
    }

    for (int i = 0; i < getmaxy(win) - (MAIN_WIN_BORDER_WIDTH -1) - pane_start_y ; ++i) {
         if (pane_start_y + i >= getmaxy(win) - (MAIN_WIN_BORDER_WIDTH -1)) break;
         wclr_pane_line(win, pane_start_y + i, pane_start_x, pane_content_width);
    }

    if (!client || pane_content_width <= DETAIL_LABEL_WIDTH + 5) {
        if (pane_content_width > 5) mvwprintw(win, pane_start_y, pane_start_x, " (No details) ");
        if (color_was_set) {
            wattroff(win, COLOR_PAIR(COLOR_PAIR_INPUT_CURRENT));
        }
        return;
    }

    int y = pane_start_y;
    int label_w = DETAIL_LABEL_WIDTH;
    int value_w = pane_content_width - label_w - 2;
    if (value_w < 1) value_w = 1;

    #define PRINT_PANE_DETAIL(label, value_fmt, ...) \
    do { \
        if (y >= getmaxy(win) - (MAIN_WIN_BORDER_WIDTH -1)) break; \
        mvwprintw(win, y, pane_start_x, "%-*.*s: ", label_w, label_w, label); \
        char temp_val_buf[MAX_STR_LEN*2]; \
        snprintf(temp_val_buf, sizeof(temp_val_buf), value_fmt, ##__VA_ARGS__); \
        wprintw(win, "%.*s", value_w, temp_val_buf); \
        y++; \
    } while(0)

    char num_buf[32];

    PRINT_PANE_DETAIL("ID", "%d", client->id);
    PRINT_PANE_DETAIL("Business Name", "%s", client->business_name);
    PRINT_PANE_DETAIL("Email", "%s", client->email);
    PRINT_PANE_DETAIL("Phone", "%s", client->phone);
    PRINT_PANE_DETAIL("Website", "%s", client->website);
    PRINT_PANE_DETAIL("Street", "%s", client->street);
    PRINT_PANE_DETAIL("City", "%s", client->city);
    PRINT_PANE_DETAIL("State", "%s", client->state);
    PRINT_PANE_DETAIL("Zip Code", "%s", client->zip_code);
    PRINT_PANE_DETAIL("Country", "%s", client->country);
    PRINT_PANE_DETAIL("Tax Number", "%s", client->tax_number);
    snprintf(num_buf, sizeof(num_buf), "%d", client->num_employees);
    PRINT_PANE_DETAIL("Num Employees", "%s", num_buf);
    PRINT_PANE_DETAIL("Industry", "%s", client->industry);
    PRINT_PANE_DETAIL("Contact Person", "%s", client->contact_person);
    PRINT_PANE_DETAIL("Contact Email", "%s", client->contact_email);
    PRINT_PANE_DETAIL("Contact Phone", "%s", client->contact_phone);
    PRINT_PANE_DETAIL("Status", "%s", client->status);
    PRINT_PANE_DETAIL("Created At", "%s", client->created_at);

    if (y < getmaxy(win) - (MAIN_WIN_BORDER_WIDTH -1) -1) {
        mvwprintw(win, y++, pane_start_x, "%-*.*s:", label_w, label_w, "Notes");
        const char *notes_ptr = client->notes;
        int notes_text_start_x = pane_start_x + 2;
        int notes_available_width = pane_content_width - 2;
        if (notes_available_width < 1) notes_available_width = 1;


        while (*notes_ptr && y < getmaxy(win) - (MAIN_WIN_BORDER_WIDTH -1)) {
            wmove(win, y, notes_text_start_x);
            int bytes_to_print_this_line = 0;
            int current_visual_width = 0;
            const char* line_start_ptr = notes_ptr;

            const char* temp_ptr = notes_ptr;
            while (*temp_ptr && *temp_ptr != '\n') {
                wchar_t wch;
                int mb_len = mbtowc(&wch, temp_ptr, MB_CUR_MAX);

                if (mb_len <= 0) {
                    if (mb_len < 0) {
                        bytes_to_print_this_line++; temp_ptr++;
                    }
                    break;
                }

                int char_width = wcwidth(wch); // wcwidth warning might still appear if not resolved by macro
                if (char_width < 0) char_width = 1;

                if (current_visual_width + char_width > notes_available_width) {
                    break;
                }
                current_visual_width += char_width;
                bytes_to_print_this_line += mb_len;
                temp_ptr += mb_len;
            }

            if (bytes_to_print_this_line > 0) {
                waddnstr(win, line_start_ptr, bytes_to_print_this_line);
            }

            y++;
            notes_ptr += bytes_to_print_this_line;
            // --- BEGIN MODIFIED SECTION for infinite loop fix ---
            if (*notes_ptr == '\n') {
                notes_ptr++; // Consume the newline
            } else if (bytes_to_print_this_line == 0 && *notes_ptr != '\0') {
                // This condition is met if:
                // 1. No bytes were actually processed for printing on the current line
                //    (e.g., the first character was too wide, or it was an encoding error
                //     that the inner loop didn't convert to printable bytes).
                // 2. We are not at the end of the notes string.
                //
                // To prevent an infinite loop, we MUST advance notes_ptr.
                notes_ptr++; // Advance past the problematic byte/character.
                mbtowc(NULL, NULL, 0); // Reset mbtowc's internal shift state, as we've skipped a byte.
            }
            // --- END MODIFIED SECTION ---
        }
    }
    #undef PRINT_PANE_DETAIL

    if (color_was_set) {
        wattroff(win, COLOR_PAIR(COLOR_PAIR_INPUT_CURRENT));
    }
}

void display_interactive_client_list(const char *title, const char *sql_query, InteractiveListAction action_type) {
    check_and_handle_resize();
    if (!main_win || !input_win || !status_win) return;

    ClientListItem *items = NULL; int total_items = 0, capacity = 0;
    FetchListData list_data = { &items, &total_items, &capacity };

    cchar_t title_sep_char;
    setcchar(&title_sep_char, (const wchar_t[]){WC_RF_TITLE_SEP_CHAR, L'\0'}, A_NORMAL, 0, NULL);
    cchar_t pane_sep_char;
    setcchar(&pane_sep_char, (const wchar_t[]){WC_RF_PANE_VSEP, L'\0'}, A_NORMAL, 0, NULL);


    show_loading_indicator(true); show_status("Searching customers...");
    bool fetch_success = db_execute(sql_query, fetch_list_items_callback, &list_data);
    show_loading_indicator(false); clear_status();

    if (!fetch_success) { free(items); return; }
    if (total_items == 0) {
        werase(main_win); draw_custom_box(main_win);
        mvwprintw(main_win, SCREEN_TITLE_Y, (getmaxx(main_win) - strlen(title)) / 2, "%s", title);

        int sep_len = strlen(title);
        if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
        int max_sep_len = getmaxx(main_win) - (2 * MAIN_WIN_BORDER_WIDTH);
        if (max_sep_len < 0) max_sep_len = 0;
        if (sep_len > max_sep_len) sep_len = max_sep_len;

        if (sep_len > 0) {
            int sep_x = (getmaxx(main_win) - sep_len) / 2;
            wmove(main_win, SCREEN_SEPARATOR_Y, sep_x);
            for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
        }

        mvwprintw(main_win, SCREEN_CONTENT_Y_MENU, MENU_INDENT, "No customers found matching your search criteria.");
        wrefresh(main_win);

        werase(input_win); draw_custom_box(input_win);
        mvwprintw(input_win, 1, 1, "Press any key to return...");
        wrefresh(input_win);

        wgetch(main_win);
        free(items);
        return;
    }

    int top_item_index = 0, selected_item_index = 0;
    Client current_detailed_client;
    bool details_loaded_for_selected = false;
    int prev_selected_item_index = -1;
    int key;

    ListColumnWidths list_col_widths;
    int main_win_height, main_win_width;

    int title_bar_h = (SCREEN_SEPARATOR_Y - SCREEN_TITLE_Y) + 1;
    int list_header_h = 1;
    int instruction_h = 0;

    while (!exit_requested) {
        check_and_handle_resize();
        if (!main_win || !input_win || !status_win) {
            if (items) free(items);
            return;
        }

        main_win_height = getmaxy(main_win);
        main_win_width = getmaxx(main_win);

        int content_below_separator_y = SCREEN_SEPARATOR_Y + 1;
        int list_items_start_y = content_below_separator_y + list_header_h;

        int list_pane_content_height = main_win_height - (2 * (MAIN_WIN_BORDER_WIDTH-1))
                                     - title_bar_h;
        if (list_pane_content_height < 1) list_pane_content_height = 1;


        int available_content_width = main_win_width - (2 * MAIN_WIN_BORDER_WIDTH);
        int list_pane_w = (int)(available_content_width * LIST_PANE_PERCENT);
        int detail_pane_w = available_content_width - list_pane_w - PANE_SEPARATOR_WIDTH;

        if (detail_pane_w < DETAIL_PANE_MIN_WIDTH && list_pane_w > (DETAIL_PANE_MIN_WIDTH + PANE_SEPARATOR_WIDTH + 20)) {
            detail_pane_w = DETAIL_PANE_MIN_WIDTH;
            list_pane_w = available_content_width - detail_pane_w - PANE_SEPARATOR_WIDTH;
        } else if (list_pane_w < 30 && detail_pane_w > (30 + PANE_SEPARATOR_WIDTH + DETAIL_PANE_MIN_WIDTH)) {
            list_pane_w = 30;
            detail_pane_w = available_content_width - list_pane_w - PANE_SEPARATOR_WIDTH;
        }
        if (list_pane_w < 0) list_pane_w = 0;
        if (detail_pane_w < 0) detail_pane_w = 0;

        int list_pane_start_x = MAIN_WIN_BORDER_WIDTH;
        int separator_x_pane = list_pane_start_x + list_pane_w;
        int detail_pane_start_x = separator_x_pane + PANE_SEPARATOR_WIDTH;

        werase(main_win); draw_custom_box(main_win);
        mvwprintw(main_win, SCREEN_TITLE_Y, (main_win_width - strlen(title)) / 2, "%s", title);

        int sep_len = strlen(title);
        if (sep_len < MIN_SEPARATOR_WIDTH) sep_len = MIN_SEPARATOR_WIDTH;
        int max_sep_len = main_win_width - (2 * MAIN_WIN_BORDER_WIDTH);
        if (max_sep_len < 0) max_sep_len = 0;
        if (sep_len > max_sep_len) sep_len = max_sep_len;

        if (sep_len > 0) {
            int sep_x_title = (main_win_width - sep_len) / 2;
            wmove(main_win, SCREEN_SEPARATOR_Y, sep_x_title);
            for (int k = 0; k < sep_len; ++k) wadd_wch(main_win, &title_sep_char);
        }

        if (PANE_SEPARATOR_WIDTH > 0 && list_pane_w > 0 && detail_pane_w > 0) {
            if(has_colors()) wattron(main_win, COLOR_PAIR(COLOR_PAIR_PANE_SEPARATOR));
            for (int i = content_below_separator_y; i < main_win_height - (MAIN_WIN_BORDER_WIDTH -1) - instruction_h; ++i) {
                mvwadd_wch(main_win, i, separator_x_pane, &pane_sep_char);
            }
            if(has_colors()) wattroff(main_win, COLOR_PAIR(COLOR_PAIR_PANE_SEPARATOR));
        }

        int items_per_page_list = list_pane_content_height - list_header_h;
        if (items_per_page_list <=0) items_per_page_list = 1;


        if (list_pane_w > 0 && total_items > 0) {
            calculate_list_column_widths_for_pane(&list_col_widths, list_pane_w);
            draw_list_header_in_pane(main_win, &list_col_widths, content_below_separator_y, list_pane_start_x, list_pane_w);

            int last_visible_on_page = top_item_index + items_per_page_list - 1;
            if (last_visible_on_page >= total_items) last_visible_on_page = total_items - 1;

            for (int i = top_item_index; i <= last_visible_on_page; ++i) {
                int screen_y = list_items_start_y + (i - top_item_index);
                if (screen_y >= content_below_separator_y + list_pane_content_height) break;
                draw_list_item_in_pane(main_win, screen_y, &items[i], &list_col_widths, (i == selected_item_index), list_pane_start_x, list_pane_w);
            }
        } else if (list_pane_w > 0 && total_items == 0) {
             calculate_list_column_widths_for_pane(&list_col_widths, list_pane_w);
             draw_list_header_in_pane(main_win, &list_col_widths, content_below_separator_y, list_pane_start_x, list_pane_w);
             if (list_items_start_y < content_below_separator_y + list_pane_content_height) {
                mvwprintw(main_win, list_items_start_y, list_pane_start_x + 1, "(No items)");
             }
        }

        if (total_items > 0 && selected_item_index >= 0 && selected_item_index < total_items) {
            if (selected_item_index != prev_selected_item_index || !details_loaded_for_selected) {
                show_loading_indicator(true);
                if (fetch_client_by_id(items[selected_item_index].id, &current_detailed_client)) {
                    details_loaded_for_selected = true;
                } else {
                    details_loaded_for_selected = false;
                    memset(&current_detailed_client, 0, sizeof(Client));
                    snprintf(current_detailed_client.business_name, MAX_STR_LEN, "Error loading ID %d", items[selected_item_index].id);
                }
                prev_selected_item_index = selected_item_index;
                show_loading_indicator(false);
            }
            if (detail_pane_w > 0) {
                 draw_client_details_in_pane(main_win, &current_detailed_client, content_below_separator_y, detail_pane_start_x, detail_pane_w);
            }
        } else if (detail_pane_w > 0) {
            draw_client_details_in_pane(main_win, NULL, content_below_separator_y, detail_pane_start_x, detail_pane_w);
        }

        wrefresh(main_win);

        werase(input_win); draw_custom_box(input_win);
        char instruction_buf[MAX_STR_LEN];
        const char* action_key_str = (action_type == INTERACTIVE_LIST_ACTION_EDIT) ? "E/Enter: Edit" : "D/Enter: Delete";
        snprintf(instruction_buf, sizeof(instruction_buf),
                 "Arrows/PgUp/PgDn | %s | Q/ESC: Back | Item %d/%d",
                 action_key_str,
                 total_items > 0 ? selected_item_index + 1 : 0, total_items);
        mvwprintw(input_win, 1, 1, "%.*s", getmaxx(input_win) - 2, instruction_buf);
        wrefresh(input_win);

        clear_status();

        key = wgetch(main_win);
        if (key == ERR && !exit_requested) continue;

        int items_per_page_nav = items_per_page_list > 0 ? items_per_page_list : 1;

        switch (key) {
            case KEY_NAV_UP:
                if (total_items > 0 && selected_item_index > 0) {
                    selected_item_index--;
                    if (selected_item_index < top_item_index) top_item_index = selected_item_index;
                } else beep();
                break;
            case KEY_NAV_DOWN:
                if (total_items > 0 && selected_item_index < total_items - 1) {
                    selected_item_index++;
                    if (selected_item_index >= top_item_index + items_per_page_nav) {
                        top_item_index = selected_item_index - items_per_page_nav + 1;
                        if (top_item_index < 0) top_item_index = 0;
                    }
                } else beep();
                break;
            case KEY_NAV_PPAGE:
                if (total_items > 0) {
                    selected_item_index -= items_per_page_nav;
                    if (selected_item_index < 0) selected_item_index = 0;
                    top_item_index = selected_item_index;
                } else beep();
                break;
            case KEY_NAV_NPAGE:
                if (total_items > 0) {
                    selected_item_index += items_per_page_nav;
                    if (selected_item_index >= total_items) selected_item_index = total_items > 0 ? total_items -1 : 0;
                    top_item_index = selected_item_index - items_per_page_nav + 1;
                    if (top_item_index < 0) top_item_index = 0;
                    if (total_items > 0 && top_item_index + items_per_page_nav > total_items) {
                         top_item_index = total_items - items_per_page_nav;
                         if (top_item_index < 0) top_item_index = 0;
                    }
                } else beep();
                break;
            case KEY_NAV_HOME:
                if (total_items > 0) { selected_item_index = 0; top_item_index = 0; }
                else beep();
                break;
            case KEY_NAV_END:
                if (total_items > 0) {
                    selected_item_index = total_items - 1;
                    top_item_index = total_items - items_per_page_nav;
                    if (top_item_index < 0) top_item_index = 0;
                } else beep();
                break;

            case KEY_EDIT_CLIENT: case KEY_EDIT_CLIENT_ALT:
                if (action_type != INTERACTIVE_LIST_ACTION_EDIT) { beep(); break; }
            // Fall through
            case KEY_ACTION_DELETE: case KEY_ACTION_DELETE_ALT:
                 if (action_type != INTERACTIVE_LIST_ACTION_DELETE && key != KEY_ACTION_SELECT && key != KEY_ACTION_ENTER) {
                    beep(); break;
                 }
                 if (action_type == INTERACTIVE_LIST_ACTION_DELETE && (key == KEY_EDIT_CLIENT || key == KEY_EDIT_CLIENT_ALT)) {
                     beep(); break;
                 }
            // Fall through
            case KEY_ACTION_SELECT: case KEY_ACTION_ENTER:
                if (total_items > 0 && selected_item_index >=0 && selected_item_index < total_items) {
                    int client_id_action = items[selected_item_index].id;
                    char client_name_action[MAX_STR_LEN];
                    strncpy(client_name_action, items[selected_item_index].business_name, MAX_STR_LEN -1);
                    client_name_action[MAX_STR_LEN-1] = '\0';

                    werase(input_win); draw_custom_box(input_win); wrefresh(input_win);

                    if (action_type == INTERACTIVE_LIST_ACTION_EDIT) {
                         if (key == KEY_ACTION_DELETE || key == KEY_ACTION_DELETE_ALT) {beep(); break;}
                        edit_customer_form_screen(client_id_action);
                        details_loaded_for_selected = false;
                        prev_selected_item_index = -1;

                        Client temp_full_client_refresh;
                        if (fetch_client_by_id(client_id_action, &temp_full_client_refresh)) {
                            strncpy(items[selected_item_index].business_name, temp_full_client_refresh.business_name, MAX_STR_LEN - 1);
                            items[selected_item_index].business_name[MAX_STR_LEN - 1] = '\0';
                        }
                    } else if (action_type == INTERACTIVE_LIST_ACTION_DELETE) {
                        if (key == KEY_EDIT_CLIENT || key == KEY_EDIT_CLIENT_ALT) {beep(); break;}
                        char confirm_prompt[MAX_STR_LEN + 50];
                        snprintf(confirm_prompt, sizeof(confirm_prompt), "Delete '%s' (ID:%d)? (Y/N): ", client_name_action, client_id_action);
                        mvwprintw(input_win, INPUT_PROMPT_Y, INPUT_PROMPT_X, "%.*s", getmaxx(input_win) - 2 - INPUT_PROMPT_X, confirm_prompt);
                        wrefresh(input_win);
                        int confirm_key = wgetch(input_win);
                        werase(input_win); draw_custom_box(input_win); wrefresh(input_win);

                        if (toupper(confirm_key) == 'Y') {
                            show_loading_indicator(true);
                            if (db_delete_client(client_id_action)) {
                                show_status("Customer '%s' (ID: %d) deleted.", client_name_action, client_id_action);
                                if (selected_item_index < total_items - 1) {
                                    memmove(&items[selected_item_index], &items[selected_item_index + 1], (total_items - 1 - selected_item_index) * sizeof(ClientListItem));
                                }
                                total_items--;

                                if (total_items == 0) {
                                    selected_item_index = 0;
                                } else if (selected_item_index >= total_items) {
                                    selected_item_index = total_items - 1;
                                }
                                prev_selected_item_index = -1;
                                details_loaded_for_selected = false;
                            }
                            show_loading_indicator(false);
                        } else {
                            show_status("Deletion of '%s' cancelled.", client_name_action);
                        }
                        napms(1500);
                    }
                } else beep();
                break;

            case KEY_ACTION_QUIT: case KEY_ACTION_QUIT_ALT: case KEY_ESC:
                free(items);
                werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
                return;

            case KEY_RESIZE:
                details_loaded_for_selected = false;
                prev_selected_item_index = -1;
                break;
            default: beep(); break;
        }

        if (total_items > 0) {
            if (selected_item_index < 0) selected_item_index = 0;
            if (selected_item_index >= total_items) selected_item_index = total_items - 1;

            if (top_item_index > selected_item_index) top_item_index = selected_item_index;
            if (top_item_index < 0) top_item_index = 0;

            if (selected_item_index >= top_item_index + items_per_page_nav) {
                 top_item_index = selected_item_index - items_per_page_nav + 1;
            }
            if (selected_item_index < top_item_index) {
                top_item_index = selected_item_index;
            }
            if (top_item_index > 0 && top_item_index + items_per_page_nav > total_items) {
                 top_item_index = total_items - items_per_page_nav;
                 if (top_item_index < 0) top_item_index = 0;
            }
        } else {
            selected_item_index = 0;
            top_item_index = 0;
            prev_selected_item_index = -1;
            details_loaded_for_selected = false;
        }
    }

    free(items);
    werase(input_win); draw_custom_box(input_win); wrefresh(input_win);
}


// --- Other Functions ---
void execute_gextux_crm() {
    show_status("Exiting editor and attempting to launch gextux_crm...");
    wrefresh(status_win); napms(1000);

    close_db();
    cleanup_ncurses();

    execlp("gextux_crm", "gextux_crm", (char *)NULL);

    perror("execlp failed to launch gextux_crm");

    init_ncurses();
    show_error("Failed to launch gextux_crm. Check console for details.");
    napms(3000);
    cleanup_ncurses();

    fprintf(stderr, "Error: Could not launch 'gextux_crm'.\n");
    fprintf(stderr, "Please ensure 'gextux_crm' is in your PATH and executable.\n");
    exit(EXIT_FAILURE);
}

// --- Callbacks & Helpers ---
int fetch_list_items_callback(void *data, int argc, char **argv, char **azColName) {
    (void)azColName;
    FetchListData *list_data = (FetchListData*)data;

    if (argc < 6) {
        fprintf(stderr, "Callback Error: Expected at least 6 columns for list item, got %d.\n", argc);
        return SQLITE_ABORT;
    }

    if (*list_data->count_ptr >= *list_data->capacity_ptr) {
        int new_capacity = (*list_data->capacity_ptr == 0) ? 50 : *list_data->capacity_ptr * 2;
        ClientListItem *new_items = realloc(*list_data->items_ptr, new_capacity * sizeof(ClientListItem));
        if (!new_items) {
            fprintf(stderr, "Failed to reallocate memory for customers list items.\n");
            return SQLITE_ABORT;
        }
        *list_data->items_ptr = new_items;
        *list_data->capacity_ptr = new_capacity;
    }

    ClientListItem *current_item = &((*list_data->items_ptr)[*list_data->count_ptr]);
    memset(current_item, 0, sizeof(ClientListItem));

    current_item->id = argv[0] ? atoi(argv[0]) : 0;
    if (argv[1]) strncpy(current_item->business_name, argv[1], MAX_STR_LEN - 1); else strcpy(current_item->business_name, "N/A");
    if (argv[2]) strncpy(current_item->city,          argv[2], MAX_STR_LEN - 1); else strcpy(current_item->city, "N/A");
    if (argv[3]) strncpy(current_item->phone,         argv[3], MAX_STR_LEN - 1); else strcpy(current_item->phone, "N/A");
    if (argv[4]) strncpy(current_item->email,         argv[4], MAX_STR_LEN - 1); else strcpy(current_item->email, "N/A");
    if (argv[5]) strncpy(current_item->contact_person,argv[5], MAX_STR_LEN - 1); else strcpy(current_item->contact_person, "N/A");
    current_item->business_name[MAX_STR_LEN -1] = '\0';
    current_item->city[MAX_STR_LEN -1] = '\0';
    current_item->phone[MAX_STR_LEN -1] = '\0';
    current_item->email[MAX_STR_LEN -1] = '\0';
    current_item->contact_person[MAX_STR_LEN -1] = '\0';

    (*list_data->count_ptr)++;
    return 0;
}

// --- Main Function ---
int main(int argc, char *argv[]) {
    strncpy(db_path, DEFAULT_DB_NAME, sizeof(db_path) - 1);
    db_path[sizeof(db_path) - 1] = '\0';

    int opt;
    while ((opt = getopt(argc, argv, "d:h")) != -1) {
        switch (opt) {
            case 'd':
                strncpy(db_path, optarg, sizeof(db_path) - 1);
                db_path[sizeof(db_path) - 1] = '\0';
                break;
            case 'h':
                printf("GexTuX Customer Editor\n");
                printf("Usage: %s [-d database_file]\n", argv[0]);
                printf("  -d database_file: Specify the SQLite database file to use.\n");
                printf("                    Default: %s\n", DEFAULT_DB_NAME);
                printf("  -h: Display this help message and exit.\n");
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-d database_file]\n", argv[0]);
                return 1;
        }
    }

    init_ncurses();

    if (!init_db(db_path)) {
        napms(2000);
        cleanup_ncurses();
        fprintf(stderr, "Failed to initialize database '%s'. Exiting.\n", db_path);
        return 1;
    }
    if (!db) {
        if (status_win) show_error("Database handle is NULL after init. Critical error.");
        else fprintf(stderr, "Database handle is NULL after init. Critical error.\n");
        napms(2000);
        cleanup_ncurses();
        fprintf(stderr, "Database handle is NULL after successful initialization. Critical error. Exiting.\n");
        return 1;
    }

    display_editor_main_menu();

    close_db();
    cleanup_ncurses();

    if (exit_requested) {
        printf("GexTuX Customers Editor terminated by signal.\n");
    } else {
        printf("GexTuX Customers Editor terminated normally.\n");
    }
    return 0;
}