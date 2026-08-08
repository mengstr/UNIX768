package main

import (
	"testing"
	"time"
)

func TestCyclePacerDelay(t *testing.T) {
	now := time.Unix(100, 0)
	pacer := cyclePacer{hz: 100_000_000}

	if got, want := pacer.delay(now, 1_000_000), 10*time.Millisecond; got != want {
		t.Fatalf("first delay = %v, want %v", got, want)
	}
	if got, want := pacer.delay(now.Add(3*time.Millisecond), 1_000_000), 17*time.Millisecond; got != want {
		t.Fatalf("second delay = %v, want %v", got, want)
	}
}

func TestCyclePacerResetsAfterLargeLag(t *testing.T) {
	now := time.Unix(100, 0)
	pacer := cyclePacer{hz: 50_000_000}
	pacer.delay(now, 1_000_000)

	late := now.Add(200 * time.Millisecond)
	if got, want := pacer.delay(late, 1_000_000), time.Duration(0); got != want {
		t.Fatalf("delay after lag = %v, want %v", got, want)
	}
	if got, want := pacer.delay(late, 1_000_000), 20*time.Millisecond; got != want {
		t.Fatalf("delay after reset = %v, want %v", got, want)
	}
}

func TestCyclePacerDisabled(t *testing.T) {
	pacer := cyclePacer{}
	if got := pacer.delay(time.Now(), 1_000_000); got != 0 {
		t.Fatalf("disabled delay = %v, want 0", got)
	}
}
