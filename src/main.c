/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"

// declarando fontes
LV_FONT_DECLARE(dseg70); 
LV_FONT_DECLARE(dseg50);
LV_FONT_DECLARE(dseg60);
LV_FONT_DECLARE(dseg20);

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (240)

/*A static or global variable to store the buffers*/ 
static lv_disp_draw_buf_t disp_buf;
static  lv_obj_t * labelBtn1; //Voc�s podem definir o label como vari�vel global, permitindo assim que outra parte do c�digo altera o valor escrito.
static  lv_obj_t * labelBtnMenu;
static  lv_obj_t * labelBtnRelogio;
static  lv_obj_t * labelBtnFlechaCima;
static  lv_obj_t * labelBtnFlechaBaixo;
lv_obj_t * labelFloor;
lv_obj_t * labelFloorDecimal;
lv_obj_t * labelHora;
lv_obj_t * labelTemp;



/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY + 1)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/

static void event_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void event_BtnMenuhandler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void event_handler_RELOGIO(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void event_handler_FLECHA_CIMA(lv_event_t * e) {
lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
		c = lv_label_get_text(labelTemp);
		temp = atoi(c);
		lv_label_set_text_fmt(labelTemp, "%02d", temp + 1);
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}
static void event_handler_FLECHA_BAIXO(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
		c = lv_label_get_text(labelTemp);
		temp = atoi(c);
		lv_label_set_text_fmt(labelTemp, "%02d", temp - 1);
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/


void lv_ex_btn_1(void) {
	lv_obj_t * label;

	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);

	label = lv_label_create(btn1);
	lv_label_set_text(label, "Corsi");
	lv_obj_center(label);

	lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
	lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_height(btn2, LV_SIZE_CONTENT);

	label = lv_label_create(btn2);
	lv_label_set_text(label, "Toggle");
	lv_obj_center(label);
}


 void lv_termostato(void) {
//      lv_obj_t * labelBtn1;
// 	 lv_obj_t * labelBtnMenu;
// 	 lv_obj_t * labelFloor;


	// estilo da fonte
	 static lv_style_t style;
	 lv_style_init(&style);
	 lv_style_set_bg_color(&style, lv_color_black());
	 //lv_style_set_border_color(&style, 0);  sem borda
	 lv_style_set_border_width(&style, 5);
	 
	 //Alinhando botao
     lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
     lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
     lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 0, 0); // aqui que muda a posicao do botao em relacao a tela  
	 lv_obj_add_style(btn1, &style, 0);// add para o estilo
	 //lv_obj_set_width(btn1, 60);  lv_obj_set_height(btn1, 60); // mudar tamanho e altura
     // se eu quisesse alinhar com outra referencia � so colocar ela de segundo argumento dentro da funcao lv_obj_align()
     labelBtn1 = lv_label_create(btn1);
     lv_label_set_text(labelBtn1, "[ " LV_SYMBOL_POWER); // so dois argumentos
     lv_obj_center(labelBtn1);
	 
	 //botao Menu
	 lv_obj_t * btnMenu = lv_btn_create(lv_scr_act());
	 lv_obj_add_event_cb(btnMenu, event_BtnMenuhandler, LV_EVENT_ALL, NULL);
	 lv_obj_align_to(btnMenu,btn1,LV_ALIGN_RIGHT_MID, 40, -11);
	 lv_obj_add_style(btnMenu, &style, 0);// add para o estilo 
	 
	 //lv_obj_set_width(btnMenu, 60);  lv_obj_set_height(btnMenu, 60); // mudar tamanho e altura
	 
	 labelBtnMenu = lv_label_create(btnMenu);
	 lv_label_set_text(labelBtnMenu, "| M"); // casa
	 lv_obj_center(labelBtnMenu);

	 // botao relogio
	 lv_obj_t * btnRelogio = lv_btn_create(lv_scr_act());
	 lv_obj_add_event_cb(btnRelogio, event_handler_RELOGIO, LV_EVENT_ALL, NULL);
	 lv_obj_align_to(btnRelogio,btnMenu,LV_ALIGN_RIGHT_MID, 40, -11);
	 lv_obj_add_style(btnRelogio, &style, 0);// add para o estilo

	 labelBtnRelogio = lv_label_create(btnRelogio);
	 lv_label_set_text(labelBtnRelogio,"| " LV_SYMBOL_SETTINGS " ]"); // casa
	 lv_obj_center(labelBtnRelogio);

	 // botao flecha para cima
	 lv_obj_t * btnFlechaCima = lv_btn_create(lv_scr_act());
	 lv_obj_add_event_cb(btnFlechaCima, event_handler_FLECHA_CIMA, LV_EVENT_ALL, NULL);
	 lv_obj_align(btnFlechaCima,LV_ALIGN_BOTTOM_RIGHT, -80, 0);
	 lv_obj_add_style(btnFlechaCima, &style, 0);

	 labelBtnFlechaCima = lv_label_create(btnFlechaCima);
	 lv_label_set_text(labelBtnFlechaCima, "[ "LV_SYMBOL_UP);
	 lv_obj_center(labelBtnFlechaCima);

	 // botao flecha para baixo
	 lv_obj_t * btnFlechaBaixo = lv_btn_create(lv_scr_act());
	 lv_obj_add_event_cb(btnFlechaBaixo, event_handler_FLECHA_BAIXO, LV_EVENT_ALL, NULL);
	 lv_obj_align(btnFlechaBaixo,LV_ALIGN_BOTTOM_RIGHT, -25, 0);
	 lv_obj_add_style(btnFlechaBaixo, &style, 0);

	 labelBtnFlechaBaixo = lv_label_create(btnFlechaBaixo);
	 lv_label_set_text(labelBtnFlechaBaixo,LV_SYMBOL_DOWN" ]");
	 lv_obj_center(labelBtnFlechaBaixo); 
	 
	 
	 
	 //Floor
	 	 labelFloor = lv_label_create(lv_scr_act());
	 	 lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 35 , -45);
	 	 lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
	 	 lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
	 	 lv_label_set_text_fmt(labelFloor, "%02d", 23);
	
	 // Floor decimal 
		lv_obj_t * labelFloorDecimal = lv_label_create(lv_scr_act());
		lv_obj_align_to(labelFloorDecimal, labelFloor, LV_ALIGN_OUT_RIGHT_MID, 0, 0); // Alinhe à direita da parte inteira
		lv_obj_set_style_text_font(labelFloorDecimal, &dseg20, LV_STATE_DEFAULT); // Use uma fonte menor
		lv_obj_set_style_text_color(labelFloorDecimal, lv_color_white(), LV_STATE_DEFAULT);
		lv_label_set_text(labelFloorDecimal, ".4");
	 
	 // hora
	 	 lv_obj_t * labelHora = lv_label_create(lv_scr_act());
	 	 lv_obj_align(labelHora, LV_ALIGN_TOP_RIGHT, 0 , 0);
	 	 lv_obj_set_style_text_font(labelHora, &dseg50, LV_STATE_DEFAULT);
	 	 lv_obj_set_style_text_color(labelHora, lv_color_white(), LV_STATE_DEFAULT);
	 	 lv_label_set_text_fmt(labelHora, "%02d:%02d", 20, 30);


 	 //Temp
	 	 lv_obj_t * labelTemp = lv_label_create(lv_scr_act());
	 	 lv_obj_align(labelTemp, LV_ALIGN_TOP_RIGHT, -15 , 68);
	 	 lv_obj_set_style_text_font(labelTemp, &dseg60, LV_STATE_DEFAULT);
	 	 lv_obj_set_style_text_color(labelTemp, lv_color_white(), LV_STATE_DEFAULT);
	 	 lv_label_set_text_fmt(labelTemp, "%02d°C", 25);
 }
	 
 
