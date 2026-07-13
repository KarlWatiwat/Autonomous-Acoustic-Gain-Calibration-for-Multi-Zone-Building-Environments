# Autonomous Acoustic Gain Calibration for Multi-Zone Building Environments

An Arduino Nano-based embedded system that automatically adjusts public address (PA) speaker volume in real time based on ambient noise levels, aiming to keep announcements and alarms intelligible regardless of surrounding noise conditions.

This was built as a final group project for an Electronic Systems Design (ESD) course.

## Overview

Conventional PA systems in buildings (malls, airports, industrial facilities) typically use a fixed volume setting, which struggles in noisy areas and can be uncomfortably loud in quiet ones. This project implements a closed-loop **Automatic Gain Control (AGC)** system that continuously senses ambient noise and adjusts speaker output volume accordingly, so that announcements and emergency alarms stay clear and audible without manual intervention.

The system supports three operating modes:
- **Announcement Mode** — plays a prerecorded message at noise-adjusted volume
- **Fire Alarm Mode** — plays a prerecorded emergency alarm at noise-adjusted volume
- **Auto-Silence / Noise Detection Mode** — continuously monitors ambient noise and automatically triggers a reminder message when noise stays above a threshold for a sustained period (with a cooldown to prevent repeated triggering)

## System Architecture

```
Button/Toggle Selection → Arduino Nano (Brain) → DFPlayer Mini → PAM8403 Amplifier → Speaker
                                  ↑                                      
                            MAX9814 Mic ← Feedback (ambient + playback noise)
```

The Arduino Nano polls user input (buttons/toggle), triggers audio playback via the DFPlayer Mini, and continuously samples the MAX9814 microphone to estimate ambient loudness. Based on that reading, it dynamically sets the DFPlayer's output volume before/during playback, forming a closed feedback loop between the acoustic environment and the audio output level.

## Core Components

| Component | Role |
|---|---|
| Arduino Nano | Central microcontroller — runs control logic and the AGC algorithm |
| MAX9814 | Electret microphone amplifier module — ambient/acoustic noise sensor |
| DFPlayer Mini | MP3 module — stores and plays prerecorded announcement/alarm audio from SD card |
| PAM8403 | Class-D audio amplifier — drives the output speaker |
| Speaker (5W, 4Ω) | Audio output |
| Push buttons (D3, D4) | Manual "Fire Alarm" and "Announce" triggers |
| Toggle switch (D2) | Enables/disables Auto-Silence (auto noise monitoring) mode |

## Pin Mapping

| Signal | Arduino Pin |
|---|---|
| Microphone input (MAX9814 OUT) | A0 |
| Sensitivity pin (unused in current logic) | A1 |
| Auto-mode toggle switch | D2 |
| Fire alarm button | D3 |
| Announce button | D4 |
| DFPlayer RX | D10 |
| DFPlayer TX | D11 |

> DFPlayer communicates over `SoftwareSerial` (D11 = RX, D10 = TX on the Nano side). A 1kΩ resistor is placed in series on the line into the DFPlayer's RX pin to protect it from logic-level mismatch.

## PCB Layout
<img width="621" height="425" alt="image" src="https://github.com/user-attachments/assets/2f051f3d-3357-49bf-b7b0-b756aed7bba4" />
<img width="450" height="311" alt="image" src="https://github.com/user-attachments/assets/77484cd7-2630-49d3-9d91-507e74035273" />
<img width="369" height="256" alt="image" src="https://github.com/user-attachments/assets/f913c6c0-2d85-41db-bc5d-b851ae189dca" />


## How the AGC Logic Works

1. The microphone (MAX9814) continuously outputs an analog signal on `A0` representing ambient sound pressure.
2. The Arduino reads this signal and computes a relative amplitude by taking `abs(analogRead(A0) - 300)` — using 300 as an assumed DC bias/quiet-baseline reference point.
3. If this amplitude crosses a fixed threshold (`>= 200`), the system considers the environment "noisy" and sets output volume to a higher level (`30`); otherwise it uses a lower baseline volume (`23`).
4. For **Announce** and **Fire Alarm** triggers, the current adaptive volume is applied immediately before playback.
5. For **Auto-Silence** mode, a `noiseCounter` accumulates over repeated noisy readings (+5 per noisy sample, -2 decay per quiet sample) — once it exceeds a threshold (`50`), a reminder message plays automatically, gated by a 10-second cooldown to prevent re-triggering back-to-back.

## Hardware Integration

- **Audio mixing:** MAX9814 output and DFPlayer `DAC_L` output are combined into the PAM8403 amplifier input through a summing resistor network (10kΩ), allowing simultaneous monitoring and playback without one signal loading down the other.
- **Power:** The Nano supplies regulated 5V to the DFPlayer Mini and PAM8403, and 3.3V to the MAX9814 for a cleaner reference voltage.
- **PCB:** A custom PCB was designed in EasyEDA to consolidate the Arduino Nano, DFPlayer Mini, MAX9814, and interface components into a single soldered prototype board.

## Testing

The system was validated across three scenarios:
- **Case 1 — Announcement Mode:** Manual trigger played a message while volume adapted live to changing ambient noise.
- **Case 2 — Fire Alarm Mode:** Manual trigger played the emergency alarm with the same adaptive volume behavior.
- **Case 3 — Sound Detection Mode:** System left in auto-monitoring mode correctly detected sustained high noise and auto-triggered a reminder alert without manual input.

All three cases confirmed correct sensor-to-signal integration and real-time adaptive gain behavior.

## Files

- `AGC.ino` — Full Arduino sketch (setup, main loop, AGC/noise-monitoring logic)

## Known Limitations / Future Work

- Volume levels and noise thresholds (`300` baseline, `200` amplitude threshold, `23`/`30` volume steps) are hardcoded constants tuned for a specific test environment rather than calibrated dynamically — a more robust version would auto-calibrate the "quiet" baseline per zone.
- The current implementation is single-zone (one mic, one speaker channel); the "multi-zone" concept in the original proposal would require multiple sensor/output pairs coordinated by the same or networked controllers.
- `SENSITIVITY_PIN` (A1) is defined but not yet used in the control logic — reserved for a potential hardware sensitivity trim input.
- Gain transitions are step-based (two fixed volume levels) rather than continuously proportional to noise level.

## Team

Africa, Eufe Gabriel · Agno, Charles Lanz · Gepiala, Jessa · Landicho, Kyle · Pabito, Ken Kirby · Rosales, John Michael · Watiwat, Karl Fredrich
BS Electronics and Communication Engineering — JIECEP and IECEP Batangas Student Chapter

## License

No license specified — add one (e.g., MIT) if you'd like others to be able to reuse this freely.
