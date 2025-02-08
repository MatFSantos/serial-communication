#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"

#include "modules/led.h"
#include "modules/push_button.h"
#include "modules/ssd1306.h"
#include "modules/ws2812b.h"

_ws2812b * ws;
ssd1306_t ssd;
static volatile uint32_t last_time = 0;

char c = '\0';
char *msg_blue;
char *msg_green;

/**
 * @brief Initialize the SSD1306 display
 *
 */
void init_display(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
}


/**
 * @brief Initialize the all GPIOs that will be used in project
 *      - I2C;
 *      - RGB LED;
 *      - Push buttons A and B 
 */
void init_gpio(){
    /** initialize i2c communication */
    int baudrate = 400*1000; // 400kHz baud rate for i2c communication
    i2c_init(I2C_PORT, baudrate);

    // set GPIO pin function to I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    /** initialize RGB LED */
    init_rgb_led();

    /** initialize push buttons */
    init_push_button(PIN_BUTTON_A);
    init_push_button(PIN_BUTTON_B);
}

/**
 * @brief Makes the message that will be show in display, with the LED status
 * 
 * @param  pin The GPIO pin number of the LED
 * 
 * @return the message that will be show in display
 */
char * get_led_msg(uint8_t pin){
    if (pin == PIN_GREEN_LED)
        return gpio_get(PIN_GREEN_LED) ? "green led on" : "green led off";
    else if (pin == PIN_BLUE_LED)
        return gpio_get(PIN_BLUE_LED) ? "blue led on" : "blue led off";
}

/**
 * @brief Update the display informations
 */
void update_display() {
    ssd1306_fill(&ssd, false);
    msg_green = get_led_msg(PIN_GREEN_LED);
    ssd1306_draw_string(&ssd, msg_green, 0, 0);

    msg_blue = get_led_msg(PIN_BLUE_LED);
    ssd1306_draw_string(&ssd, msg_blue, 0, 10);

    if (c != '\0')
    {
        ssd1306_draw_string(&ssd, "caractere", 27, 26);
        ssd1306_draw_char(&ssd, c, 59, 42);
    }
    ssd1306_send_data(&ssd); // update display
}


/**
 * @brief Handler function to interruption
 * 
 * @param gpio GPIO that triggered the interruption
 * @param event The event which caused the interruption
 */
void gpio_irq_handler(uint gpio, uint32_t event) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    uint led;
    if(current_time - last_time >= 200) { // debounce
        // select the LED according pressed button
        if (gpio == PIN_BUTTON_A) led = PIN_GREEN_LED;
        else led = PIN_BLUE_LED; 
        
        // update led and display
        gpio_put(led, !gpio_get(led));
        printf("%s\n", get_led_msg(led));
        update_display();

        last_time = current_time;
    }
}

int main (){
    PIO pio = pio0;
    bool ok;

    // set clock freq to 128MHz
    ok = set_sys_clock_khz(128000, false);
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // init gpios and stdio functions
    stdio_init_all();
    init_gpio();

    // get ws and ssd struct
    ws = init_ws2812b(pio, PIN_WS2812B);
    init_display();

    // Clear display 
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // configure interruptions handlers
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    sleep_ms(50);

    // show informations in display
    update_display();
    while (1) {
        if (stdio_usb_connected()) {
            if (scanf("%c", &c) == 1) {
                if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) // Letras (A-Z ou a-z)
                    printf("Letra recebida: '%c'.\n", c);
                else if (c >= 48 && c <= 57) {  // Números (0-9)
                    printf("Número recebido: '%c'.\n", c);
                    ws2812b_plot(ws, NUMERIC[c - '0']); // convert char in a integer
                } else {
                    printf("O caractere '%c' não é uma letra nem um número.\n", c);
                    c = '\0'; 
                }
                update_display();
            }
        }
        sleep_ms(200);
    }

    return 0;
}