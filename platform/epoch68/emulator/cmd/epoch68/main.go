package main

/*
#cgo CFLAGS: -I${SRCDIR}/../../../../../build/user/tools/include/musashi -I${SRCDIR}/../../../../../third_party/miniaudio
#cgo LDFLAGS: -Wl,-max_default_common_align,0x4000 -L${SRCDIR}/../../../../../build/user/tools/lib -lmusashi -lm
#cgo darwin LDFLAGS: -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework CoreFoundation
#cgo linux LDFLAGS: -ldl -lpthread
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_RESOURCE_MANAGER
#include "miniaudio.h"
#include "m68k.h"
#include "ram.h"

#define EPOCH68_LOG_INSN_PREFIX 0

extern void epoch68_uart_tx(unsigned int channel, unsigned int value, unsigned int pc);
extern int epoch68_uart_rx_available(unsigned int channel);
extern unsigned int epoch68_uart_rx_read(unsigned int channel);

enum {
	epoch68_cpu_address_mask = 0x00ffffff,
	epoch68_window_size = 256 * 1024,
	epoch68_window_mask = epoch68_window_size - 1,
	epoch68_disk_size = 16 * 1024 * 1024,
	epoch68_disk_pages = epoch68_disk_size / epoch68_window_size,
	epoch68_disk_click_track_size = 16 * 1024,
	epoch68_disk_click_queue_size = 32,
	epoch68_disk_click_min_interval_ms = 20,
	epoch68_rom_size = 64 * 1024,
	epoch68_rom_mask = epoch68_rom_size - 1,
	epoch68_mmio_mask = 64 * 1024 - 1,
	epoch68_supervisor_sr_bit = 0x2000,

	epoch68_y0 = 0,
	epoch68_y1 = 1,
	epoch68_y2 = 2,
	epoch68_y3 = 3,
	epoch68_y4 = 4,
	epoch68_y5 = 5,
	epoch68_y6 = 6,

	epoch68_kernel_metadata_page = 63,

	epoch68_trace_opcode = 0x4afc,
	epoch68_trace_off = 0xe680,
	epoch68_trace_compact = 0xe681,
	epoch68_trace_full = 0xe682,
	epoch68_disk_save = 0xe683,
	epoch68_user_trace_string = 0xe684,
	epoch68_user_trace_hex32 = 0xe685,
	epoch68_kernel_debug_string = 0xe686,
	epoch68_kernel_debug_hex32 = 0xe687,
	epoch68_kernel_debug_string_hex32 = 0xe688,
	epoch68_kernel_debug_dump512 = 0xe689,
	epoch68_trace_page63_writes_on = 0xe68a,
	epoch68_trace_page63_writes_off = 0xe68b,
	epoch68_trace_full_8192 = 0xe68c,
	epoch68_trace_user_full = 0xe68d,
	epoch68_watch_shell_arena = 0xe68e,

	epoch68_fault_address = 0x10,
	epoch68_fault_pc = 0x14,
	epoch68_fault_access = 0x18,
	epoch68_fault_size = 0x19,
	epoch68_fault_mode = 0x1a,
	epoch68_fault_y = 0x1b,
	epoch68_timer_control = 0x20,
	epoch68_timer_enable = 0x01,
	epoch68_machine_control = 0x21,
	epoch68_machine_return_monitor = 0x01,
	epoch68_mm58167_base = 0x40,
	epoch68_mm58167_size = 0x20,
	epoch68_mm58167_sec_units = 0,
	epoch68_mm58167_sec_tens = 1,
	epoch68_mm58167_min_units = 2,
	epoch68_mm58167_min_tens = 3,
	epoch68_mm58167_hour_units = 4,
	epoch68_mm58167_hour_tens = 5,
	epoch68_mm58167_weekday = 6,
	epoch68_mm58167_day_units = 7,
	epoch68_mm58167_day_tens = 8,
	epoch68_mm58167_month_units = 9,
	epoch68_mm58167_month_tens = 10,
	epoch68_mm58167_year_units = 11,
	epoch68_mm58167_year_tens = 12,
	epoch68_mm58167_status = 13,

	epoch68_uart_base = 0x100,
	epoch68_uart_channels = 6,
	epoch68_uart_stride = 8,
	epoch68_uart_size = epoch68_uart_channels * epoch68_uart_stride,
	epoch68_uart_rbr_thr_dll = 0,
	epoch68_uart_ier_dlm = 1,
	epoch68_uart_iir_fcr = 2,
	epoch68_uart_lcr = 3,
	epoch68_uart_mcr = 4,
	epoch68_uart_lsr = 5,
	epoch68_uart_msr = 6,
	epoch68_uart_scr = 7,
	epoch68_uart_fifo_size = 16,
	epoch68_uart_lcr_dlab = 0x80,
	epoch68_uart_ier_rx = 0x01,
	epoch68_uart_ier_tx = 0x02,
	epoch68_uart_fcr_enable = 0x01,
	epoch68_uart_fcr_clear_rx = 0x02,
	epoch68_uart_fcr_clear_tx = 0x04,
	epoch68_uart_iir_none = 0x01,
	epoch68_uart_iir_tx = 0x02,
	epoch68_uart_iir_rx = 0x04,
	epoch68_uart_iir_fifo_enabled = 0xc0,
	epoch68_uart_lsr_dr = 0x01,
	epoch68_uart_lsr_oe = 0x02,
	epoch68_uart_lsr_thre = 0x20,
	epoch68_uart_lsr_temt = 0x40,
	epoch68_uart_irq_level = 5,

	epoch68_access_read = 1,
	epoch68_access_write = 2,
	epoch68_fc_supervisor = 4,
	epoch68_fc_supervisor_program = 6,

	epoch68_debug_string = 0,
	epoch68_debug_hex32 = 1,
	epoch68_debug_string_hex32 = 2,

	epoch68_kstack_monitor_max_slots = 256,
	epoch68_watch_write_max_ranges = 64
};

struct epoch68_kstack_record {
	uint32_t max_used;
	uint32_t min_sp;
	uint32_t pc;
	uint32_t sr;
	uint32_t irq;
	uint64_t insn;
};

struct epoch68_watch_write_range {
	uint32_t start;
	uint32_t end;
};

struct epoch68_uart {
	uint8_t ier;
	uint8_t iir;
	uint8_t fcr;
	uint8_t lcr;
	uint8_t mcr;
	uint8_t lsr;
	uint8_t msr;
	uint8_t scr;
	uint8_t dll;
	uint8_t dlm;
	uint8_t rx_fifo[epoch68_uart_fifo_size];
	uint8_t rx_head;
	uint8_t rx_tail;
	uint8_t rx_count;
};

static uint8_t epoch68_disk[epoch68_disk_size];
static uint8_t epoch68_rom[epoch68_rom_size];
static char epoch68_disk_image_path[1024];
static size_t epoch68_disk_image_size;
static uint8_t epoch68_system_page;
static uint8_t epoch68_mirror1_page;
static uint8_t epoch68_mirror2_page;
static uint8_t epoch68_disk_page;
static uint8_t epoch68_boot_overlay;
static uint8_t epoch68_trace_mode;
static uint8_t epoch68_trace_page63_writes;
static uint8_t epoch68_irq_pending_mask;
static uint8_t epoch68_current_irq_level;
static uint8_t epoch68_timer_enabled;
static int64_t epoch68_mm58167_offset;
static uint8_t epoch68_current_access_size;
static uint8_t epoch68_current_fc;
static uint8_t epoch68_watch_write_enabled;
static uint8_t epoch68_kstack_monitor_enabled;
static uint8_t epoch68_kstack_monitor_dumped;
static uint32_t epoch68_watch_write_count;
static uint32_t epoch68_kstack_base;
static uint32_t epoch68_kstack_slot_size;
static uint32_t epoch68_kstack_slots;
static uint32_t epoch68_last_fault_address;
static uint32_t epoch68_last_fault_pc;
static uint8_t epoch68_last_fault_access;
static uint8_t epoch68_last_fault_size;
static uint8_t epoch68_last_fault_mode;
static uint8_t epoch68_last_fault_y;
static struct epoch68_uart epoch68_uarts[epoch68_uart_channels];
static uint64_t epoch68_instruction_count;
static uint64_t epoch68_trace_sample = 1;
static uint64_t epoch68_trace_stop_instruction;
static int epoch68_log_line_start = 1;
#define epoch68_pc_dump_max 64
struct epoch68_pc_dump_trigger {
	uint32_t address;
	uint8_t armed;
};
static struct epoch68_pc_dump_trigger epoch68_pc_dump_triggers[epoch68_pc_dump_max];
static uint32_t epoch68_pc_dump_count;
static struct epoch68_watch_write_range epoch68_watch_write_ranges[epoch68_watch_write_max_ranges];
static struct epoch68_kstack_record epoch68_kstack_records[epoch68_kstack_monitor_max_slots];
static uint32_t epoch68_window_offset(uint32_t address);

struct epoch68_disk_click_event {
	float gain;
	float frequency;
	float noise_mix;
	uint32_t samples;
};

static ma_device epoch68_disk_click_device;
static struct epoch68_disk_click_event epoch68_disk_click_queue[epoch68_disk_click_queue_size];
static _Atomic unsigned int epoch68_disk_click_queue_read;
static _Atomic unsigned int epoch68_disk_click_queue_write;
static uint8_t epoch68_disk_click_enabled;
static uint8_t epoch68_disk_click_device_started;
static uint8_t epoch68_disk_click_track_valid;
static uint32_t epoch68_disk_click_last_track;
static uint64_t epoch68_disk_click_last_time_ms;
static uint32_t epoch68_disk_click_noise = 0x68c0ffeeU;
static uint32_t epoch68_disk_click_samples_left;
static uint32_t epoch68_disk_click_total_samples;
static float epoch68_disk_click_gain;
static float epoch68_disk_click_frequency;
static float epoch68_disk_click_noise_mix;
static float epoch68_disk_click_phase;

static uint64_t epoch68_disk_click_time_ms(void) {
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0;
	}
	return (uint64_t)ts.tv_sec * 1000U + (uint64_t)ts.tv_nsec / 1000000U;
}

static void epoch68_disk_click_enqueue(float gain, float frequency, float noise_mix, uint32_t samples) {
	unsigned int read = atomic_load_explicit(&epoch68_disk_click_queue_read, memory_order_acquire);
	unsigned int write = atomic_load_explicit(&epoch68_disk_click_queue_write, memory_order_relaxed);

	if (write - read >= epoch68_disk_click_queue_size) {
		return;
	}
	epoch68_disk_click_queue[write % epoch68_disk_click_queue_size].gain = gain;
	epoch68_disk_click_queue[write % epoch68_disk_click_queue_size].frequency = frequency;
	epoch68_disk_click_queue[write % epoch68_disk_click_queue_size].noise_mix = noise_mix;
	epoch68_disk_click_queue[write % epoch68_disk_click_queue_size].samples = samples;
	atomic_store_explicit(&epoch68_disk_click_queue_write, write + 1, memory_order_release);
}

static int epoch68_disk_click_dequeue(struct epoch68_disk_click_event *event) {
	unsigned int read = atomic_load_explicit(&epoch68_disk_click_queue_read, memory_order_relaxed);
	unsigned int write = atomic_load_explicit(&epoch68_disk_click_queue_write, memory_order_acquire);

	if (read == write) {
		return 0;
	}
	*event = epoch68_disk_click_queue[read % epoch68_disk_click_queue_size];
	atomic_store_explicit(&epoch68_disk_click_queue_read, read + 1, memory_order_release);
	return 1;
}

static void epoch68_disk_click_callback(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
	float *samples = (float *)output;
	struct epoch68_disk_click_event event;
	ma_uint32 frame;

	(void)device;
	(void)input;
	for (frame = 0; frame < frame_count; frame++) {
		float sample = 0.0f;
		if (epoch68_disk_click_samples_left == 0 && epoch68_disk_click_dequeue(&event)) {
			epoch68_disk_click_samples_left = event.samples;
			epoch68_disk_click_total_samples = event.samples;
			epoch68_disk_click_gain = event.gain;
			epoch68_disk_click_frequency = event.frequency;
			epoch68_disk_click_noise_mix = event.noise_mix;
			epoch68_disk_click_phase = 0.0f;
		}
		if (epoch68_disk_click_samples_left != 0) {
			float envelope = (float)epoch68_disk_click_samples_left / (float)epoch68_disk_click_total_samples;
			float noise;

			epoch68_disk_click_noise = epoch68_disk_click_noise * 1664525U + 1013904223U;
			noise = (float)((epoch68_disk_click_noise >> 8) & 0xffffU) / 32768.0f - 1.0f;
			sample = epoch68_disk_click_gain * (noise * epoch68_disk_click_noise_mix +
				sinf(epoch68_disk_click_phase) * (1.0f - epoch68_disk_click_noise_mix)) * envelope * envelope;
			epoch68_disk_click_phase += 6.28318530718f * epoch68_disk_click_frequency / 48000.0f;
			epoch68_disk_click_samples_left--;
		}
		samples[frame * 2] = sample;
		samples[frame * 2 + 1] = sample * 0.92f;
	}
}

int epoch68_disk_click_start(void) {
	ma_device_config config;
	ma_result result;

	if (epoch68_disk_click_device_started) {
		return 0;
	}
	atomic_store(&epoch68_disk_click_queue_read, 0);
	atomic_store(&epoch68_disk_click_queue_write, 0);
	epoch68_disk_click_samples_left = 0;
	epoch68_disk_click_total_samples = 0;
	config = ma_device_config_init(ma_device_type_playback);
	config.playback.format = ma_format_f32;
	config.playback.channels = 2;
	config.sampleRate = 48000;
	config.dataCallback = epoch68_disk_click_callback;
	result = ma_device_init(NULL, &config, &epoch68_disk_click_device);
	if (result != MA_SUCCESS) {
		return (int)result;
	}
	result = ma_device_start(&epoch68_disk_click_device);
	if (result != MA_SUCCESS) {
		ma_device_uninit(&epoch68_disk_click_device);
		return (int)result;
	}
	epoch68_disk_click_device_started = 1;
	epoch68_disk_click_enabled = 1;
	// 300 ms startup tone confirms that the host audio path is live.
	epoch68_disk_click_enqueue(0.25f, 523.25f, 0.0f, 14400);
	return 0;
}

void epoch68_disk_click_stop(void) {
	if (!epoch68_disk_click_device_started) {
		return;
	}
	epoch68_disk_click_enabled = 0;
	ma_device_uninit(&epoch68_disk_click_device);
	epoch68_disk_click_device_started = 0;
}

static void epoch68_disk_click_access(uint8_t page, uint32_t address, uint8_t is_write) {
	uint32_t absolute;
	uint32_t track;
	uint32_t distance;
	uint64_t now;
	float gain;
	float frequency;

	if (!epoch68_disk_click_enabled) {
		return;
	}
	absolute = (uint32_t)(page % epoch68_disk_pages) * epoch68_window_size + epoch68_window_offset(address);
	track = absolute / epoch68_disk_click_track_size;
	if (epoch68_disk_click_track_valid && track == epoch68_disk_click_last_track) {
		return;
	}
	distance = epoch68_disk_click_track_valid && track > epoch68_disk_click_last_track ?
		track - epoch68_disk_click_last_track : epoch68_disk_click_track_valid ?
		epoch68_disk_click_last_track - track : 32;
	epoch68_disk_click_last_track = track;
	epoch68_disk_click_track_valid = 1;
	now = epoch68_disk_click_time_ms();
	if (now != 0 && epoch68_disk_click_last_time_ms != 0 &&
	    now - epoch68_disk_click_last_time_ms < epoch68_disk_click_min_interval_ms) {
		return;
	}
	epoch68_disk_click_last_time_ms = now;
	gain = 0.035f + (distance > 128 ? 0.12f : (float)distance * 0.0009375f);
	frequency = (is_write ? 820.0f : 1120.0f) + (distance > 96 ? 180.0f : (float)distance * 1.875f);
	epoch68_disk_click_enqueue(gain, frequency, 0.75f, 1440);
}

unsigned int m68k_read_memory_8(unsigned int address);
static void epoch68_log_prefix(void);
static int epoch68_is_supervisor(void);

void epoch68_unbuffer_stdio(void) {
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

void epoch68_set_trace_sample(uint64_t sample) {
	epoch68_trace_sample = sample;
}

void epoch68_force_trace_full(void) {
	epoch68_trace_mode = 2;
}

void epoch68_force_trace_compact(void) {
	epoch68_trace_mode = 1;
}

void epoch68_clear_watch_writes(void) {
	epoch68_watch_write_enabled = 0;
	epoch68_watch_write_count = 0;
}

void epoch68_add_watch_write(uint32_t start, uint32_t length) {
	struct epoch68_watch_write_range *range;

	if (length == 0 || epoch68_watch_write_count >= epoch68_watch_write_max_ranges) {
		return;
	}
	range = &epoch68_watch_write_ranges[epoch68_watch_write_count++];
	range->start = start & epoch68_cpu_address_mask;
	range->end = (start + length) & epoch68_cpu_address_mask;
	epoch68_watch_write_enabled = 1;
}

void epoch68_set_kstack_monitor(uint32_t base, uint32_t slot_size, uint32_t slots) {
	if (slots > epoch68_kstack_monitor_max_slots) {
		slots = epoch68_kstack_monitor_max_slots;
	}
	epoch68_kstack_base = base & epoch68_cpu_address_mask;
	epoch68_kstack_slot_size = slot_size;
	epoch68_kstack_slots = slots;
	epoch68_kstack_monitor_enabled = (slot_size != 0 && slots != 0) ? 1 : 0;
	epoch68_kstack_monitor_dumped = 0;
	memset(epoch68_kstack_records, 0, sizeof(epoch68_kstack_records));
}

void epoch68_dump_kstack_monitor(void) {
	if (!epoch68_kstack_monitor_enabled || epoch68_kstack_monitor_dumped) {
		return;
	}
	epoch68_kstack_monitor_dumped = 1;
	printf("\n--- epoch68 kstack monitor base=%06x slot=%u slots=%u ---\n",
		epoch68_kstack_base,
		epoch68_kstack_slot_size,
		epoch68_kstack_slots);
	for (uint32_t i = 0; i < epoch68_kstack_slots; i++) {
		struct epoch68_kstack_record *r = &epoch68_kstack_records[i];
		if (r->max_used == 0) {
			continue;
		}
		printf("slot=%u max_used=%u min_sp=%06x pc=%06x sr=%04x irq=%u insn=%llu\n",
			i,
			r->max_used,
			r->min_sp,
			r->pc,
			r->sr & 0xffff,
			r->irq,
			(unsigned long long)r->insn);
	}
	printf("--- epoch68 kstack monitor end ---\n");
	fflush(stdout);
}

static void epoch68_kstack_monitor_instruction(uint32_t pc) {
	uint32_t sp;
	uint32_t end;
	uint32_t offset;
	uint32_t in_slot;
	uint32_t slot;
	uint32_t top;
	uint32_t used;
	struct epoch68_kstack_record *r;

	if (!epoch68_kstack_monitor_enabled ||
	    (m68k_get_reg(0, M68K_REG_SR) & epoch68_supervisor_sr_bit) == 0) {
		return;
	}
	sp = m68k_get_reg(0, M68K_REG_A7) & epoch68_cpu_address_mask;
	end = epoch68_kstack_base + (epoch68_kstack_slot_size * epoch68_kstack_slots);
	if (sp < epoch68_kstack_base || sp >= end) {
		return;
	}
	offset = sp - epoch68_kstack_base;
	in_slot = offset % epoch68_kstack_slot_size;
	if (in_slot == 0 && offset != 0) {
		// 68k stacks grow downward. A boundary SP is the empty top of
		// the lower slot, not a full stack in the higher slot.
		slot = (offset / epoch68_kstack_slot_size) - 1;
	} else {
		slot = offset / epoch68_kstack_slot_size;
	}
	if (slot >= epoch68_kstack_slots) {
		return;
	}
	top = epoch68_kstack_base + ((slot + 1) * epoch68_kstack_slot_size);
	used = top - sp;
	r = &epoch68_kstack_records[slot];
	if (used <= r->max_used) {
		return;
	}
	r->max_used = used;
	r->min_sp = sp;
	r->pc = pc & epoch68_cpu_address_mask;
	r->sr = m68k_get_reg(0, M68K_REG_SR);
	r->irq = epoch68_current_irq_level;
	r->insn = epoch68_instruction_count;
}

static void epoch68_trace_emit_line(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	epoch68_log_prefix();
	vprintf(fmt, ap);
	epoch68_log_line_start = 1;
	fflush(stdout);
	va_end(ap);
}

static void epoch68_log_prefix(void) {
	if (!epoch68_log_line_start) {
		return;
	}
#if EPOCH68_LOG_INSN_PREFIX
	printf("insn=%llu irq=%u ",
		(unsigned long long)epoch68_instruction_count,
		(unsigned int)epoch68_current_irq_level);
#endif
	epoch68_log_line_start = 0;
}

static void epoch68_log_putc(int ch) {
	epoch68_log_prefix();
	fputc(ch, stdout);
	if (ch == '\n') {
		epoch68_log_line_start = 1;
	}
}

static void epoch68_log_string_from_guest(unsigned int address, unsigned int maxlen) {
	for (unsigned int i = 0; i < maxlen; i++) {
		unsigned int ch = m68k_read_memory_8(address + i) & 0xff;
		if (ch == 0) {
			break;
		}
		epoch68_log_putc((int)ch);
	}
}

static uint32_t epoch68_address(uint32_t address) {
	return address & epoch68_cpu_address_mask;
}

static uint32_t epoch68_y(uint32_t address) {
	return epoch68_address(address) >> 21;
}

static uint32_t epoch68_window_offset(uint32_t address) {
	return epoch68_address(address) & epoch68_window_mask;
}

static uint32_t epoch68_rom_offset(uint32_t address) {
	return epoch68_address(address) & epoch68_rom_mask;
}

static uint8_t epoch68_page_offset(uint8_t page, uint32_t address) {
	return epoch68_ram_read8(page, epoch68_window_offset(address));
}

static void epoch68_trace_page63_write(uint8_t page, uint32_t address, uint32_t value) {
	uint32_t sr;
	uint32_t pc;

	if (!epoch68_trace_page63_writes || epoch68_ram_page(page) != epoch68_kernel_metadata_page) {
		return;
	}

	sr = m68k_get_reg(0, M68K_REG_SR);
	pc = m68k_get_reg(0, M68K_REG_PC) & epoch68_cpu_address_mask;
	epoch68_trace_emit_line("page63w mode=%c pc=%06x sr=%04x size=%u addr=%06x off=%05x val=%02x\n",
		(sr & epoch68_supervisor_sr_bit) ? 'S' : 'U',
		pc,
		sr & 0xffff,
		(unsigned int)epoch68_current_access_size,
		address & epoch68_cpu_address_mask,
		epoch68_window_offset(address),
			value & 0xff);
	}

static int epoch68_watch_write_range_matches(struct epoch68_watch_write_range *range, uint32_t address) {
	if (range->start <= range->end) {
		return address >= range->start && address < range->end;
	}
	return address >= range->start || address < range->end;
}

static void epoch68_watch_write(uint8_t page_known, uint8_t page, uint32_t address, uint32_t value) {
	uint32_t sr;
	uint32_t pc;
	uint32_t ppc;
	uint32_t a;
	uint32_t i;
	char mode;

	if (!epoch68_watch_write_enabled) {
		return;
	}
	a = address & epoch68_cpu_address_mask;
	for (i = 0; i < epoch68_watch_write_count; i++) {
		if (!epoch68_watch_write_range_matches(&epoch68_watch_write_ranges[i], a)) {
			continue;
		}
		sr = m68k_get_reg(0, M68K_REG_SR);
		pc = m68k_get_reg(0, M68K_REG_PC) & epoch68_cpu_address_mask;
		ppc = m68k_get_reg(0, M68K_REG_PPC) & epoch68_cpu_address_mask;
		mode = (sr & epoch68_supervisor_sr_bit) ? 'S' : 'U';
		if (page_known) {
			epoch68_trace_emit_line("watchw mode=%c pc=%06x ppc=%06x sr=%04x size=%u addr=%06x page=%02x off=%05x val=%02x\n",
				mode,
				pc,
				ppc,
				sr & 0xffff,
				(unsigned int)epoch68_current_access_size,
				a,
				(unsigned int)epoch68_ram_page(page),
				epoch68_window_offset(a),
				value & 0xff);
		} else {
			epoch68_trace_emit_line("watchw mode=%c pc=%06x ppc=%06x sr=%04x size=%u addr=%06x page=-- off=%05x val=%02x\n",
				mode,
				pc,
				ppc,
				sr & 0xffff,
				(unsigned int)epoch68_current_access_size,
				a,
				epoch68_window_offset(a),
				value & 0xff);
		}
		return;
	}
}

static void epoch68_set_page_offset(uint8_t page, uint32_t address, uint32_t value) {
	epoch68_trace_page63_write(page, address, value);
	epoch68_watch_write(1, page, address, value);
	epoch68_ram_write8(page, epoch68_window_offset(address), (uint8_t)value);
}

static uint8_t epoch68_disk_offset(uint8_t page, uint32_t address) {
	epoch68_disk_click_access(page, address, 0);
	return epoch68_disk[((uint32_t)(page % epoch68_disk_pages) * epoch68_window_size) + epoch68_window_offset(address)];
}

static void epoch68_set_disk_offset(uint8_t page, uint32_t address, uint32_t value) {
	epoch68_disk_click_access(page, address, 1);
	epoch68_watch_write(0, 0, address, value);
	epoch68_disk[((uint32_t)(page % epoch68_disk_pages) * epoch68_window_size) + epoch68_window_offset(address)] = (uint8_t)value;
}

static int epoch68_is_supervisor(void) {
	return (epoch68_current_fc & epoch68_fc_supervisor) != 0;
}

static void epoch68_set_fc(unsigned int fc) {
	epoch68_current_fc = (uint8_t)(fc & 7u);
}

static void epoch68_set_disk_image_path(const char *path) {
	if (path == NULL) {
		epoch68_disk_image_path[0] = '\0';
		return;
	}
	snprintf(epoch68_disk_image_path, sizeof(epoch68_disk_image_path), "%s", path);
}

static void epoch68_set_disk_image_size(size_t size) {
	if (size > epoch68_disk_size) {
		size = epoch68_disk_size;
	}
	epoch68_disk_image_size = size;
}

int epoch68_save_disk_image(void) {
	size_t size = epoch68_disk_image_size;

	if (epoch68_disk_image_path[0] == '\0') {
		fprintf(stderr, "platform/epoch68/emulator: no disk image path configured for save\n");
		fflush(stderr);
		return 0;
	}
	if (size == 0) {
		size = epoch68_disk_size;
	}

	FILE *fp = fopen(epoch68_disk_image_path, "wb");
	if (fp == NULL) {
		fprintf(stderr, "platform/epoch68/emulator: save disk %s: open failed\n", epoch68_disk_image_path);
		fflush(stderr);
		return 0;
	}
	if (fwrite(epoch68_disk, 1, size, fp) != size) {
		fprintf(stderr, "platform/epoch68/emulator: save disk %s: short write\n", epoch68_disk_image_path);
		fclose(fp);
		fflush(stderr);
		return 0;
	}
	if (fclose(fp) != 0) {
		fprintf(stderr, "platform/epoch68/emulator: save disk %s: close failed\n", epoch68_disk_image_path);
		fflush(stderr);
		return 0;
	}

	printf("platform/epoch68/emulator: saved disk to %s (%zu bytes)\n", epoch68_disk_image_path, size);
	fflush(stdout);
	return 1;
}

static uint8_t epoch68_u32_byte(uint32_t value, uint32_t byte) {
	return (uint8_t)(value >> ((3 - (byte & 3)) * 8));
}

static void epoch68_uart_reset(void) {
	for (unsigned int i = 0; i < epoch68_uart_channels; i++) {
		epoch68_uarts[i].ier = 0x00;
		epoch68_uarts[i].iir = 0x01;
		epoch68_uarts[i].fcr = 0x00;
		epoch68_uarts[i].lcr = 0x00;
		epoch68_uarts[i].mcr = 0x00;
		epoch68_uarts[i].lsr = epoch68_uart_lsr_thre | epoch68_uart_lsr_temt;
		epoch68_uarts[i].msr = 0x00;
		epoch68_uarts[i].scr = 0x00;
		epoch68_uarts[i].dll = 0x00;
		epoch68_uarts[i].dlm = 0x00;
		epoch68_uarts[i].rx_head = 0x00;
		epoch68_uarts[i].rx_tail = 0x00;
		epoch68_uarts[i].rx_count = 0x00;
	}
}

static unsigned int epoch68_uart_rx_capacity(struct epoch68_uart *uart) {
	if (uart->fcr & epoch68_uart_fcr_enable) {
		return(epoch68_uart_fifo_size);
	}
	return(1);
}

static void epoch68_uart_rx_clear(struct epoch68_uart *uart) {
	uart->rx_head = 0;
	uart->rx_tail = 0;
	uart->rx_count = 0;
	uart->lsr &= ~(epoch68_uart_lsr_dr | epoch68_uart_lsr_oe);
}

static void epoch68_uart_refill_rx(struct epoch68_uart *uart, unsigned int channel) {
	unsigned int capacity = epoch68_uart_rx_capacity(uart);

	while (uart->rx_count < capacity && epoch68_uart_rx_available(channel)) {
		uart->rx_fifo[uart->rx_tail] = (uint8_t)epoch68_uart_rx_read(channel);
		uart->rx_tail = (uint8_t)((uart->rx_tail + 1) % epoch68_uart_fifo_size);
		uart->rx_count++;
	}
}

static unsigned int epoch68_uart_rx_pop(struct epoch68_uart *uart, unsigned int channel) {
	unsigned int value;

	epoch68_uart_refill_rx(uart, channel);
	if (uart->rx_count == 0) {
		return(0x00);
	}
	value = uart->rx_fifo[uart->rx_head];
	uart->rx_head = (uint8_t)((uart->rx_head + 1) % epoch68_uart_fifo_size);
	uart->rx_count--;
	epoch68_uart_refill_rx(uart, channel);
	return(value);
}

static void epoch68_uart_update_lsr(struct epoch68_uart *uart, unsigned int channel) {
	epoch68_uart_refill_rx(uart, channel);
	if (uart->rx_count > 0) {
		uart->lsr |= epoch68_uart_lsr_dr;
	} else {
		uart->lsr &= ~epoch68_uart_lsr_dr;
	}
}

static uint8_t epoch68_highest_pending_irq_level(uint8_t mask) {
	for (int level = 7; level >= 1; level--) {
		if (mask & (1u << level)) {
			return((uint8_t)level);
		}
	}
	return(0);
}

static void epoch68_apply_irq_line(void) {
	uint8_t new_line = epoch68_highest_pending_irq_level(epoch68_irq_pending_mask);

	epoch68_current_irq_level = new_line;
	if (new_line) {
		m68k_set_irq(new_line);
	} else {
		m68k_set_irq(M68K_IRQ_NONE);
	}
}

static void epoch68_request_irq_level(unsigned int level) {
	if (level < 1 || level > 7) {
		return;
	}
	epoch68_irq_pending_mask |= (uint8_t)(1u << level);
	epoch68_apply_irq_line();
}

static void epoch68_clear_irq_level(unsigned int level) {
	if (level < 1 || level > 7) {
		return;
	}
	epoch68_irq_pending_mask &= (uint8_t)~(1u << level);
	epoch68_apply_irq_line();
}

static void epoch68_timer_reprime(void) {
	epoch68_timer_enabled = 1;
	epoch68_clear_irq_level(6);
}

void epoch68_timer_tick(void) {
	if (!epoch68_timer_enabled) {
		return;
	}
	epoch68_timer_enabled = 0;
	epoch68_request_irq_level(6);
}

static void epoch68_return_to_monitor(void) {
	epoch68_save_disk_image();
	epoch68_boot_overlay = 1;
	epoch68_irq_pending_mask = 0;
	epoch68_current_irq_level = 0;
	epoch68_timer_enabled = 0;
	m68k_set_irq(M68K_IRQ_NONE);
	m68k_pulse_reset();
}

static unsigned int epoch68_bcd_digit(unsigned int value, unsigned int tens) {
	return tens ? ((value / 10) % 10) : (value % 10);
}

static void epoch68_mm58167_fill_tm(struct tm *out) {
	time_t now;
	struct tm *tm;

	now = time(NULL) + (time_t)epoch68_mm58167_offset;
	tm = gmtime(&now);
	if (tm == NULL) {
		memset(out, 0, sizeof(*out));
		return;
	}
	*out = *tm;
}

static time_t epoch68_timegm_simple(struct tm *tm) {
	static const int mdays[12] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31,
	};
	long days = 0;
	int year = tm->tm_year + 1900;
	int month = tm->tm_mon + 1;

	for (int y = 1970; y < year; y++) {
		days += (y % 4) == 0 ? 366 : 365;
	}
	for (int m = 1; m < month; m++) {
		days += mdays[m - 1];
		if (m == 2 && (year % 4) == 0) {
			days++;
		}
	}
	days += tm->tm_mday - 1;
	return (time_t)((((days * 24L) + tm->tm_hour) * 60L + tm->tm_min) * 60L + tm->tm_sec);
}

static unsigned int epoch68_mm58167_read(uint32_t reg) {
	struct tm current;
	unsigned int year;

	epoch68_mm58167_fill_tm(&current);
	year = (unsigned int)((current.tm_year + 1900) % 100);
	switch (reg) {
	case epoch68_mm58167_sec_units:
		return epoch68_bcd_digit((unsigned int)current.tm_sec, 0);
	case epoch68_mm58167_sec_tens:
		return epoch68_bcd_digit((unsigned int)current.tm_sec, 1);
	case epoch68_mm58167_min_units:
		return epoch68_bcd_digit((unsigned int)current.tm_min, 0);
	case epoch68_mm58167_min_tens:
		return epoch68_bcd_digit((unsigned int)current.tm_min, 1);
	case epoch68_mm58167_hour_units:
		return epoch68_bcd_digit((unsigned int)current.tm_hour, 0);
	case epoch68_mm58167_hour_tens:
		return epoch68_bcd_digit((unsigned int)current.tm_hour, 1);
	case epoch68_mm58167_weekday:
		return (unsigned int)current.tm_wday;
	case epoch68_mm58167_day_units:
		return epoch68_bcd_digit((unsigned int)current.tm_mday, 0);
	case epoch68_mm58167_day_tens:
		return epoch68_bcd_digit((unsigned int)current.tm_mday, 1);
	case epoch68_mm58167_month_units:
		return epoch68_bcd_digit((unsigned int)current.tm_mon + 1, 0);
	case epoch68_mm58167_month_tens:
		return epoch68_bcd_digit((unsigned int)current.tm_mon + 1, 1);
	case epoch68_mm58167_year_units:
		return epoch68_bcd_digit(year, 0);
	case epoch68_mm58167_year_tens:
		return epoch68_bcd_digit(year, 1);
	case epoch68_mm58167_status:
		return 0;
	default:
		return 0;
	}
}

static void epoch68_mm58167_write(uint32_t reg, uint32_t value) {
	struct tm current;
	unsigned int digit = value & 0xf;
	int year;

	if (digit > 9) {
		return;
	}
	epoch68_mm58167_fill_tm(&current);
	switch (reg) {
	case epoch68_mm58167_sec_units:
		current.tm_sec = (current.tm_sec / 10) * 10 + (int)digit;
		break;
	case epoch68_mm58167_sec_tens:
		current.tm_sec = (int)digit * 10 + current.tm_sec % 10;
		break;
	case epoch68_mm58167_min_units:
		current.tm_min = (current.tm_min / 10) * 10 + (int)digit;
		break;
	case epoch68_mm58167_min_tens:
		current.tm_min = (int)digit * 10 + current.tm_min % 10;
		break;
	case epoch68_mm58167_hour_units:
		current.tm_hour = (current.tm_hour / 10) * 10 + (int)digit;
		break;
	case epoch68_mm58167_hour_tens:
		current.tm_hour = (int)digit * 10 + current.tm_hour % 10;
		break;
	case epoch68_mm58167_day_units:
		current.tm_mday = (current.tm_mday / 10) * 10 + (int)digit;
		break;
	case epoch68_mm58167_day_tens:
		current.tm_mday = (int)digit * 10 + current.tm_mday % 10;
		break;
	case epoch68_mm58167_month_units:
		current.tm_mon = ((current.tm_mon + 1) / 10) * 10 + (int)digit - 1;
		break;
	case epoch68_mm58167_month_tens:
		current.tm_mon = (int)digit * 10 + ((current.tm_mon + 1) % 10) - 1;
		break;
	case epoch68_mm58167_year_units:
		year = (current.tm_year + 1900) % 100;
		year = (year / 10) * 10 + (int)digit;
		current.tm_year = (year < 70 ? year + 2000 : year + 1900) - 1900;
		break;
	case epoch68_mm58167_year_tens:
		year = (current.tm_year + 1900) % 100;
		year = (int)digit * 10 + year % 10;
		current.tm_year = (year < 70 ? year + 2000 : year + 1900) - 1900;
		break;
	default:
		return;
	}
	epoch68_mm58167_offset = (int64_t)epoch68_timegm_simple(&current) - (int64_t)time(NULL);
}

static void epoch68_uart_refresh_irqs(void) {
	unsigned int pending = 0;

	for (unsigned int channel = 0; channel < epoch68_uart_channels; channel++) {
		struct epoch68_uart *uart = &epoch68_uarts[channel];
		epoch68_uart_update_lsr(uart, channel);
		if ((uart->ier & epoch68_uart_ier_rx) && (uart->lsr & epoch68_uart_lsr_dr)) {
			uart->iir = epoch68_uart_iir_rx;
			pending = 1;
		} else if ((uart->ier & epoch68_uart_ier_tx) &&
		    (uart->lsr & epoch68_uart_lsr_thre)) {
			uart->iir = epoch68_uart_iir_tx;
			pending = 1;
		} else {
			uart->iir = epoch68_uart_iir_none;
		}
	}

	if (pending) {
		epoch68_request_irq_level(epoch68_uart_irq_level);
	} else {
		epoch68_clear_irq_level(epoch68_uart_irq_level);
	}
}

static unsigned int epoch68_fault(uint32_t address, uint8_t access) {
	uint32_t a = epoch68_address(address);

	epoch68_last_fault_address = a;
	epoch68_last_fault_pc = m68k_get_reg(0, M68K_REG_PC) & epoch68_cpu_address_mask;
	epoch68_last_fault_access = access;
	epoch68_last_fault_size = epoch68_current_access_size;
	epoch68_last_fault_mode = epoch68_is_supervisor() ? 'S' : 'U';
	epoch68_last_fault_y = epoch68_y(a);
	if (epoch68_watch_write_enabled) {
		epoch68_trace_emit_line(
			"fault mode=%c pc=%06x addr=%06x size=%u a0=%08x a4=%08x a5=%08x a6=%08x usp=%08x\n",
			epoch68_last_fault_mode,
			epoch68_last_fault_pc,
			epoch68_last_fault_address,
			(unsigned int)epoch68_last_fault_size,
			m68k_get_reg(0, M68K_REG_A0),
			m68k_get_reg(0, M68K_REG_A4),
			m68k_get_reg(0, M68K_REG_A5),
			m68k_get_reg(0, M68K_REG_A6),
			m68k_get_reg(0, M68K_REG_USP));
	}

	m68k_pulse_bus_error();
	return 0xff;
}

void epoch68_clear_pc_dumps(void) {
	epoch68_pc_dump_count = 0;
}

void epoch68_add_pc_dump(uint32_t address) {
	if (epoch68_pc_dump_count >= epoch68_pc_dump_max) {
		return;
	}
	epoch68_pc_dump_triggers[epoch68_pc_dump_count].address =
		address & epoch68_cpu_address_mask;
	epoch68_pc_dump_triggers[epoch68_pc_dump_count].armed = 1;
	epoch68_pc_dump_count++;
}

static void epoch68_dump_registers(const char *tag, unsigned int pc) {
	unsigned int sr = m68k_get_reg(0, M68K_REG_SR);
	const char *mode = (sr & epoch68_supervisor_sr_bit) ? "S" : "U";
	unsigned int opcode = m68k_read_memory_16(pc);

	epoch68_trace_emit_line("%s %s pc=%06x opcode=%04x sr=%04x d0=%08x d1=%08x d2=%08x d3=%08x d4=%08x d5=%08x d6=%08x d7=%08x a0=%08x a1=%08x a2=%08x a3=%08x a4=%08x a5=%08x a6=%08x a7=%08x usp=%08x isp=%08x\n",
		tag,
		mode,
		pc & epoch68_cpu_address_mask,
		opcode,
		sr,
		m68k_get_reg(0, M68K_REG_D0),
		m68k_get_reg(0, M68K_REG_D1),
		m68k_get_reg(0, M68K_REG_D2),
		m68k_get_reg(0, M68K_REG_D3),
		m68k_get_reg(0, M68K_REG_D4),
		m68k_get_reg(0, M68K_REG_D5),
		m68k_get_reg(0, M68K_REG_D6),
		m68k_get_reg(0, M68K_REG_D7),
		m68k_get_reg(0, M68K_REG_A0),
		m68k_get_reg(0, M68K_REG_A1),
		m68k_get_reg(0, M68K_REG_A2),
		m68k_get_reg(0, M68K_REG_A3),
		m68k_get_reg(0, M68K_REG_A4),
		m68k_get_reg(0, M68K_REG_A5),
		m68k_get_reg(0, M68K_REG_A6),
		m68k_get_reg(0, M68K_REG_A7),
		m68k_get_reg(0, M68K_REG_USP),
		m68k_get_reg(0, M68K_REG_ISP));
}

static void epoch68_check_pc_dump(unsigned int pc) {
	uint32_t address = pc & epoch68_cpu_address_mask;

	for (uint32_t i = 0; i < epoch68_pc_dump_count; i++) {
		struct epoch68_pc_dump_trigger *trigger = &epoch68_pc_dump_triggers[i];
		if (trigger->armed && trigger->address == address) {
			unsigned int sp;

			trigger->armed = 0;
			epoch68_dump_registers("pcdump", pc);
			sp = m68k_get_reg(0, M68K_REG_A7);
			epoch68_trace_emit_line(
				"pcdump stack sp=%08x +0=%08x +4=%08x +8=%08x +c=%08x\n",
				sp,
				m68k_read_memory_32(sp),
				m68k_read_memory_32(sp + 4),
				m68k_read_memory_32(sp + 8),
				m68k_read_memory_32(sp + 12));
		}
	}
}

static void epoch68_trace_instruction(unsigned int pc) {
	epoch68_instruction_count++;
	epoch68_kstack_monitor_instruction(pc);
	epoch68_check_pc_dump(pc);
	if (epoch68_trace_stop_instruction != 0 &&
	    epoch68_instruction_count > epoch68_trace_stop_instruction) {
		epoch68_trace_mode = 0;
		epoch68_trace_stop_instruction = 0;
		epoch68_trace_emit_line("trace bounded end pc=%06x\n",
			pc & epoch68_cpu_address_mask);
	}

	if (!epoch68_trace_mode) {
		return;
	}
	if (epoch68_trace_sample == 0) {
		return;
	}
	if (epoch68_trace_sample > 1 && (epoch68_instruction_count % epoch68_trace_sample) != 0) {
		return;
	}

	unsigned int sr = m68k_get_reg(0, M68K_REG_SR);
	if (epoch68_trace_mode == 3 && (sr & epoch68_supervisor_sr_bit)) {
		return;
	}
	const char *mode = (sr & epoch68_supervisor_sr_bit) ? "S" : "U";
	unsigned int opcode = m68k_read_memory_16(pc);

	if (epoch68_trace_mode == 1) {
		epoch68_trace_emit_line("trace %s pc=%06x opcode=%04x sr=%04x d0=%08x a0=%08x\n",
			mode,
			pc & epoch68_cpu_address_mask,
			opcode,
			sr,
			m68k_get_reg(0, M68K_REG_D0),
			m68k_get_reg(0, M68K_REG_A0));
		return;
	}

	epoch68_dump_registers("trace", pc);
}

static int epoch68_handle_illegal(int opcode) {
	if ((opcode & 0xffff) != epoch68_trace_opcode) {
		return 0;
	}

	unsigned int pc = m68k_get_reg(0, M68K_REG_PC);
	unsigned int command = m68k_read_memory_16(pc);

	switch (command) {
	case epoch68_trace_off:
		epoch68_trace_mode = 0;
		epoch68_trace_stop_instruction = 0;
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_compact:
		epoch68_trace_mode = 1;
		epoch68_trace_stop_instruction = 0;
		if (epoch68_trace_sample == 0) {
			epoch68_trace_sample = 1;
		}
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_full:
		epoch68_trace_mode = 2;
		epoch68_trace_stop_instruction = 0;
		if (epoch68_trace_sample == 0) {
			epoch68_trace_sample = 1;
		}
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_full_8192:
		epoch68_trace_mode = 2;
		epoch68_trace_sample = 1;
		epoch68_trace_stop_instruction = epoch68_instruction_count + 8192;
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_user_full:
		epoch68_trace_mode = 3;
		epoch68_trace_stop_instruction = 0;
		epoch68_trace_sample = 1;
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_watch_shell_arena:
		epoch68_trace_mode = 0;
		epoch68_trace_stop_instruction = 0;
		epoch68_clear_watch_writes();
		epoch68_add_watch_write(0x67b8, 16);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_page63_writes_on:
		epoch68_trace_page63_writes = 1;
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_trace_page63_writes_off:
		epoch68_trace_page63_writes = 0;
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_disk_save:
		if (epoch68_save_disk_image()) {
			m68k_set_reg(M68K_REG_PC, pc + 2);
			return 1;
		}
		return 0;
	case epoch68_user_trace_string:
	{
		unsigned int address = m68k_get_reg(0, M68K_REG_A0);
		epoch68_log_string_from_guest(address, 4096);
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	}
	case epoch68_user_trace_hex32:
		epoch68_log_prefix();
		printf("%08x", m68k_get_reg(0, M68K_REG_D1));
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_kernel_debug_string:
	{
		unsigned int address = m68k_get_reg(0, M68K_REG_A0);
		epoch68_log_string_from_guest(address, 4096);
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	}
	case epoch68_kernel_debug_hex32:
		epoch68_log_prefix();
		printf("%08x", m68k_get_reg(0, M68K_REG_D1));
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	case epoch68_kernel_debug_string_hex32:
	{
		unsigned int address = m68k_get_reg(0, M68K_REG_A0);
		epoch68_log_string_from_guest(address, 4096);
		printf("%08x\n", m68k_get_reg(0, M68K_REG_D1));
		epoch68_log_line_start = 1;
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	}
	case epoch68_kernel_debug_dump512:
	{
		unsigned int address = m68k_get_reg(0, M68K_REG_A0) & epoch68_cpu_address_mask;

		epoch68_log_prefix();
		printf("dump %06x\n", address);
		epoch68_log_line_start = 1;
		for (unsigned int row = 0; row < 512; row += 16) {
			epoch68_log_prefix();
			printf("%06x  ", (address + row) & epoch68_cpu_address_mask);
			for (unsigned int col = 0; col < 16; col++) {
				unsigned int ch = m68k_read_memory_8(address + row + col) & 0xff;
				printf("%02x", ch);
				if (col == 7) {
					printf("  ");
				} else {
					printf(" ");
				}
			}
			printf(" |");
			for (unsigned int col = 0; col < 16; col++) {
				unsigned int ch = m68k_read_memory_8(address + row + col) & 0xff;
				if (ch >= 32 && ch <= 126) {
					fputc((int)ch, stdout);
				} else {
					fputc('.', stdout);
				}
			}
			printf("|\n");
			epoch68_log_line_start = 1;
		}
		fflush(stdout);
		m68k_set_reg(M68K_REG_PC, pc + 2);
		return 1;
	}
	default:
		return 0;
	}
}

static int epoch68_handle_trap(int trap) {
	if (trap == 13) {
		fputc((int)(m68k_get_reg(0, M68K_REG_D0) & 0xff), stdout);
		fflush(stdout);
		return 1;
	}

	if (trap == 14) {
		unsigned int address = m68k_get_reg(0, M68K_REG_A0) & epoch68_cpu_address_mask;

		epoch68_log_prefix();
		printf("dump %06x\n", address);
		epoch68_log_line_start = 1;
		for (unsigned int row = 0; row < 512; row += 16) {
			epoch68_log_prefix();
			printf("%06x  ", (address + row) & epoch68_cpu_address_mask);
			for (unsigned int col = 0; col < 16; col++) {
				unsigned int ch = m68k_read_memory_8(address + row + col) & 0xff;
				printf("%02x", ch);
				if (col == 7) {
					printf("  ");
				} else {
					printf(" ");
				}
			}
			printf(" |");
			for (unsigned int col = 0; col < 16; col++) {
				unsigned int ch = m68k_read_memory_8(address + row + col) & 0xff;
				if (ch >= 32 && ch <= 126) {
					fputc((int)ch, stdout);
				} else {
					fputc('.', stdout);
				}
			}
			printf("|\n");
			epoch68_log_line_start = 1;
		}
		fflush(stdout);
		return 1;
	}

	if (trap != 15) {
		return 0;
	}

	unsigned int command = m68k_get_reg(0, M68K_REG_D0) & 0xff;
	if (command == epoch68_debug_hex32) {
		epoch68_log_prefix();
		printf("%08x", m68k_get_reg(0, M68K_REG_D1));
		fflush(stdout);
		return 1;
	}
	if (command == epoch68_debug_string_hex32) {
		unsigned int address = m68k_get_reg(0, M68K_REG_A0);
		epoch68_log_string_from_guest(address, 4096);
		printf("%08x\n", m68k_get_reg(0, M68K_REG_D1));
		epoch68_log_line_start = 1;
		fflush(stdout);
		return 1;
	}

	unsigned int address = m68k_get_reg(0, M68K_REG_A0);
	epoch68_log_string_from_guest(address, 4096);

	fflush(stdout);
	return 1;
}

static int epoch68_int_ack(int int_level) {
	if (epoch68_irq_pending_mask & (1u << int_level)) {
		epoch68_clear_irq_level((unsigned int)int_level);
	}
	return M68K_INT_ACK_AUTOVECTOR;
}

void epoch68_request_irq(unsigned int level) {
	epoch68_request_irq_level(level);
}

void epoch68_poll_devices(void) {
	epoch68_uart_refresh_irqs();
}

void epoch68_install_callbacks(unsigned int trace_hook) {
	if (trace_hook) {
		m68k_set_instr_hook_callback(epoch68_trace_instruction);
	} else {
		m68k_set_instr_hook_callback(NULL);
	}
	m68k_set_illg_instr_callback(epoch68_handle_illegal);
	m68k_set_trap_instr_callback(epoch68_handle_trap);
	m68k_set_int_ack_callback(epoch68_int_ack);
	m68k_set_fc_callback(epoch68_set_fc);
}

void epoch68_mem_clear(void) {
	epoch68_ram_clear();
	memset(epoch68_disk, 0, sizeof(epoch68_disk));
	memset(epoch68_rom, 0xff, sizeof(epoch68_rom));
	epoch68_system_page = 0;
	epoch68_mirror1_page = 0;
	epoch68_mirror2_page = 0;
	epoch68_disk_page = 0;
	epoch68_disk_click_track_valid = 0;
	epoch68_disk_click_last_time_ms = 0;
	epoch68_boot_overlay = 1;
	epoch68_current_fc = epoch68_fc_supervisor_program;
	epoch68_trace_mode = 0;
	epoch68_trace_stop_instruction = 0;
	epoch68_trace_page63_writes = 0;
	epoch68_irq_pending_mask = 0;
	epoch68_current_irq_level = 0;
	epoch68_timer_enabled = 0;
	epoch68_current_access_size = 1;
	epoch68_last_fault_address = 0;
	epoch68_last_fault_pc = 0;
	epoch68_last_fault_access = 0;
	epoch68_last_fault_size = 0;
	epoch68_last_fault_mode = 0;
	epoch68_last_fault_y = 0;
	epoch68_disk_image_size = 0;
	epoch68_instruction_count = 0;
	epoch68_log_line_start = 1;
	epoch68_uart_reset();
}

void epoch68_rom_write8(uint32_t address, uint32_t value) {
	epoch68_rom[epoch68_rom_offset(address)] = (uint8_t)value;
}

void epoch68_disk_write8(uint32_t address, uint32_t value) {
	if (address >= epoch68_disk_size) {
		return;
	}
	epoch68_disk[address] = (uint8_t)value;
}

void epoch68_rom_write16(uint32_t address, uint32_t value) {
	epoch68_rom_write8(address, value >> 8);
	epoch68_rom_write8(address + 1, value);
}

void epoch68_rom_write32(uint32_t address, uint32_t value) {
	epoch68_rom_write16(address, value >> 16);
	epoch68_rom_write16(address + 2, value);
}

static unsigned int epoch68_read_mmio(uint32_t address) {
	uint32_t offset = address & epoch68_mmio_mask;

	if (offset >= epoch68_uart_base && offset < epoch68_uart_base + epoch68_uart_size) {
		uint32_t uart_offset = offset - epoch68_uart_base;
		unsigned int channel = uart_offset / epoch68_uart_stride;
		struct epoch68_uart *uart = &epoch68_uarts[channel];
		uint32_t reg = uart_offset & (epoch68_uart_stride - 1);

		switch (reg) {
		case epoch68_uart_rbr_thr_dll: {
			unsigned int value;

			if (uart->lcr & epoch68_uart_lcr_dlab) {
				return uart->dll;
			}
			value = epoch68_uart_rx_pop(uart, channel);
			epoch68_uart_refresh_irqs();
			return value;
		}
		case epoch68_uart_ier_dlm:
			return (uart->lcr & epoch68_uart_lcr_dlab) ? uart->dlm : uart->ier;
		case epoch68_uart_iir_fcr:
			return uart->iir |
			    ((uart->fcr & epoch68_uart_fcr_enable) ? epoch68_uart_iir_fifo_enabled : 0);
		case epoch68_uart_lcr:
			return uart->lcr;
		case epoch68_uart_mcr:
			return uart->mcr;
		case epoch68_uart_lsr:
			epoch68_uart_refresh_irqs();
			return uart->lsr;
		case epoch68_uart_msr:
			return uart->msr;
		case epoch68_uart_scr:
			return uart->scr;
		default:
			return epoch68_fault(address, epoch68_access_read);
		}
	}

	if (offset >= epoch68_mm58167_base &&
	    offset < epoch68_mm58167_base + epoch68_mm58167_size) {
		return epoch68_mm58167_read(offset - epoch68_mm58167_base);
	}

	switch (offset) {
	case 0:
		return epoch68_system_page;
	case 1:
		return epoch68_mirror1_page;
	case 2:
		return epoch68_mirror2_page;
	case 3:
		return epoch68_disk_page;
	case epoch68_fault_address:
	case epoch68_fault_address + 1:
	case epoch68_fault_address + 2:
	case epoch68_fault_address + 3:
		return epoch68_u32_byte(epoch68_last_fault_address, offset - epoch68_fault_address);
	case epoch68_fault_pc:
	case epoch68_fault_pc + 1:
	case epoch68_fault_pc + 2:
	case epoch68_fault_pc + 3:
		return epoch68_u32_byte(epoch68_last_fault_pc, offset - epoch68_fault_pc);
	case epoch68_fault_access:
		return epoch68_last_fault_access;
	case epoch68_fault_size:
		return epoch68_last_fault_size;
	case epoch68_fault_mode:
		return epoch68_last_fault_mode;
	case epoch68_fault_y:
		return epoch68_last_fault_y;
	case epoch68_timer_control:
		return epoch68_timer_enabled ? epoch68_timer_enable : 0;
	case epoch68_machine_control:
		return 0;
	default:
		return epoch68_fault(address, epoch68_access_read);
	}
}

static void epoch68_write_mmio(uint32_t address, uint32_t value) {
	uint32_t offset = address & epoch68_mmio_mask;

	if (offset >= epoch68_uart_base && offset < epoch68_uart_base + epoch68_uart_size) {
		uint32_t uart_offset = offset - epoch68_uart_base;
		struct epoch68_uart *uart = &epoch68_uarts[uart_offset / epoch68_uart_stride];
		uint32_t reg = uart_offset & (epoch68_uart_stride - 1);
		uint8_t byte = (uint8_t)value;

		switch (reg) {
		case epoch68_uart_rbr_thr_dll:
			if (uart->lcr & epoch68_uart_lcr_dlab) {
				uart->dll = byte;
				break;
			}
			uart->lsr &= ~(epoch68_uart_lsr_thre | epoch68_uart_lsr_temt);
			epoch68_uart_tx(uart_offset / epoch68_uart_stride, byte,
				m68k_get_reg(0, M68K_REG_PPC) & epoch68_cpu_address_mask);
			uart->lsr |= epoch68_uart_lsr_thre | epoch68_uart_lsr_temt;
			break;
		case epoch68_uart_ier_dlm:
			if (uart->lcr & epoch68_uart_lcr_dlab) {
				uart->dlm = byte;
			} else {
				uart->ier = byte & 0x0f;
				epoch68_uart_refresh_irqs();
			}
			break;
		case epoch68_uart_iir_fcr:
			uart->fcr = byte;
			if (byte & epoch68_uart_fcr_clear_rx) {
				epoch68_uart_rx_clear(uart);
			}
			if (byte & epoch68_uart_fcr_clear_tx) {
				uart->lsr |= epoch68_uart_lsr_thre | epoch68_uart_lsr_temt;
			}
			epoch68_uart_refresh_irqs();
			break;
		case epoch68_uart_lcr:
			uart->lcr = byte;
			break;
		case epoch68_uart_mcr:
			uart->mcr = byte;
			break;
		case epoch68_uart_lsr:
			break;
		case epoch68_uart_msr:
			break;
		case epoch68_uart_scr:
			uart->scr = byte;
			break;
		default:
			epoch68_fault(address, epoch68_access_write);
			break;
		}
		return;
	}

	if (offset >= epoch68_mm58167_base &&
	    offset < epoch68_mm58167_base + epoch68_mm58167_size) {
		epoch68_mm58167_write(offset - epoch68_mm58167_base, value);
		return;
	}

	switch (address & epoch68_mmio_mask) {
	case epoch68_timer_control:
		if (value & epoch68_timer_enable)
			epoch68_timer_reprime();
		else
			epoch68_timer_enabled = 0;
		break;
	case epoch68_machine_control:
		if (value == epoch68_machine_return_monitor)
			epoch68_return_to_monitor();
		break;
	case 0:
		epoch68_system_page = epoch68_ram_page(value);
		break;
	case 1:
		epoch68_mirror1_page = epoch68_ram_page(value);
		break;
	case 2:
		epoch68_mirror2_page = epoch68_ram_page(value);
		break;
	case 3:
		epoch68_disk_page = value % epoch68_disk_pages;
		break;
	default:
		epoch68_fault(address, epoch68_access_write);
		break;
	}
}

unsigned int m68k_read_memory_8(unsigned int address) {
	uint8_t old_access_size = epoch68_current_access_size;
	uint32_t a = epoch68_address(address);
	uint32_t y = epoch68_y(a);
	unsigned int value;

	if (epoch68_current_access_size == 0) {
		epoch68_current_access_size = 1;
	}

	if (epoch68_boot_overlay) {
		value = epoch68_rom[epoch68_rom_offset(a)];
		epoch68_current_access_size = old_access_size;
		return value;
	}

	if (!epoch68_is_supervisor()) {
		switch (y) {
		case epoch68_y0:
			value = epoch68_page_offset(epoch68_system_page, a);
			break;
		case epoch68_y5:
			if ((a & epoch68_mmio_mask) >= epoch68_uart_base &&
			    (a & epoch68_mmio_mask) < epoch68_uart_base + epoch68_uart_size)
				value = epoch68_read_mmio(a);
			else
				value = epoch68_fault(a, epoch68_access_read);
			break;
		default:
			value = epoch68_fault(a, epoch68_access_read);
			break;
		}
		epoch68_current_access_size = old_access_size;
		return value;
	}

	switch (y) {
	case epoch68_y0:
		value = epoch68_page_offset(0, a);
		break;
	case epoch68_y1:
		value = epoch68_page_offset(epoch68_mirror1_page, a);
		break;
	case epoch68_y2:
		value = epoch68_page_offset(epoch68_mirror2_page, a);
		break;
	case epoch68_y3:
		value = epoch68_disk_offset(epoch68_disk_page, a);
		break;
	case epoch68_y4:
		value = epoch68_rom[epoch68_rom_offset(a)];
		break;
	case epoch68_y5:
		value = epoch68_read_mmio(a);
		break;
	case epoch68_y6:
		value = epoch68_page_offset(epoch68_kernel_metadata_page, a);
		break;
	default:
		value = epoch68_fault(a, epoch68_access_read);
		break;
	}

	epoch68_current_access_size = old_access_size;
	return value;
}

unsigned int m68k_read_memory_16(unsigned int address) {
	uint8_t old_access_size = epoch68_current_access_size;
	unsigned int value;

	epoch68_current_access_size = 2;
	value = (m68k_read_memory_8(address) << 8) |
		m68k_read_memory_8(address + 1);
	epoch68_current_access_size = old_access_size;

	return value;
}

unsigned int m68k_read_memory_32(unsigned int address) {
	uint8_t old_access_size = epoch68_current_access_size;
	unsigned int value;

	epoch68_current_access_size = 4;
	value = (m68k_read_memory_8(address) << 24) |
		(m68k_read_memory_8(address + 1) << 16) |
		(m68k_read_memory_8(address + 2) << 8) |
		m68k_read_memory_8(address + 3);
	epoch68_current_access_size = old_access_size;

	return value;
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
	uint8_t old_access_size = epoch68_current_access_size;
	uint32_t a = epoch68_address(address);
	uint32_t y = epoch68_y(a);

	if (epoch68_current_access_size == 0) {
		epoch68_current_access_size = 1;
	}

	if (epoch68_boot_overlay) {
		epoch68_boot_overlay = 0;
		epoch68_current_access_size = old_access_size;
		return;
	}

	if (!epoch68_is_supervisor()) {
		switch (y) {
		case epoch68_y0:
			epoch68_set_page_offset(epoch68_system_page, a, value);
			break;
		case epoch68_y5:
			if ((a & epoch68_mmio_mask) >= epoch68_uart_base &&
			    (a & epoch68_mmio_mask) < epoch68_uart_base + epoch68_uart_size)
				epoch68_write_mmio(a, value);
			else
				epoch68_fault(a, epoch68_access_write);
			break;
		default:
			epoch68_fault(a, epoch68_access_write);
			break;
		}
		epoch68_current_access_size = old_access_size;
		return;
	}

	switch (y) {
	case epoch68_y0:
		epoch68_set_page_offset(0, a, value);
		break;
	case epoch68_y1:
		epoch68_set_page_offset(epoch68_mirror1_page, a, value);
		break;
	case epoch68_y2:
		epoch68_set_page_offset(epoch68_mirror2_page, a, value);
		break;
	case epoch68_y3:
		epoch68_set_disk_offset(epoch68_disk_page, a, value);
		break;
	case epoch68_y5:
		epoch68_write_mmio(a, value);
		break;
	case epoch68_y6:
		epoch68_set_page_offset(epoch68_kernel_metadata_page, a, value);
		break;
	default:
		epoch68_fault(a, epoch68_access_write);
		break;
	}

	epoch68_current_access_size = old_access_size;
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
	uint8_t old_access_size = epoch68_current_access_size;

	epoch68_current_access_size = 2;
	m68k_write_memory_8(address, value >> 8);
	m68k_write_memory_8(address + 1, value);
	epoch68_current_access_size = old_access_size;
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
	uint8_t old_access_size = epoch68_current_access_size;

	epoch68_current_access_size = 4;
	m68k_write_memory_8(address, value >> 24);
	m68k_write_memory_8(address + 1, value >> 16);
	m68k_write_memory_8(address + 2, value >> 8);
	m68k_write_memory_8(address + 3, value);
	epoch68_current_access_size = old_access_size;
}
*/
import "C"

