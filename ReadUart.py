import serial

START_BYTE = 0xA5
PACKET_SIZE = 32

def find_start(buffer):
    """Find index of start byte (0xA5) inside buffer."""
    try:
        return buffer.index(START_BYTE)
    except ValueError:
        return -1

def parse_packet(packet):
    """Convert bytes to hex string + return payload + crc."""
    hex_string = " ".join(f"{b:02X}" for b in packet)
    return hex_string

def main():
    ser = serial.Serial(
        port="/dev/ttyUSB0",      # change as needed (COM3 on Windows)
        baudrate=115200,
        timeout=0.1
    )

    buffer = bytearray()

    print("Listening...")

    while True:
        # Read what’s available
        data = ser.read(64)  # read chunks
        if not data:
            continue

        buffer.extend(data)

        while True:
            start = find_start(buffer)
            if start < 0:
                # No start byte found → keep last few bytes only to avoid infinite growth
                buffer.clear()
                break

            # Not enough bytes after start → wait for more
            if len(buffer) - start < PACKET_SIZE:
                break

            # Extract packet
            packet = buffer[start:start + PACKET_SIZE]

            # Remove processed portion
            del buffer[:start + PACKET_SIZE]

            # Print packet
            print("Packet:", parse_packet(packet))

            # Payload & CRC (you can decode your data later)
            payload = packet[1:-1]
            crc = packet[-1]

            print("Payload:", " ".join(f"{b:02X}" for b in payload))
            print("CRC:", f"{crc:02X}")
            print("-----")


if __name__ == "__main__":
    main()
