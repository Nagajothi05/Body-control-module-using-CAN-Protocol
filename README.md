Body control module using CAN Protocol 	

Platform: LPC2129 Microcontroller   |   Language: Embedded C 

●	Designed and implemented a Body Control Module (BCM) using the Controller Area Network (CAN) protocol for real-time communication in automotive applications.

●	Configured a 4-node CAN network (1 transmitter, 3 receivers) and assigned unique Message IDs to enable accurate message routing and LED control on receiver nodes.

●	Built CAN Transmitter logic to acquire analog data via ADC (Analog to Digital Converter) and transmit it as structured CAN messages at 250 kbps.

●	Developed Receiver-side firmware to decode CAN frames, extract command data, and trigger LED outputs based on received values.

●	Integrated UART0 for debugging, hexadecimal data logging, and system monitoring; implemented Interrupt Service Routines (ISRs) for CAN RX to enhance responsiveness.

●	Wrote low-level drivers for CAN, ADC, UART, GPIO, and implemented digital input handling and millisecond delay functions for precise control.
