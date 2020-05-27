## Install + Run
Download this repo to your max packages folder.
1. (Max CPG library) [https://github.com/DanBennettDev/max_cpg]
2. From CPG library run neurhythmicGUI in max_cpg-master\NeurythmicApp\gui\
3. Download this repo and run CPGrains.maxpat, use presentation mode
4. Add up to 4 nodes using neurhytmicGUI to control grain rhythms

## Project Log
### 27thMay
Redo of grainGen object now one gen with 128 channels. Basic gui created in presentation mode allowing users to change parameters for each node. Currently can choose grain size, grain start and frequency multiplier from main node.
### 21st May
Change from using 4 grainGen objects to 1 grainGen obj with 128 channels. Signals from all CPG nodes are sent into this grain object. Not sure how to specify parameters for each node now, will need to send all information about grains at signal rate. Possible to have lots of inlets to grainGen and change depending on what node has just been triggered.

Need to change how grains are triggered and how to choose which channel. Right now it just uses a counter that goes up forever and takes a modulus from it to choose a channel, probably not great....

Can now use neurhythmic app to control nodes, just need to start the app after starting Max. Some interesting timbral things happen when a network has been set up. Maybe good technique to first build an interesting rhythm with the nodes then speed up afterwards... It's sometimes hard to hear exactly how the network is affecting the final sound.

### 19th May
Now using 4 CPG nodes with grainGen objects, each with 32 channels. This can be reduced to 16 channels without making the synth sound bad. Still have issue with slow output from CPG network. Instead of using trigger output I am using the main output and detecting when the signal crosses 0.5f. This works ok but the network can still not go faster than 400Hz. This is probably fast enough all considered but this might be improved later to go fastfast.

Still quite cpu intensive not sure where to save though. Could look at code inside graingen to see where power can be saved.

Stil no way to change the weights between nodes but it works well just now with set relationships between the nodes.

### 18th May
Issue with CPG network Max object, trigger output from the network cannot go above around 50Hz. Output just goes to 1 instead of pulsing. This may need to be fixed in the CPG_network source code.

Currently using 3 grainGen objects each corresponding to a node in the network. This works well at low speeds but uses a lot of CPU. Could reduce the multichannel size to lower than 64 channels but this may be a problem at high playback speeds.

