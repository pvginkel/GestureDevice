# SEED-NOTES — GestureDevice

First architecture artifact (batch 2 of the producer backfill). See
`../../BATCH-2-FINDINGS.md` and `../../REVIEW-2.md` for the shared rationale; this
file records what is specific to this repo.

- **Producer id:** `gesture-device`  ·  **introduced:** `2026-02-10` (repo's first commit, full clone).
- **What this repo owns:** the FIRMWARE «SoftwareProduct» only. The physical device is registered in IoT Support and will be generated there as a `device:` instance — not modeled here.
- **Universal MDM edges (no boundBy):** firmware → `svc:iotsupport-api` (device API),
  → `cap:pub-sub-broker` (MQTT), → `cap:iam` (Keycloak M2M). Addresses are written
  into device NVS by IoT Support from its own config, so the firmware carries no
  `boundBy` recipe; IoT Support's planned device generator will emit the realized
  per-device `Serving` edges. Modeled as element kind **SystemSoftware** (bare-metal
  firmware) so a later `device: —Assignment→ ss:` resolves cleanly.
- **Device-specific:** none — SEN0626 gesture + LDR → MQTT only.
- **Home Assistant MQTT integration:** → `svc:home-assistant-mqtt` (self-producer, `home-automation.yaml`). Publishes HA MQTT discovery so the device's entities surface in Home Assistant; topic convention hardcoded → no boundBy.
- **Validation:** `./scripts/arch-validate.py docs/architecture/architecture.yaml` → OK.
