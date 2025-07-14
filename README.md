# README: Button-Triggered AT Command for nRF91 Series

This application demonstrates how to use a button press to trigger an AT command on an nRF91 Series device using the nRF Connect SDK. When the button is pressed, the application sends an AT command to the modem and prints the response. This can be used, for example, to retrieve information from the modem that may be required for device claiming or attestation.

## Features

- **Button Input with Debounce:** Uses a GPIO button (defined by the `sw1` devicetree alias) with software debounce to avoid multiple triggers.
- **AT Command Execution:** Sends an AT command (`AT&ATTESTTOKEN`) to the modem and prints the response.
- **Modem Library Integration:** Utilizes the nRF Modem library for communication with the modem core.

## Requirements

- nRF91 Series device (e.g., nRF9160 DK, nRF9161 DK, Thingy:91, etc.)
- nRF Connect SDK with the following libraries enabled:
  - Modem library (`CONFIG_NRF_MODEM_LIB`)
  - AT command interface (`nrf_modem_at.h`)
  - Modem information library (`modem_info.h`)
- Proper devicetree configuration for the button (`sw1` alias)

## How It Works

1. **Initialization:**
   - The modem library is initialized to enable communication with the modem core. This is typically done by calling `nrf_modem_lib_init()` early in your application, though this is not shown explicitly in the code snippet.  
   - The button is configured as an input with a pull-up resistor and interrupt on falling edge (button press).

2. **Button Press Handling:**
   - When the button is pressed, the interrupt is disabled and a debounce timer is started.
   - The `send_at_command()` function is called, which sends the `AT&ATTESTTOKEN` command to the modem using `nrf_modem_at_cmd()`.
   - The modem's response is printed to the console.

3. **Debounce Logic:**
   - After the debounce period, the button interrupt is re-enabled to allow for subsequent presses.

## Example Output

When you press the button, you should see output similar to:

AT&ATTESTTOKEN
"UUID"
OK
