#ifndef EPOCH68_TRACE_H
#define EPOCH68_TRACE_H

#if defined(__clang__)

void epoch68_trace_off(void);
void epoch68_trace_compact(void);
void epoch68_trace_full(void);
void epoch68_disk_save(void);
void epoch68_trace_page63_writes_on(void);
void epoch68_trace_page63_writes_off(void);

#define EPOCH68_TRACE_OFF()     epoch68_trace_off()
#define EPOCH68_TRACE_COMPACT() epoch68_trace_compact()
#define EPOCH68_TRACE_FULL()    epoch68_trace_full()
#define EPOCH68_DISK_SAVE()     epoch68_disk_save()
#define EPOCH68_TRACE_PAGE63_WRITES_ON()  epoch68_trace_page63_writes_on()
#define EPOCH68_TRACE_PAGE63_WRITES_OFF() epoch68_trace_page63_writes_off()

#ifndef DEBUG_S
void epoch68_debug_s(const char *message);
void epoch68_debug_hex32(unsigned long value);
void epoch68_debug_sl(const char *message, unsigned long value);
void epoch68_debug_dump512(const void *address);

#define DEBUG_S(message) epoch68_debug_s(message)
#define DEBUG_HEX32(value) epoch68_debug_hex32((unsigned long)(value))
#define DEBUG_SL(message, value) epoch68_debug_sl((message), (unsigned long)(value))
#define DEBUG_DUMP512(address) epoch68_debug_dump512((const void *)(address))
#endif

#else

void epoch68_trace_off(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e680\n"
    "\teinline";

void epoch68_trace_compact(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e681\n"
    "\teinline";

void epoch68_trace_full(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e682\n"
    "\teinline";

void epoch68_disk_save(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e683\n"
    "\teinline";

void epoch68_trace_page63_writes_on(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e68a\n"
    "\teinline";

void epoch68_trace_page63_writes_off(void) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e68b\n"
    "\teinline";

#define EPOCH68_TRACE_OFF()     epoch68_trace_off()
#define EPOCH68_TRACE_COMPACT() epoch68_trace_compact()
#define EPOCH68_TRACE_FULL()    epoch68_trace_full()
#define EPOCH68_DISK_SAVE()     epoch68_disk_save()
#define EPOCH68_TRACE_PAGE63_WRITES_ON()  epoch68_trace_page63_writes_on()
#define EPOCH68_TRACE_PAGE63_WRITES_OFF() epoch68_trace_page63_writes_off()

#ifndef DEBUG_S
void epoch68_debug_s(__reg("a0") const char *message) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e686\n"
    "\teinline";

void epoch68_debug_hex32(__reg("d1") unsigned long value) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e687\n"
    "\teinline";

void epoch68_debug_sl(__reg("a0") const char *message, __reg("d1") unsigned long value) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e688\n"
    "\teinline";

void epoch68_debug_dump512(__reg("a0") const void *address) =
    "\tinline\n"
    "\tdc.w\t$4afc,$e689\n"
    "\teinline";

#define DEBUG_S(message) epoch68_debug_s(message)
#define DEBUG_HEX32(value) epoch68_debug_hex32((unsigned long)(value))
#define DEBUG_SL(message, value) epoch68_debug_sl((message), (unsigned long)(value))
#define DEBUG_DUMP512(address) epoch68_debug_dump512((const void *)(address))
#endif

#endif

#endif
