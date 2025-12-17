/*
 * Copyright (C) 2025 Daniel McCarthy <daniel@dragonzap.com>
 * Part of the PeachOS Part Two Development Series.
 * https://github.com/nibblebits/PeachOS64BitCourse
 * https://github.com/nibblebits/PeachOS64BitModuleTwo
 * Licensed under the GNU General Public License version 2 (GPLv2).
 *
 * Community contributors to this source file:
 * NONE AS OF YET
 * ----------------
 * Disclaimer: Contributors are hobbyists that contributed to the public source code, they are not affiliated or endorsed by Daniel McCarthy the author of the PeachOS Kernel      
 * development video series. Contributors did not contribute to the video content or the teaching and have no intellectual property rights over the video content for the course video * material and did not contribute to the video material in anyway.
 */

#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "window.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#include "graphics.h"
#include "gui/gui.h"
#include "gui/element.h"
#include "gui/image_element.h"
#include "gui/plane.h"
#include "gui/button.h"
#include "gui/textfield.h"
#include "gui/listarea.h"
#include "gui/property.h"
#include "image.h"
#include "delay.h"
#include "gui/exampleproperty.h"

#include "file.h"

enum
{
    CALCULATOR_PARSING_LEFT_NUMBER = 0,
    CALCULATOR_PARSING_RIGHT_NUMBER = 1,
};

#define RESULT_TEXTFIELD 1000
#define BUTTON_DIVIDE 1001
#define BUTTON_MULTIPLY 1002
#define BUTTON_ADD 1003
#define BUTTON_SUBTRACT 1004
#define BUTTON_EQUALS 1005

#define BUTTON_7 7
#define BUTTON_8 8
#define BUTTON_9 9
#define BUTTON_4 4
#define BUTTON_5 5
#define BUTTON_6 6
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define BUTTON_0 0

typedef int BUTTON_ID;

// We shall loop through the array and create buttons.
BUTTON_ID button_ids[] = {BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_0};

struct gui_element *result_textfield = NULL;

GUI_EVENT_HANDLER_RESPONSE numerical_button_event_handler(struct gui_event *gui_event)
{
    if (gui_event->type != GUI_EVENT_TYPE_ELEMENT_CLICKED)
    {
        // We dont care for non-click events
        return GUI_EVENT_HANDLER_RESPONSE_IGNORED;
    }
    // Alright there was a button click which one was it
    BUTTON_ID btn_id = gui_event->element.id;
    // We have the button id we shall use this as the value
    // to add to the textfield
    // Whats the current text for the textfield
    char c = *itoa(btn_id);
    gui_element_textfield_put_char(result_textfield, c);

    // Now we have the new text lets set the textfield again
    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}

void calculator_add()
{
    gui_element_textfield_put_char(result_textfield, '+');
}

void calculator_subtract()
{
    gui_element_textfield_put_char(result_textfield, '-');
}

void calculator_divide()
{
    gui_element_textfield_put_char(result_textfield, '/');
}

void calculator_multiply()
{
    gui_element_textfield_put_char(result_textfield, '*');
}

bool calculator_is_operator(char c)
{
    return c == '+' || c == '-' || c == '/' || c == '*';
}

int calculator_do_math(int left, int right, char op)
{
    int result = 0;
    switch (op)
    {
    case '+':
        result = left + right;
        break;
    case '-':
        result = left - right;
        break;

    case '*':
        result = left * right;
        break;

    case '/':
        if (right == 0)
        {
            // divide by zero is illegal
            result = 0;
            break;
        }
        result = left / right;
        break;
    }

    return result;
}

