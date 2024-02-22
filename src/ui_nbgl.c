#ifdef HAVE_NBGL
#include "bolos_target.h"

#include "ui.h"

#include "baking_auth.h"
#include "exception.h"
#include "globals.h"
#include "glyphs.h"  // ui-menu
#include "keys.h"
#include "memory.h"
#include "os_cx.h"  // ui-menu
#include "to_string.h"

#include <stdbool.h>
#include <string.h>

#include "nbgl_use_case.h"

static const char* const infoTypes[] = {"Version", "Developer", "Copyright"};
static const char* const infoContents[] = {VERSION, "Ledger", "(c) 2023 Ledger"};

void ui_initial_screen(void) {
    ux_idle_screen(NULL, NULL);
}

#ifdef BAKING_APP
#define MAX_LENGTH 200
static char* bakeInfoContents[3];
static char buffer[3][MAX_LENGTH];

static const char* const bakeInfoTypes[] = {
    "Chain",
    "Public Key Hash",
    "High Watermark",
};

void update_baking_idle_screens(void) {
}

static bool navigation_cb_baking(uint8_t page, nbgl_pageContent_t* content) {
    UNUSED(page);

    bakeInfoContents[0] = buffer[0];
    bakeInfoContents[1] = buffer[1];
    bakeInfoContents[2] = buffer[2];

    copy_chain(buffer[0], sizeof(buffer[0]), &N_data.main_chain_id);
    copy_key(buffer[1], sizeof(buffer[1]), &N_data.baking_key);
    copy_hwm(buffer[2], sizeof(buffer[2]), &N_data.hwm.main);

    if (page == 0) {
        content->type = INFOS_LIST;
        content->infosList.nbInfos = 3;
        content->infosList.infoTypes = bakeInfoTypes;
        content->infosList.infoContents = bakeInfoContents;
    } else {
        content->type = INFOS_LIST;
        content->infosList.nbInfos = 3;
        content->infosList.infoTypes = infoTypes;
        content->infosList.infoContents = infoContents;
    }

    return true;
}

void ui_menu_about_baking(void) {
    nbgl_useCaseSettings("Tezos baking",
                         0,
                         2,
                         false,
                         ui_initial_screen,
                         navigation_cb_baking,
                         NULL);
}
#endif

void ux_idle_screen(ui_callback_t ok_c, ui_callback_t cxl_c) {
    (void) ok_c;
    (void) cxl_c;

#ifdef BAKING_APP
    nbgl_useCaseHome("Tezos Baking", &C_tezos, NULL, false, ui_menu_about_baking, exit_app);
#endif
}

#endif  // HAVE_NBGL
