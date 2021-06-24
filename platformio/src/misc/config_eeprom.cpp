#include "config_eeprom.h"

#include <hardware/flash.h>
#include <hardware/sync.h>
#include <stdio.h>

#include "acquisition/analyzer.h"
#include "misc/CRC32.h"
#include "misc/elapsed.h"
#include "pico/stdlib.h"

// TODO: Revisit the address choice.
constexpr uint32_t kFlashBaseAddress = 0x1009F000;
constexpr uint32_t kFlashBaseOffset = kFlashBaseAddress - XIP_BASE;
constexpr size_t kEraseSize = 4096;
constexpr size_t kWriteSize = 256;

const uint8_t* kReadBase = (uint8_t*)kFlashBaseAddress;
static uint8_t write_buffer[kWriteSize];

namespace config_eeprom {

// Size should be <= kWriteSize.
// Setting initial values for determinism.
struct ConfigPayload {
  // Acquisition channels offsets.
  int16_t offset1 = 0;
  int16_t offset2 = 0;
  // Acquisition direction flag.
  bool reverse_direction = false;
  // backlight level, 0 means not set
  uint8_t backlight_percents = 100;
  // Reserve for maintaining a fixed size of 44.
  // Always write as 0.
  uint8_t reserved[32] = {};
};

// sizeof() = 40 as of Jan 2021.
struct ConfigPacket {
  uint32_t crc;
  ConfigPayload payload;
};

// Use this as default package value.
static const ConfigPayload kDefaultConfigPayload = {
    .offset1 = 1900,
    .offset2 = 1900,
    .reverse_direction = false,
    .backlight_percents = 100,
};

void check_packet_size() {
  // We need to preserve packet size of 44 for backward comatibility.
  // Otherwise the config packet will be reset to defaults.
  printf("Eeprom packet: actual_size: %u, expected_size: %u\n",
         sizeof(ConfigPacket), 44);
}

// packet.reserved should be pre cleared.
static uint32_t compute_crc(const ConfigPayload& payload) {
  // This is a light class. Ok to have on stack.
  CRC32 crc;
  crc.update(&payload, 1);
  return crc.finalize();
}

// Does nothing if no change.
static bool write_buffer_to_flash() {
  // Assuming second core is not running so no need to block it from accessing
  // the flash.

  // Do nothing if no change. E.g. when existing the Settings screen
  // with no change.
  if (memcmp(write_buffer, (void*)kFlashBaseAddress, kWriteSize) == 0) {
    printf("No change in settings.");
    return true;
  }

  Elapsed elapsed;
  printf("\nWriting\n");
  const uint32_t saved_state = save_and_disable_interrupts();
  flash_range_erase(kFlashBaseOffset, kEraseSize);
  flash_range_program(kFlashBaseOffset, write_buffer, kWriteSize);
  restore_interrupts(saved_state);
  uint32_t millis = elapsed.elapsed_millis();
  printf("Writing took %lu millis\n", millis);

  const bool written_ok =
      memcmp(write_buffer, (void*)kFlashBaseAddress, kWriteSize) == 0;
  last_status = written_ok ? "Config written OK" : "Config writing failed";
  return written_ok;
}

// Settings and backlight_percents are nullable.
static void copy_settings(const ConfigPayload& payload,
                          analyzer::Settings* analyzer_settings,
                          uint8_t* backlight_percents) {
  if (analyzer_settings != nullptr) {
    analyzer_settings->offset1 = payload.offset1;
    analyzer_settings->offset2 = payload.offset2;
    analyzer_settings->reverse_direction = payload.reverse_direction;
  }
  if (backlight_percents != nullptr) {
    *backlight_percents = payload.backlight_percents;
  }
}

const char* last_status = "NONE";

// Returns true if read ok. Otherwise default settings are returned.
bool read_settings(analyzer::Settings* analyzer_settings,
                   uint8_t* backlight_percents) {
  const ConfigPacket* packet = (ConfigPacket*)kFlashBaseAddress;

  const uint32_t crc = compute_crc(packet->payload);

  if (crc != packet->crc) {
    copy_settings(kDefaultConfigPayload, analyzer_settings, backlight_percents);
    last_status = "CRC_ERROR";
    return false;
  }

  copy_settings(packet->payload, analyzer_settings, backlight_percents);
  // We introduced this setting in June 2021. Until than this
  // value was resreved and was set to zero.
  if (*backlight_percents == 0) {
    *backlight_percents = 100;
  }
  last_status = "OK";
  return true;
}

// Returns true if written ok.
// Does nothing if no change.
bool write_settings(const analyzer::Settings& analyzer_settings,
                    uint8_t backlight_percents) {
  memset(write_buffer, 0, sizeof(write_buffer));
  // Assuming packet is smaller than write buffer.
  ConfigPacket* packet = (ConfigPacket*)&write_buffer;

  // Populate payload.
  packet->payload.offset1 = analyzer_settings.offset1;
  packet->payload.offset2 = analyzer_settings.offset2;
  packet->payload.reverse_direction = analyzer_settings.reverse_direction;

  packet->payload.backlight_percents = backlight_percents;

  // Compute checkscum.
  packet->crc = compute_crc(packet->payload);

  //  Does nothing is no change.
  return write_buffer_to_flash();
}

// For testing. Writes and verifies settings.
// void test() {
//   static uint8_t seed = 0;
//   for (int i = 0; i < kWriteSize; i++) {
//     write_buffer[i] = i + seed;  // overflow ok.
//   }
//   seed++;

//   Elapsed elapsed;
//   printf("\nWriting\n");
//   const uint32_t saved_state = save_and_disable_interrupts();
//   flash_range_erase(kFlashBaseOffset, kEraseSize);
//   flash_range_program(kFlashBaseOffset, write_buffer, kWriteSize);
//   restore_interrupts(saved_state);
//   uint32_t millis = elapsed.elapsed_millis();
//   printf("\nVerifying\n");

//   for (int i = 0; i < 256; i++) {
//     printf("%02x ", kReadBase[i]);
//     if (i % 16 == 15) {
//       printf("\n");
//     }
//   }
//   printf("Writing took %lu millis\n", millis);
// }

}  // namespace config_eeprom