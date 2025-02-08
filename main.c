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

ssd1306_t * init_display(){
    ssd1306_t * ssd;
    ssd1306_init(ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);

    return ssd;
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

void gpio_irq_handler(uint gpio, uint32_t event) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    uint led;
    char * msg;
    if(current_time - last_time >= 200){
        if (gpio == PIN_BUTTON_A) {
            led = PIN_GREEN_LED;
            msg = gpio_get(led) ? "LED verde apagado" : "LED verde aceso";
        } else {
            led = PIN_BLUE_LED;
            msg = gpio_get(led)? "LED azul apagado" : "LED azul aceso";
        }
        gpio_put(led, !gpio_get(led));
        printf("%s\n", msg);
        // ssd1306_draw_string(ssd, msg, x, y);
        last_time = current_time;
    }
}

int main (){
    PIO pio = pio0;
    bool ok;
    char c;
    // set clock freq to 128MHz
    // ok = set_sys_clock_khz(128000, false);
    // if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    stdio_init_all();
    init_gpio();

    // get ws and ssd struct
    ws = init_ws2812b(pio, PIN_WS2812B);
    ssd = init_display();

    // Clear display 
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);

    // configure interruptions handlers
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    sleep_ms(50);

    while (1) {
        // if (stdio_usb_connected()) {
            if (scanf("%c", &c) == 1) {
                if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {  // Letras (A-Z ou a-z)
                    printf("Letra recebida: '%c'.\n", c);
                    // ssd1306_draw_string(ssd, &c, x, y);
                } else if (c >= 48 && c <= 57) {  // Números (0-9)
                    printf("Número recebido: '%c'.\n", c);
                    // ssd1306_draw_string(ssd, &c, x, y);
                    ws2812b_plot(ws, NUMERIC[c - '0']); // convert char in a integer
                } else {
                    printf("O caractere '%c' não é uma letra nem um número.\n", c);
                }
            }
        // }
    }

    return 0;
}