#!/bin/bash

atom ./hw/char/nrf52_uart.c           \
      ./hw/arm/nrf52_soc.c            \
      ./hw/arm/cap_machine.c          \
      ./include/hw/char/nrf52_uart.h  \
      ./include/hw/arm/nrf52_soc.h    \
      ./include/hw/arm/nrf52840.h
      # atom ./hw/char/samx7x_uart.c
      # atom ./include/hw/char/samx7x_uart.h