import (
	"errors"
	"flag"
	"fmt"
	"net"
	"os"
	"os/signal"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"
	"unsafe"
)

const romSize = 64 * 1024
const diskSize = 16 * 1024 * 1024
const uartChannels = 6
const defaultCPUHz = 10000000
const defaultMaxCPUHz = 100000000
const defaultTimerHz = 60
const epoch68TimerIRQLevel = 6
const uartTerminalClear = "\x1b[2J\x1b[H\x1b[3J"

var (
	uartMu               sync.Mutex
	uartConns            [uartChannels]net.Conn
	timerCyclesUntilTick int
	timerWallNext        time.Time
)

func loadROM(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	if len(data) > romSize {
		return fmt.Errorf("ROM image is %d bytes, maximum is %d", len(data), romSize)
	}
	for i, b := range data {
		C.epoch68_rom_write8(C.uint32_t(i), C.uint32_t(b))
	}
	return nil
}

func loadDisk(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	if len(data) > diskSize {
		return fmt.Errorf("disk image is %d bytes, maximum is %d", len(data), diskSize)
	}
	for i, b := range data {
		C.epoch68_disk_write8(C.uint32_t(i), C.uint32_t(b))
	}
	C.epoch68_set_disk_image_size(C.size_t(len(data)))
	return nil
}

