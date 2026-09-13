# Jarolift ESP -> ioBroker command events

## MQTT event from ESP

Topic per physical shutter:

```text
jarolift/status/command/shutter/1
...
jarolift/status/command/shutter/16
```

The actual MQTT base remains configurable; `jarolift` is only the current example.

Payload:

```json
{"cmd":"UP","source":"controller","seq":123}
```

Supported motion-related commands:

```text
UP
DOWN
STOP
SHADE
SETSHADE
```

`source` currently distinguishes:

```text
controller  command executed from the ESP command queue
remote      command received from a registered original remote
```

The topic is deliberately **not retained**. `seq` is a runtime event sequence and may restart after an ESP reboot.

## ioBroker object model

`Jarolift-Control` maps every command event into the existing physical shutter tree:

```text
0_userdata.0.Jarolift.Shutter.01.LastCommand
0_userdata.0.Jarolift.Shutter.01.LastCommandSource
0_userdata.0.Jarolift.Shutter.01.LastCommandSequence
0_userdata.0.Jarolift.Shutter.01.LastCommandReceivedAt
```

and analogously through shutter 16.

Recommended state types:

```text
LastCommand             string, read-only
LastCommandSource       string, read-only
LastCommandSequence     number, read-only
LastCommandReceivedAt   number, read-only, milliseconds since Unix epoch
```

On an MQTT event, `Jarolift-Control` should write in this order:

1. `LastCommand`
2. `LastCommandSource`
3. `LastCommandSequence`
4. `LastCommandReceivedAt = Date.now()` **last**

All values are written with `ack:true`.

`LastCommandReceivedAt` is the local ioBroker event trigger. This avoids losing repeated equal commands such as `UP` followed later by another `UP`, and it remains reliable when the ESP event sequence restarts after a reboot.

## HomeKit boundary

`Homekit-Rollladen-Jarolift` must not subscribe to ESP MQTT topics.

It consumes only the ioBroker objects under:

```text
0_userdata.0.Jarolift.Shutter.*
```

For external motion synchronization it watches `LastCommandReceivedAt`, then reads `LastCommand` and `LastCommandSource` from the same shutter object.

Thus the data flow is:

```text
ESP/WebUI/MQTT/timer/remote
        -> ESP executes radio command
        -> MQTT command event
        -> Jarolift-Control
        -> 0_userdata.0.Jarolift.Shutter.xx.*
        -> Homekit-Rollladen-Jarolift
        -> HomeKit states
```

The existing retained `jarolift/status/shutter/N` position status remains unchanged and continues to be mapped by `Jarolift-Control` as before.

The existing `jarolift/status/remote/<serial>` diagnostic event also remains unchanged.
