#include <Elementary.h>

static void
on_done(void *data, Evas_Object *obj, void *event_info)
{
   // quit the mainloop (elm_run function will return)
   elm_exit();
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *vertBox;
   Evas_Object *serverLabel, *serverEntry, *serverBox;
   Evas_Object *handleLabel, *handleEntry, *handleBox;
   Evas_Object *loginBtn, *quitBtn, *buttonBox;

   // new window - do the usual and give it a name (hello) and title (Hello)
   win = elm_win_util_standard_add("eCANiBUS", "eCANiBUS");
   // when the user clicks "close" on a window there is a request to delete
   evas_object_smart_callback_add(win, "delete,request", on_done, NULL);

   vertBox = elm_box_add(win);
   elm_win_resize_object_add(win, vertBox);
   evas_object_size_hint_weight_set(vertBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vertBox);

   // add a box object - default is vertical. a box holds children in a row,
   // either horizontally or vertically. nothing more.
   serverBox = elm_box_add(win);
   // make the box horizontal
   elm_box_horizontal_set(serverBox, EINA_TRUE);
   elm_box_pack_end(vertBox, serverBox);
   evas_object_show(serverBox);

   // add a label widget, set the text and put it in the pad frame
   serverLabel = elm_label_add(win);
   // set default text of the label
   elm_object_text_set(serverLabel, "Server");
   // pack the label at the end of the box
   elm_box_pack_end(serverBox, serverLabel);
   evas_object_show(serverLabel);
   serverEntry = elm_entry_add(win);
   evas_object_size_hint_weight_set(serverEntry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(serverEntry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(serverBox, serverEntry);
   evas_object_show(serverEntry);

   handleBox = elm_box_add(win);
   // make the box horizontal
   elm_box_horizontal_set(handleBox, EINA_TRUE);
   // add object as a resize object for the window (controls window minimum
   // size as well as gets resized if window is resized)
   elm_box_pack_end(vertBox, handleBox);
   evas_object_show(handleBox);
   handleLabel = elm_label_add(win);
   elm_object_text_set(handleLabel, "Username");
   elm_box_pack_end(handleBox, handleLabel);
   evas_object_show(handleLabel);
   handleEntry = elm_entry_add(win);
   elm_box_pack_end(handleBox, handleEntry);
   evas_object_show(handleEntry);

   buttonBox = elm_box_add(win);
   elm_box_horizontal_set(buttonBox, EINA_TRUE);
   elm_box_pack_end(vertBox, buttonBox);
   evas_object_show(buttonBox);

   // add an ok button
   quitBtn = elm_button_add(win);
   // set default text of button to "OK"
   elm_object_text_set(quitBtn, "Quit");
   // pack the button at the end of the box
   elm_box_pack_end(buttonBox, quitBtn);
   evas_object_show(quitBtn);
   // call on_done when button is clicked
   evas_object_smart_callback_add(quitBtn, "clicked", on_done, NULL);

   loginBtn = elm_button_add(win);
   elm_object_text_set(loginBtn, "Login");
   elm_box_pack_end(buttonBox, loginBtn);
   evas_object_show(loginBtn);

   // now we are done, show the window
   evas_object_show(win);

   evas_object_resize(win, 300, 300);
   // run the mainloop and process events and callbacks
   elm_run();
   elm_shutdown();
   return 0;
}
ELM_MAIN()