func decodeUARTScript(s string) ([]byte, error) {
	if s == "" {
		return nil, nil
	}
	decoded, err := strconv.Unquote(`"` + strings.ReplaceAll(s, `"`, `\"`) + `"`)
	if err != nil {
		return nil, err
	}
	return []byte(decoded), nil
}

func parseTripleUint32(spec string, label string) (uint32, uint32, uint32, error) {
	parts := strings.Split(spec, ":")
	if len(parts) != 3 {
		return 0, 0, 0, fmt.Errorf("%s must be base:slot_size:slots", label)
	}
	values := [3]uint32{}
	for i, part := range parts {
		value, err := strconv.ParseUint(part, 0, 32)
		if err != nil {
			return 0, 0, 0, fmt.Errorf("parse %s field %d %q: %w", label, i+1, part, err)
		}
		values[i] = uint32(value)
	}
	return values[0], values[1], values[2], nil
}

func parseRangeUint32(spec string, label string) (uint32, uint32, error) {
	startText := spec
	lengthText := "1"
	if before, after, ok := strings.Cut(spec, ":"); ok {
		startText = before
		lengthText = after
	}
	start, err := strconv.ParseUint(startText, 0, 32)
	if err != nil {
		return 0, 0, fmt.Errorf("parse %s address %q: %w", label, startText, err)
	}
	length, err := strconv.ParseUint(lengthText, 0, 32)
	if err != nil {
		return 0, 0, fmt.Errorf("parse %s length %q: %w", label, lengthText, err)
	}
	return uint32(start), uint32(length), nil
}

