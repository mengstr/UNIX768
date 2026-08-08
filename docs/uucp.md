# UUCP and mail gateway

UNIX768 uses the historical V7 UUCP suite and its userland `g` protocol to
exchange files and remote `rmail` jobs with a Taylor UUCP gateway. V7 always
places the call. A single session can push queued outbound mail and collect
inbound work, so the gateway never has to initiate a connection to V7.

The currently tested gateway is capture-only: it accepts mail from V7 and
stores each message durably on Alpine Linux. The final SMTP sender and IMAP
receiver described below are an intended extension, not code that is presently
installed by this repository.

## Topology and names

| Role | Current value |
| --- | --- |
| V7 UUCP name | `unix768` |
| Alpine Taylor name | `dedirock` |
| Taylor called login | `neighbor_login` |
| V7 device | `/dev/tty3`, Epoch68 UART5 |
| Line settings | 1200 baud, 8N1, raw, no getty |
| Emulator UART endpoint | `127.0.0.1:24005` |
| Alpine listener | TCP port 540 |

Some older development notes use `uucpgw` for the peer. The active `L.sys` and
Alpine provisioning defaults use `dedirock`; do not mix those names. The old
name still present in `USERFILE` applies only if V7 acts as the called system,
which the current topology does not use. Likewise, no conversation sequence
check is enabled unless `SQFILE` contains the active peer name on both sides.

In the emulator, `uartbridge` exposes UART5 on port 24005. The repository's
`uucp-tcp-gateway` acts as a virtual automatic calling unit: when V7 writes its
dial string to `/dev/tty3`, the helper opens a fresh TCP connection to Alpine
and then forwards bytes unchanged. Build and manage it with:

```sh
make uucp-tcp-start
make uucp-tcp-status
make uucp-tcp-stop
```

`uartbridge` must already be running. Override the defaults with
`UUCP_UART_PORT`, `UUCP_REMOTE_HOST`, and `UUCP_REMOTE_PORT` when starting the
helper. Its state and log are kept under `build/uucp-tcp`.

## V7 installation and configuration

The normal `/dist/bootstrap` build installs mail and UUCP from the `comm`
source group. It installs `/bin/mail` with `/bin/rmail` as a hard link, the
user commands `uucp`, `uux`, and `uulog`, and `uucico`, `uuxqt`, and `uuclean`
under `/usr/lib/uucp`. It also creates the spool directories and applies the
historical V7 ownership and set-user-ID modes.

Before calling, verify these points inside V7:

- `/etc/ttys` contains `02tty3`, so `init` does not start a getty there.
- `/dev/tty3` is reserved for UUCP and no interactive process has it open.
- `/usr/spool/uucp` and `.XQTDIR` are owned by UID/GID 4 and mode `0777`, as
  required by this historical implementation.
- `L-devices` contains `tty3 tty3 1200`.
- The first field of `L.sys` is `dedirock`, with line type `ACU` and speed
  1200.
- The login and password in `L.sys` exactly match the Alpine `called-login`
  and `/etc/uucp/passwd`. Replace the repository's development password on
  both systems before exposing the service.
- `uuxqt` permits `rmail` but not a general shell or arbitrary commands.

The active files are `/usr/lib/uucp/L-devices`, `L.sys`, `L-dialcodes`,
`USERFILE`, and `SQFILE`. They are deliberately protected and owned by the
`uucp` account.

Start with one diagnostic call rather than cron:

```sh
/usr/lib/uucp/uucico -r1 -sdedirock -x7
uulog -sdedirock
```

A successful empty call proves the login chat, initial handshake, `g` protocol
startup, role reversal, and clean shutdown. If it fails before protocol
startup, compare the login prompts and the initial control message on both
sides. If it fails during `g` startup, first verify that the UART path passes
all byte values without parity stripping, newline conversion, flow control, or
buffer loss. Do not compensate for a line, kernel, or ABI problem in UUCP or
`mail`.

Once manual calls are reliable, a five-minute entry may be added to
`/usr/lib/crontab`:

```text
0,5,10,15,20,25,30,35,40,45,50,55 * * * * /usr/lib/uucp/uucico -r1 -sdedirock
```

UUCP locks prevent two polls from owning the device simultaneously. Use debug
levels only for manual diagnosis; continuous packet logging is too noisy for
normal operation.

## Sending and receiving mail

V7 mail treats the first bang component as the UUCP system. To send Internet
mail through the gateway, use exactly one gateway hop followed by a domain and
local part:

```text
mail dedirock!example.net!alice
```

V7 queues a remote execution request equivalent to `dedirock!rmail` with
`example.net!alice` as its recipient. The gateway must validate that form and
translate it to `alice@example.net`; it should reject empty components,
additional hops, control characters, and shell syntax.

For mail in the other direction, queue an RFC 822 message on Alpine for an
allowlisted V7 login:

```sh
doas -u uucp /usr/bin/uux -r -I /etc/uucp/config \
    unix768!rmail root <message.mail
```

