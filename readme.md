## Project Log
### 19th May
Now using 4 CPG nodes with grainGen objects, each with 32 channels. This can be reduced to 16 channels without making the synth sound bad. Still have issue with slow output from CPG network. Instead of using trigger output I am using the main output and detecting when the signal crosses 0.5f. This works ok but the network can still not go faster than 400Hz. This is probably fast enough all considered but this might be improved later to go fastfast.

Still quite cpu intensive not sure where to save though. Could look at code inside graingen to see where power can be saved.

Stil no way to change the weights between nodes but it works well just now with set relationships between the nodes.

### 18th May
Issue with CPG network Max object, trigger output from the network cannot go above around 50Hz. Output just goes to 1 instead of pulsing. This may need to be fixed in the CPG_network source code.

Currently using 3 grainGen objects each corresponding to a node in the network. This works well at low speeds but uses a lot of CPU. Could reduce the multichannel size to lower than 64 channels but this may be a problem at high playback speeds.

TODO:
Create basic UI to control each grain gen subpatcher.
Allow weight changing between nodes.
