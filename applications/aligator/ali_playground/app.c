#include <furi.h>
#include <stdio.h>
#include <gui/gui.h>

/**
 * This function is for drawing the screen GUI, everytime
 * the Flip.x0 refreshes the display.
 */
static void draw_callback(Canvas* canvas, void* _ctx) {
    UNUSED(_ctx);

    canvas_clear(canvas);

    // Set the font, the position and the content of the label
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 32, 14, "Hello, World!");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 15, 40, " press back to exit FAP");

    canvas_draw_line(canvas, 2, 23, 126, 23);
}

/**
 * This function is an handler for the user input (the buttons on the ring
 * of the Flip.x0 used for navigation, confirm and back).
 */
static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

/**
 * @brief      Main function for alis playground app.
 * @details    This function is the entry point for the playground application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t ali_playground(void* _p) {
    UNUSED(_p);

    // Current event of type InputEvent
    InputEvent event;

    // Event queue for 88 elements of size InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // ViewPort is needed to draw the GUI
    ViewPort* view_port = view_port_alloc();

    // we give to this [view_port_draw_callback_set] the function defined
    // before (draw_callback) for drawing the GUI on the Flip.x0 display.
    view_port_draw_callback_set(view_port, draw_callback, NULL);

    // The same concept for the input
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // You need to create a GUI structure and associate it to the viewport
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Infinite loop for the app
    while(true) {
        // get events
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // If the event is a back button press, we break the loop
        if(event.key == InputKeyBack) {
            break;
        }
    }

    // free resources
    furi_message_queue_free(event_queue);

    // We remove the gui from the associated view port
    gui_remove_view_port(gui, view_port);

    // Freeing up memory removing the view_port and close
    // the GUI record
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}
