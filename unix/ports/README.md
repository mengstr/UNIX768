# Target Ports

This directory contains third-party applications adapted to run on the Epoch68
UNIX target. The stage makefiles are authoritative for which ports are active
and where they are installed in the guest filesystem.

Keeping these sources separate from `unix/v7` distinguishes imported
applications from the historical V7 tree and from project-owned base-system
commands. Host-only dependencies belong in `third_party`; unadopted source
snapshots remain available in the archived UNIX768 repository.

Active ports include:

- `red`: a focused VT100 screen editor retaining WordStar-like control keys
  and line-block copy, cut/move, and insertion operations.
- `xargs`: the historical PWB/UNIX argument builder, adapted to the Epoch68
  ABI without adding modern option extensions.
