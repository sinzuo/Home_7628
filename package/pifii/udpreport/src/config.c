/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"

#define FREE(x) do { free(x); x = NULL; } while (0);

static bool first_run = true;
static struct uci_context *uci_ctx;
static struct uci_package *uci_relayroam;

struct core_config *config;

/*
static void config_free_roam(void) {
	if (config->roam) {
		FREE(config->roam->enable);
		FREE(config->roam->route_sta_th);
		FREE(config->roam->route_pro_th);
		FREE(config->roam->relay_sta_th);
		FREE(config->roam->relay_pro_th);
	}
}
*/

static int config_init_roam(void)
{
	struct uci_section *s;
	struct uci_element *e1, *e2;

	uci_foreach_element(&uci_relayroam->sections, e1) {
		s = uci_to_section(e1);
		if (strcmp(s->type, "roam") == 0)
			goto section_found;
	}
	debug("uci section roam not found...");
	return -1;

section_found:
	//config_free_roam();

	uci_foreach_element(&s->options, e1) {
		if (!strcmp((uci_to_option(e1))->e.name, "enable")) {
			//config->roam->enable = strdup(uci_to_option(e1)->v.string);
			config->roam->enable = (unsigned char)atoi(uci_to_option(e1)->v.string);
			debug("enable=%d", config->roam->enable);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "routeStaTh")) {
			if (0 <= atoi((uci_to_option(e1))->v.string)) {
				debug("in section routeStaTh has invalid value,use default value:-70");
			        config->roam->route_sta_th = (unsigned char)-70;
				return -1;
			}
			config->roam->route_sta_th = (unsigned char)atoi(uci_to_option(e1)->v.string);
			debug("routeStaTh=%d", (char)config->roam->route_sta_th);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "routeProTh")) {
			if (0 <= atoi((uci_to_option(e1))->v.string)) {
				debug("in section routeProTh has invalid value,use default value:-55");
			        config->roam->route_pro_th = (unsigned char)-55;
				return -1;
			}
			config->roam->route_pro_th = (unsigned char)atoi(uci_to_option(e1)->v.string);
			debug("routeProTh=%d", (char)config->roam->route_pro_th);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "relayStaTh")) {
			if (0 <= atoi((uci_to_option(e1))->v.string)) {
				debug("in section relayStaTh has invalid value,use default value:-65");
			        config->roam->relay_sta_th = (unsigned char)-65;
				return -1;
			}
			config->roam->relay_sta_th = (unsigned char)atoi(uci_to_option(e1)->v.string);
			debug("relayStaTh=%d", (char)config->roam->relay_sta_th);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "relayProTh")) {
			if (0 <= atoi((uci_to_option(e1))->v.string)) {
				debug("in section relayProTh has invalid value,use default value:-55");
			        config->roam->relay_pro_th = (unsigned char)-55;
				return -1;
			}
			config->roam->relay_pro_th = (unsigned char)atoi(uci_to_option(e1)->v.string);
			debug("relayProTh=%d", (char)config->roam->relay_pro_th);
			goto next;
		}

next:
		;
	}
        /*
	if (!config->roam->enable) {
		printf("in roam you must define enable\n");
		return -1;
	}
        */
	return 0;
}

static struct uci_package *
config_init_package(const char *c)
{
	struct uci_context *ctx = uci_ctx;
	struct uci_package *p = NULL;

	if (first_run) {
		config = calloc(1, sizeof(struct core_config));
		if (!config) goto error;

		config->roam = calloc(1, sizeof(struct roam));
		if (!config->roam) goto error;
		config->roam->enable=1;
		config->roam->route_sta_th=(unsigned char)-70;
		config->roam->route_pro_th=(unsigned char)-55;
		config->roam->relay_sta_th=(unsigned char)-65;
		config->roam->relay_pro_th=(unsigned char)-55;
	}

	if (!ctx) {
		ctx = uci_alloc_context();
		if (!ctx) goto error;
		uci_ctx = ctx;
/*
#ifdef DUMMY_MODE
		uci_set_confdir(ctx, "./ext/openwrt/config");
		uci_set_savedir(ctx, "./ext/tmp");
#endif
*/

	} else {
		p = uci_lookup_package(ctx, c);
		if (p)
			uci_unload(ctx, p);
	}

	if (uci_load(ctx, c, &p)) {
		uci_free_context(ctx);
		return NULL;
	}

	return p;

error:
	if (config) {
		//config_free_roam();
		FREE(config->roam);
		FREE(config);
		uci_free_context(uci_ctx);
	}
	return NULL;
}

void config_exit(void)
{
	//config_free_roam();
	if (config) {
	    FREE(config->roam);
	    FREE(config);
	    uci_free_context(uci_ctx);
        }
}

void config_load(void)
{
	uci_relayroam = config_init_package("relayroam");
	if (!uci_relayroam) goto error;

	if (config_init_roam()) goto error;

	first_run = false;
	return;

error:
	debug("error:configuration (re)loading failed");
	exit(EXIT_FAILURE);
}