void calculator_equals()
{
    // This is a special function we have to parse the entire
    // textfield and then preform mathematical operations on it
    // to keep things simple we wont obay BODMAS or other
    // mathematical rules yet
    // this application serves as a demonstration
    // that we have full GUI support for the most part
    // with all the elements ready and implemented.

    const char *result_text = gui_element_textfield_text(result_textfield);
    const char *current_c = result_text;
    int parse_state = CALCULATOR_PARSING_LEFT_NUMBER;
    char number_str[124] = {0};
    int number_str_index = 0;
    int left_number_int = 0;
    char current_op = 0;
    int result = 0;

    while (*current_c)
    {
        // Do we have a new operator or are we about to finish
        // our expression
        if (calculator_is_operator(*current_c))
        {
            if (parse_state == CALCULATOR_PARSING_RIGHT_NUMBER)
            {
                // We finished a particular expression
                // we can atoi and add then reset.
                int right_number_int = atoi(number_str);
                // What is the operator we need to deal with
                result = calculator_do_math(left_number_int, right_number_int, current_op);

                // Incase theres nested expressions left number becomes result
                // i.e 50+20+30  (50+20=70) 70+(FUTURE RIGHT NO)
                left_number_int = result;
                right_number_int = 0;

                // Parse state shall never default back to the left number
                // why because expressions are continious
                // i.e 5 + 5 + 10 + 20 + 30, we always pay attention
                // to the left operand, therefore we shall never
                // switch back to the left operand as its coninuious in our
                // implementation
            }
            else
            {
                left_number_int = atoi(number_str);
                parse_state = CALCULATOR_PARSING_RIGHT_NUMBER;
            }

            current_op = *current_c;
            current_c++;
            memset(number_str, 0x00, sizeof(number_str));
            number_str_index = 0;
            continue;
        }
        // We have a number, so lets push it straight
        // into the array
        number_str[number_str_index] = *current_c;
        current_c++;
        number_str_index++;
    }

    // If the current_op isnt null then we have an unfinished expression
    // that we have to deal with
    if (current_op != 0)
    {
        int right_number_int = atoi(number_str);
        result = calculator_do_math(left_number_int, right_number_int, current_op);
    }
    else
    {
        // maybe they entered 5000 without ever using an operator.
        result = atoi(number_str);
    }

    // We have the full result, lets now
    // set the textfield to that result
    const char *result_str = (const char *)itoa(result);
    gui_element_textfield_text_set(result_textfield, result_str);
}

GUI_EVENT_HANDLER_RESPONSE mathimatical_operation_button_event_handler(struct gui_event *gui_event)
{
    // We wont deal with non click events..
    if (gui_event->type != GUI_EVENT_TYPE_ELEMENT_CLICKED)
        return GUI_EVENT_HANDLER_RESPONSE_IGNORED;

    BUTTON_ID btn_id = gui_event->element.id;
    switch (btn_id)
    {
    case BUTTON_ADD:
        calculator_add();
        break;

    case BUTTON_SUBTRACT:
        calculator_subtract();
        break;

    case BUTTON_MULTIPLY:
        calculator_multiply();
        break;

    case BUTTON_DIVIDE:
        calculator_divide();
        break;

    case BUTTON_EQUALS:
        calculator_equals();
        break;
    }

    return GUI_EVENT_HANDLER_RESPONSE_PROCESSED_CONTINUE_WITH_CHILDREN;
}

static int blank_listarea_property_attached(struct gui_element* element)
{
    // No setup required for now.
    (void)element;
    return 0;
}

static int blank_listarea_property_event(struct gui_element* element, struct gui_event* event)
{
    (void)element;
    if (event->type == GUI_EVENT_TYPE_LISTAREA_ITEM_SELECTED)
    {
        printf("The element %i was selected\n", (int)event->data.listarea_item_selected.selected_index);
    }
    return 0;
}

static struct gui_element_property blank_listarea_property = {
    .listeners.click = 0,
    .listeners.move = 0,
    .listeners.attached = blank_listarea_property_attached,
    .listeners.unattached = 0,
    .listeners.event = blank_listarea_property_event};

int main(int argc, char **argv)
{
    struct window *main_win = window_main();
    if (!main_win)
    {
        return -1;
    }

    // Update the window title bar to calculator.
    window_title_set(main_win, "List area test");
    struct gui *gui = gui_bind_to_window(main_win, NULL);
    if (!gui)
    {
        printf("GUI initilization issue\n");
        return -1;
    }

    struct gui_element* list_element = gui_element_listarea_create(gui, NULL, 10, 10, 100, 200, -1);
    if (!list_element)
    {
        printf("List element creation failed\n");
        return -1;
    }

    gui_element_listarea_list_add(list_element, "Element 1", NULL);
    gui_element_listarea_list_add(list_element, "Element 2", NULL);

    // now we can capture whatever...
    if (gui_element_property_attach(list_element, &blank_listarea_property) < 0)
    {
        printf("Listarea listener property attach failed\n");
    }
    gui_element_property_example_property_attach(list_element);


    // DONT LEAVE!
    while (gui_process(gui) >= 0)
    {
        // future sleep here...
    }

    return 0;
}
