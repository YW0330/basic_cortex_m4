# Basic Programming on ARM Cortex-M4 Processor
本專案為針對 Udemy 線上課程 [Embedded Systems Programming on ARM Cortex-M3/M4 Processor](https://www.udemy.com/course/embedded-system-programming-on-arm-cortex-m3m4/) 的實作內容進行開發。[開發筆記](https://hackmd.io/@YW0330/r1KNL4NEj/%2Fbfo7mPYWQEaxJuOYy0_JiQ) 請另行參閱。


## 硬體規格
- 核心 [STM32F303ZE](https://www.st.com/en/microcontrollers-microprocessors/stm32f303ze.html#documentation)

- 開發板 [NUCLEO-F303ZE](https://www.st.com/en/evaluation-tools/nucleo-f303ze.html#documentation)


## 使用方法
- 編譯
    ```shell
    $ make
    ```
    編譯完成後，可執行檔為 `dist/*.elf`；中間檔案會存放在 `build` 資料夾下。
- 燒錄
    ```shell
    $ make upload
    ```
- 終端機執行
    ```shell
    $ make terminal_serial
    ```
- 清理專案
    ```shell
    $ make clean
    ```