/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_lcd(void *pvParameters) {
	int px, py;

	
	lv_termostato();

	for (;;)  {
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
	}
}


/************************************************************************/
/* configs                                                              */
/************************************************************************/

static void configure_lcd(void) {
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);  //
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	
	ili9341_init();
	ili9341_backlight_on();
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
	};

	/* Configure console UART. */
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* port lvgl                                                            */
/************************************************************************/

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
	ili9341_set_top_left_limit(area->x1, area->y1);   ili9341_set_bottom_right_limit(area->x2, area->y2);
	ili9341_copy_pixels_to_screen(color_p,  (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
	
	/* IMPORTANT!!!
	* Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
	int px, py, pressed;
	
	if (readPoint(&px, &py))
		data->state = LV_INDEV_STATE_PRESSED;
	else
		data->state = LV_INDEV_STATE_RELEASED; 
	
	data->point.x = px;
	data->point.y = py;
}

void configure_lvgl(void) {
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
	
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;      /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = LV_VER_RES_MAX;      /*Set the vertical resolution in pixels*/

	lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	/* Init input on LVGL */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
	/* board and sys init */
	board_init();
	sysclk_init();
	configure_console();

	/* LCd, touch and lvgl init*/
	configure_lcd();
	configure_touch();
	configure_lvgl();



	/* Create task to control oled */
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
