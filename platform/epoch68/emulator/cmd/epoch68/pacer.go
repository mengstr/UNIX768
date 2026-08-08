package main

import "time"

const pacerMaxLag = 100 * time.Millisecond

// cyclePacer limits sustained emulated CPU throughput without changing timer
// delivery. It deliberately paces only between execution batches, keeping the
// instruction loop and device polling paths free of host scheduling calls.
type cyclePacer struct {
	hz       int64
	deadline time.Time
}

func (p *cyclePacer) delay(now time.Time, cycles int) time.Duration {
	if p.hz <= 0 || cycles <= 0 {
		return 0
	}

	// Do not try to make up a large deficit after the host process was paused
	// or starved; resume pacing from the current wall clock instead.
	if p.deadline.IsZero() {
		p.deadline = now
	} else if now.Sub(p.deadline) > pacerMaxLag {
		p.deadline = now
		return 0
	}

	cycleTime := (int64(cycles)*int64(time.Second) + p.hz - 1) / p.hz
	p.deadline = p.deadline.Add(time.Duration(cycleTime))
	return p.deadline.Sub(now)
}

func (p *cyclePacer) wait(cycles int) {
	if delay := p.delay(time.Now(), cycles); delay > 0 {
		time.Sleep(delay)
	}
}