The job waits in Taylor's spool until V7 next calls. V7 `uuxqt` then invokes
`/bin/rmail`, which is the delivery mode of the normal V7 mail program, and
places the message in the user's mailbox.

## Alpine gateway

The tested server is Alpine Linux with Taylor UUCP from `edge/testing` and
`busybox-extras` for `tcpsvd`. No special hardware is required for the TCP
topology: a small VM with persistent spool storage, a correct clock, DNS, and
an address reachable from the virtual ACU is sufficient. An Internet-mail
worker additionally needs outbound TLS access to its SMTP and IMAP services.

Copy
`host/scripts/provision-uucp-gateway-alpine` to the server, enable the testing
repository, and run it as root:

```sh
doas sh -c "printf '%s\n' \
    '@testing https://dl-cdn.alpinelinux.org/alpine/edge/testing' \
    >>/etc/apk/repositories"
doas apk update
doas env UUCP_PASSWORD='<same password as V7 L.sys>' \
    /tmp/provision-uucp-gateway-alpine
```

Optional environment variables set the Taylor node, permitted remote system,
called login, listen port, and capture directory. The script writes an
isolated Taylor configuration below `/etc/uucp`, permits only `rmail`, creates
the spool and logs, installs an OpenRC `uucp-netserver` service, validates the
configuration with `uuchk`, and listens with one `uucico` slave per TCP
connection. Captured messages appear under `/var/spool/uucp/inbox`.

Important Alpine details are already handled by the script: all spool, lock,
and log paths are explicit; `config` and `sys` are mode `0644` because Alpine's
setuid-but-not-setgid Taylor tools must read them; the credential file is
owned by `uucp` and mode `0600`. Restrict TCP port 540 at the firewall to the
known bridge address wherever possible. Historical UUCP authentication and
traffic are not encrypted; across an untrusted network, carry this TCP service
through a private network or VPN rather than exposing port 540 generally.

Useful checks are:

```sh
doas /usr/sbin/uuchk -I /etc/uucp/config
doas rc-service uucp-netserver status
doas tail -n 100 /var/log/uucp-netserver.log
doas tail -n 100 /var/log/uucp/Log
doas ls -la /var/spool/uucp/inbox
```

## Physical hardware

The current Alpine provisioning uses TCP and therefore needs only a networked
machine; it does not configure a physical serial port. A real Epoch68 system
can instead connect UART5 directly to a Raspberry Pi or other Alpine host, but
Taylor's port configuration and service must then be changed from `tcpsvd` to
a direct serial port. On V7, change the `L.sys` line type from `ACU` to `tty3`
for this topology so `uucico` opens the data line directly instead of sending
the virtual-ACU dial terminator first.

Use a dedicated full-duplex, eight-bit-clean link with crossed TX/RX and a
common ground. Match electrical levels: never connect a 3.3 V or 5 V TTL UART
directly to RS-232 voltages; use the appropriate level shifter or an isolated
USB serial adapter. Disable Linux console/login services on that port, give
the `uucp` account access to the stable device path, and start at 1200 baud,
8N1, with software and hardware flow control disabled. Raise the speed only
after repeated binary transfers and checksum comparisons succeed.

## Connecting to Internet mail

Keep Internet delivery asynchronous from UUCP. The gateway `rmail` command
should first commit the complete message and recipient metadata to a durable
local queue, then return success to `uuxqt`. A separate worker converts the
single `domain!localpart` recipient, adds missing `Date` and `Message-ID`
headers, records retry state, and submits through an authenticated TLS SMTP
relay. Never hold a UUCP call open while waiting for an Internet server.

For the previously proposed Purelymail setup, the current official settings
are SMTP over TLS at `smtp.purelymail.com:465` or STARTTLS on port 587, and
IMAP over TLS at `imap.purelymail.com:993`; authentication uses the full
mailbox address and an app password when appropriate. See Purelymail's
[technical setup](https://purelymail.com/docs/setup/technical).

For inbound mail, route a dedicated gateway domain or a catch-all rule to one
mailbox; Purelymail supports catch-all account routing as described in its
[routing documentation](https://purelymail.com/docs/routing). An Alpine worker
can poll that mailbox over IMAP, recover the original envelope recipient from
delivery headers, map its local part only to an explicit V7-user allowlist, and
queue `unix768!rmail <user>` through Taylor. `To` and `Cc` are only fallbacks:
they do not reliably identify Bcc or redirected recipients.

Track IMAP `UIDVALIDITY` and UID, plus `Message-ID` when present. Mark or move
a message only after Taylor has accepted the UUCP job. Quarantine unknown,
ambiguous, or malformed recipients instead of sending them to `root`. Keep
SMTP/IMAP credentials out of the repository and readable only by the worker;
retry temporary network failures with backoff and retain permanent failures
for inspection.

The natural monitoring points are queue depth, the last successful UUCP call,
the last SMTP submission, the last IMAP poll, and the quarantine count. Rotate
logs, but never delete an unprocessed message merely because it is old.
