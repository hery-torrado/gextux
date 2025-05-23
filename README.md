# GexTuX: A Lightweight Command-Line Customer Database with an Authentic 1980s Interface

**GexTuX Customer Editor** is a terminal-based (TUI) application for managing a customer's database. It is designed with a retro-futuristic aesthetic and uses SQLite for data storage. This editor is a component of a future larger "GexTuX CRM" system.

**Version:** 1.0 (May 2025)

## Features

*   **Customer Data Management:**
    *   Add new customer records.
    *   Search for customers by ID, name, contact, email, or city.
    *   View detailed customer information.
    *   Edit existing customer records.
    *   Delete customer records.
*   **Retro-Futuristic Terminal Interface:**
    *   Uses custom box-drawing characters and symbols for a unique look and feel.
    *   Color-coded UI elements (if supported by the terminal).
    *   Status bar displaying program title, current time, and messages/errors.
    *   Loading indicator for database operations.
*   **Interactive List & Detail Panes:**
    *   When searching/viewing customers, a two-pane layout is used:
        *   Left pane: Scrollable list of matching customers.
        *   Right pane: Detailed information of the selected customer.
*   **SQLite Backend:**
    *   All customer data is stored in an SQLite database file (default: `gextux.db`).
    *   The database schema is automatically created and can be upgraded if necessary (e.g., adding new columns).
*   **UTF-8 Support:**
    *   Supports non-ASCII characters in data and UI elements, provided the terminal is configured correctly.
*   **Robust Input:**
    *   Input fields show current values when editing.
    *   Specific input required for mandatory fields (e.g., Business Name).
    *   Option to cancel individual field inputs or abort an entire form.
*   **Integration:**
    *   Option to exit the editor and return to a main "gextux_crm" program.
*   **Responsive UI:**
    *   Handles terminal window resizing.

## Customer Data Fields

The following information can be stored for each customer:

*   ID (Automatically generated)
*   Business Name (Required, Unique)
*   Email
*   Phone
*   Website
*   Street Address
*   City
*   State/Province
*   Zip/Postal Code
*   Country
*   Tax Number
*   Number of Employees
*   Industry
*   Contact Person Name
*   Contact Person Email
*   Contact Person Phone
*   Status (Active, Inactive, Prospect, Lead, Former)
*   Notes (Multi-line)
*   Created At (Automatically timestamped)

## Dependencies

To compile and run GexTuX Customer Editor, you will need:

*   A C compiler (e.g., GCC, Clang).
*   **ncursesw** library (ncurses with wide character support for UTF-8).
*   **SQLite3** library (version 3.x).
*   A terminal emulator that supports UTF-8 and has a font that includes the special characters used for the UI (see "UTF-8 Support" section).

## Compilation

1.  Ensure you have the development packages for `ncursesw` and `sqlite3` installed.
    *   On Debian/Ubuntu: `sudo apt-get install libncursesw5-dev libsqlite3-dev`
    *   On Fedora: `sudo dnf install ncurses-devel sqlite-devel`
    *   On macOS (using Homebrew): `brew install ncurses sqlite`

2.  Compile the `gextux_customer_editor.c` file (assuming the source code is in this file) using a C compiler. Link against the `ncursesw` and `sqlite3` libraries. The program also uses POSIX/XSI extensions, so appropriate feature test macros are beneficial.

    ```bash
    gcc gextux_customer_editor.c -o gextux_customer_editor -lncursesw -lsqlite3 -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE_EXTENDED=1
    ```
    *(Note: The necessary `#define`s are already in the C source file, but explicitly including them in the compile command can be good practice or help if there are system differences.)*

## Installation

After successful compilation, an executable file named `gextux_customer_editor` will be created. No special installation is required. You can copy
this executable to a directory in your system's `PATH` (e.g., `/usr/local/bin`) for easier access.

## Usage

Run the editor from your terminal:

