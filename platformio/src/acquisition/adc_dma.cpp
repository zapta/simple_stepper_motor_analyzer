// Implementation of the acquisition module. It uses the ADC/DMA
// interrupts to process the ADC sampling.


#include "adc_dma.h"

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "io.h"

// Analyzer provides this callback.
namespace analyzer {
void isr_handle_one_sample(const uint16_t raw_v1, const uint16_t raw_v2);
}

namespace adc_dma {

void disable_irq() {
  // irq_set_mask_enabled(1 << DMA_IRQ_0 | 1 << DMA_IRQ_1, false);
  irq_set_mask_enabled(1 << DMA_IRQ_0, false);
}

void enable_irq() {
  // irq_set_mask_enabled(1 << DMA_IRQ_0 | 1 << DMA_IRQ_1, true);
  irq_set_mask_enabled(1 << DMA_IRQ_0, true);
}

//------------------------------------------

// void isr_handle_one_sample(const uint16_t raw_v1, const uint16_t raw_v2) ;

#define ADC_CHANNEL_1 0
#define ADC_CHANNEL_2 1

// Assumed to have a sizeof() of exactly 4 bytes.
struct SamplePair {
  uint16_t v1;
  uint16_t v2;
};

#define BUFFER_SIZE 512
#define BUFFER_DMA_CCOUNT BUFFER_SIZE * 2
#define BUFFER_SIZE_BYTES BUFFER_SIZE * 4
// Number of bytes for 512x4 bytes address.
#define CAPTURE_RING_BITS 11

// Aligned for the DMA ring address warp.
static SamplePair capture_buf1[BUFFER_SIZE]
    __attribute__((aligned(BUFFER_SIZE_BYTES)));
static SamplePair capture_buf2[BUFFER_SIZE]
    __attribute__((aligned(BUFFER_SIZE_BYTES)));

static uint dma_chan1;
static uint dma_chan2;

static int irq_counter1 = 0;
static int irq_counter2 = 0;
static int irq_counter3 = 0;  // Should never count

// Called when one of the DMA buffers is full
static void dma_handler() {
  LED1_ON;
  // DMA chan 1.
  if (dma_hw->ints0 & 1u << dma_chan1) {
    irq_counter1++;
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << dma_chan1;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      analyzer::isr_handle_one_sample(capture_buf1[i].v1, capture_buf1[i].v2);
    }
    // DMA chan 2.
  } else if (dma_hw->ints0 & 1u << dma_chan2) {
    irq_counter2++;
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << dma_chan2;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      analyzer::isr_handle_one_sample(capture_buf2[i].v1, capture_buf2[i].v2);
    }
  } else {
    // Should neve happen.2
    irq_counter3++;
  }
  LED1_OFF;
}

void setup() {
  // -------ADC init

  // Make sure GPIO is high impedance, no pullups, etc.
  adc_gpio_init(26 + ADC_CHANNEL_1);
  adc_gpio_init(26 + ADC_CHANNEL_2);

  adc_init();

  // This determines the first channel that will be scanned in
  // each round robin cycle.
  adc_select_input(ADC_CHANNEL_1);
  // Alternating ADC sampling.
  adc_set_round_robin(1 << ADC_CHANNEL_1 | 1 << ADC_CHANNEL_2);

  adc_fifo_setup(true,  // Write each completed conversion to the sample FIFO
                 true,  // Enable DMA data request (DREQ)
                 1,  // DREQ (and IRQ) asserted when at least 1 sample present
                 true,  // Collect also the error bit.
                 false  // Do not reduce samples to 8 bits.
  );

  // Determines the ADC sampling rate as a divisor of the basic
  // 48Mhz clock. Set to have 100k sps on each of the two ADC
  // channels.
  adc_set_clkdiv(240 - 1);  // Total rate 200k sps.
                            // adc_set_clkdiv(240 - 1);  // Total rate 200k sps.

  // --------------- DMA

  dma_chan1 = dma_claim_unused_channel(true);
  dma_chan2 = dma_claim_unused_channel(true);

  // Chan 1
  {
    dma_channel_config dma_config1 = dma_channel_get_default_config(dma_chan1);
    channel_config_set_transfer_data_size(&dma_config1, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config1, false);  // ADC fifo
    channel_config_set_write_increment(&dma_config1, true);  // RAM buffer.
    // channel_config_set_write_increment(&dma_config1, true);
    // Wrap to begining of buffer. Assuming buffer is well alligned.
    channel_config_set_ring(&dma_config1, true, CAPTURE_RING_BITS);
    // Paced by ADC genered requests.
    channel_config_set_dreq(&dma_config1, DREQ_ADC);
    // When done, start the other channel.
    channel_config_set_chain_to(&dma_config1, dma_chan2);
    // Using interrupt channel 0
    dma_channel_set_irq0_enabled(dma_chan1, true);
    dma_channel_configure(dma_chan1, &dma_config1,
                          capture_buf1,       // dst
                          &adc_hw->fifo,      // src
                          BUFFER_DMA_CCOUNT,  // transfer count
                          true                // start immediately
    );
  }

  // Chan 2
  {
    dma_channel_config dma_config2 = dma_channel_get_default_config(dma_chan2);
    channel_config_set_transfer_data_size(&dma_config2, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config2, false);
    channel_config_set_write_increment(&dma_config2, true);
    channel_config_set_ring(&dma_config2, true, CAPTURE_RING_BITS);
    channel_config_set_dreq(&dma_config2, DREQ_ADC);
    // When done, start the other channel.
    channel_config_set_chain_to(&dma_config2, dma_chan1);
    dma_channel_set_irq0_enabled(dma_chan2, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_handler2);
    //  irq_set_enabled(DMA_IRQ_1, true);
    dma_channel_configure(dma_chan2, &dma_config2,
                          capture_buf2,       // dst
                          &adc_hw->fifo,      // src
                          BUFFER_DMA_CCOUNT,  // transfer count
                          false               // Do not start immediately
    );
  }

  // Set an IRQ handler for both channesl.
  irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);

  enable_irq();

  // Start the ADC free run sampling.
  adc_run(true);
}

void dump_state() {
  printf("DMA counters: %lu, %lu, %lu\n", irq_counter1, irq_counter2,
         irq_counter3);
}

}  // namespace adc_dma
