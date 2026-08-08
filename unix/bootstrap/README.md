Bootstrap orchestration for the generated V7 filesystem.

`monitor` runs in the ROM monitor. It creates the filesystem, transfers the
minimal kernel, commands, compiler pipeline, runtime, and `/dist/bootstrap`,
then boots UNIX.

`system` runs under `osh` inside V7. It receives each source archive and
invokes the canonical `build.*`, `install.*`, and `clean.*` package scripts.
It finishes by transferring `/dist/installdocs`, sourced from `docs`; running
that script manually receives and installs the optional documentation archive.
