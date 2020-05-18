## Project Log
### 18th May
Issue with CPG network Max object, trigger output from the network cannot go above around 50Hz. Output just goes to 1 instead of pulsing. This may need to be fixed in the CPG_network source code.

Currently using 3 grainGen objects each corresponding to a node in the network. This works well at low speeds but uses a lot of CPU. Could reduce the multichannel size to lower than 64 channels but this may be a problem at high playback speeds.

TODO:
Create basic UI to control each grain gen subpatcher.
Research how to lower CPU cost.
