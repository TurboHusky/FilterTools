/************************************************************************************************
 * FilterTools/test_signal_generator.c
 * 
 * Author	: Ben Passman
 * Created	: 22/12/2020
 * 
 * Description	: Generator functions for creating test waveforms. 
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 22/12/2020	Ben P		1.0	Sine, saw, triangle and square wave generators implemented.
 * 25/12/2020	Ben P		1.01	Generator code freeze, begin work on ncurses menu code.
 *
 ************************************************************************************************ */

#include "test_signal_generator.h"
#include "ASCII_parser.h"

#include<errno.h>
#include<math.h>
#include<stdlib.h>

#include<menu.h>
#include<form.h>
#include<panel.h>

struct Menu {
	MENU * menu;
	ITEM ** items;
	int item_count;
};

struct Form {
	FORM * form;
	FIELD ** fields;
	int field_count;
};

long double string_to_long_double(char * string)
{
	errno = 0;
	char *endptr;
	long double conv = strtod(string, &endptr);

	if (errno != 0 || *endptr != '\0')
	{
		printf("Failed to parse string, error code %d\n", errno);
		return errno;
	}

	return conv;
}

long double saw_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 2.0L * amplitude * freq * (t - delay) + amplitude;
	output_point = fmodl(output_point, 2.0L * amplitude) - amplitude;
	if (output_point < -amplitude)  // Correction for -ve time
	{
		output_point += 2.0L * amplitude;
	}

	return output_point;
}

long double triangle_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 4.0L * amplitude * freq * (t - delay);
	output_point = fmodl(output_point, 4.0L * amplitude);
	if (output_point < 0)	// Correction for -ve time
	{
		output_point += 4.0L * amplitude;
	}
	if (output_point > amplitude)	
	{
		output_point = 2.0L * amplitude - output_point;
	}
	if (output_point < -amplitude)
	{
		output_point = -output_point - 2.0L * amplitude;
	}

	return output_point;
}

long double square_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double T = 1 / freq;
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double local_time = fmodl(t - delay, T);
	if (local_time < 0) // Correction for -ve time
	{
		local_time = T + local_time;
	}

	long double output_point = amplitude;
	if (local_time > duty * T) 
	{
		output_point = -amplitude;
	}

	return output_point;
}

long double sine_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double w = (t * 2.0L * acosl(-1) * freq) - phase;
	long double outputPoint = amplitude * sinl(w);

	return outputPoint;
}

void generate_signal(const int number_of_samples, const long double sample_rate)
{
	FILE *fp;
	fp = fopen("FFT Test Data.csv", "w");

	if(fp != NULL) {
		long double T = 1.0/sample_rate;	// Sampling period
		long double pi = acosl(-1);		// Use long double for Pi
	
		for(long double i = -number_of_samples * T; i <= number_of_samples * T; i+=T)
		{
			long double w = i * 2.0 * pi;
			long double test_wave = sinl(5.0*w) + 
						sinl(10.0*w) + 
						sinl(50.0*w) + 
						sinl(55.0*w) + 
						sinl(300.0*w) +
						sinl(450.0*w) + 
						//sinl(7000.0*w) + 
						//sinl(10000.0*w) + 
						triangle_wave(i, 7000, 1.2, 0, 0) +
						saw_wave(i, 10000, 1.4, 1.5, 0) +
						square_wave(i, 20000, 1.0, 0.75, 0.3) +
						//sinl(12000.0*w) + 
						//sinl(25000.0*w);
						sine_wave(i, 25000, 1.0, 1.5, 0);
			fprintf(fp, "%Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n",
				i, 
				sinl(5.0*w), 
				sinl(10.0*w), 
				sinl(50.0*w), 
				sinl(55.0*w), 
				sinl(300.0*w),
				sinl(450.0*w), 
				//sinl(7000.0*w), 
				//sinl(10000.0*w), 
				triangle_wave(i, 2000, 1.0, 0.75, 0),
				saw_wave(i, 1000, 1.4, 1.5, 0),
				square_wave(i, 2000, 1.0, 3.14, 0.3),
				sine_wave(i, 25000, 1.0, 1.5, 0),
				//sinl(12000.0*w), 
				//sinl(25000.0*w),
				test_wave);
		}
	}
	else
	{
		printf("Could not open file for editing.\n");
		exit(1);
	}
	fclose(fp);
}

struct Menu main_menu_setup(WINDOW * menu_window)
{
	struct Menu main_menu;
	const char * main_menu_options[] = MAIN_MENU_OPTIONS;
	main_menu.item_count = sizeof(main_menu_options) / sizeof(main_menu_options[0]);

	main_menu.items = (ITEM **)calloc(main_menu.item_count + 1, sizeof(ITEM *));

	for(int i = 0; i < main_menu.item_count; ++i)
	{
		main_menu.items[i] = new_item(main_menu_options[i], "");
	}
	main_menu.items[main_menu.item_count] = (ITEM *)NULL;

	main_menu.menu = new_menu((ITEM **)main_menu.items);
	set_menu_win(main_menu.menu, menu_window);
	set_menu_sub(main_menu.menu, derwin(menu_window, 0, 0, 1, 1));
	set_menu_mark(main_menu.menu, " * ");
	
	post_menu(main_menu.menu);
	
