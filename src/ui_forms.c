/************************************************************************************************
 * FilterTools/ui_forms.c
 * 
 * Author	: Ben Passman
 * Created	: 16/1/2021
 * 
 * Description	: Form layouts and access functions
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 16/1/2021    Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "ui_forms.h"
#include <stdlib.h>

static const char *wave_types[] = { "Sine", "Cosine", "Sawtooth", "Triangle", "Square" };
static const char *wave_modes[] = { "Add", "Subtract", "Multiply", "Divide", "AM", "FM" };
static struct Dropdown settings_dropdowns[] = { 
	{ 0, &wave_types[0], sizeof wave_types / sizeof wave_types[0] },
	{ 0, &wave_modes[0], sizeof wave_modes / sizeof wave_modes[0] }
}; 
	

struct Form wave_settings_setup (WINDOW *window)
{
	mvwprintw(window, 2, 4, "Shape:");
	mvwprintw(window, 4, 4, "Amplitude:");
	mvwprintw(window, 6, 4, "Frequency:");
	mvwprintw(window, 6, 36, "Hz");
	mvwprintw(window, 8, 4, "Phase:");
	mvwprintw(window, 10, 4, "Duty:");
	mvwprintw(window, 12, 4, "Mode:");
	mvwprintw(window, 14, 4, "DC Offset:");
	
	static enum FieldType types[] = { 
		LIST_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		LIST_FIELD, 
		NUMBER_FIELD, 
		OK_FIELD, 
		CANCEL_FIELD 
	};
	
	struct Form settings;
	settings.field_count = 9;
	settings.fields = calloc(settings.field_count + 1, sizeof(FIELD *));
	settings.field_types = &types[0];
        
	settings.fields[0] = new_field(1, 20, 0, 0, 0, 0);
	settings.fields[1] = new_field(1, 20, 2, 0, 0, 0);
	settings.fields[2] = new_field(1, 20, 4, 0, 0, 0);
	settings.fields[3] = new_field(1, 20, 6, 0, 0, 0);
	settings.fields[4] = new_field(1, 20, 8, 0, 0, 0);
	settings.fields[5] = new_field(1, 20, 10, 0, 0, 0);
	settings.fields[6] = new_field(1, 20, 12, 0, 0, 0);
	settings.fields[7] = new_field(1, 8, 14, 0, 0, 0);
	settings.fields[8] = new_field(1, 8, 14, 10, 0, 0);
	settings.fields[9] = NULL;
    
	set_field_userptr(settings.fields[0], (void *) &settings_dropdowns[0]);
	set_field_userptr(settings.fields[5], (void *) &settings_dropdowns[1]);
	set_field_buffer(settings.fields[7], 0, "   Ok   ");
	set_field_buffer(settings.fields[8], 0, " Cancel ");
    	
	field_setup(settings.fields, types, settings.field_count);
	settings.form = form_setup(window, settings.fields, 8, 28);

	return settings;
}

void set_wave_fields(const struct Form *wave_settings, const struct WaveForm *wave)
{
    struct Dropdown *drop_down;
    
    drop_down = (struct Dropdown *) field_userptr(wave_settings->fields[0]);
    drop_down->index = wave->type;
    set_field_buffer(wave_settings->fields[0], 0, wave_types[drop_down->index]);
    char temp[20];
    sprintf(temp, "%lf", wave->amplitude);
    set_field_buffer(wave_settings->fields[1], 0, temp);
    sprintf(temp, "%lf", wave->frequency);
    set_field_buffer(wave_settings->fields[2], 0, temp);
    sprintf(temp, "%lf", wave->phase);
    set_field_buffer(wave_settings->fields[3], 0, temp);
    sprintf(temp, "%lf", wave->duty);
    set_field_buffer(wave_settings->fields[4], 0, temp);
    drop_down = (struct Dropdown *) field_userptr(wave_settings->fields[5]);
    drop_down->index = wave->mode;
    set_field_buffer((wave_settings->fields)[5], 0, wave_modes[drop_down->index]);
    sprintf(temp, "%lf", wave->dc_offset);
    set_field_buffer(wave_settings->fields[6], 0, temp);
}

void get_wave_fields(const struct Form *form, struct WaveForm *wave)
{
    struct Dropdown *drop_down;
    
    drop_down = (struct Dropdown *) field_userptr(form->fields[0]);
    wave->type = drop_down->index;
    wave->amplitude = ASCII_string_to_double(field_buffer(form->fields[1], 0));
    wave->frequency = ASCII_string_to_double(field_buffer(form->fields[2], 0));
    wave->phase = ASCII_string_to_double(field_buffer(form->fields[3], 0));
    wave->duty = ASCII_string_to_double(field_buffer(form->fields[4], 0));
    drop_down = (struct Dropdown *) field_userptr(form->fields[5]);
    wave->mode = drop_down->index;
    wave->dc_offset = ASCII_string_to_double(field_buffer(form->fields[6], 0));
}
