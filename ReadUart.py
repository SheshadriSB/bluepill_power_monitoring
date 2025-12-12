import serial

START_BYTE = 0xA5
PACKET_SIZE = 32
CRC_POLY = 0x07  # CRC-8 polynomial

def crc8(data):
    """Compute CRC-8 using polynomial 0x07."""
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ CRC_POLY
            else:
                crc <<= 1
            crc &= 0xFF
    return crc

def find_start(buffer):
    try:
        return buffer.index(START_BYTE)
    except ValueError:
        return -1

def parse_packet(packet):
    return " ".join(f"{b:02X}" for b in packet)

def main():
    ser = serial.Serial(
        port="/dev/ttyUSB1",
        baudrate=115200,
        timeout=0.1
    )

    buffer = bytearray()

    print("Listening...")

    while True:
        data = ser.read(64)
        if not data:
            continue

        buffer.extend(data)

        while True:
            start = find_start(buffer)
            if start < 0:
                buffer.clear()
                break

            if len(buffer) - start < PACKET_SIZE:
                break

            # Extract full packet
            packet = buffer[start:start + PACKET_SIZE]

            # Remove processed bytes
            del buffer[:start + PACKET_SIZE]

            print("\nPacket:", parse_packet(packet))

            # Split fields
            payload = packet[1:-1]  # exclude start byte and crc
            recv_crc = packet[-1]

            print("Payload:", " ".join(f"{b:02X}" for b in payload))
            print("Received CRC:", f"{recv_crc:02X}")

            # ---- CRC CHECK ----
            calc_crc = crc8(payload)
            print("Calculated CRC:", f"{calc_crc:02X}")

            if calc_crc == recv_crc:
                print("CRC OK ✓")
            else:
                print("CRC FAIL ✗")

            print("-----")

if __name__ == "__main__":
    main()
