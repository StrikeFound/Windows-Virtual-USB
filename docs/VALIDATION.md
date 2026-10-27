# Validation and implemented scope

Windows Virtual USB 0.1.0.0 has its own verified Microsoft-signed package. Catalog memberships, unchanged INF files and protected executable content match the submitted candidate. Native SDK/example tests, installer payload checks and offline UI/state tests pass.

Live installation/device-lifecycle testing of this edition and physical-hardware validation remain pending. The baseline evidence below concerns the preceding edition only.

## Inherited runtime evidence and known limitation

The preceding Windows-Virtual-Devices edition passed its five virtual-device
checks on Windows 11 build 26200 with targeted Driver Verifier, and on Windows 10
build 19045 in normal mode. Those results are baseline evidence only, not tests
of Windows-Virtual-USB.

Windows 10 xusb22.sys 10.0.19041.6389 produced C9/7 under I/O verification during
virtual Xbox 360 surprise removal with a pending request. The older edition's
state/rumble/burst assertions finished before the failure; older lab dumps show
the same bucket. The failure remains unresolved. This edition does not claim
to fix it, and the passing cases do not establish universal compatibility.

## Scope

KMDF/UdeCx, USB low/full/high speed, one configuration, alternate setting zero,
interrupt/bulk endpoints. SuperSpeed, isochronous audio, external hubs and extra
configurations are not implemented. VID/PID changes alone do not reproduce
firmware or console authentication.

Offline tests cover descriptor construction, raw protocol state machines,
installer ownership, endpoint selection and borrowed byte-buffer submission.
Only guest-local test signing or the matching Microsoft return may be used for
runtime testing. Never install unsigned candidates on a normal user's PC.
