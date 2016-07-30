const int TRACK_PIN = 2;
#define TRACK_BUS_BUFFER_SIZE 16

typedef struct TrackBusEntry {
  unsigned char id;
  bool function;
  char speed; // 1 is reversal command, 2-15 are real speeds.
} TrackBusEntry;

TrackBusEntry track_bus_buffer[TRACK_BUS_BUFFER_SIZE];
unsigned int track_bus_front = 0;
unsigned int track_bus_back = 0;
unsigned int track_bus_size = 0;

void setup() {
  pinMode(TRACK_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TRACK_PIN), track_ISR, CHANGE);
}

static char read_trit(const char* buff){
  char a = *buff, b = *(buff + 1);
  if (!a && !b) return 0;
  if (a && b) return 1;
  if (a && !b) return 2;
  return ~0;
}

static void track_ISR() {
  static char packet[18], packet_ptr;
  static long last_rise_ts, last_fall_ts;
  static bool drop;

  long now = micros();
  bool pin_val = digitalRead(TRACK_PIN);
  if (pin_val) {
    last_rise_ts = now;
    return;
  }

  if (now - last_fall_ts > 1000) {
    // Quiet time between packets - resynchronize...
    packet_ptr = 0;
  }

  // For whatever reason, each bit is seen in duplicate.
  // So, we drop half of them.
  drop = !drop;
  if (!drop) {
    // Long pulses (~200us) are 1, short pulses (~25us) are 0.
    packet[packet_ptr++] = (now - last_rise_ts) > 100;
    // Packets have 18 bits.
    if (packet_ptr == 18) {
      packet_ptr = 0;
      char a_1 = read_trit(packet);
      char a_2 = read_trit(packet + 2);
      char a_3 = read_trit(packet + 4);
      char a_4 = read_trit(packet + 6);
      char addr = (a_1 + a_2 * 3 + a_3 * 9 + a_4 * 27);

      bool func = read_trit(packet + 8) == 1;

      // NB there is more data we are ignoring here - e.g. absolute direction.
      char s_a = *(packet + 10);
      char s_b = *(packet + 12);
      char s_c = *(packet + 14);
      char s_d = *(packet + 16);
      char speed = (s_d << 3) | (s_c << 2) | (s_b << 1) | s_a;

      // Drop packets if the buffer is full.
      if (track_bus_size + 1 < TRACK_BUS_BUFFER_SIZE) {
        track_bus_buffer[track_bus_back++] = (TrackBusEntry){addr, func, speed};
        if (track_bus_back == TRACK_BUS_BUFFER_SIZE) {
          track_bus_back = 0;
        }
        track_bus_size++;
      }
    }
  }

  if (!pin_val && !drop) {
    last_fall_ts = now;
  }
}

void loop() {
  while (track_bus_size) {
    TrackBusEntry entry = track_bus_buffer[track_bus_front++];
    if (track_bus_front == TRACK_BUS_BUFFER_SIZE) {
      track_bus_front = 0;
    }
    track_bus_size--;

    // Do things here.
  }
}
