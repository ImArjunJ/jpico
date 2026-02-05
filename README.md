# jpico

a little helper library for the raspberry pi pico 2 w.

## what it does

- wifi wrapper for easy connection
- ili9341 display driver over spi
- canvas for drawing shapes and text
- just makes pico stuff a bit nicer

## building

```
cmake -B build .
cmake --build build
```

## flashing

```
picotool load build/jpico.uf2 -fx
```

## pins

| function | gpio |
| -------- | ---- |
| dc       | 15   |
| cs       | 13   |
| rst      | 14   |
| sclk     | 6    |
| mosi     | 7    |

## wifi

edit `include/config.hpp` with your ssid and password.

## license

do whatever you want with it.
