# jpico

modular c++23 toolkit for the raspberry pi pico 2 w. link only what you need.

## modules

| module          | target           | what it is                                           |
| --------------- | ---------------- | ---------------------------------------------------- |
| core            | `jpico_core`     | type aliases, `result<T>`, colors, logging, concepts |
| hal             | `jpico_hal`      | raii wrappers for spi, i2c, gpio, dma                |
| drivers/ili9341 | `jpico_ili9341`  | ili9341 tft driver (satisfies `jpico::display`)      |
| graphics        | `jpico_graphics` | `canvas<D>` — draw primitives + text on any display  |
| network         | `jpico_network`  | cyw43 wifi manager for pico w                        |

## using it

add jpico as a subdirectory and link the modules you want:

```cmake
add_subdirectory(jpico)
target_link_libraries(my_app jpico_hal jpico_ili9341 jpico_graphics)
```

modules pull in their own dependencies, so linking `jpico_ili9341` gets you
`jpico_hal` and `jpico_core` automatically.

## building the examples

```
cmake -B build .
cmake --build build
```

three examples are included:

| example         | links                        | size  |
| --------------- | ---------------------------- | ----- |
| `blink`         | core, hal                    | ~50K  |
| `display_hello` | core, hal, ili9341, graphics | ~102K |
| `wifi_connect`  | core, network                | ~703K |

## flashing

```
sudo picotool load build/examples/blink/example_blink.uf2 -fx
```

## quick taste

```cpp
#include <jpico/core.hpp>
#include <jpico/drivers/ili9341.hpp>
#include <jpico/graphics/canvas.hpp>
#include <jpico/hal/hal.hpp>

using namespace jpico;

int main() {
  stdio_init_all();

  hal::spi_bus spi(spi0, {.baudrate = 40'000'000, .pin_sck = 6, .pin_tx = 7});
  hal::output_pin cs(13, true), dc(15), rst(14, true);

  drivers::ili9341 display(spi, cs, dc, rst);
  display.init();
  display.set_rotation(1);

  graphics::canvas canvas(display);
  canvas.create_framebuffer();
  canvas.clear();
  canvas.set_text_color(colors::cyan.raw);
  canvas.print("hello, jpico!");
  canvas.flush();
}
```

## cmake options

| option                  | default | what it does              |
| ----------------------- | ------- | ------------------------- |
| `JPICO_ENABLE_GRAPHICS` | `ON`    | build the graphics module |
| `JPICO_ENABLE_NETWORK`  | `ON`    | build the network module  |
| `JPICO_ENABLE_ILI9341`  | `ON`    | build the ili9341 driver  |
| `JPICO_ENABLE_EXAMPLES` | `ON`    | build example programs    |

## license

do whatever you want with it.