	return main_menu;
}

struct Form form_setup(WINDOW * form_window)
{
	struct Form this_form;
	this_form.field_count = 3;

	this_form.fields = (FIELD **)calloc(this_form.field_count + 1, sizeof(FIELD *));

	this_form.fields[0] = new_field(1, 10, 4, 18, 0, 0);
	this_form.fields[1] = new_field(1, 10, 6, 18, 0, 0);
	this_form.fields[2] = new_field(1, 10, 8, 18, 0, 0);
	this_form.fields[3] = NULL;

	set_field_back(this_form.fields[0], A_UNDERLINE);
	field_opts_off(this_form.fields[0], O_AUTOSKIP);
	set_field_buffer(this_form.fields[1], 0, "Testing...");
	set_field_back(this_form.fields[1], A_UNDERLINE);
	field_opts_off(this_form.fields[1], O_AUTOSKIP);
	set_field_back(this_form.fields[2], A_UNDERLINE);
	field_opts_off(this_form.fields[2], O_AUTOSKIP);	

	int rows = 8;
	int cols = 28;
	this_form.form = new_form(this_form.fields);
	scale_form(this_form.form, &rows, &cols);
	set_form_win(this_form.form, form_window);
	set_form_sub(this_form.form, derwin(form_window, rows, cols, 2, 2));
	post_form(this_form.form);

	mvwprintw(form_window, 4, 10, "Value 1:");
	mvwprintw(form_window, 6, 10, "Value 2:");

	return this_form;
}

void free_menu_struct(struct Menu menu_struct)
{
	unpost_menu(menu_struct.menu);
	free_menu(menu_struct.menu);
	for(int i =0; i < menu_struct.item_count; i++)
		free_item(menu_struct.items[i]);
}

void free_form_struct(struct Form form_struct)
{
	unpost_form(form_struct.form);
	free_form(form_struct.form);
	for(int i = 0; i < form_struct.field_count; i++)
		free_field(form_struct.fields[i]);
}

int main(int argc, char** argv)
{
/*	if (argc > 2)
	{
		long double sampleCount = string_to_long_double(argv[1]);
		long double sampleFreq = string_to_long_double(argv[2]);
		// Tested 100 samples at 1MHz
		generate_signal(sampleCount, sampleFreq);
	}
	else
	{
		printf("Missing function arguments\n");
	}*/

	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed
	curs_set(0);	// Hide cursor
	
	WINDOW * menu_window = newwin(MAIN_MENU_SIZE, MAIN_MENU_LOCATION);
	WINDOW * output_window = newwin(OUTPUT_WINDOW_SIZE, OUTPUT_WINDOW_LOCATION);
	WINDOW * popup_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);
		
	box(menu_window, 0, 0);
	box(output_window, 0, 0);
	box(popup_window, 0, 0);	
	
	struct Menu main_menu = main_menu_setup(menu_window);

	PANEL * menu_panel;
	PANEL *	output_panel;
	PANEL * popup_panel;
	menu_panel = new_panel(menu_window);
	output_panel = new_panel(output_window);
	popup_panel = new_panel(popup_window);
	hide_panel(popup_panel);
	update_panels();

	struct Form file_form = form_setup(popup_window);
	
	doupdate();

	// MENU INTERFACE
	int c;
	keypad(menu_window, TRUE);
	while((c = wgetch(menu_window)) != 'q')
	{
		switch(c)
		{
			case KEY_DOWN :
				menu_driver(main_menu.menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP :
				menu_driver(main_menu.menu, REQ_UP_ITEM);
				break;
			case '\n' :
				if (panel_hidden(popup_panel))
				{
					show_panel(popup_panel);
					hide_panel(menu_panel);
					hide_panel(output_panel);
					bottom_panel(menu_panel);
				}
				else
				{
					hide_panel(popup_panel);
					show_panel(menu_panel);
					show_panel(output_panel);
				}
				break;
		}
		
		if (panel_hidden(popup_panel))
		{
			// Debug
			mvwprintw(output_window, 1, 2, "Sampling frequency: 555 kHz");
			mvwprintw(output_window, 2, 2, "Sample count:       1000");
			mvwprintw(output_window, 4, 2, "Shape: Amplitude: Phase: Freq: Duty: Mode:");
			mvwprintw(output_window, 5, 2, "Main menu size: %d", main_menu.item_count);
			mvwprintw(output_window, 6, 2, "Selected option: %d", item_index(current_item(main_menu.menu)));
		}

		update_panels();
		doupdate();
	}
	
	// FORM INTERFACE
	/*curs_set(1);
	keypad(popup_window, TRUE);
	while((c = wgetch(popup_window)) != KEY_F(1))
	{
		switch(c)
		{
			case KEY_DOWN :
				form_driver(file_form.form, REQ_NEXT_FIELD);
				form_driver(file_form.form, REQ_END_LINE);
				break;
			case KEY_UP :
				form_driver(file_form.form, REQ_PREV_FIELD);
				form_driver(file_form.form, REQ_END_LINE);
				break;
			case '\n' :
				
				break;
			default :
				form_driver(file_form.form, c);
				break;
		}

		
	}*/

	free_menu_struct(main_menu);
	free_form_struct(file_form);
	endwin();
	return 1;
}	