```bash
./gextux_customer_editor [options]

Command-line Options:

-d <database_file>: Specify the SQLite database file to use.

Example: ./gextux_customer_editor -d my_customers.db

Default: gextux.db (created in the current directory if it doesn't exist).

-h: Display a help message and exit.

Keybindings

General:

Q (or q): Quit the application (from main menu or list view).

ESC: Go back to the previous menu/screen, or cancel the current operation (e.g., field input).

Arrow Keys (↑, ↓, ←, →): Navigate menus, lists, and selection prompts.

Enter: Select an option or confirm input.

Main Menu:

1, 2, 3, 4: Directly select menu options.

Input Fields (Add/Edit Customer):

Type text for the field.

Enter:

If text was entered: Confirm the input.

If field was left empty (and editing): Keep the current value.

If field was left empty (and adding new, if allowed): Accept empty value.

ESC: Cancel input for the current field (reverts to original value if editing, or clears if adding).

!!CANCEL!! (typed exactly and then Enter): Abort the entire Add New Customer or Edit Customer form.

Customer Status Selection:

← / →: Navigate between status options.

Enter: Select the highlighted status.

ESC: Cancel selection and keep the current/default status.

Interactive Customer List (Search/Edit/Delete Screens):

↑ / ↓: Move selection up/down in the customer list.

Page Up / Page Down: Scroll through the list by a page.

Home: Go to the first customer in the list.

End: Go to the last customer in the list.

Enter:

If in "Edit/Search" mode: Open the selected customer for editing.

If in "Delete" mode: Prompt for deletion of the selected customer.

E (or e): (In "Edit/Search" mode) Open the selected customer for editing.

D (or d): (In "Delete" mode) Prompt for deletion of the selected customer.

Q (or q) / ESC: Return to the main menu.

Database

File: By default, the application uses gextux.db in the directory from which it's launched. This can be changed with the -d option.
Schema: The clients table stores customer information. Key fields include:
id (INTEGER, PRIMARY KEY, AUTOINCREMENT)
business_name (TEXT, NOT NULL, UNIQUE, COLLATE NOCASE)
email, phone, website, street, city, state, zip_code, country, tax_number, industry, contact_person, contact_email, contact_phone, notes(all TEXT)
num_employees (INTEGER)
status (TEXT, with CHECK constraint: 'Active', 'Inactive', 'Prospect', 'Lead', 'Former')
created_at (DATETIME, DEFAULT CURRENT_TIMESTAMP)

The application will attempt to create this table if it doesn't exist and add tax_number and zip_code columns if they are missing from an older
schema.

UTF-8 Support

The application uses UTF-8 characters for its retro-futuristic UI elements (borders, prompts, etc.). For these to display correctly:

Terminal Emulator: Your terminal emulator (e.g., xterm, gnome-terminal, iTerm2, Windows Terminal) must be configured to use UTF-8 encoding.

Font: You need a font installed and selected in your terminal that includes the special characters used, such as:
║ ═ ╔ ╗ ╚ ╝ ━ ┃ ➔ » ▐ ▌ « ⢿

Many modern "Nerd Fonts" or other comprehensive Unicode fonts will work.

Locale: Your system locale should be set to one that supports UTF-8 (e.g., en_US.UTF-8). The program attempts to set the locale via
setlocale(LC_ALL, "").

ncursesw: The program must be compiled against ncursesw (wide character support) as instructed in the "Compilation" section.

If characters appear as question marks, boxes, or are misaligned, check your terminal and font settings.

Returning to GexTuX CRM

The main menu includes an option "4. Return to Main Program". Selecting this will attempt to close the editor and execute
a program named gextux_crm. For this to work, gextux_crm must be an executable program found in your system's PATH.
Troubleshooting. Note that gextux_crm is not already made.

Incorrect Character Display: See the "UTF-8 Support" section.

Database Errors: Ensure you have write permissions in the directory where the database file is being created/accessed. Check SQLite3
error messages displayed in the status bar.

"Failed to launch gextux_crm": This means the gextux_crm executable could not be found or executed when trying to return to
the main program. Ensure it's installed and in your PATH.

wcwidth warnings during compilation or runtime: The code includes macros (_GNU_SOURCE, etc.) intended to help wcwidth function
correctly with UTF-8. If issues persist, it might be related to specific terminal or library versions. The program should generally still function.