func parseRangeList(spec string, label string, add func(start uint32, length uint32)) error {
	for _, part := range strings.Split(spec, ",") {
		part = strings.TrimSpace(part)
		if part == "" {
			continue
		}
		start, length, err := parseRangeUint32(part, label)
		if err != nil {
			return err
		}
		add(start, length)
	}
	return nil
}

func parseAddressList(spec string, label string) ([]uint32, error) {
	addresses := make([]uint32, 0, 8)
	for _, part := range strings.Split(spec, ",") {
		part = strings.TrimSpace(part)
		if part == "" {
			continue
		}
		address, err := strconv.ParseUint(part, 0, 32)
		if err != nil {
			return nil, fmt.Errorf("parse %s address %q: %w", label, part, err)
		}
		addresses = append(addresses, uint32(address))
		if len(addresses) > 64 {
			return nil, fmt.Errorf("%s accepts at most 64 addresses", label)
		}
	}
	if len(addresses) == 0 {
		return nil, fmt.Errorf("%s requires at least one address", label)
	}
	return addresses, nil
}

func enqueueUARTScript(ch int, data []byte) {
	for _, b := range data {
		queueUARTByte(ch, b)
	}
}

func connectUARTBridge(host string, backendBase int) {
	if host == "" || backendBase <= 0 {
		return
	}
	for i := 0; i < uartChannels; i++ {
		addr := net.JoinHostPort(host, strconv.Itoa(backendBase+i))
		conn, err := net.Dial("tcp", addr)
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d bridge %s: %v\n", i, addr, err)
			continue
		}
		uartConns[i] = conn
		go readUARTBridge(i, conn)
	}
}

