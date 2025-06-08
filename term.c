#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib.h>

// Function to handle window close event
void on_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// Copy text function
void copy_text(GtkMenuItem *menuitem, VteTerminal *terminal) {
    vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
}

// Paste text function
void paste_text(GtkMenuItem *menuitem, VteTerminal *terminal) {
    vte_terminal_paste_clipboard(terminal);
}

// Right-click menu for copy/paste
gboolean on_right_click(VteTerminal *terminal, GdkEventButton *event) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        GtkWidget *menu = gtk_menu_new();
        GtkWidget *copy_item = gtk_menu_item_new_with_label("Copy");
        GtkWidget *paste_item = gtk_menu_item_new_with_label("Paste");

        gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste_item);

        g_signal_connect(copy_item, "activate", G_CALLBACK(copy_text), terminal);
        g_signal_connect(paste_item, "activate", G_CALLBACK(paste_text), terminal);

        gtk_widget_show_all(menu);
        gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *) event);
        return TRUE;
    }
    return FALSE;
}
// Function to load settings from config file
void load_config(VteTerminal *terminal) {
    GKeyFile *key_file = g_key_file_new();
    GError *error = NULL;

    // Load config file from ~/.config/gxmct/config.ini
    gchar *config_path = g_build_filename(g_get_user_config_dir(), "gxmct/config.ini", NULL);
    if (!g_key_file_load_from_file(key_file, config_path, G_KEY_FILE_NONE, &error)) {
        g_print("Failed to load config file: %s\n", error->message);
        g_clear_error(&error);
    } else {
        // Load font
        gchar *font_name = g_key_file_get_string(key_file, "Appearance", "font", NULL);
        if (font_name) {
            PangoFontDescription *font_desc = pango_font_description_from_string(font_name);
            vte_terminal_set_font(terminal, font_desc);
            pango_font_description_free(font_desc);
            g_print("Font set to: %s\n", font_name);
            g_free(font_name);
        }

        // Load background color
        gchar *bg_color_str = g_key_file_get_string(key_file, "Appearance", "background", NULL);
        if (bg_color_str) {
            GdkRGBA bg_color;
            if (gdk_rgba_parse(&bg_color, bg_color_str)) {
                vte_terminal_set_color_background(terminal, &bg_color);
                g_print("Background color set to: %s\n", bg_color_str);
            } else {
                g_print("Invalid background color format: %s\n", bg_color_str);
            }
            g_free(bg_color_str);
        }

        // Load transparency
        gdouble transparency = g_key_file_get_double(key_file, "Appearance", "transparency", NULL);
        if (transparency < 0.0 || transparency > 1.0) {
            transparency = 1.0; // Default to fully opaque if invalid
        }
        GdkRGBA bg_color = {0, 0, 0, transparency}; // RGBA with alpha for transparency
        vte_terminal_set_color_background(terminal, &bg_color);
        g_print("Transparency set to: %.2f\n", transparency);
    }

    g_key_file_free(key_file);
    g_free(config_path);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gxmct");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    // Create box layout
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create terminal widget
    VteTerminal *terminal = VTE_TERMINAL(vte_terminal_new());

    // Set terminal expansion
    gtk_widget_set_hexpand(GTK_WIDGET(terminal), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(terminal), TRUE);

    // Pack terminal into box
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(terminal), TRUE, TRUE, 0);

    // Load configuration settings
    load_config(terminal);

    // Connect right-click event for context menu
    g_signal_connect(terminal, "button-press-event", G_CALLBACK(on_right_click), terminal);

    // Start a shell in the terminal
    char *argv_shell[] = {g_strdup(g_getenv("SHELL")), NULL};
    vte_terminal_spawn_async(terminal, VTE_PTY_DEFAULT, NULL, argv_shell, NULL, 0, NULL, NULL, NULL, -1, NULL, NULL, NULL);

    // Show all widgets and start GTK loop
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
