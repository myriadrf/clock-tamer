# ClockTamer SPI specifications #

  * ClockTamer is a **slave** on SPI bus, thus you have to provide clock on `SCK` pin by yourself.
  * When idle, `SCK` is high, i.e. **`CPOL=1`**. Data are read on the clock's rising edge (low->high transition), i.e. **`CPHA=0`**. In other words, ClockTamer implements [\*SPI type 2\*](http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus#Mode_Numbers).
  * Data is shifted out with the **Most Significant Bit first**.
  * Before exchanging data you should set **`nSS` pin to 0**.

[(more info about SPI in Wikipedia)](http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus)

# ClockTamer pinout #

| ClockTamer pin | USRP daughter-board pin | Comments |
|:---------------|:------------------------|:---------|
| `nSS`          | `io_rx_08`              | SPI Slave Select: active low |
| `SCK`          | `io_rx_09`              | SPI Serial Clock: driven by USRP |
| `MOSI`         | `io_rx_10`              | SPI data: USRP -> ClockTamer |
| `MISO`         | `io_rx_11`              | SPI data: ClockTamer -> USRP |
| `nRESET`       | `io_rx_12`              | Reset: Set to 0 to reset ClockTamer |

# Switching to USB-flashing mode #

To start flashing-over-USB mode you should perform following operations:

  1. Set `nSS` to 0.
  1. Set `nReset` to 0.
  1. Set `nReset` to 1.
  1. Set `nSS` to 1.

# Control protocol SPI-specific operation #

**TODO**
<a href='Hidden comment: 
чтобы получить ответ надо писать в устройство FF
23:02:42: когда будет читаться FF значит все из устройства прочитано
23:02:56: когда будут читаться 00 значит данные ещё не готовы
23:03:10: все остальнвые символы полезные данный

(когда будут читаться 00 значит данные ещё не готовы)  хотя по началу этого не будет, будут FF, просто поставить какойто ватч-дог,
(я забыл ещё в коде это не предусмотрено)
23:13:51: тоесть алгоритм такой
записать команду (при этом читаться может результат предыдущей, если ответа не дождались, и если все было прочитано то FF )
записывть FF (c сограничением по ватч догу, скажем 1000раз) и читать, как появился первый не FF байт, значит пошел ответ,
после его прочтения будут читаться FF
'></a>