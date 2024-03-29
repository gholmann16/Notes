#define DEFAULT_FONT "Sans"
#define DEFAULT_FONTSIZE 10
#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512
#define DEFAULT_WRAP TRUE
#define DEFAULT_SYNTAX FALSE

// Initiate i18n and icon
void init_app(GtkWindow * window);

// Initiate preferences from janusrc
void init_preferences(struct Document * document);

// Initiates the menu bar
void init_menu(GtkWidget * bar, GtkAccelGroup * accel, struct Document * document);