func clearUARTBridgeTerminals() {
	var conns [uartChannels]net.Conn

	uartMu.Lock()
	copy(conns[:], uartConns[:])
	uartMu.Unlock()

	for i, conn := range conns {
		if conn == nil || i == 1 {
			continue
		}
		_ = conn.SetWriteDeadline(time.Now().Add(uartBridgeWriteTimeout))
		if _, err := conn.Write([]byte(uartTerminalClear)); err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: UART%d bridge clear failed: %v\n", i, err)
		}
	}
}

func closeUARTBridge() {
	uartMu.Lock()
	defer uartMu.Unlock()

	for i, conn := range uartConns {
		if conn != nil {
			_ = conn.Close()
			uartConns[i] = nil
		}
		uartRXQueues[i] = uartRXQueues[i][:0]
	}
}

func runWithTimer(totalCycles int, cyclesPerTick int, unlimited bool, wallClock bool, timerInterval time.Duration) int {
	if cyclesPerTick <= 0 {
		if unlimited {
			C.m68k_execute(1000000)
			return 1000000
		}
		return int(C.m68k_execute(C.int(totalCycles)))
	}

	if wallClock && unlimited {
		remaining := 1000000
		cyclesRun := 0
		stepLimit := cyclesPerTick / 4
		if stepLimit < 1000 {
			stepLimit = 1000
		}
		if timerWallNext.IsZero() {
			timerWallNext = time.Now().Add(timerInterval)
		}

		for remaining > 0 {
			C.epoch68_poll_devices()
			step := remaining
			if step > stepLimit {
				step = stepLimit
			}
			ran := int(C.m68k_execute(C.int(step)))
			cyclesRun += ran
			remaining -= ran

			now := time.Now()
			catchup := 0
			for !now.Before(timerWallNext) {
				C.epoch68_timer_tick()
				timerWallNext = timerWallNext.Add(timerInterval)
				catchup++
				if catchup >= 4 {
					timerWallNext = now.Add(timerInterval)
					break
				}
			}

			C.epoch68_poll_devices()
			if ran == 0 {
				break
			}
		}

		return cyclesRun
	}

	remaining := totalCycles
	if unlimited {
		remaining = 1000000
	}
	cyclesRun := 0
	if timerCyclesUntilTick <= 0 {
		timerCyclesUntilTick = cyclesPerTick
	}

	for remaining > 0 {
		C.epoch68_poll_devices()
		step := remaining
		if step > timerCyclesUntilTick {
			step = timerCyclesUntilTick
		}
		ran := int(C.m68k_execute(C.int(step)))
		cyclesRun += ran
		remaining -= ran
		timerCyclesUntilTick -= ran
		if timerCyclesUntilTick <= 0 {
			C.epoch68_timer_tick()
			timerCyclesUntilTick = cyclesPerTick
		}
		C.epoch68_poll_devices()
		if ran == 0 {
			break
		}
	}

	return cyclesRun
}

