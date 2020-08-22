# StealthProto
This is based on Unreal Engine's Third Person game template that provided `StealthProto`, `StealthProtoGameMode` and `StealthProto.Build` classes and basis for `StealthProtoCharacter`.

Uses Unreal Engine version 4.25.

# About
This prototype is only for testing a stealth game system, without any gameplay content. Player can run or walk/sneak around the level (hold down left shift to sneak) and try to avoid different types of detectors. Sneaking makes less noise than running as player makes noise based on their movement speed, note that moving in the air is silent.

## Detectors
There are two types of detectors: sound and visual detectors. When detector detects the player it will turn red and stay alarmed for a specific time. Detectors also have a dead zone where they will not be alarmed but will turn yellow to indicate that they are cautios.

### Sound Detector
Will detect if player moves around close to them. If player stops the alarm will eventually be dropped as they are not making in noise anymore. Sound detectors will also periodically turn inactive and drop the green color.

### Visual Detector
Visual detectors will detect player if they enter inside their field of view and are close enough. Visual detectors will instantly drop alarm or cautios mode when player is out of sight. They are also never inactive.
