package main

import (
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"syscall"
)

const daemonEnv = "UARTBRIDGE_DAEMON"

func daemonizeLauncher() {
	if os.Getenv(daemonEnv) != "" {
		return
	}

	exe, err := os.Executable()
	if err != nil {
		log.Fatalf("resolve executable path: %v", err)
	}

	cmd := exec.Command(exe, os.Args[1:]...)
	cmd.Env = append(os.Environ(), daemonEnv+"=1")
	cmd.Stdin = nil
	cmd.Stdout = nil
	cmd.Stderr = nil
	cmd.SysProcAttr = &syscall.SysProcAttr{Setsid: true}

	if err := cmd.Start(); err != nil {
		log.Fatalf("start daemon: %v", err)
	}
	os.Exit(0)
}

func writeDaemonPidFile() {
	if os.Getenv(daemonEnv) == "" {
		return
	}

	pidFile := os.Getenv("UARTBRIDGE_PIDFILE")
	if pidFile == "" {
		pidFile = filepath.Join(os.TempDir(), "uartbridge.pid")
	}

	if err := os.WriteFile(pidFile, []byte(strconv.Itoa(os.Getpid())+"\n"), 0644); err != nil {
		log.Printf("write pid file %s: %v", pidFile, err)
	}
}