func main() {
	romPath := flag.String("rom", "build/firmware/bootrom.bin", "path to 64 KiB boot ROM image")
	diskPath := flag.String("disk", "ramdisk.img", "path to ramdisk image loaded into the Y3 disk window backing store")
	cycleBudget := flag.Int("cycles", 0, "number of 68000 cycles to execute; 0 runs until interrupted")
	cpuHz := flag.Int("cpu-hz", defaultCPUHz, "assumed 68000 clock rate for periodic timer generation")
	maxCPUHz := flag.Int64("max-cpu-hz", defaultMaxCPUHz, "maximum sustained 68000 clock rate; 0 runs without throttling")
	timerHz := flag.Int("timer-hz", defaultTimerHz, "periodic level-6 clock interrupt rate; 0 disables timer interrupts")
	traceSample := flag.Int("trace-sample", 1, "emit one trace record every N instructions while trace mode is enabled; 0 disables trace output")
	traceStartCompact := flag.Bool("trace-start-compact", false, "enable compact instruction trace from emulator startup")
	traceStartFull := flag.Bool("trace-start-full", false, "enable full instruction trace from emulator startup")
	tracePCDump := flag.String("trace-pc-dump", "", "dump all CPU registers once when executing each address in a comma-separated list; values accept decimal or 0x-prefixed hex")
	traceWatchWrite := flag.String("trace-watch-write", "", "continuously log guest writes touching address[:length][,address[:length]...]; values accept decimal or 0x-prefixed hex")
	kstackMonitor := flag.String("kstack-monitor", "", "track supervisor A7 low-water in kernel stacks as base:slot_size:slots; values accept decimal or 0x-prefixed hex")
	uartScript0Startup := flag.String("uart-script0-startup", "", "scripted UART0 input queued immediately at startup using Go-style escapes like \\n, \\r, \\t, \\x41")
	uartScript0After := flag.String("uart-script0-after", "", "queue uart-script0 when UART0 output contains this literal marker")
	uartScript0 := flag.String("uart-script0", "", "scripted UART0 input queued when uart-script0-after matches, using Go-style escapes like \\n, \\r, \\t, \\x41")
	uartScript0Stream := flag.Bool("uart-script0-stream", false, "when used with uart-script0-after, queue the entire uart-script0 as one stream on the first match")
	uartScript0Loop := flag.Bool("uart-script0-loop", false, "when used with uart-script0-stream, keep repeating uart-script0 after the first trigger")
	uartScript0IntervalMS := flag.Int("uart-script0-interval-ms", 0, "delay in milliseconds between streamed uart-script0 chunks")
	uartScript0JitterPct := flag.Int("uart-script0-jitter-pct", 25, "randomize streamed uart-script0 chunk delays by this percentage up or down")
	uartTerminalBaud := flag.Int("uart-terminal-baud", 0, "emulated output baud for terminal UARTs; 0 disables, UART1 YMODEM/serial-disk is never throttled")
	uartRXTrace := flag.Bool("uart-rx-trace", false, "trace UART bridge receive bytes and emulator RX dequeue order")
	uartHost := flag.String("uart-host", "127.0.0.1", "uartbridge backend host")
	uartBackendBase := flag.Int("uart-backend-base", 23000, "uartbridge backend port for UART0; UARTn uses backend-base+n")
	diskClick := flag.Bool("disk-click", false, "play synthesized, rate-limited seek clicks for 16 KiB Y3 disk-track changes")
	flag.Parse()
	if *maxCPUHz < 0 {
		fmt.Fprintln(os.Stderr, "platform/epoch68/emulator: max-cpu-hz must be non-negative")
		os.Exit(2)
	}
	setUARTTerminalBaud(*uartTerminalBaud)
	setUARTBridgeRXTrace(*uartRXTrace)
	C.epoch68_unbuffer_stdio()
	C.epoch68_set_trace_sample(C.uint64_t(*traceSample))
	if *tracePCDump != "" {
		addresses, err := parseAddressList(*tracePCDump, "trace-pc-dump")
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: %v\n", err)
			os.Exit(1)
		}
		C.epoch68_clear_pc_dumps()
		for _, address := range addresses {
			C.epoch68_add_pc_dump(C.uint32_t(address))
		}
	}
	if *traceWatchWrite != "" {
		C.epoch68_clear_watch_writes()
		err := parseRangeList(*traceWatchWrite, "trace-watch-write", func(start uint32, length uint32) {
			C.epoch68_add_watch_write(C.uint32_t(start), C.uint32_t(length))
		})
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: %v\n", err)
			os.Exit(1)
		}
	}
	if *kstackMonitor != "" {
		base, slotSize, slots, err := parseTripleUint32(*kstackMonitor, "kstack-monitor")
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: %v\n", err)
			os.Exit(1)
		}
		C.epoch68_set_kstack_monitor(C.uint32_t(base), C.uint32_t(slotSize), C.uint32_t(slots))
	}
	// Temporarily disabled with the other bring-up diagnostics.
	// defer C.epoch68_dump_kstack_monitor()

	connectUARTBridge(*uartHost, *uartBackendBase)
	clearUARTBridgeTerminals()
	defer closeUARTBridge()
	setUARTTriggerTiming(time.Duration(*uartScript0IntervalMS)*time.Millisecond, *uartScript0JitterPct)
	if *uartScript0Startup != "" {
		startup, err := decodeUARTScript(*uartScript0Startup)
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: decode uart-script0-startup: %v\n", err)
			os.Exit(1)
		}
		enqueueUARTScript(0, startup)
	}
	if *uartScript0After != "" || *uartScript0 != "" {
		if *uartScript0After == "" || *uartScript0 == "" {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: uart-script0-after and uart-script0 must be used together\n")
			os.Exit(1)
		}
		triggered, err := decodeUARTScript(*uartScript0)
		if err != nil {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: decode uart-script0: %v\n", err)
			os.Exit(1)
		}
		setUARTTriggerScript(0, []byte(*uartScript0After), triggered, *uartScript0Stream, *uartScript0Loop)
	}

	C.epoch68_mem_clear()
	if *diskClick {
		if result := C.epoch68_disk_click_start(); result != 0 {
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: disk-click audio initialization failed (%d)\n", int(result))
			os.Exit(1)
		}
		defer C.epoch68_disk_click_stop()
	}
	if err := loadROM(*romPath); err != nil {
		fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: load ROM %q: %v\n", *romPath, err)
		os.Exit(1)
	}
	if *diskPath != "" {
		if err := loadDisk(*diskPath); err != nil {
			if errors.Is(err, os.ErrNotExist) {
				fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: disk %q not found, starting with empty disk\n", *diskPath)
			} else {
				fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: load disk %q: %v\n", *diskPath, err)
				os.Exit(1)
			}
		}
		cDiskPath := C.CString(*diskPath)
		C.epoch68_set_disk_image_path(cDiskPath)
		C.free(unsafe.Pointer(cDiskPath))
	}

	C.m68k_init()
	C.m68k_set_cpu_type(C.M68K_CPU_TYPE_68000)
	traceHook := C.uint(0)
	if *tracePCDump != "" || *traceStartCompact || *traceStartFull || *kstackMonitor != "" {
		traceHook = 1
	}
	C.epoch68_install_callbacks(traceHook)
	C.m68k_pulse_reset()
	if *traceStartCompact {
		C.epoch68_force_trace_compact()
	}
	if *traceStartFull {
		C.epoch68_force_trace_full()
	}

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt, syscall.SIGTERM)
	defer signal.Stop(sig)

	cyclesPerTick := 0
	if *timerHz > 0 {
		cyclesPerTick = *cpuHz / *timerHz
		if cyclesPerTick <= 0 {
			cyclesPerTick = 1
		}
		timerCyclesUntilTick = cyclesPerTick
	}

	if *cycleBudget > 0 {
		cycles := runWithTimer(*cycleBudget, cyclesPerTick, false, false, 0)
		pc := C.m68k_get_reg(nil, C.M68K_REG_PC)
		fmt.Printf("platform/epoch68/emulator: executed %d cycles, PC=$%08x\n", cycles, uint32(pc))
		C.epoch68_dump_kstack_monitor()
		return
	}

	timerInterval := time.Duration(0)
	if *timerHz > 0 {
		timerInterval = time.Second / time.Duration(*timerHz)
	}

	pacer := cyclePacer{hz: *maxCPUHz}
	for {
		select {
		case s := <-sig:
			fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: caught %s, shutting down\n", s)
			C.epoch68_dump_kstack_monitor()
			if *diskPath != "" && C.epoch68_save_disk_image() == 0 {
				fmt.Fprintf(os.Stderr, "platform/epoch68/emulator: disk save failed during shutdown\n")
			}
			return
		default:
		}
		cycles := runWithTimer(0, cyclesPerTick, true, true, timerInterval)
		pacer.wait(cycles)
	}
}
