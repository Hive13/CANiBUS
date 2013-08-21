#include <Elementary.h>

static void
on_done(void *data, Evas_Object *obj, void *event_info)
{
   /* quit the mainloop (elm_run) */
   elm_exit();
}

EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *bg, *box, *lab, *btn;

   /* new window - do the usual and give it a name, title and delete handler */
   win = elm_win_add(NULL, "hello", ELM_WIN_BASIC);
   elm_win_title_set(win, "Hello");
   /* when the user clicks "close" on a window there is a request to delete */
   evas_object_smart_callback_add(win, "delete,request", on_done, NULL);

   /* add a standard bg */
   bg = elm_bg_add(win);
   /* not allow bg to expand. let's limit dialog size to contents */
   evas_object_size_hint_weight_set(bg, 0.0, 0.0);
   /* add object as a resize object for the window (controls window minimum
    * size as well as gets resized if window is resized) */
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   /* add a box object - default is vertical. a box holds children in a row,
    * either horizontally or vertically. nothing more. */
   box = elm_box_add(win);
   /* make the box hotizontal */
   elm_box_horizontal_set(box, EINA_TRUE);
   /* not not allow box to expand. let's limit dialog size to contents */
   evas_object_size_hint_weight_set(box, 0.0, 0.0);
   /* add object as a resize object for the window (controls window minimum
    * size as well as gets resized if window is resized) */
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   /* add a label widget, set the text and put it in the pad frame */
   lab = elm_label_add(win);
   /* set text of the label */
   elm_label_label_set(lab, "Hello out there world!");
   /* do not allow label to expand */
   evas_object_size_hint_weight_set(lab, 0.0, 0.0);
   /* pack the label at the end of the box */
   elm_box_pack_end(box, lab);
   evas_object_show(lab);

   /* add an ok button */
   btn = elm_button_add(win);
   /* se label for button to "OK" */
   elm_button_label_set(btn, "OK");
   /* do not allow button to expand */
   evas_object_size_hint_weight_set(btn, 0.0, 0.0);
   /* pack the button at the end of the box */
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   /* call on_don when button is clicked */
   evas_object_smart_callback_add(btn, "clicked", on_done, NULL);

   /* now we are done, show the window */
   evas_object_show(win);

   /* run the mainloop and process events and callbacks */
   elm_run();
   return 0;
}
ELM_MAIN()
